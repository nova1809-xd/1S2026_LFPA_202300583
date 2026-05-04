// mainwindow.h
// ventana principal con editor, tablas y botones para integrar el backend
#ifndef PROYECTO2_MAINWINDOW_H
#define PROYECTO2_MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTabWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void onLoadFile();
    void onAnalyze();
    void onOpenKanban();
    void onOpenResponsables();
    void onOpenArbol();
    void onOpenErrores();

private:
    // widgets principales
    QPlainTextEdit *editor;
    QPushButton *btnLoad;
    QPushButton *btnAnalyze;
    QTabWidget *tabs;
    QTableWidget *tblTokens;
    QTableWidget *tblErrors;
    QPushButton *btnOpenKanban;
    QPushButton *btnOpenResponsables;
    QPushButton *btnOpenArbol;
    QPushButton *btnOpenErrores;
};

#endif // PROYECTO2_MAINWINDOW_H
