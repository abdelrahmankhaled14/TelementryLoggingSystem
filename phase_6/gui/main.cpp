#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCoreApplication>

#include "SystemMonitorBackend.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    SystemMonitorBackend backend;
    engine.rootContext()->setContextProperty("backend", &backend);

    const QUrl url = QUrl::fromLocalFile(
        QCoreApplication::applicationDirPath() + "/../qml/main.qml"
    );

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    // Start playing values from log1.txt
    backend.start();

    return app.exec();
}
