#include "pages/mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QDebug>

#include "app_context.h"

int main(int argc, char *argv[])
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication a(argc, argv);

    // instance
    AppContext::instance().init(&a);

    MainWindow w;    
    w.show();
    return a.exec();
}
