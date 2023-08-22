#pragma once
#include "mainwindow.h"
#include "qhttpserver.h"
#include "qhttpconnection.h"
#include "qhttprequest.h"
#include "qhttpresponse.h"

#include <QScopedPointer>

class HttpServerThread : public QObject
{
    Q_OBJECT
public:
    HttpServerThread(QWidget* p_main_widget);
    ~HttpServerThread();

private slots:
    void onRequestReceived(QHttpRequest *req, QHttpResponse *res);

private:
    MainWindow* m_main_widget   = nullptr;

    QHttpServer *server         = nullptr;

};

/// Responder

class Responder : public QObject
{
    Q_OBJECT

public:
    Responder(QHttpRequest *req, QHttpResponse *res);
    ~Responder();

signals:
    void done();

private slots:
    void accumulate(const QByteArray &data);
    void reply();

private:
    QScopedPointer<QHttpRequest> m_req;
    QHttpResponse *m_res;
};

