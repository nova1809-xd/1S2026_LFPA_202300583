// mainwindow.cpp
// implementacion de la ventana principal usando widgets de qt

#include "mainwindow.h"
#include "lexical_analyzer.h"
#include "syntax_analyzer.h"
#include "report_generator.h"
#include "token.h"
#include "error.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QHeaderView>
#include <sstream>

// constructor: armamos la ui en codigo, sin usar .ui
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // linkeando widgets principales
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    editor = new QPlainTextEdit(central);
    editor->setPlaceholderText("escribe o carga tu codigo taskscript aqui...");

    btnLoad = new QPushButton("cargar archivo", central);
    btnAnalyze = new QPushButton("analizar", central);

    // tablas en pestañas
    tabs = new QTabWidget(central);
    tblTokens = new QTableWidget(0, 5, tabs);
    tblTokens->setHorizontalHeaderLabels({"no.", "lexema", "tipo", "linea", "columna"});
    tblTokens->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tblErrors = new QTableWidget(0, 5, tabs);
    tblErrors->setHorizontalHeaderLabels({"no.", "tipo", "lexema", "linea", "columna"});
    tblErrors->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tabs->addTab(tblTokens, "tokens");
    tabs->addTab(tblErrors, "errores");

    // botones para abrir reportes
    btnOpenKanban = new QPushButton("abrir kanban", central);
    btnOpenResponsables = new QPushButton("abrir responsables", central);
    btnOpenArbol = new QPushButton("abrir arbol", central);
    btnOpenErrores = new QPushButton("abrir errores", central);

    // layout principal
    QVBoxLayout *mainLay = new QVBoxLayout(central);
    QHBoxLayout *topLay = new QHBoxLayout();
    topLay->addWidget(btnLoad);
    topLay->addWidget(btnAnalyze);
    mainLay->addLayout(topLay);
    mainLay->addWidget(editor);
    mainLay->addWidget(tabs);

    QHBoxLayout *bottomLay = new QHBoxLayout();
    bottomLay->addWidget(btnOpenKanban);
    bottomLay->addWidget(btnOpenResponsables);
    bottomLay->addWidget(btnOpenArbol);
    bottomLay->addWidget(btnOpenErrores);
    mainLay->addLayout(bottomLay);

    // conexiones
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadFile);
    connect(btnAnalyze, &QPushButton::clicked, this, &MainWindow::onAnalyze);
    connect(btnOpenKanban, &QPushButton::clicked, this, &MainWindow::onOpenKanban);
    connect(btnOpenResponsables, &QPushButton::clicked, this, &MainWindow::onOpenResponsables);
    connect(btnOpenArbol, &QPushButton::clicked, this, &MainWindow::onOpenArbol);
    connect(btnOpenErrores, &QPushButton::clicked, this, &MainWindow::onOpenErrores);

    setWindowTitle("taskscript - editor y analizador");
    resize(1000, 700);
}

// slot: cargar archivo en el editor
void MainWindow::onLoadFile() {
    QString path = QFileDialog::getOpenFileName(this, "seleccionar archivo", "", "*.task;*.*");
    if (path.isEmpty()) return;
    QFile f(path);
    if (!f.open(QFile::ReadOnly | QFile::Text)) return;
    QByteArray data = f.readAll();
    editor->setPlainText(QString::fromUtf8(data));
}

// slot: ejecutar analisis (lexer + parser) y poblar tablas
void MainWindow::onAnalyze() {
    // limpiamos tablas antes de la nueva pasada
    tblTokens->setRowCount(0);
    tblErrors->setRowCount(0);

    QString text = editor->toPlainText();
    std::istringstream ss(text.toStdString());

    // instanciando el lexer y parser (no tocamos su logica interna)
    errores.clear();
    lexicalanalyzer lexer(ss);
    syntaxanalyzer parser(lexer);

    // parseamos y el lexer ira llenando su vector interno de tokens
    std::unique_ptr<ProgramNode> ast = parser.parse();

    // llenamos la tabla de tokens desde el lexer
    const auto &tokens = lexer.getTokens();
    tblTokens->setRowCount((int)tokens.size());
    for (int i = 0; i < (int)tokens.size(); ++i) {
        const token &t = tokens[i];
        tblTokens->setItem(i, 0, new QTableWidgetItem(QString::number(t.number)));
        tblTokens->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(t.lexeme)));
        tblTokens->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(std::to_string((int)t.type))));
        tblTokens->setItem(i, 3, new QTableWidgetItem(QString::number(t.line)));
        tblTokens->setItem(i, 4, new QTableWidgetItem(QString::number(t.column)));
    }

    // llenamos tabla de errores usando el vector global `errores`
    tblErrors->setRowCount((int)errores.size());
    for (int i = 0; i < (int)errores.size(); ++i) {
        const error_t &e = errores[i];
        tblErrors->setItem(i, 0, new QTableWidgetItem(QString::number(e.number)));
        tblErrors->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(e.tipo)));
        tblErrors->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(e.lexeme)));
        tblErrors->setItem(i, 3, new QTableWidgetItem(QString::number(e.line)));
        tblErrors->setItem(i, 4, new QTableWidgetItem(QString::number(e.column)));
    }

    // generamos reportes si hay arbol (kanban, responsables, arbol.dot)
    reportgenerator gen;
    if (ast) {
        gen.generarKanban(ast.get());
        gen.generarCargaResponsables(ast.get());
        gen.generarArbolDot(ast.get());
    }

    // generamos el reporte de errores (aunque este vacio, lo creamos igual)
    gen.generarHtmlErrores(errores);
}

// slots para abrir los archivos generados
void MainWindow::onOpenKanban() {
    QDesktopServices::openUrl(QUrl::fromLocalFile("tablero.html"));
}
void MainWindow::onOpenResponsables() {
    QDesktopServices::openUrl(QUrl::fromLocalFile("responsables.html"));
}
void MainWindow::onOpenArbol() {
    QDesktopServices::openUrl(QUrl::fromLocalFile("arbol.dot"));
}
void MainWindow::onOpenErrores() {
    // abriendo el reporte de errores en el navegador por defecto
    QDesktopServices::openUrl(QUrl::fromLocalFile("errores.html"));
}
