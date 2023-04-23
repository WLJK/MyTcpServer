#include "sqlbusiness.h"
#include "mytcpsocket.h"


sqlbusiness::sqlbusiness(QObject *parent)
	: QObject(parent)
{}

void sqlbusiness::mainBusiness(qintptr handle) {
	mytcpsocket* tcpSocket = new mytcpsocket(this, handle);
	connect(tcpSocket, &mytcpsocket::loginSiganl, this, &sqlbusiness::socketLoginHandle);
    connect(tcpSocket, &mytcpsocket::infRegSiganl, this, &sqlbusiness::socketInfRegHandle);
    connect(tcpSocket, &mytcpsocket::questionInsertSiganl, this, &sqlbusiness::socketQuestionInsertHandle);
    connect(tcpSocket, &mytcpsocket::studentAnswerSiganl, this, &sqlbusiness::socketStudentAnswerHandle);
    connect(tcpSocket, &mytcpsocket::qqInsertSiganl, this, &sqlbusiness::socketQqInsertHandle);
    connect(tcpSocket, &mytcpsocket::questionDelectSiganl, this, &sqlbusiness::socketQuestionDelectHandle);
    connect(tcpSocket, &mytcpsocket::questionUpdateSiganl, this, &sqlbusiness::socketquestionUpdateHandle);
    connect(tcpSocket, &mytcpsocket::delectSiganl, this, &sqlbusiness::socketDelectHandle);
    connect(tcpSocket, &mytcpsocket::mulTableInsertSiganl, this, &sqlbusiness::socketMulTableInsertHandle);
	connect(this, &sqlbusiness::sendSiganl, tcpSocket, &mytcpsocket::sendHandle);	//发送函数
    connect(tcpSocket, &mytcpsocket::onlineSiganl, this, &sqlbusiness::socketOnlineHandle);
	qDebug() << "进入专属线程";
	tcpSocket->setSocketDescriptor(handle);
}

void sqlbusiness::socketLoginHandle(QString usertype, QString username, QString password, qintptr handle)
{
	emit loginSiganl(usertype, username, password, handle);
}

void sqlbusiness::socketQqInsertHandle(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType)
{
    emit qqInsertSiganl(messagetype, fieldNames, fieldValues, handle, messageType);
}

void sqlbusiness::socketStudentAnswerHandle(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType)
{
    emit studentAnswerSiganl(messagetype, fieldNames, fieldValues, handle, messageType);
}

void sqlbusiness::socketQuestionInsertHandle(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType)
{
    qDebug() << "emit questionInsertSiganl" ;
    emit questionInsertSiganl(messagetype, fieldNames, fieldValues, handle, messageType);
}

void sqlbusiness::socketquestionUpdateHandle(QString tableName, QString fieldNames, QString fieldValues, qintptr handle, QString messageType)
{
    emit questionUpdateSiganl(tableName, fieldNames, fieldValues, handle, messageType);
}

void sqlbusiness::socketQuestionDelectHandle(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType)
{
    emit questionDelectSiganl(messagetype, fieldNames, fieldValues, handle, messageType);
}

void sqlbusiness::socketDelectHandle(QString messagetype, QString fieldNames, QString fieldValues, qintptr handle, QString messageType)
{
    emit delectSiganl(messagetype, fieldNames, fieldValues, handle, messageType);
}

void sqlbusiness::socketInfRegHandle(QString messagetype, QString registertype, QString information, qintptr handle, QString messageType)
{
    emit infRegSiganl(messagetype, registertype, information, handle, messageType);
}

void sqlbusiness::socketMulTableInsertHandle(QString messagetype, QString registertype, QString information, qintptr handle, QString messageType)
{
    emit mulTableInsertSiganl(messagetype, registertype, information, handle, messageType);
}

void sqlbusiness::socketOnlineHandle(qintptr handle)
{
    emit onlineSiganl(handle);
}

sqlbusiness::~sqlbusiness()
{}
