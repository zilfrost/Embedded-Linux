#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "vehicledata.h"

int main(int argc, char *argv[])
{
    /* Enable high-DPI scaling for crisp rendering on the 7" HDMI panel */
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setApplicationName("IPC Dashboard");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("EmbeddedLinux");

    QQmlApplicationEngine engine;

    /* Expose the vehicle data model to QML */
    VehicleData vehicleData;
    engine.rootContext()->setContextProperty("vehicleData", &vehicleData);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
