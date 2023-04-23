#include "mytcpsocket.h"
#include "MyTcpServer.h"

mytcpsocket::mytcpsocket(QObject *parent, qintptr handle)
	: QTcpSocket(parent)
{
	this->handle = handle;
    qDebug() << "传入socket" ;
	connect(this, &mytcpsocket::readyRead, this, &mytcpsocket::readyReadHandle);
	connect(this, &mytcpsocket::disconnected, this, &mytcpsocket::disconnectedHandle);
}

mytcpsocket::~mytcpsocket()
{}

void mytcpsocket::readyReadHandle() {
    QByteArray data = this->readAll();
    qDebug() << "进入readyRead"<< data;
    // 将字节数组转换为 JSON 文档
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    // 解析 JSON 文档
    //qDebug() << "开始解析";
    QJsonObject jsonObj = jsonDoc.object();
    QString sqlType = jsonObj["sql_type"].toString();
    qDebug() << "sqlType" << sqlType;
    if ("login" == sqlType) {
        QString usertype, username, password;
        MyJSON::parseLogin(jsonObj, usertype, username, password);
        // 处理登录逻辑
        qDebug() << "usertype" << usertype << "username" << username << "password" << password;
        emit loginSiganl(usertype, username, password, handle);
    }
    else if ("register" == sqlType) {

    }
    else if ("select" == sqlType) {
        QString messagetype, registertype, information;
        QString messageType = jsonObj["message_type"].toString();
        MyJSON::parseInformation(jsonObj, messagetype, registertype, information);
        qDebug() << "registertype"<< registertype;
        emit infRegSiganl(messagetype, registertype, information, handle, messageType);
    }   else if ("insert" == sqlType)   {
        QString messagetype, fieldNames, fieldValues;
        QString messageType = jsonObj["message_type"].toString();
        MyJSON::parseInformation(jsonObj, messagetype, fieldNames, fieldValues);
        emit questionInsertSiganl(messagetype, fieldNames, fieldValues, handle, messageType);
    }   else if ("mulTableInsert" == sqlType)   {
        QString value1, value2, value3;
        QString messageType = jsonObj["message_type"].toString();
        MyJSON::parseInformation(jsonObj, value1, value2, value3);
        emit mulTableInsertSiganl(value1, value2, value3, handle, messageType);
    }   else if ("delectQuestion" == sqlType)   {
        QString messagetype, fieldNames, fieldValues;
        QString messageType = jsonObj["message_type"].toString();
        MyJSON::parseInformation(jsonObj, messagetype, fieldNames, fieldValues);
        emit questionDelectSiganl(messagetype, fieldNames, fieldValues, handle, messageType);
    }   else if ("delect" == sqlType)   {
        QString messagetype, fieldNames, fieldValues;
        QString messageType = jsonObj["message_type"].toString();
        MyJSON::parseInformation(jsonObj, messagetype, fieldNames, fieldValues);
        emit delectSiganl(messagetype, fieldNames, fieldValues, handle, messageType);
    }   else if ("update" == sqlType)   {
        QString tableName, fieldNames, fieldValues;
        QString messageType = jsonObj["message_type"].toString();
        MyJSON::parseInformation(jsonObj, tableName, fieldNames, fieldValues);
        qDebug() << "emit updateSiganl";
        emit questionUpdateSiganl(tableName, fieldNames, fieldValues, handle, messageType);
    }   else if ("qqInsert" == sqlType) {
        QString tableName, fieldNames, fieldValues;
        QString messageType = jsonObj["message_type"].toString();
        MyJSON::parseInformation(jsonObj, tableName, fieldNames, fieldValues);
        qDebug() << "emit qqInsertSiganl";
        emit qqInsertSiganl(tableName, fieldNames, fieldValues, handle, messageType);
    }   else if ("studentAnswer" == sqlType) {
        QString tableName, fieldNames, fieldValues;
        QString messageType = jsonObj["message_type"].toString();
        MyJSON::parseInformation(jsonObj, tableName, fieldNames, fieldValues);
        qDebug() << "emit studentAnswerSiganl";
        emit studentAnswerSiganl(tableName, fieldNames, fieldValues, handle, messageType);
    }
}

void mytcpsocket::disconnectedHandle()
{
    emit onlineSiganl(handle);
    qDebug() << "disconnectedHandle";
}

void mytcpsocket::sendHandle(QByteArray toSend)
{
    qDebug() <<"发送数据";
    this->write(toSend);
    this->waitForBytesWritten(-1);  //阻塞发送
}
