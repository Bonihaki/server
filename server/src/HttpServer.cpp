#include "HttpServer.h"
#include <QHttpServer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

HttpServer::HttpServer(QObject *parent) : QObject(parent), middleware(&sessionManager) {}

void HttpServer::start() {
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

    server->listen(QHostAddress::Any, 8080);
}

// Метод для настройки маршрутов
void HttpServer::setupRoutes() {
    routeTable["POST /register"] = std::bind(&HttpServer::handleRegistrationRequest, this, std::placeholders::_1, std::placeholders::_2);
    routeTable["POST /authenticate"] = std::bind(&HttpServer::handleAuthenticationRequest, this, std::placeholders::_1, std::placeholders::_2);
    routeTable["GET /users"] = std::bind(&HttpServer::handleGetUsersRequest, this, std::placeholders::_1, std::placeholders::_2);
    routeTable["PUT /users"] = std::bind(&HttpServer::handleUpdateUserRequest, this, std::placeholders::_1, std::placeholders::_2);
    routeTable["DELETE /users"] = std::bind(&HttpServer::handleDeleteUserRequest, this, std::placeholders::_1, std::placeholders::_2);
}

// Реализация обработчиков
void HttpServer::handleRegistrationRequest(QHttpRequest *req, QHttpResponse *res) {
    QByteArray body = req->body();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(body);
    QJsonObject json = jsonDoc.object();

    QString username = json["username"].toString();
    QString email = json["email"].toString();
    QString password = json["password"].toString();
    QString role = "user"; // По умолчанию у новых пользователей роль "user"

    if (registerUser(dbManager, username, password, email, role)) {
        QString token = sessionManager.generateToken(username);
        QDateTime expiresAt = QDateTime::currentDateTime().addDays(1);
        sessionManager.createSession(dbManager.getUser(username)["id"].toInt(), token, expiresAt);

        QJsonObject responseJson;
        responseJson["token"] = token;
        res->writeHead(200);
        res->end(QJsonDocument(responseJson).toJson());
    } else {
        res->writeHead(500);
        res->end("Error registering user");
    }
}

void HttpServer::handleAuthenticationRequest(QHttpRequest *req, QHttpResponse *res) {
    QByteArray body = req->body();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(body);
    QJsonObject json = jsonDoc.object();

    QString username = json["username"].toString();
    QString password = json["password"].toString();

    QString token = authenticateUser(sessionManager, dbManager, username, password);
    if (!token.isEmpty()) {
        QJsonObject responseJson;
        responseJson["token"] = token;
        res->writeHead(200);
        res->end(QJsonDocument(responseJson).toJson());
    } else {
        res->writeHead(401);
        res->end("Unauthorized");
    }
}

// Только для админа - получение пользователей
void HttpServer::handleGetUsersRequest(QHttpRequest *req, QHttpResponse *res) {
    int userId;
    QString role;
    if (!middleware.isAuthorized(req, userId, role) || role != "admin") {
        res->writeHead(403);
        res->end("Forbidden");
        return;
    }

    QJsonArray usersArray = dbManager.getAllUsers();
    QJsonDocument responseDoc(usersArray);
    res->writeHead(200);
    res->end(responseDoc.toJson());
}

void HttpServer::handleUpdateUserRequest(QHttpRequest *req, QHttpResponse *res) {
    int userId;
    QString role;
    if (!middleware.isAuthorized(req, userId, role) || role != "admin") {
        res->writeHead(403);
        res->end("Forbidden");
        return;
    }

    res->writeHead(200);
    res->end("User updated");
}

void HttpServer::handleDeleteUserRequest(QHttpRequest *req, QHttpResponse *res) {
    int userId;
    QString role;
    if (!middleware.isAuthorized(req, userId, role) || role != "admin") {
        res->writeHead(403);
        res->end("Forbidden");
        return;
    }

    res->writeHead(200);
    res->end("User deleted");
}
