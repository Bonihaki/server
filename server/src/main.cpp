
#include <QCoreApplication>
#include "HttpServer.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    
    HttpServer server;
    server.start();

    return a.exec();
}
