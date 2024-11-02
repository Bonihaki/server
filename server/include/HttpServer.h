#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QMap>
#include <functional>
#include "DatabaseManager.h"
#include "SessionManager.h"
#include "Middleware.h"
#include <QHttpRequest>
#include <QHttpResponse>

class HttpServer : public QObject {
    Q_OBJECT
public:
    explicit HttpServer(QObject *parent = nullptr);
    void start();

private:
    using RouteHandler = std::function<void(QHttpRequest*, QHttpResponse*)>;
    QMap<QString, RouteHandler> routeTable;

    void setupRoutes();

    void handleRegistrationRequest(QHttpRequest *req, QHttpResponse *res);
    void handleAuthenticationRequest(QHttpRequest *req, QHttpResponse *res);
    void handleGetUsersRequest(QHttpRequest *req, QHttpResponse *res);
    void handleUpdateUserRequest(QHttpRequest *req, QHttpResponse *res);
    void handleDeleteUserRequest(QHttpRequest *req, QHttpResponse *res);

    DatabaseManager dbManager;
    SessionManager sessionManager;
    Middleware middleware;
};

#endif // HTTPSERVER_H
