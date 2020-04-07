#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    client = nullptr;
    server = nullptr;

    ui->setupUi(this);

    layout = new QVBoxLayout();
    contentWidget = new QWidget();
    contentWidget->setLayout(layout);
    ui->scrollArea->setWidget(contentWidget);
    ui->scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

    ui->pushButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    for (unsigned int i = 0; i < image.size(); i++)
        for (unsigned int j = 0; j < image[i].size(); j++)
        {
            painter.setPen(QColor(image[i][j].red,      //
                                  image[i][j].green,    // получение цвета i-того j-того пикскля
                                  image[i][j].blue));   //
            painter.drawPoint(650 + j, i);  //отрисовка i-того j-того пикскля
        }
}

//получение сигнала от сервера, что извображение получено и
//можно его отрисовывать
void MainWindow::onDrawImage()
{
    repaint();

    client->wait();
    server->wait();

    delete client;
    delete server;

    server = nullptr;
    client = nullptr;
}

//логирование
void MainWindow::onInfo(QString i)
{
    l.push_back(new QLabel(i));
    layout->addWidget(l.last());
    ui->scrollArea->verticalScrollBar()->setSliderPosition(ui->scrollArea->verticalScrollBar()->maximum());
}

void MainWindow::onError(QString e)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Error");
    msgBox.setText(e);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
}

//start button
void MainWindow::on_pushButton_clicked()
{
    clearLog();
    createObj(); //создание объектов клиента и сервера

    connect(server, &Server::drawImage, this, &MainWindow::onDrawImage);
    connect(server, &Server::info, this, &MainWindow::onInfo);
    connect(client, &Client::info, this, &MainWindow::onInfo);
    connect(server, &Server::error, this, &MainWindow::onError);
    connect(client, &Client::error, this, &MainWindow::onError);

    server->start();
    QThread::msleep(100);
    client->start();
}

void MainWindow::clearLog()
{
    for (int i = 0; i < l.size(); i++)
        delete l[i];
    l.clear();
}

//создание объектов клиана и сервера
void MainWindow::createObj()
{
    QString namePort1 = ui->lineEdit->text();   //информация о порте 1
    QString namePort2 = ui->lineEdit_2->text(); //информация о порте 2
    u_int baud = ui->lineEdit_3->text().toInt();//информация о скорости

    if (baud < 115200)
    {
        baud = 115200;
        ui->lineEdit_3->setText("115200");
        onInfo("ERROR: Baud should be greater than 115200.\nBaud set 115200.");
        onError("Baud should be greater than 115200.\nBaud set 115200.");
    }

    client = new Client(namePort1, baud, fileName, this);
    server = new Server(namePort2, baud, this);
}

//получение пути к изображению
void MainWindow::on_pushButton_2_clicked()
{
    fileName = QFileDialog::getOpenFileName(0, QObject::tr("Open File"),
                                                    ".\\",
                                                    QObject::tr("Images (*.bmp)"));
    QPixmap pm;
    if (fileName != "")
    {
        pm.load(fileName);
        if ((pm.width() <= 640) && (pm.width() >= 32) && (pm.height() <= 480) && (pm.height() >= 32))
        {
            ui->label_5->setPixmap(pm);
            ui->label_5->setAlignment(Qt::AlignTop);

            ui->label_4->setText(fileName);
            ui->pushButton->setEnabled(true);
        } else
        {
            fileName = ui->label_4->text();
            onInfo("ERROR: Image size should be less than 640x480");
            onError("Image size should be less than 640x480");
        }
    } else
    {
        if (ui->label_4->text() != "")
        {
            ui->pushButton->setEnabled(true);
            fileName = ui->label_4->text();
            pm.load(fileName);
        }
    }
}
