#pragma once
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class MyJSON {
public:
    static QJsonObject createLogin(const QString& usertype, const QString& username, const QString& password);
    static void parseLogin(const QJsonObject& jsonObject, QString& usertype, QString& username, QString& password);

    static QJsonObject createRegister(const QString& usertype, const QString& username, const QString& password);
    static void parseRegister(const QJsonObject& jsonObject, QString& usertype, QString& username, QString& password);

    static QJsonObject loginOrRegisterBack(const bool state, const QString& text);
    static QByteArray loginOrRegisterBackToByteArray(bool state, const QString& userdata);
    static void parseBack(const QJsonObject& jsonObject, bool& state, QString& text);


    static QJsonObject informationRegister(const QString& messagetype, const QString& registertype, const QString& information, const QString& messageType);    //信息查询
    static void parseInformation(const QJsonObject& jsonObject, QString& messagetype, QString& registertype, QString& information);

    static QJsonObject sqlRegisterBack(const bool state, const QString& text, const QString& messageType);	//失败
    static QJsonObject sqlRegisterBack(const bool state, const QJsonArray& text, const QString& messageType);	//成功
    static void parseSqlRegisterBack(const QJsonObject& jsonObject, bool& state, QString& text, QString& messageType);
    static void parseSqlRegisterBack(const QJsonObject& jsonObject, bool& state, QJsonArray& text, QString& messageType);




};




