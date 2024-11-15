
void HttpServer::handleRegistrationRequest(QHttpRequest *req, QHttpResponse *res) {
    QByteArray body = req->body();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(body);
    QJsonObject json = jsonDoc.object();

    QString username = json["username"].toString();
    QString email = json["email"].toString();
    QString password = json["password"].toString();
    QString role = "user"; // Роль по умолчанию для новых пользователей

    if (dbManager.addUser(username, password, email, "client_key")) {
        QString token = sessionManager.generateToken(username);
        QDateTime expiresAt = QDateTime::currentDateTime().addDays(1);
        sessionManager.createSession(dbManager.getUser(username)["id"].toInt(), token, expiresAt);

        QJsonObject responseJson;
        responseJson["token"] = token;
        res->writeHead(200);
        res->end(QJsonDocument(responseJson).toJson());
    } else {
        res->writeHead(500);
        res->end("Ошибка регистрации пользователя");
    }
}
