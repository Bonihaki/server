
#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include "DatabaseManager.h"
#include "SessionManager.h"
#include "Middleware.h"

class HttpServer : public QObject {
    Q_OBJECT
public:
    explicit HttpServer(QObject *parent = nullptr);
    void start();

private slots:
    void handleRegistrationRequest(QHttpRequest *req, QHttpResponse *res);
    void handleAuthenticationRequest(QHttpRequest *req, QHttpResponse *res);
    void handleGetUsersRequest(QHttpRequest *req, QHttpResponse *res);
    void handleUpdateUserRequest(QHttpRequest *req, QHttpResponse *res);
    void handleDeleteUserRequest(QHttpRequest *req, QHttpResponse *res);

private:
    DatabaseManager dbManager;
    SessionManager sessionManager;
    Middleware middleware;
};

#endif // HTTPSERVER_H
