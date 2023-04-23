#pragma once
#include <QSqlQuery>

class database
{
public:
	database();
	//数据库连接函数
	static bool getConnection();
	static void quitConnection();
private:
	QSqlDatabase db;		//定义变量db做数据库实例
};

