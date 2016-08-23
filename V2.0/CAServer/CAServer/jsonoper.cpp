#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

////
/// \brief MainWindow::readJson
/// \param json JSON对象
/// \return true or false
/// 读取JSON文件
///
bool MainWindow::readJsonFile(QJsonObject &json)
{
    QString j_dir;
    j_dir = coredir + "signlist.json";
    QFile loadFile(j_dir);
    if (!loadFile.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QByteArray loaddata = loadFile.readAll();
    QJsonDocument readJsonDoc(QJsonDocument::fromJson(loaddata));
    json = readJsonDoc.object();
    loadFile.close();
    return true;
}

////
/// \brief MainWindow::saveJsonFile
/// \param json JSON对象
/// \return true or false
/// 写入信息至JSON文件
///
bool MainWindow::saveJsonFile(QJsonObject &json)
{
    QString j_dir;
    j_dir = coredir + "signlist.json";
    if(json.isEmpty())
    {
        return false;
    }
    QFile SaveFile(j_dir);
    if (!SaveFile.open(QIODevice::WriteOnly))
    {
        return false;
    }
    QJsonDocument SaveJsonDoc(json);
    SaveFile.write(SaveJsonDoc.toJson());
    SaveFile.close();
    return true;
}

////
/// \brief MainWindow::writeSerial2Json
/// \param serial 签名文件序列号
/// \return true or false
/// 将签发证书序列号写入JSON文件中
///
bool MainWindow::writeSerial2Json(const int &serial)
{
    QJsonObject addsign;
    addsign.insert("serialNumber",serial);
    addsign.insert("time",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    addsign.insert("status",false);
    QJsonArray array = jsignlist["signlist"].toArray();
    array.append(addsign);
    jsignlist["signlist"] = array;
    saveJsonFile(jsignlist);
    return true;
}
