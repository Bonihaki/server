#include "HttpServer.h"
#include <QHttpServer>
#include <QJsonDocument>
#include <QJsonObject>
#include "dotenv.h" // Подключаем dotenv для загрузки конфигураций

HttpServer::HttpServer(QObject *parent) : QObject(parent), middleware(&sessionManager) {
    dotenv::init(".env"); // Загружаем переменные окружения из .env файла

    // Настройка хоста и порта сервера
    QString serverHost = QString::fromStdString(std::getenv("SERVER_HOST"));
    int serverPort = QString::fromStdString(std::getenv("SERVER_PORT")).toInt();
    
    QHttpServer *server = new QHttpServer(this);

    // Настройка маршрутов
    setupRoutes();

    // Обработка входящих запросов
    connect(server, &QHttpServer::newRequest, this, [=](QHttpRequest *req, QHttpResponse *res) {
        QString key = req->method() + " " + req->url().path();
        
        if (routeTable.contains(key)) {
            routeTable[key](req, res);  // Вызов соответствующего обработчика
        } else {
            res->writeHead(404);
            res->end("Not Found");
        }
    });

    // Запуск сервера
    if (server->listen(QHostAddress(serverHost), serverPort)) {
        qDebug() << "Сервер запущен на" << serverHost << "с портом" << serverPort;
    } else {
        qDebug() << "Не удалось запустить сервер";
    }
}
