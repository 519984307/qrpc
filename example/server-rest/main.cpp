#include <QCoreApplication>
#include <QRpc/Server>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //server class
    QRpc::QRPCServer server;

    //set settings to run service
    if(!server.setSettingsFileName(QStringLiteral(":/settings.json"))){
        qWarning()<<"invalid settings";
        return 0;
    }

    //start service
    server.start();

    //start eventloop application
    return a.exec();
}
