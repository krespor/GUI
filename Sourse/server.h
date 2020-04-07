#ifndef SERVER_H
#define SERVER_H

#include <QThread>
#include <QDebug>
#include <QSerialPort>

#include <iostream>
#include <vector>

using namespace std;

struct Pixel
{
    unsigned int red;
    unsigned int green;
    unsigned int blue;
};

inline vector<vector<Pixel>> image;

class Server : public QThread
{
    Q_OBJECT
public:
    explicit Server(QString namePort, uint32_t baud, QObject *parent = 0);


private:
    QSerialPort serial;

    QString namePort;
    uint32_t baud;

    uint32_t height;
    uint32_t width;

    int setSettings();
    int translate();
    void run() override;
    void serverError(string e);
    void serverInfo(string i);

signals:
    void drawImage();
    void info(QString i);
    void error(QString e);
};

#endif // SERVER_H
