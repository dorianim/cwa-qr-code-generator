#include <QGuiApplication>
#include <QQmlApplicationEngine>
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
    // TODO: handle path, maybe using argv
    CwaQrCodeGenerator::registerQMLImageProvider(TEST_CONFIG, engine);
#endif

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
    &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
