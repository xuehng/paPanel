#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <paservice.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    QQmlContext* context = engine.rootContext();

    PaService *pa = new PaService("127.0.0.1", 27015);
    context->setContextProperty("pa", pa);

    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
