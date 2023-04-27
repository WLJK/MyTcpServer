#pragma once

#include <QReadWriteLock>
#include <QObject>
#include <QTcpServer>
#include "sqlbusiness.h"
#include "mytcpsocket.h"


class MyTcpServer  : public QTcpServer
{
	Q_OBJECT

public:
	explicit MyTcpServer(QObject* parent = nullptr);
	void incomingConnection(qintptr handle);
	~MyTcpServer();
	enum USERTYPE {
		WHAT = -1,
		ROOT = 0,
		TEACHER = 1,
		STUDENT = 2
	};
    USERTYPE getUSERTYPE() const { return m_userType; }

signals:
    void clientDisconnected(qintptr handle);

protected:
    QHash<QString, bool> userOnlineHash;	//在线
	QHash<qintptr, QThread*> socketThreadHash;	//套接字对应线程
    QHash<QThread*, QString> userThreadHash;    //线程用户
	QHash<QThread*, sqlbusiness*> threadSqlHash;	//每个线程对应sql实例
protected slots:
    void onlineHandle(qintptr handle);
	void loginHandle(QString usertype, QString username, QString password, qintptr handle);
    void infRegHandle(QString tabletype, QString registertype, QString information, qintptr handle, QString messageType);
    void questionInsertHandle(QString messagetype, QString registertype, QString information, qintptr handle, QString messageType);
    void qqInsertHandle(QString messagetype, QString registertype, QString information, qintptr handle, QString messageType);
    void studentAnswerHandle(QString messagetype, QString registertype, QString information, qintptr handle, QString messageType);
    void questionDelectHandle(QString messagetype, QString registertype, QString information, qintptr handle, QString messageType);
    void questionUpdateHandle(QString tableName, QString fieldNames, QString fieldValues, qintptr handle, QString messageType);
    void delectHandle(QString messagetype, QString registertype, QString information, qintptr handle, QString messageType);
    void mulTableInsertHandle(QString value1, QString value2, QString value3, qintptr handle, QString messageType);
private:
	USERTYPE m_userType;
    QReadWriteLock dbLock;
};
