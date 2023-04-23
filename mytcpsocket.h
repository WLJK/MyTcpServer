#pragma once
#include <QObject>
#include <QTcpSocket>
#include "MyJSON.h"

class mytcpsocket : public QTcpSocket
{
	Q_OBJECT

public:
	explicit mytcpsocket(QObject* parent = nullptr, qintptr handle = NULL);
	~mytcpsocket();
protected:
	qintptr handle;
;
protected slots:
	void readyReadHandle();
	void disconnectedHandle();

public slots:
    void sendHandle(QByteArray toSend);	//发送

signals:
    void onlineSiganl(qintptr handle);
	void loginSiganl(QString usertype, QString username, QString password, qintptr handle);
    void infRegSiganl(QString messagetype, QString registertype, QString information, qintptr handle, QString messageType);
    void questionInsertSiganl(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void qqInsertSiganl(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void studentAnswerSiganl(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void questionDelectSiganl(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void questionUpdateSiganl(QString tableName, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void delectSiganl(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void mulTableInsertSiganl(QString value1, QString value2, QString value3, qintptr handle, QString messageType);
private:
	
};



