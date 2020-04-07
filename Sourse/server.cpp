#include "server.h"

Server::Server(QString namePort, unsigned int baud, QObject *parent) : QThread(parent)
{
    this->namePort = namePort;
    this->baud = baud;

    height = 0;
    width = 0;

    image.clear();
}

int Server::setSettings()
{
    //настройка порта
    serial.setBaudRate(baud);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    return 1;
}

int Server::translate()
{
    //ожидание  от клиента данных о ширине и высоте изображения 1 сек.
    if(serial.waitForReadyRead(1000))
    {
        QList<QByteArray> listWH = serial.readAll().split(' ');
        serial.write("OK");  //сообщает клиенту об успешном принятии данных
        serial.waitForBytesWritten(-1);
        serial.flush();

        width = listWH[0].toInt();
        height = listWH[1].toInt();
        image.resize(height);
        Pixel p;

        for (uint32_t i = 0; i < height; i++)
        {
            //ждёт от клиента данных о i-той строки
            if(serial.waitForReadyRead(-1))
            {
                QList<QByteArray> byteLine = serial.readAll().split(' ');

                //проверка на целостность данных
                if (uint32_t(byteLine.size()) != width * 3)
                {
                    //сообщение клиенту о нецелостности данных
                    serial.write("DONT OK");
                    serial.waitForBytesWritten(-1);
                    serial.flush();

                    serverError("Transmission lines " + to_string(i+1) + " of " + to_string(height) + ".\nExpected " + to_string(width * 3) + " instead of " + to_string(byteLine.size()));
                    return 0;
                } else
                {
                    //сообщение пользователю об успешной передачи i-той строки
                    serverInfo("Line " + to_string(i + 1) + " of " + to_string(height) + " received successfully.");
                }

                //сохранение строки изображения
                uint32_t k = 0;
                for (unsigned int j = 0; j < width; j++)
                {
                    //заполнение пикселя 'p'
                    p.red = byteLine[k++].toInt();
                    p.green = byteLine[k++].toInt();
                    p.blue = byteLine[k++].toInt();

                    image[height - i - 1].push_back(p); //добавление его с конца
                }

                //сообщение клиенту об успешном принятии строки
                serial.write("OK");
                serial.waitForBytesWritten(-1);
                serial.flush();
            }
        }
        //сигнал 'окну', что изображение получено, можно отрисовывать
        emit drawImage();
    } else
    {
        serverError("Server time out.");
        return 0;
    }
    return 1;
}

void Server::run()
{
    serial.setPortName(namePort);

    if(serial.open(QIODevice::ReadWrite))
    {
        serverInfo("Port " + namePort.toStdString() + " is open.");

        if (setSettings())
        {
            serverInfo("Settings for " + namePort.toStdString() + " set sucsessful.");
            translate();
        } else
            serverError("Failed to set settings for " + namePort.toStdString() + " port.");

        serial.close();
        serverInfo("Port " + namePort.toStdString() + " is closed.");

    }else
        serverError("Port " + namePort.toStdString() + " not open.");
}

void Server::serverError(string e)
{
    emit error(QString::fromStdString(e));
    e = "ERROR: " + e;
    emit info(QString::fromStdString(e));
    qDebug() << QString::fromStdString(e);
}

void Server::serverInfo(string i)
{
    i = "INFO: " + i;
    emit info(QString::fromStdString(i));
    qDebug() << QString::fromStdString(i);
}
