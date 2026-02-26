#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "LogParser.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    LogParser parser;
    // If your log path is fixed somewhere else:
    parser.setLogFilePath("/home/abdo/projects/linux/C++/project/TelementryLoggingSystem/phase_7/build/cpu.log");

    QQmlApplicationEngine engine;

    // expose backend to QML as "logParser"
    engine.rootContext()->setContextProperty("logParser", &parser);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}