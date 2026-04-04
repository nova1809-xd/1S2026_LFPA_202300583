#include "MainWindow.h"

#include "ReportGenerator.h"
#include "../include/LexicalAnalyzer.h"
#include "../include/Token.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      editorCodigo(nullptr),
      tablaTokens(nullptr),
      tablaErrores(nullptr),
      botonCargar(nullptr),
      botonAnalizar(nullptr),
      botonReportes(nullptr) {
    construirUI();
}

void MainWindow::construirUI() {
    QWidget* central = new QWidget(this);
    QVBoxLayout* layoutPrincipal = new QVBoxLayout();
    QHBoxLayout* layoutBotones = new QHBoxLayout();

    editorCodigo = new QTextEdit();
    editorCodigo->setPlaceholderText("Pega o carga aqui el codigo .med");

    botonCargar = new QPushButton("Cargar Archivo");
    botonAnalizar = new QPushButton("Analizar");
    botonReportes = new QPushButton("Generar Reportes");

    tablaTokens = new QTableWidget();
    tablaErrores = new QTableWidget();

    configurarTablas();

    layoutBotones->addWidget(botonCargar);
    layoutBotones->addWidget(botonAnalizar);
    layoutBotones->addWidget(botonReportes);

    layoutPrincipal->addLayout(layoutBotones);
    layoutPrincipal->addWidget(editorCodigo, 3);
    layoutPrincipal->addWidget(tablaTokens, 2);
    layoutPrincipal->addWidget(tablaErrores, 1);

    central->setLayout(layoutPrincipal);
    setCentralWidget(central);
    setWindowTitle("MedLexer - Interfaz Grafica");
    resize(1100, 760);

    connect(botonCargar, &QPushButton::clicked, this, &MainWindow::cargarArchivoMed);
    connect(botonAnalizar, &QPushButton::clicked, this, &MainWindow::analizarCodigo);
    connect(botonReportes, &QPushButton::clicked, this, &MainWindow::generarReportes);
}

void MainWindow::configurarTablas() {
    tablaTokens->setColumnCount(4);
    tablaTokens->setHorizontalHeaderLabels({"Linea", "Columna", "Tipo", "Lexema"});
    tablaTokens->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tablaErrores->setColumnCount(1);
    tablaErrores->setHorizontalHeaderLabels({"Errores Lexicos"});
    tablaErrores->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::cargarArchivoMed() {
    QString ruta = QFileDialog::getOpenFileName(this, "Selecciona archivo .med", QString(), "Archivos MedLang (*.med);;Todos (*.*)");
    if (ruta.isEmpty()) {
        return;
    }

    QFile archivo(ruta);
    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "No se pudo abrir el archivo seleccionado.");
        return;
    }

    QTextStream stream(&archivo);
    editorCodigo->setPlainText(stream.readAll());
    archivo.close();
}

void MainWindow::analizarCodigo() {
    tablaTokens->setRowCount(0);
    tablaErrores->setRowCount(0);

    QString texto = editorCodigo->toPlainText();
    LexicalAnalyzer analizador(texto.toStdString());

    while (true) {
        Token token = analizador.nextToken();
        if (token.getTipo() == TokenType::EOF_TOKEN) {
            break;
        }

        int fila = tablaTokens->rowCount();
        tablaTokens->insertRow(fila);
        tablaTokens->setItem(fila, 0, new QTableWidgetItem(QString::number(token.getLinea())));
        tablaTokens->setItem(fila, 1, new QTableWidgetItem(QString::number(token.getColumna())));
        tablaTokens->setItem(fila, 2, new QTableWidgetItem(QString::fromStdString(Token::tipoToString(token.getTipo()))));
        tablaTokens->setItem(fila, 3, new QTableWidgetItem(QString::fromStdString(token.getLexema())));
    }

    const auto& errores = analizador.obtenerErrores();
    for (size_t i = 0; i < errores.size(); ++i) {
        int fila = tablaErrores->rowCount();
        tablaErrores->insertRow(fila);
        tablaErrores->setItem(fila, 0, new QTableWidgetItem(QString::fromStdString(errores[i])));
    }
}

void MainWindow::generarReportes() {
    ReportGenerator generador("docs");

    bool ok1 = generador.generarReportePacientes();
    bool ok2 = generador.generarReporteMedicos();
    bool ok3 = generador.generarReporteCitas();
    bool ok4 = generador.generarReporteEstadisticas();
    bool ok5 = generador.generarDiagramaGraphviz();

    if (ok1 && ok2 && ok3 && ok4 && ok5) {
        QMessageBox::information(this, "Reportes", "Reportes generados correctamente en carpeta docs.");
    } else {
        QMessageBox::warning(this, "Reportes", "No se pudieron generar todos los reportes.");
    }
}
