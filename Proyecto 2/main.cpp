// main.cpp
// inicializador de la aplicacion qt que muestra la ventana principal
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char **argv) {
    // instanciando la app y mostrando la ventana
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
