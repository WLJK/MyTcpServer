#include <QThread>
#include "MyTcpServer.h"
#include "sqlbusiness.h"
#include "database.h"
#include <QSqlRecord>
#include <QSqlError>


MyTcpServer::MyTcpServer(QObject *parent)
    : QTcpServer{ parent }, m_userType(WHAT)
{
    
}

MyTcpServer::~MyTcpServer()
{}

void MyTcpServer::incomingConnection(qintptr handle)
{
    qDebug() << "已接收到连接";
    QThread* thread = new QThread(this);
    sqlbusiness* sqlBusiness = new sqlbusiness();
    sqlBusiness->moveToThread(thread);
    socketThreadHash[handle] = thread;
    threadSqlHash[thread] = sqlBusiness;
    connect(sqlBusiness, &sqlbusiness::start, sqlBusiness, &sqlbusiness::mainBusiness);
    connect(sqlBusiness, &sqlbusiness::onlineSiganl, this, &MyTcpServer::onlineHandle);
    connect(sqlBusiness, &sqlbusiness::loginSiganl, this, &MyTcpServer::loginHandle);
    connect(sqlBusiness, &sqlbusiness::infRegSiganl, this, &MyTcpServer::infRegHandle);
    connect(sqlBusiness, &sqlbusiness::questionInsertSiganl, this, &MyTcpServer::questionInsertHandle);
    connect(sqlBusiness, &sqlbusiness::studentAnswerSiganl, this, &MyTcpServer::studentAnswerHandle);
    connect(sqlBusiness, &sqlbusiness::qqInsertSiganl, this, &MyTcpServer::qqInsertHandle);
    connect(sqlBusiness, &sqlbusiness::questionUpdateSiganl, this, &MyTcpServer::questionUpdateHandle);
    connect(sqlBusiness, &sqlbusiness::questionDelectSiganl, this, &MyTcpServer::questionDelectHandle);
    connect(sqlBusiness, &sqlbusiness::delectSiganl, this, &MyTcpServer::delectHandle);
    connect(sqlBusiness, &sqlbusiness::mulTableInsertSiganl, this, &MyTcpServer::mulTableInsertHandle);
    qRegisterMetaType<qintptr>("qintptr");
    thread->start();
    emit sqlBusiness->start(handle);
}

void MyTcpServer::loginHandle(QString usertype, QString username, QString password, qintptr handle)
{
    qDebug() << "进入登陆槽函数";
    database::getConnection();
    //查询数据
    QSqlQuery query;
    QString qs;
    qDebug() << "usertype:" << usertype << "username:" << username << "password:" << password;
    qs = QString("select * from %1 where username = :username and password = :password").arg(usertype);
    query.prepare(qs);
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    if (!query.exec()) {
        qDebug() << query.lastError().text();   //输出错误信息
        bool state = false;
        QString userdata = "服务器异常";
        QByteArray dataToSend = MyJSON::loginOrRegisterBackToByteArray(state, userdata);
        emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);
        return;
    }
    //获取查询结果
    if (query.next()) {
        if (!userOnlineHash[username]) {
            userOnlineHash[username] = true;
            QThread* thread = socketThreadHash[handle];
            userThreadHash[thread] = username;
            bool state = true;
            QString userdata = "登录成功";
            if (usertype == "teacher")
                m_userType = TEACHER;
            else if (usertype == "student")
                m_userType = STUDENT;
            qDebug() << "state:" << state << "userdata:" << userdata;
            QByteArray dataToSend = MyJSON::loginOrRegisterBackToByteArray(state, userdata);
            emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);
        }
        else
        {
            bool state = false;
            QString userdata = "登录失败：该用户已在线";
            QByteArray dataToSend = MyJSON::loginOrRegisterBackToByteArray(state, userdata);
            database::quitConnection();
            emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);
        }
    }
    else {

        QString userdata = "登录失败：无效的用户名或密码";
        bool state = false;
        QByteArray dataToSend = MyJSON::loginOrRegisterBackToByteArray(state, userdata);
        database::quitConnection();
        emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);
    }
}

void MyTcpServer::mulTableInsertHandle(QString value1, QString value2, QString value3, qintptr handle, QString messageType) {
    QSqlQuery query;
    QThread *thread = socketThreadHash.value(handle); // 根据 handle 获取线程
    QString username = userThreadHash.value(thread); // 根据线程获取用户名
    query.prepare("INSERT INTO Questionnaire (title, teacher_username) VALUES (:title, :teacher_username)");
        query.bindValue(":title", value2);
        query.bindValue(":teacher_username", username);
        if (!query.exec())  {
            bool state = false;
            QString errorText = "插入失败原因:" + query.lastError().text();
            QJsonObject messageObject = MyJSON::sqlRegisterBack(state, errorText, messageType);
            messageObject["sql_type"] = "insert";
            QJsonDocument doc(messageObject);
            QByteArray dataToSend = doc.toJson();
            emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);

            qDebug() << query.lastError().text();
        }
        // 获取自动生成的 questionnaire_id
        int questionnaireId = query.lastInsertId().toInt();
        // 解析 combinedData 并插入到 QuestionnaireQuestion 表
        QStringList pairs = value3.split(";");
        for (const QString &pair : pairs) {
            QStringList questionInfo = pair.split(":");

            if (questionInfo.size() != 2) {
                qDebug() << "信息无效:" << pair;
                continue;
            }

            QString questionId = questionInfo[0];
            QString questionType = questionInfo[1];

            query.prepare("INSERT INTO QuestionnaireQuestion (questionnaire_id, question_id, question_type) VALUES (:questionnaire_id, :question_id, :question_type)");
            query.bindValue(":questionnaire_id", questionnaireId);
            query.bindValue(":question_id", questionId.toInt());
            query.bindValue(":question_type", questionType);
            query.exec();
        }
        bool state = true;
        qDebug() << "插入信号发送成功";
        QJsonObject messageObject = MyJSON::sqlRegisterBack(state, "", messageType);
        messageObject["sql_type"] = "insert";
        QJsonDocument doc(messageObject);
        QByteArray dataToSend = doc.toJson();
        qDebug() <<"dataToSend" <<dataToSend;
        emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);
        query.clear();
}

void MyTcpServer::questionUpdateHandle(QString tableName, QString fieldNames, QString fieldValues, qintptr handle, QString messageType) {
    QSqlQuery query;
    QStringList fieldNameList = fieldNames.split(';');
    QStringList fieldValueList = fieldValues.split(';');

    QString setString = "";
    for (int i = 0; i < fieldNameList.size() - 1; ++i) { // 减一去掉question_id
        setString += fieldNameList.at(i) + "='" + fieldValueList.at(i) + "'";
        if (i < fieldNameList.size() - 2) {
            setString += ", ";
        }
    }

    QString updateSql = QString("UPDATE %1 SET %2 WHERE %3 = '%4'").arg(tableName).arg(setString).arg(fieldNameList.last()).arg(fieldValueList.last());


    qDebug() << "updateSql" << updateSql;

    if (!query.exec(updateSql)) {
        bool state = false;
        QString errorText = "更新失败原因: " + query.lastError().text();
        QJsonObject messageObject = MyJSON::sqlRegisterBack(state, errorText, messageType);
        messageObject["sql_type"] = "insert";
        QJsonDocument doc(messageObject);
        QByteArray dataToSend = doc.toJson();
        emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);

        qDebug() << query.lastError().text();
    }
    else
    {
        bool state = true;
        qDebug() << "更新信号发送成功";
        QJsonObject messageObject = MyJSON::sqlRegisterBack(state, "", messageType);
        messageObject["sql_type"] = "insert";
        QJsonDocument doc(messageObject);
        QByteArray dataToSend = doc.toJson();
        qDebug() << "dataToSend" << dataToSend;
        emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);
    }
    query.clear();
}

void MyTcpServer::qqInsertHandle(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType) {
    QString targetClass = messagetype;
    int questionnaireId = fieldValues.toInt();

    qDebug() << "Target class:" << targetClass;
    qDebug() << "Questionnaire ID:" << questionnaireId;

    QSqlQuery query;
    QString queryString = QString("SELECT * FROM Student WHERE class = '%1'").arg(targetClass);
    query.prepare(queryString);

    if (query.exec()) {
        while (query.next()) {
            QString studentUsername = query.value("username").toString();
            qDebug() << "Student username:" << studentUsername;

            // 查找对应的问卷问题列表
            QSqlQuery qqQuery;
            QString qqQueryString = QString("SELECT qq_id FROM QuestionnaireQuestion WHERE questionnaire_id = %1").arg(questionnaireId);
            qqQuery.prepare(qqQueryString);

            if (qqQuery.exec()) {
                while (qqQuery.next()) {
                    int qqId = qqQuery.value("qq_id").toInt();
                    qDebug() << "QuestionnaireQuestion ID:" << qqId;

                    // 为每个学生插入新的StudentAnswer记录
                    QSqlQuery insertQuery;
                    QString insertQueryString = QString("INSERT INTO StudentAnswer (student_username, qq_id, answer) VALUES ('%1', %2, NULL)").arg(studentUsername).arg(qqId);
                    insertQuery.prepare(insertQueryString);

                    if (!insertQuery.exec()) {
                        qDebug() << "Query error:" << query.lastError().text();
                        bool state = false;
                        QString errorText = query.lastError().text();
                        QJsonObject messageObject = MyJSON::sqlRegisterBack(state, errorText, messageType);
                        QJsonDocument doc(messageObject);
                        QByteArray dataToSend = doc.toJson();
                        emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);
                    } else {
                        qDebug() << "Inserted record for student:" << studentUsername << ", QQ ID:" << qqId;
                    }
                }
            } else {
                qDebug() << "Query error:" << query.lastError().text();
                bool state = false;
                QString errorText = query.lastError().text();
                QJsonObject messageObject = MyJSON::sqlRegisterBack(state, errorText, messageType);
                QJsonDocument doc(messageObject);
                QByteArray dataToSend = doc.toJson();
                emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);
            }
        }
    } else {
        qDebug() << "Query error:" << query.lastError().text();
        bool state = false;
        QString errorText = query.lastError().text();
        QJsonObject messageObject = MyJSON::sqlRegisterBack(state, errorText, messageType);
        QJsonDocument doc(messageObject);
        QByteArray dataToSend = doc.toJson();
        emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);
    }

    bool state = true;
    qDebug() << "发放问卷信号发送成功";
    QJsonObject messageObject = MyJSON::sqlRegisterBack(state, "", messageType);
    messageObject["sql_type"] = "insert";
    QJsonDocument doc(messageObject);
    QByteArray dataToSend = doc.toJson();
    qDebug() <<"dataToSend" <<dataToSend;
    emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);
    query.clear();

}

void MyTcpServer::studentAnswerHandle(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType) {
    QSqlQuery query;
    QStringList answerList = fieldValues.split(';');
    QStringList qq_idList = fieldNames.split(';');


    // 获取 student_username
    QThread *thread = socketThreadHash.value(handle);
    QString student_username = userThreadHash.value(thread);

    // 验证 answerList 和 qq_idList 的长度是否相同
    if (answerList.size() != qq_idList.size()) {
        qDebug() << "Error: 长度不一致";
        bool state = false;
        QString errorText = "长度不一致";
        QJsonObject messageObject = MyJSON::sqlRegisterBack(state, errorText, messageType);
        messageObject["sql_type"] = "insert";
        QJsonDocument doc(messageObject);
        QByteArray dataToSend = doc.toJson();
        emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);
        return;
    }

    // 遍历每个问题的答案并更新 StudentAnswer 表中的 answer 字段
    for (int i = 0; i < answerList.size(); ++i) {
        QString answer = answerList[i];
        QString qq_id = qq_idList[i];

        // 更新 StudentAnswer 表的 answer 字段
        query.prepare("UPDATE StudentAnswer SET answer = :answer WHERE student_username = :student_username AND qq_id = :qq_id");
        query.bindValue(":answer", answer);
        query.bindValue(":student_username", student_username);
        query.bindValue(":qq_id", qq_id.toInt());

        if (!query.exec()) {
            bool state = false;
            QString errorText = "提交失败原因:" + query.lastError().text();
            QJsonObject messageObject = MyJSON::sqlRegisterBack(state, errorText, messageType);
            messageObject["sql_type"] = "insert";
            QJsonDocument doc(messageObject);
            QByteArray dataToSend = doc.toJson();
            emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);

            qDebug() << query.lastError().text();
            return;
        }
    }
    bool state = true;
    qDebug() << "提交信号发送成功";
    QJsonObject messageObject = MyJSON::sqlRegisterBack(state, "", messageType);
    messageObject["sql_type"] = "insert";
    QJsonDocument doc(messageObject);
    QByteArray dataToSend = doc.toJson();
    qDebug() <<"dataToSend" <<dataToSend;
    emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);

}

void MyTcpServer::questionInsertHandle(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType) {
    QSqlQuery query;
    QStringList fieldNameList = fieldNames.split(';');
    QStringList fieldValueList = fieldValues.split(';');

    QString columns = "(";
    for (int i = 0; i < fieldNameList.size(); ++i)  {
        columns += fieldNameList.at(i);
        if (i < fieldNameList.size() -1)    {
            columns += ",";
        }
    }
    columns += ")";

    QString values = "VALUES (";
    for (int i = 0; i < fieldValueList.size(); ++i) {
        values += "'" + fieldValueList.at(i) + "'";
        if (i < fieldValueList.size() - 1) {
            values += ", ";
        }
    }
    values += ")";
    qDebug() <<"columns" << columns << "values" << values;
    QString insertSql = "INSERT INTO " + messagetype + " " + columns + " " + values;

    if (!query.exec(insertSql)) {
        bool state = false;
        QString errorText = "插入失败原因:" + query.lastError().text();
        QJsonObject messageObject = MyJSON::sqlRegisterBack(state, errorText, messageType);
        messageObject["sql_type"] = "insert";
        QJsonDocument doc(messageObject);
        QByteArray dataToSend = doc.toJson();
        emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);

        qDebug() << query.lastError().text();
    }
    else
    {
        bool state = true;
        qDebug() << "插入信号发送成功";
        QJsonObject messageObject = MyJSON::sqlRegisterBack(state, "", messageType);
        messageObject["sql_type"] = "insert";
        QJsonDocument doc(messageObject);
        QByteArray dataToSend = doc.toJson();
        qDebug() <<"dataToSend" <<dataToSend;
        emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);
    }
    query.clear();
}

void MyTcpServer::questionDelectHandle(QString messagetype, QString registertype, QString information, qintptr handle, QString messageType)  {
    QSqlQuery query;
    QStringList fieldNameList = registertype.split(';');
    QStringList fieldValueList = information.split(';');

    for (int i = 0; i < fieldNameList.size(); ++i) {
        qDebug() << fieldNameList.at(i).toInt();
        if (!fieldNameList.at(i).isEmpty() && !fieldValueList.at(i).isEmpty())  {
        QString deleteFromQuestionnaireSql = QString("DELETE FROM questionnairequestion WHERE question_id = %1 AND question_type = '%2'").arg(fieldNameList.at(i).toInt()).arg(fieldValueList.at(i));
        if (!query.exec(deleteFromQuestionnaireSql)) {
            bool state = false;
            QString errorText = "删除失败原因:" + query.lastError().text();
            QJsonObject messageObject = MyJSON::sqlRegisterBack(state, errorText, messageType);
            messageObject["sql_type"] = "insert";
            QJsonDocument doc(messageObject);
            QByteArray dataToSend = doc.toJson();
            emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);

            qDebug() << "questionnairequestion" + query.lastError().text();
            return;
        }
        }
    }

    for (int i = 0; i < fieldNameList.size(); ++i) {
        if (!fieldNameList.at(i).isEmpty() && !fieldValueList.at(i).isEmpty()) {
        QString deleteFromQuestionTypeSql = QString("DELETE FROM %1 WHERE question_id = %2").arg(fieldValueList.at(i)).arg(fieldNameList.at(i).toInt());

        if (!query.exec(deleteFromQuestionTypeSql)) {
            bool state = false;
            QString errorText = "删除失败原因:" + query.lastError().text();
            QJsonObject messageObject = MyJSON::sqlRegisterBack(state, errorText, messageType);
            messageObject["sql_type"] = "insert";
            QJsonDocument doc(messageObject);
            QByteArray dataToSend = doc.toJson();
            emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);
            qDebug() << "题库删除失败" + query.lastError().text();
            return;
        }
        }
    }
    bool state = true;
    qDebug() << "删除信号发送成功";
    QJsonObject messageObject = MyJSON::sqlRegisterBack(state, "", messageType);
    messageObject["sql_type"] = "insert";
    QJsonDocument doc(messageObject);
    QByteArray dataToSend = doc.toJson();
    qDebug() <<"dataToSend" <<dataToSend;
    emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);
    query.clear();

}

void MyTcpServer::delectHandle(QString messagetype, QString registertype, QString information, qintptr handle, QString messageType) {
    QSqlQuery query;
    QStringList tableList = messagetype.split(';');
    QStringList fieldNameList = registertype.split(';');
    QStringList fieldValueList = information.split(';');
    for (int i = 0; i < fieldNameList.size(); ++i) {
        qDebug() << fieldNameList.at(i).toInt();
        if (!fieldNameList.at(i).isEmpty() && !fieldValueList.at(i).isEmpty())  {
            QString deleteSql = QString("DELETE FROM %1 WHERE %2 = :information")
                    .arg(tableList.at(i))
                    .arg(fieldNameList.at(i));
            query.prepare(deleteSql);
            query.bindValue(":information", fieldValueList.at(i));
            if (!query.exec()) {
                bool state = false;
                QString errorText = "删除失败原因:" + query.lastError().text();
                QJsonObject messageObject = MyJSON::sqlRegisterBack(state, errorText, messageType);
                messageObject["sql_type"] = "insert";
                QJsonDocument doc(messageObject);
                QByteArray dataToSend = doc.toJson();
                emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);

                qDebug() << query.lastError().text();
                return;
            }
        }
    }
    bool state = true;
    qDebug() << "删除信号发送成功";
    QJsonObject messageObject = MyJSON::sqlRegisterBack(state, "", messageType);
    messageObject["sql_type"] = "insert";
    QJsonDocument doc(messageObject);
    QByteArray dataToSend = doc.toJson();
    qDebug() <<"dataToSend" <<dataToSend;
    emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);
    query.clear();
}

void MyTcpServer::infRegHandle(QString messagetype, QString registertype, QString information, qintptr handle, QString messageType) {
    QSqlQuery query;
    QJsonArray jsonArray;

    if( "myInformation" == messageType) {
        query.prepare(QString("SELECT * FROM %1 WHERE %2 = '%3'")
                      .arg(messagetype)
                      .arg(registertype)
                      .arg(information));
        qDebug() << "query" << query.lastQuery();}
    else if ("studentInformation" == messageType)   {
        if ("" != information)  {
            query.prepare(QString("SELECT * FROM student WHERE `%1` = :value ORDER BY class ASC, username ASC").arg(registertype));
            query.bindValue(":value", information);
        }   else {
            query.prepare(QString("SELECT * FROM student ORDER BY class ASC, username ASC").arg(registertype));
        }}
    else if ("questionInformation" == messageType)  {
        query.prepare("SELECT DISTINCT title, questionnaire_id FROM Questionnaire");
    }
    else if ("questionnairePre" == messageType || "questionnairState" == messageType) {
        qDebug() << "Questionnaire ID:" << information;
        query.prepare("SELECT 'MultipleChoice' AS question_type, mc.question_id, mc.content AS question, "
                      "CONCAT(mc.option_a, ';', mc.option_b, ';', mc.option_c, ';', mc.option_d) AS options, mc.answer AS answer, qq.qq_id AS qq_id "
                      "FROM QuestionnaireQuestion qq "
                      "JOIN MultipleChoice mc ON mc.question_id = qq.question_id "
                      "WHERE qq.question_type = 'MultipleChoice' AND qq.questionnaire_id = :questionnaire_id "
                      "UNION ALL "
                      "SELECT 'FillInTheBlank' AS question_type, fb.question_id, fb.content AS question, NULL AS options, fb.answer AS answer, qq.qq_id AS qq_id "
                      "FROM QuestionnaireQuestion qq "
                      "JOIN FillInTheBlank fb ON fb.question_id = qq.question_id "
                      "WHERE qq.question_type = 'FillInTheBlank' AND qq.questionnaire_id = :questionnaire_id "
                      "UNION ALL "
                      "SELECT 'TrueOrFalse' AS question_type, tf.question_id, tf.content AS question, NULL AS options, IF(tf.answer, '正确', '错误') AS answer, qq.qq_id AS qq_id "
                      "FROM QuestionnaireQuestion qq "
                      "JOIN TrueOrFalse tf ON tf.question_id = qq.question_id "
                      "WHERE qq.question_type = 'TrueOrFalse' AND qq.questionnaire_id = :questionnaire_id "
                      "UNION ALL "
                      "SELECT 'Discussion' AS question_type, d.question_id, d.content AS question, NULL AS options, NULL AS answer, qq.qq_id AS qq_id "
                      "FROM QuestionnaireQuestion qq "
                      "JOIN Discussion d ON d.question_id = qq.question_id "
                      "WHERE qq.question_type = 'Discussion' AND qq.questionnaire_id = :questionnaire_id");
        query.bindValue(":questionnaire_id", information);

    }
    else if ("questionBank" == messageType)    {
        if ("" == messagetype || "类型选择" == messagetype) {
            query.prepare("SELECT 'MultipleChoice' AS question_type, question_id, content AS question, "
                          "CONCAT(option_a, ';', option_b, ';', option_c, ';', option_d) AS options, answer AS answer "
                          "FROM MultipleChoice "
                          "UNION ALL "
                          "SELECT 'FillInTheBlank' AS question_type, question_id, content AS question, NULL AS options, answer AS answer "
                          "FROM FillInTheBlank "
                          "UNION ALL "
                          "SELECT 'TrueOrFalse' AS question_type, question_id, content AS question, NULL AS options, IF(answer, '正确', '错误') AS answer "
                          "FROM TrueOrFalse "
                          "UNION ALL "
                          "SELECT 'Discussion' AS question_type, question_id, content AS question, NULL AS options, NULL AS answer "
                          "FROM Discussion ");
        }   else if ("选择题" == messagetype) {
            query.prepare("SELECT 'MultipleChoice' AS question_type, question_id, content AS question, "
                          "CONCAT(option_a, ';', option_b, ';', option_c, ';', option_d) AS options, answer AS answer "
                          "FROM MultipleChoice ");
        }   else if ("填空题" == messagetype) {
            query.prepare("SELECT 'FillInTheBlank' AS question_type, question_id, content AS question, NULL AS options, answer AS answer "
                          "FROM FillInTheBlank ");
        }   else if ("判断题" == messagetype) {
            query.prepare("SELECT 'TrueOrFalse' AS question_type, question_id, content AS question, NULL AS options, IF(answer, '正确', '错误') AS answer "
                          "FROM TrueOrFalse ");
        }   else if ("讨论题" == messagetype) {
            query.prepare("SELECT 'Discussion' AS question_type, question_id, content AS question, NULL AS options, NULL AS answer "
                          "FROM Discussion ");
        }
    }
    else if ("studentQuestionnair" == messageType)  {
        query.prepare("SELECT DISTINCT "
                      "qn.questionnaire_id, "
                      "qn.title, "
                      "CAST(NOT EXISTS (SELECT 1 FROM StudentAnswer sa "
                      "JOIN QuestionnaireQuestion qq ON sa.qq_id = qq.qq_id "
                      "WHERE sa.student_username = :student_username "
                      "AND qq.questionnaire_id = qn.questionnaire_id "
                      "AND sa.answer IS NULL) AS UNSIGNED) AS completed "
                      "FROM "
                      "StudentAnswer sa "
                      "JOIN QuestionnaireQuestion qq ON sa.qq_id = qq.qq_id "
                      "JOIN Questionnaire qn ON qq.questionnaire_id = qn.questionnaire_id "
                      "WHERE "
                      "sa.student_username = :student_username");
        query.bindValue(":student_username", information);
    }
    else if ("historyQuestionnair" == messageType) {
        // 获取 teacher_username
        QThread *thread = socketThreadHash.value(handle);
        QString teacher_username = userThreadHash.value(thread);
        qDebug() << "teacher_username" << teacher_username;
        query.prepare("SELECT qn.questionnaire_id, qn.title, sa.student_username, st.name AS student_name, qq.question_type, qq.question_id, sa.answer, "
                       "CASE "
                           "WHEN qq.question_type = 'MultipleChoice' THEN mc.content "
                           "WHEN qq.question_type = 'FillInTheBlank' THEN fib.content "
                           "WHEN qq.question_type = 'TrueOrFalse' THEN tof.content "
                           "WHEN qq.question_type = 'Discussion' THEN dis.content "
                           "ELSE NULL "
                       "END AS content "
                       "FROM Questionnaire qn "
                       "JOIN QuestionnaireQuestion qq ON qn.questionnaire_id = qq.questionnaire_id "
                       "JOIN StudentAnswer sa ON qq.qq_id = sa.qq_id "
                       "JOIN Student st ON sa.student_username = st.username "
                       "LEFT JOIN MultipleChoice mc ON qq.question_type = 'MultipleChoice' AND qq.question_id = mc.question_id "
                       "LEFT JOIN FillInTheBlank fib ON qq.question_type = 'FillInTheBlank' AND qq.question_id = fib.question_id "
                       "LEFT JOIN TrueOrFalse tof ON qq.question_type = 'TrueOrFalse' AND qq.question_id = tof.question_id "
                       "LEFT JOIN Discussion dis ON qq.question_type = 'Discussion' AND qq.question_id = dis.question_id "
                       "WHERE qn.teacher_username = :teacher_username "
                       "ORDER BY qn.questionnaire_id, sa.student_username, qq.question_type, qq.question_id;");

        query.bindValue(":teacher_username", teacher_username);
    }

    if (query.exec())   {
        int rowCount = 0;
        while (query.next()) {
            rowCount++;
            QJsonObject rowObject;
            for (int i = 0; i < query.record().count(); ++i) {
                rowObject[query.record().fieldName(i)] = QJsonValue::fromVariant(query.value(i));
            }
            jsonArray.append(rowObject);
            qDebug() << "Appending row to jsonArray: " << rowObject; // 添加此行以打印当前行
        }
        qDebug() << "Total rows fetched: " << rowCount;
        query.clear();
        bool state = true;
        QJsonObject messageObject = MyJSON::sqlRegisterBack(state, jsonArray, messageType);
        QJsonDocument doc(messageObject);
        QByteArray dataToSend = doc.toJson();
        qDebug() <<"dataToSend" <<dataToSend << "size:" << dataToSend.size();
        emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);
    }
    else { // 查询失败
        qDebug() << "Query error:" << query.lastError().text();
        bool state = false;
        QString errorText = "查询失败原因:" + query.lastError().text();
        QJsonObject messageObject = MyJSON::sqlRegisterBack(state, errorText, messageType);
        QJsonDocument doc(messageObject);
        QByteArray dataToSend = doc.toJson();
        emit threadSqlHash[socketThreadHash[handle]]->sendSiganl(dataToSend);
    }
}

void MyTcpServer::onlineHandle(qintptr handle)   {
    QThread *thread = socketThreadHash.value(handle); // 根据 handle 获取线程
        if (thread != nullptr)
        {
            QString username = userThreadHash.value(thread); // 根据线程获取用户名
            if (!username.isEmpty())
            {
                userOnlineHash.insert(username, false); // 更新用户在线状态
                qDebug() << "User" << username << "is now offline.";
            }
        }
    qDebug() <<"注销成功";
    database::quitConnection();
}


