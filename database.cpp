#include "database.h"
#include <qsqldatabase.h>
#include <QSql>
#include <QSqlQuery>    //操作数据库
#include <QSqlError>
#include <QDebug>



database::database()
{
	qDebug() << "数据库实例已创建";
}

bool database::getConnection()
{
    //连接数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");  //创建mysql数据库连接
    db.setHostName("rm-cn-pe335t2ij0003p0o.rwlb.rds.aliyuncs.com");    //主机服务器
    db.setPort(3306);
    db.setUserName("root");
    db.setPassword("Xv@@1020");
    db.setDatabaseName("mydb");
    if (db.open())
    {
        qDebug() << u8"数据库连接成功";
        return true;
    }
    else {
        qDebug() << u8"数据库连接失败";
        qDebug() << db.lastError().text();  //输出错误信息
        return false;
    }
}

void database::quitConnection()
{
    QSqlDatabase::database().close();
}
