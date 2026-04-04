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

#include <unordered_map>
#include <unordered_set>
#include <ctime>

namespace {

std::string limpiarCadena(const std::string& valor) {
    if (valor.size() >= 2 && valor.front() == '"' && valor.back() == '"') {
        return valor.substr(1, valor.size() - 2);
    }
    return valor;
}

int calcularEdadDesdeFecha(const std::string& fecha) {
    if (fecha.size() < 4) {
        return 0;
    }

    int anio = 0;
    try {
        anio = std::stoi(fecha.substr(0, 4));
    } catch (...) {
        return 0;
    }
    std::time_t now = std::time(nullptr);
    std::tm* local = std::localtime(&now);
    if (!local) {
        return 0;
    }
    int edad = (local->tm_year + 1900) - anio;
    if (edad < 0) {
        return 0;
    }
    return edad;
}

void extraerDatosDesdeTokens(
    const std::vector<Token>& tokens,
    std::vector<PacienteFila>& pacientes,
    std::vector<MedicoFila>& medicos,
    std::vector<CitaFila>& citas) {

    std::unordered_map<std::string, std::string> detallePorCita;
    std::unordered_map<std::string, size_t> indicePacientePorCodigo;
    TokenType seccion = TokenType::ERROR_TOKEN;

    for (size_t i = 0; i < tokens.size(); ++i) {
        TokenType tipo = tokens[i].getTipo();

        if (tipo == TokenType::PACIENTES || tipo == TokenType::MEDICOS ||
            tipo == TokenType::CITAS || tipo == TokenType::DIAGNOSTICOS) {
            seccion = tipo;
            continue;
        }

        if (seccion == TokenType::PACIENTES && tipo == TokenType::PACIENTE) {
            std::unordered_map<std::string, std::string> campos;
            size_t j = i + 1;
            while (j + 2 < tokens.size() && tokens[j].getTipo() != TokenType::LLAVE_CERRADA) {
                if (tokens[j].getTipo() == TokenType::IDENTIFICADOR && tokens[j + 1].getTipo() == TokenType::DOS_PUNTOS) {
                    campos[tokens[j].getLexema()] = tokens[j + 2].getLexema();
                    j += 3;
                } else {
                    ++j;
                }
            }

            PacienteFila fila;
            std::string codigoPaciente = campos["codigo"];
            fila.paciente = limpiarCadena(campos["nombre"]);
            fila.edad = calcularEdadDesdeFecha(campos["nacimiento"]);
            fila.sangre = campos["sangre"];
            fila.diagnostico = "sin dato";
            fila.medicamento = "sin dato";
            fila.estado = "sin dato";
            pacientes.push_back(fila);
            if (!codigoPaciente.empty()) {
                indicePacientePorCodigo[codigoPaciente] = pacientes.size() - 1;
            }
            continue;
        }

        if (seccion == TokenType::MEDICOS && tipo == TokenType::MEDICO) {
            std::unordered_map<std::string, std::string> campos;
            size_t j = i + 1;
            while (j + 2 < tokens.size() && tokens[j].getTipo() != TokenType::LLAVE_CERRADA) {
                if (tokens[j].getTipo() == TokenType::IDENTIFICADOR && tokens[j + 1].getTipo() == TokenType::DOS_PUNTOS) {
                    campos[tokens[j].getLexema()] = tokens[j + 2].getLexema();
                    j += 3;
                } else {
                    ++j;
                }
            }

            MedicoFila fila;
            fila.medico = limpiarCadena(campos["nombre"]);
            fila.codigo = campos["codigo"];
            fila.especialidad = campos["especialidad"];
            fila.citas = 0;
            fila.pacientes = 0;
            fila.nivelCarga = "baja";
            medicos.push_back(fila);
            continue;
        }

        if (seccion == TokenType::CITAS && tipo == TokenType::CITA) {
            std::unordered_map<std::string, std::string> campos;
            size_t j = i + 1;
            while (j + 2 < tokens.size() && tokens[j].getTipo() != TokenType::LLAVE_CERRADA) {
                if (tokens[j].getTipo() == TokenType::IDENTIFICADOR && tokens[j + 1].getTipo() == TokenType::DOS_PUNTOS) {
                    campos[tokens[j].getLexema()] = tokens[j + 2].getLexema();
                    j += 3;
                } else {
                    ++j;
                }
            }

            CitaFila fila;
            fila.codigo = campos["codigo"];
            fila.paciente = campos["paciente"];
            fila.medico = campos["medico"];
            fila.fecha = campos["fecha"];
            fila.hora = campos["hora"];
            citas.push_back(fila);
            continue;
        }

        if (seccion == TokenType::DIAGNOSTICOS && tipo == TokenType::DIAGNOSTICO) {
            std::unordered_map<std::string, std::string> campos;
            size_t j = i + 1;
            while (j + 2 < tokens.size() && tokens[j].getTipo() != TokenType::LLAVE_CERRADA) {
                if (tokens[j].getTipo() == TokenType::IDENTIFICADOR && tokens[j + 1].getTipo() == TokenType::DOS_PUNTOS) {
                    campos[tokens[j].getLexema()] = tokens[j + 2].getLexema();
                    j += 3;
                } else {
                    ++j;
                }
            }

            if (!campos["cita"].empty() && !campos["detalle"].empty()) {
                detallePorCita[campos["cita"]] = limpiarCadena(campos["detalle"]);
            }
            continue;
        }
    }

    std::unordered_map<std::string, CitaFila> citaPorCodigo;
    for (const auto& c : citas) {
        citaPorCodigo[c.codigo] = c;
    }

    for (const auto& entry : detallePorCita) {
        auto itCita = citaPorCodigo.find(entry.first);
        if (itCita != citaPorCodigo.end()) {
            auto itPaciente = indicePacientePorCodigo.find(itCita->second.paciente);
            if (itPaciente != indicePacientePorCodigo.end()) {
                PacienteFila& p = pacientes[itPaciente->second];
                p.diagnostico = entry.second;
                p.estado = "en tratamiento";
            }
        }
    }

    std::unordered_map<std::string, int> citasPorMedico;
    std::unordered_map<std::string, std::unordered_set<std::string>> pacientesPorMedico;
    for (const auto& c : citas) {
        citasPorMedico[c.medico]++;
        pacientesPorMedico[c.medico].insert(c.paciente);
    }

    for (auto& m : medicos) {
        m.citas = citasPorMedico[m.codigo];
        m.pacientes = static_cast<int>(pacientesPorMedico[m.codigo].size());
        if (m.citas >= 10) {
            m.nivelCarga = "alta";
        } else if (m.citas >= 5) {
            m.nivelCarga = "media";
        } else {
            m.nivelCarga = "baja";
        }
    }
}

} // namespace

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

        if (token.getTipo() == TokenType::ERROR_TOKEN) {
            continue;
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
    QString texto = editorCodigo->toPlainText();
    LexicalAnalyzer analizador(texto.toStdString());

    std::vector<Token> tokensValidos;
    while (true) {
        Token token = analizador.nextToken();
        if (token.getTipo() == TokenType::EOF_TOKEN) {
            break;
        }
        if (token.getTipo() != TokenType::ERROR_TOKEN) {
            tokensValidos.push_back(token);
        }
    }

    std::vector<PacienteFila> pacientes;
    std::vector<MedicoFila> medicos;
    std::vector<CitaFila> citas;
    extraerDatosDesdeTokens(tokensValidos, pacientes, medicos, citas);

    ReportGenerator generador("docs");

    bool ok1 = generador.generarReportePacientes(pacientes);
    bool ok2 = generador.generarReporteMedicos(medicos);
    bool ok3 = generador.generarReporteCitas(citas);
    bool ok4 = generador.generarReporteEstadisticas(pacientes, medicos, citas);
    bool ok5 = generador.generarDiagramaGraphviz(pacientes, medicos, citas);

    if (ok1 && ok2 && ok3 && ok4 && ok5) {
        QMessageBox::information(this, "Reportes", "Reportes generados correctamente en carpeta docs.");
    } else {
        QMessageBox::warning(this, "Reportes", "No se pudieron generar todos los reportes.");
    }
}
