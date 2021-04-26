#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QScreen>

#include "cwaqrcodegenerator.h"
#include "QZXing.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
#ifdef TEST_CONFIG
    CwaQrCodeGenerator::registerQMLImageProvider(TEST_CONFIG, engine);
#else
    if(argc != 2) {
        qDebug() << "Usage: cwa-qr-code-generator <config file path>";
        return 1;
    }

    CwaQrCodeGenerator::registerQMLImageProvider(argv[1], engine);
#endif

    QSize size = app.screens().first()->size();
    engine.rootContext()->setContextProperty("screenHeight", size.height());
    engine.rootContext()->setContextProperty("screenWidth", size.width());

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
    &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
