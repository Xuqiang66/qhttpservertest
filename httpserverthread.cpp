#include "httpserverthread.h"

#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHostAddress>
#include <QDateTime>
#include <QFile>
#include <QRegularExpression>

HttpServerThread::HttpServerThread(QWidget* p_main_widget)
{
    m_main_widget = (MainWindow*)p_main_widget;

    server = new QHttpServer(this);
    connect(server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
        this, SLOT(onRequestReceived(QHttpRequest*, QHttpResponse*)));

    server->listen(QHostAddress::Any, 8080);
}

HttpServerThread::~HttpServerThread()
{

}

void HttpServerThread::onRequestReceived(QHttpRequest *req, QHttpResponse *res)
{
    new Responder(req, res);
}

/// Responder

Responder::Responder(QHttpRequest *req, QHttpResponse *res)
    : m_req(req)
    , m_res(res)
{
    qDebug()<<"m_req->path:"<<m_req->path();
    QRegularExpression regex("data/collection/inRadarInfo", QRegularExpression::CaseInsensitiveOption);
    if(!regex.match(m_req->path()).hasMatch())
    {
        qDebug()<<"req->path is not include data/collection/inRadarInfo";
        m_res->setHeader("Content-Type", "application/json");
        m_res->writeHead(401);
        QJsonObject responseJson;
        responseJson["responseCode"] = "401";
        responseJson["responseMsg"] = "Bad Request";
        m_res->end(QJsonDocument(responseJson).toJson());
        return;
    }

    if (m_req->method() != QHttpRequest::HTTP_POST) {
        m_res->setHeader("Content-Type", "application/json");
        m_res->writeHead(400);
        QJsonObject responseJson;
        responseJson["responseCode"] = "400";
        responseJson["responseMsg"] = "Bad Request";
        m_res->end(QJsonDocument(responseJson).toJson());
        return;
    }

    connect(req, SIGNAL(data(const QByteArray&)), this, SLOT(accumulate(const QByteArray&)));
    connect(req, SIGNAL(end()), this, SLOT(reply()));
    connect(m_res, SIGNAL(done()), this, SLOT(deleteLater()));

}

Responder::~Responder()
{

}

void Responder::accumulate(const QByteArray &data)
{
    qDebug() << "req receive data:" << QString::fromStdString(data.toStdString());
    QJsonDocument jsonDoc       = QJsonDocument::fromJson(data);
    if(!jsonDoc.isNull() && jsonDoc.isObject())
    {
        QJsonObject jsonData    = jsonDoc.object();
        QString frontDistance   = jsonData["frontDistance"].toString();
        QString backDistance    = jsonData["backDistance"].toString();
        QString leftDistance    = jsonData["leftDistance"].toString();
        QString rightDistance   = jsonData["rightDistance"].toString();
        qInfo()<<QString("frontDistance:%1, backDistance:%2, leftDistance:%3, rightDistance:%4")
                 .arg(frontDistance)
                 .arg(backDistance)
                 .arg(leftDistance)
                 .arg(rightDistance);

        QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        m_res->setHeader("Content-Type", "application/json");
        m_res->writeHead(200);
        QJsonObject responseJson;
        responseJson["responseCode"] = "200";
        responseJson["responseMsg"] = "操作成功";
        responseJson["responseDate"] = currentDateTime;
        responseJson["deviceId"] = "n00047";
        m_res->end(QJsonDocument(responseJson).toJson());
    }
    else
    {
        qDebug()<<"Json parse error";

        m_res->setHeader("Content-Type", "application/json");
        m_res->writeHead(500);
        QJsonObject responseJson;
        responseJson["responseCode"] = "500";
        responseJson["responseMsg"] = "服务器内部错误";
        m_res->end(QJsonDocument(responseJson).toJson());
    }
}

void Responder::reply()
{
    qInfo()<<"req receive data end";
}

