#include "mainwindow.h"
#include <QApplication>
#include <QMainWindow>
#include <QTranslator>
int main(int argc, char *argv[])
{


    QApplication app(argc, argv);
    /*QTranslator myTranslator;
    myTranslator.load("../graph_editor/graph_ru.qm");
    app.installTranslator(&myTranslator);*/

    MainWindow window;
    window.setWindowTitle("Graph editor");

    window.show();
    return app.exec();
}
