#include "client.h"

Client::Client(QString namePort, unsigned int baud, QString path, QObject *parent) : QThread(parent)
{
    this->namePort = namePort;
    this->baud = baud;
    this->path = path;
}

//настройка порта
int Client::setSettings()
{
    serial.setBaudRate(baud);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    return 1;
}

int Client::translate()
{
    ParseBmp parser;
    if (!parser.open(path.toStdString().c_str()))
    {
        clientError("File '" + path.toStdString() + "' not found!");
        return 0;
    }


    serial.write( parser.getWH()); //отправка на сервер данных о ширине и высоте изображения
    serial.waitForBytesWritten(-1);
    serial.flush();

    //ожидании от сервера об успешном принятии данных 1 сек
    if (!serial.waitForReadyRead(1000))
    {
        clientError("Client time out.");
        return 0;
    }
    else
        serial.readAll();

    //получаем высоту изображения (!)
    unsigned int h = parser.getWH().split(' ')[1].toInt();

    for (unsigned int i = 0; i < h; i++)
    {
        //отправка на сервер данных о i-той строке
        serial.write( parser.getLine());
        serial.waitForBytesWritten(-1);
        serial.flush();

        //если данные не целые, завершается передача
        if (serial.waitForReadyRead(-1))
            if (serial.readAll().toStdString() != "OK")
                break;
    }

    parser.close();

    return 1;
}

void Client::run()
{
    serial.setPortName(namePort);

    if(serial.open(QIODevice::ReadWrite))
    {
        clientInfo("Port " + namePort.toStdString() + " is open.");

        if (setSettings())
        {
            clientInfo("Settings for " + namePort.toStdString() + " set sucsessful.");
            translate();
        } else
            clientError("Failed to set settings for " + namePort.toStdString() + " port.");

        serial.close();
        clientInfo("Port " + namePort.toStdString() + " is closed.");

    }else
        clientError("Port " + namePort.toStdString() + " not open.");
}

void Client::clientError(string e)
{
    emit error(QString::fromStdString(e));
    e = "ERROR: " + e;
    emit info(QString::fromStdString(e));
    qDebug() << QString::fromStdString(e);
}

void Client::clientInfo(string i)
{
    i = "INFO: " + i;
    emit info(QString::fromStdString(i));
    qDebug() << QString::fromStdString(i);
}
