#include "MyJSON.h"

QJsonObject MyJSON::createLogin(const QString& usertype, const QString& username, const QString& password) {
    QJsonObject dataObject{
        {"usertype", usertype},
        {"username", username},
        {"password", password}
    };

    QJsonObject messageObject{
        {"sql_type", "login"},
        {"data", dataObject}
    };

    return messageObject;
}
void MyJSON::parseLogin(const QJsonObject& jsonObject, QString& usertype, QString& username, QString& password) {
    QJsonObject dataObject = jsonObject["data"].toObject();
    usertype = dataObject["usertype"].toString();
    username = dataObject["username"].toString();
    password = dataObject["password"].toString();
}

QJsonObject MyJSON::loginOrRegisterBack(const bool state, const QString& text) {
    QJsonObject dataObject{
        {"state", state},
        {"text", text}
    };
    QJsonObject messageObject{
        {"sql_type", "loginOrRegisterBack"},
        {"data", dataObject}
    };
    return messageObject;
}
QByteArray MyJSON::loginOrRegisterBackToByteArray(bool state, const QString& userdata) {
    QJsonObject backtext = loginOrRegisterBack(state, userdata);
    QJsonDocument doc(backtext);
    return doc.toJson();
}
void MyJSON::parseBack(const QJsonObject& jsonObject, bool& state, QString& text) {
    QJsonObject dataObject = jsonObject["data"].toObject();
    state = dataObject["state"].toBool();
    text = dataObject["text"].toString();
}


QJsonObject MyJSON::createRegister(const QString& usertype, const QString& username, const QString& password) {
    QJsonObject dataObject{
        {"usertype", usertype},
        {"username", username},
        {"password", password}
    };

    QJsonObject messageObject{
        {"sql_type", "register"},
        {"data", dataObject}
    };

    return messageObject;
}
void MyJSON::parseRegister(const QJsonObject& jsonObject, QString& usertype, QString& username, QString& password) {
    QJsonObject dataObject = jsonObject["data"].toObject();
    usertype = dataObject["usertype"].toString();
    username = dataObject["username"].toString();
    password = dataObject["password"].toString();
}


QJsonObject MyJSON::informationRegister(const QString& messagetype, const QString& registertype, const QString& information, const QString& messageType) {
    QJsonObject dataObject{
        {"messagetype", messagetype},
        {"registertype", registertype},
        {"information", information},
    };
    QJsonObject messageObject{
        {"sql_type", "select"},
        {"message_type", messageType},
        {"data", dataObject}
    };
    return messageObject;
}

void MyJSON::parseInformation(const QJsonObject& jsonObject, QString& messagetype, QString& registertype, QString& information) {
    QJsonObject dataObject = jsonObject["data"].toObject();
    messagetype = dataObject["messagetype"].toString();
    registertype = dataObject["registertype"].toString();
    information = dataObject["information"].toString();
}

QJsonObject MyJSON::sqlRegisterBack(const bool state, const QString& text, const QString& messageType) {
    QJsonObject dataObject{
       {"state", state},    //查询失败
       {"errorText", text}  //返回失败原因
    };
    QJsonObject messageObject{
        {"sql_type", "sqlRegisterBack"},
        {"message_type", messageType},
        {"data", dataObject}
    };
    return messageObject;
}
QJsonObject MyJSON::sqlRegisterBack(const bool state, const QJsonArray& jsonArray, const QString& messageType) {
    QJsonObject dataObject{
       {"state", state},    //查询成功
       {"infoArray", jsonArray}  //返回结果
    };
    QJsonObject messageObject{
        {"sql_type", "sqlRegisterBack"},
        {"message_type", messageType},
        {"data", dataObject}
    };
    return messageObject;
}
void MyJSON::parseSqlRegisterBack(const QJsonObject& jsonObject, bool& state, QString& text, QString& messageType) {
    QJsonObject dataObject = jsonObject["data"].toObject();
    state = dataObject["state"].toBool();
    text = dataObject["text"].toString();
}
void MyJSON::parseSqlRegisterBack(const QJsonObject& jsonObject, bool& state, QJsonArray& infoArray, QString& messageType) {
    QJsonObject dataObject = jsonObject["data"].toObject();
    state = dataObject["state"].toBool();
    infoArray = dataObject["infoArray"].toArray();
}
