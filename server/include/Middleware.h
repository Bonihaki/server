
#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

#include <QObject>
#include <QHttpRequest>
#include <QHttpResponse>
#include "SessionManager.h"

class Middleware : public QObject {
    Q_OBJECT
public:
    Middleware(SessionManager* sessionManager, QObject *parent = nullptr);
    bool isAuthorized(QHttpRequest* req, int &userId, QString &role);
    void setCORS(QHttpResponse* res);

private:
    SessionManager* sessionManager;
};

#endif // MIDDLEWARE_H
