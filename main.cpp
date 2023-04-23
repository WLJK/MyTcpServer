#include <QtCore/QCoreApplication>
#include "mytcpserver.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MyTcpServer* tcpServer = new MyTcpServer();
    tcpServer->listen(QHostAddress::Any, quint16(10626));
    qDebug() << "开始监听端口： 10626";
    return a.exec();
}
