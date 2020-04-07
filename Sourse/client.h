#ifndef CLIENT_H
#define CLIENT_H

#include <QDebug>
#include <QSerialPort>
#include <QThread>

#include <iostream>

#include "parsebmp.h"

using namespace std;

class Client : public QThread
{
    Q_OBJECT
public:
    explicit Client(QString namePort, uint32_t baud, QString path, QObject *parent = 0);

private:
    QSerialPort serial;

    QString namePort;
    uint32_t baud;
    QString path;

    int setSettings();
    int translate();
    void run() override;

    void clientInfo(string i);
    void clientError(string e);
signals:
    void info(QString i);
    void error(QString e);

};

#endif // CLIENT_H
