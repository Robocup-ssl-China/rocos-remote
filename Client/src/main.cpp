#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include "qsingleton.h"
#include "field.h"
#include "refereebox.h"
#include "paraminterface.h"
#include "interaction.h"
#include "zos/socket.h"

#include <kddockwidgets/Config.h>
#include <kddockwidgets/FrameworkWidgetFactory.h>
QObject* interactionInstance(){
    return new Interaction();
}

void qmlRegister(){
    qmlRegisterType<Field>("ZSS",1,0,"Field");
    qmlRegisterType<RefBoxCommand>("ZSS", 1, 0, "RefBoxCommand");
    qmlRegisterType<GameState>("ZSS", 1, 0, "GameState");
    qmlRegisterType<RefereeBox>("ZSS", 1, 0, "Referee");
    qmlRegisterType<ParamInterface>("ZSS", 1, 0, "ParamModel");
    qmlRegisterSingletonType<Interaction>("ZSS", 1, 0, "Interaction",&qinstance<Interaction>);
}

class CustomFrameworkWidgetFactory : public KDDockWidgets::DefaultWidgetFactory{
public:
    ~CustomFrameworkWidgetFactory() override = default;
    QUrl titleBarFilename() const override{ return QUrl("qrc:/src/qml/CustomDW/TitleBar.qml"); }
    QUrl dockwidgetFilename() const override{ return QUrl("qrc:/src/qml/CustomDW/DockWidget.qml"); }
    QUrl frameFilename() const override{ return QUrl("qrc:/src/qml/CustomDW/Frame.qml"); }
    QUrl floatingWindowFilename() const override{ return QUrl("qrc:/src/qml/CustomDW/FloatingWindow.qml"); }
};

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);
    app.setOrganizationName("turing-zero");
    app.setOrganizationDomain("turing-zero.com");
    qmlRegister();

    app.setWindowIcon(QIcon("qrc:/resource/icon/icon.png"));

    auto &config = KDDockWidgets::Config::self();
    auto flags = config.flags();
    config.setFlags(flags);
    config.setSeparatorThickness(0);
    config.setFrameworkWidgetFactory(new CustomFrameworkWidgetFactory());

    QQmlApplicationEngine engine;
    KDDockWidgets::Config::self().setQmlEngine(&engine);
    engine.addImportPath("qrc:/src/qml");
    const QUrl url(QStringLiteral("qrc:/src/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
    bool res = app.exec();
    return res;
}
