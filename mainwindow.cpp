#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "httpserverthread.h"

#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->textBrowser->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    static bool isClicked = false;
    if(isClicked) return;
    isClicked = true;

    HttpServerThread* httpserver = new HttpServerThread(this);
    QThread *m_thread = new QThread();
    httpserver->moveToThread(m_thread);
    m_thread->start();

    connect(this, SIGNAL(showRequestData_signal(const QByteArray&)), this, SLOT(showRequestData_slot(const QByteArray&)));

}

void MainWindow::showRequestData_slot(const QByteArray& data)
{
    QString strdata = QString::fromStdString(data.toStdString());
    ui->textBrowser->setText(strdata);
}
