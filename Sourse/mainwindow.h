#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QPaintEvent>
#include <QtWidgets>
#include <QLabel>
#include <QScrollArea>
#include <QBoxLayout>
#include <QPixmap>
#include <QDial>

#include <iostream>

#include "client.h"
#include "server.h"

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    Client *client;
    Server *server;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void paintEvent(QPaintEvent *event);

//public slots:


private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void onDrawImage();
    void onInfo(QString i);
    void onError(QString e);

private:
    QString fileName;
    QList<QLabel*> l;
    QVBoxLayout* layout;
    QWidget* contentWidget;

    Ui::MainWindow *ui;

    void clearLog();
    void createObj();
};
#endif // MAINWINDOW_H
