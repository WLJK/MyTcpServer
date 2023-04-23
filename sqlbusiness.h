#pragma once
#include "MyJSON.h"
#include <QObject>

class sqlbusiness  : public QObject
{
	Q_OBJECT

public:
	explicit sqlbusiness(QObject *parent = nullptr);
	void mainBusiness(qintptr handle);
	~sqlbusiness();

protected slots:
	void socketLoginHandle(QString usertype, QString username, QString password, qintptr handle);
    void socketInfRegHandle(QString messagetype, QString registertype, QString information, qintptr handle, QString messageType);
    void socketQuestionInsertHandle(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void socketQqInsertHandle(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void socketStudentAnswerHandle(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void socketQuestionDelectHandle(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void socketquestionUpdateHandle(QString tableName, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void socketDelectHandle(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void socketMulTableInsertHandle(QString value1, QString value2, QString value3, qintptr handle, QString messageType);
    void socketOnlineHandle(qintptr handle);
signals:
	void sendSiganl(QByteArray toSend);
	void start(qintptr handle);
	void loginSiganl(QString usertype, QString username, QString password, qintptr handle);
    void infRegSiganl(QString messagetype, QString registertype, QString information, qintptr handle, QString messageType);
    void questionInsertSiganl(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void qqInsertSiganl(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void studentAnswerSiganl(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void questionUpdateSiganl(QString tableName, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void questionDelectSiganl(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void delectSiganl(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void mulTableInsertSiganl(QString value1, QString value2, QString value3, qintptr handle, QString messageType);
    void onlineSiganl(qintptr handle);
};
