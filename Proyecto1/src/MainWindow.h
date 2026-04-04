#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTextEdit;
class QTableWidget;
class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    QTextEdit* editorCodigo;
    QTableWidget* tablaTokens;
    QTableWidget* tablaErrores;
    QPushButton* botonCargar;
    QPushButton* botonAnalizar;
    QPushButton* botonReportes;

    void construirUI();
    void configurarTablas();
    void cargarArchivoMed();
    void analizarCodigo();
    void generarReportes();
};

#endif
