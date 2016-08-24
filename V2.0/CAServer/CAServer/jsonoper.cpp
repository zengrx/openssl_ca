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

////
/// \brief MainWindow::writeStatus2Json
/// \param json JSON对象
/// \param flag 更新证书状态标识 1-填充撤销 2-填充恢复
/// \return true or false
///
bool MainWindow::writeStatus2Json(int flag, QString serial)
{
    readJsonFile(jsignlist);
    if(jsignlist.isEmpty())
    {
        qDebug()<<"json file is empty";
        return false;
    }
    readJsonFile(jsignlist);
    QJsonArray array = jsignlist["signlist"].toArray();
    if(flag == 3)
    {
        //update listwidget2
        qDebug() << "index at lw1 for lightrevoke" << indexptr1;
        QJsonObject objson = array[indexptr1].toObject();
        certop.ser = QString::number(objson["serialNumber"].toInt());
        //qDebug() << "empty test early" << certop.ser;
        if(revokeCert())
        {
            objson["status"] = true;
            array[indexptr1] = objson;
            jsignlist["signlist"] = array;
            saveJsonFile(jsignlist);
            updateListWidget();
            showCrlInfo();
            //certop.ser = "";
            //qDebug() << "empty test late" << certop.ser;
            ui->pushButton_3->setEnabled(false);
            return true;
        }
    }
    else
    {
        for(int i=0;i<array.size();i++)
        {
            QJsonObject objson = array[i].toObject();
            if(flag == 1)
            {
                if(objson["serialNumber"]==certop.ser.toInt())
                {
                    objson["status"] = true;
                    array[i] = objson;
                    jsignlist["signlist"] = array;
                    saveJsonFile(jsignlist);
                }
            }
            else if(flag == 2)
            {
                if(objson["serialNumber"]==serial.toInt())
                {
                    objson["status"] = false;
                    array[i] = objson;
                    jsignlist["signlist"] = array;
                    saveJsonFile(jsignlist);
                }
            }
            else
            {
                qDebug() << "wrong arg2 set";
                return false;
            }
        }
        updateListWidget();
        return true;
    }
}

