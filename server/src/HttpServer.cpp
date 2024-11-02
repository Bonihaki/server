
#include "HttpServer.h"
#include <QHttpServer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

HttpServer::HttpServer(QObject *parent) : QObject(parent), middleware(&sessionManager) {}

void HttpServer::start() {
    QHttpServer *server = new QHttpServer(this);

    connect(server, &QHttpServer::newRequest, this, [=](QHttpRequest *req, QHttpResponse *res) {
        if (req->url().path() == "/register") {
            handleRegistrationRequest(req, res);
        } else if (req->url().path() == "/authenticate") {
            handleAuthenticationRequest(req, res);
        } else if (req->url().path() == "/users" && req->method() == "GET") {
            handleGetUsersRequest(req, res);
        } else if (req->url().path().startsWith("/users/") && req->method() == "PUT") {
            handleUpdateUserRequest(req, res);
        } else if (req->url().path().startsWith("/users/") && req->method() == "DELETE") {
            handleDeleteUserRequest(req, res);
        } else {
            res->writeHead(404);
            res->end("Not Found");
        }
    });

    server->listen(QHostAddress::Any, 8080);
}

// Implement other methods like handleRegistrationRequest, handleAuthenticationRequest etc.
