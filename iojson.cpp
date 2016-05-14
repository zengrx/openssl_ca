#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

bool MainWindow::ReadJson(QJsonObject &json)
{
    QFile loadFile("signlist.json");
    if (!loadFile.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QByteArray LoadData = loadFile.readAll();
    QJsonDocument ReadJsonDoc(QJsonDocument::fromJson(LoadData));
    json=ReadJsonDoc.object();
    loadFile.close();
    return true;
}

bool MainWindow::SaveJson(QJsonObject &json)
{
    if(json.isEmpty())
    {
        return false;
    }
    QFile SaveFile("signlist.json");
    if (!SaveFile.open(QIODevice::WriteOnly))
    {
        return false;
    }
    QJsonDocument SaveJsonDoc(json);
    SaveFile.write(SaveJsonDoc.toJson());
    SaveFile.close();
    return true;
}

bool MainWindow::WriteSerial2Json(const int &serial)
{
    QJsonObject addsign;
    addsign.insert("serialNumber",serial);
    addsign.insert("time",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    addsign.insert("stats",false);
    QJsonArray array = signlistjson["signlist"].toArray();
    array.append(addsign);
    signlistjson["signlist"]=array;
    SaveJson(signlistjson);
    return true;
}
