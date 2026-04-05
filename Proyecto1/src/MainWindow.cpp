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

bool esDigito(char c) {
    return c >= '0' && c <= '9';
}

bool esFechaValidaEstrica(const std::string& valor) {
    if (valor.size() != 10) {
        return false;
    }
    if (!esDigito(valor[0]) || !esDigito(valor[1]) || !esDigito(valor[2]) || !esDigito(valor[3]) ||
        valor[4] != '-' ||
        !esDigito(valor[5]) || !esDigito(valor[6]) ||
        valor[7] != '-' ||
        !esDigito(valor[8]) || !esDigito(valor[9])) {
        return false;
    }

    int mes = (valor[5] - '0') * 10 + (valor[6] - '0');
    int dia = (valor[8] - '0') * 10 + (valor[9] - '0');

    return mes >= 1 && mes <= 12 && dia >= 1 && dia <= 31;
}

bool esHoraValidaEstrica(const std::string& valor) {
    if (valor.size() != 5) {
        return false;
    }
    if (!esDigito(valor[0]) || !esDigito(valor[1]) ||
        valor[2] != ':' ||
        !esDigito(valor[3]) || !esDigito(valor[4])) {
        return false;
    }

    int hh = (valor[0] - '0') * 10 + (valor[1] - '0');
    int mm = (valor[3] - '0') * 10 + (valor[4] - '0');

    return hh >= 0 && hh <= 23 && mm >= 0 && mm <= 59;
}

bool esEspecialidadValida(const std::string& valor) {
    static const std::unordered_set<std::string> especialidadesValidas = {
        "CARDIOLOGIA", "NEUROLOGIA", "OFTALMOLOGIA", "ORTOPEDIA", "PEDIATRIA",
        "PSIQUIATRIA", "GASTROENTEROLOGIA", "UROLOGIA", "DERMATOLOGIA", "ONCOLOGIA"
    };
    return especialidadesValidas.find(valor) != especialidadesValidas.end();
}

bool esDosisValida(const std::string& valor) {
    static const std::unordered_set<std::string> dosisValidas = {
        "DIARIA", "CADA_8_HORAS", "CADA_12_HORAS", "CADA_6_HORAS",
        "CADA_4_HORAS", "SEGUN_SEA_NECESARIO"
    };
    return dosisValidas.find(valor) != dosisValidas.end();
}

void extraerDatosDesdeTokens(
    const std::vector<Token>& tokens,
    std::vector<PacienteFila>& pacientes,
    std::vector<MedicoFila>& medicos,
    std::vector<CitaFila>& citas,
    std::vector<std::string>* erroresSemanticos = nullptr) {
    std::unordered_map<std::string, size_t> indicePacientePorNombre;
    std::unordered_map<std::string, std::string> diagnosticoPorPaciente;
    std::unordered_map<std::string, std::string> medicamentoPorPaciente;
    TokenType seccion = TokenType::ERROR_TOKEN;

    auto esTokenClaveCampo = [](TokenType tipo) {
        return tipo == TokenType::IDENTIFICADOR ||
               tipo == TokenType::PACIENTE ||
               tipo == TokenType::MEDICO ||
               tipo == TokenType::CITA ||
               tipo == TokenType::DIAGNOSTICO;
    };

    auto extraerAtributosCorchete = [&](size_t& pos) {
        std::unordered_map<std::string, std::string> campos;
        if (pos >= tokens.size() || tokens[pos].getTipo() != TokenType::CORCHETE_ABIERTO) {
            return campos;
        }

        ++pos;
        while (pos < tokens.size() && tokens[pos].getTipo() != TokenType::CORCHETE_CERRADO) {
            if (pos + 2 < tokens.size() &&
                esTokenClaveCampo(tokens[pos].getTipo()) &&
                tokens[pos + 1].getTipo() == TokenType::DOS_PUNTOS) {
                std::string clave = limpiarCadena(tokens[pos].getLexema());
                std::string valor = limpiarCadena(tokens[pos + 2].getLexema());
                campos[clave] = valor;
                pos += 3;
                if (pos < tokens.size() && tokens[pos].getTipo() == TokenType::COMA) {
                    ++pos;
                }
            } else {
                ++pos;
            }
        }

        if (pos < tokens.size() && tokens[pos].getTipo() == TokenType::CORCHETE_CERRADO) {
            ++pos;
        }

        return campos;
    };

    for (size_t i = 0; i < tokens.size(); ++i) {
        TokenType tipo = tokens[i].getTipo();

        if (tipo == TokenType::PACIENTES || tipo == TokenType::MEDICOS ||
            tipo == TokenType::CITAS || tipo == TokenType::DIAGNOSTICOS) {
            seccion = tipo;
            continue;
        }

        if (tipo == TokenType::COMA || tipo == TokenType::PUNTO_Y_COMA ||
            tipo == TokenType::LLAVE_ABIERTA || tipo == TokenType::LLAVE_CERRADA) {
            continue;
        }

        if (seccion == TokenType::PACIENTES && tipo == TokenType::PACIENTE) {
            size_t j = i + 1;
            if (j >= tokens.size() || tokens[j].getTipo() != TokenType::DOS_PUNTOS) {
                continue;
            }
            ++j;
            if (j >= tokens.size() || tokens[j].getTipo() != TokenType::STRING) {
                continue;
            }

            std::string nombrePaciente = limpiarCadena(tokens[j].getLexema());
            ++j;
            while (j < tokens.size() && tokens[j].getTipo() != TokenType::CORCHETE_ABIERTO &&
                   tokens[j].getTipo() != TokenType::PUNTO_Y_COMA && tokens[j].getTipo() != TokenType::LLAVE_CERRADA) {
                ++j;
            }

            auto campos = extraerAtributosCorchete(j);

            PacienteFila fila;
            fila.paciente = nombrePaciente;
            fila.edad = 0;
            if (!campos["edad"].empty()) {
                try {
                    fila.edad = std::stoi(campos["edad"]);
                } catch (...) {
                    if (erroresSemanticos != nullptr) {
                        erroresSemanticos->push_back(
                            "Error semántico en línea " + std::to_string(tokens[i].getLinea()) +
                            ": edad inválida para paciente ('" + campos["edad"] + "').");
                    }
                    continue;
                }
            } else if (!campos["nacimiento"].empty()) {
                if (!esFechaValidaEstrica(campos["nacimiento"])) {
                    if (erroresSemanticos != nullptr) {
                        erroresSemanticos->push_back(
                            "Error semántico en línea " + std::to_string(tokens[i].getLinea()) +
                            ": fecha de nacimiento inválida para paciente ('" + campos["nacimiento"] + "').");
                    }
                    continue;
                }
                fila.edad = calcularEdadDesdeFecha(campos["nacimiento"]);
            }

            fila.sangre = !campos["tipo_sangre"].empty() ? campos["tipo_sangre"] : "sin dato";
            fila.diagnostico = "sin dato";
            fila.medicamento = "sin dato";
            fila.estado = "sin dato";

            if (!fila.paciente.empty()) {
                pacientes.push_back(fila);
                indicePacientePorNombre[fila.paciente] = pacientes.size() - 1;
            }
            continue;
        }

        if (seccion == TokenType::MEDICOS && tipo == TokenType::MEDICO) {
            size_t j = i + 1;
            if (j >= tokens.size() || tokens[j].getTipo() != TokenType::DOS_PUNTOS) {
                continue;
            }
            ++j;
            if (j >= tokens.size() || tokens[j].getTipo() != TokenType::STRING) {
                continue;
            }

            std::string nombreMedico = limpiarCadena(tokens[j].getLexema());
            ++j;
            while (j < tokens.size() && tokens[j].getTipo() != TokenType::CORCHETE_ABIERTO &&
                   tokens[j].getTipo() != TokenType::PUNTO_Y_COMA && tokens[j].getTipo() != TokenType::LLAVE_CERRADA) {
                ++j;
            }

            auto campos = extraerAtributosCorchete(j);

            std::string especialidad = !campos["especialidad"].empty() ? campos["especialidad"] : "sin dato";
            if (especialidad != "sin dato" && !esEspecialidadValida(especialidad)) {
                if (erroresSemanticos != nullptr) {
                    erroresSemanticos->push_back(
                        "Error semántico en línea " + std::to_string(tokens[i].getLinea()) +
                        ": especialidad inválida para médico ('" + especialidad + "').");
                }
                continue;
            }

            MedicoFila fila;
            fila.medico = nombreMedico;
            fila.codigo = !campos["codigo"].empty() ? campos["codigo"] : "sin dato";
            fila.especialidad = especialidad;
            fila.citas = 0;
            fila.pacientes = 0;
            fila.nivelCarga = "baja";
            medicos.push_back(fila);
            continue;
        }

        if (seccion == TokenType::CITAS && tipo == TokenType::CITA) {
            size_t j = i + 1;
            if (j >= tokens.size() || tokens[j].getTipo() != TokenType::DOS_PUNTOS) {
                continue;
            }
            ++j;
            if (j >= tokens.size() || tokens[j].getTipo() != TokenType::STRING) {
                continue;
            }

            std::string nombrePaciente = limpiarCadena(tokens[j].getLexema());
            ++j;

            if (j >= tokens.size() || limpiarCadena(tokens[j].getLexema()) != "con") {
                continue;
            }
            ++j;

            if (j >= tokens.size() || tokens[j].getTipo() != TokenType::STRING) {
                continue;
            }
            std::string nombreMedico = limpiarCadena(tokens[j].getLexema());
            ++j;

            while (j < tokens.size() && tokens[j].getTipo() != TokenType::CORCHETE_ABIERTO &&
                   tokens[j].getTipo() != TokenType::PUNTO_Y_COMA && tokens[j].getTipo() != TokenType::LLAVE_CERRADA) {
                ++j;
            }

            auto campos = extraerAtributosCorchete(j);
            std::string fecha = campos["fecha"];
            std::string hora = campos["hora"];

            if (!esFechaValidaEstrica(fecha) || !esHoraValidaEstrica(hora)) {
                if (erroresSemanticos != nullptr) {
                    erroresSemanticos->push_back(
                        "Error semántico en línea " + std::to_string(tokens[i].getLinea()) +
                        ": cita descartada por fecha/hora inválida (fecha='" + fecha + "', hora='" + hora + "').");
                }
                continue;
            }

            CitaFila fila;
            fila.codigo = !campos["codigo"].empty() ? campos["codigo"] : "sin dato";
            fila.paciente = nombrePaciente;
            fila.medico = nombreMedico;
            fila.fecha = fecha;
            fila.hora = hora;
            citas.push_back(fila);
            continue;
        }

        if (seccion == TokenType::DIAGNOSTICOS && tipo == TokenType::DIAGNOSTICO) {
            size_t j = i + 1;
            if (j >= tokens.size() || tokens[j].getTipo() != TokenType::DOS_PUNTOS) {
                continue;
            }
            ++j;
            if (j >= tokens.size() || tokens[j].getTipo() != TokenType::STRING) {
                continue;
            }

            std::string nombrePaciente = limpiarCadena(tokens[j].getLexema());
            ++j;
            while (j < tokens.size() && tokens[j].getTipo() != TokenType::CORCHETE_ABIERTO &&
                   tokens[j].getTipo() != TokenType::PUNTO_Y_COMA && tokens[j].getTipo() != TokenType::LLAVE_CERRADA) {
                ++j;
            }

            auto campos = extraerAtributosCorchete(j);
            std::string dosis = !campos["dosis"].empty() ? campos["dosis"] : "sin dato";
            if (dosis != "sin dato" && !esDosisValida(dosis)) {
                if (erroresSemanticos != nullptr) {
                    erroresSemanticos->push_back(
                        "Error semántico en línea " + std::to_string(tokens[i].getLinea()) +
                        ": dosis inválida en diagnóstico ('" + dosis + "').");
                }
                continue;
            }
            if (!campos["condicion"].empty()) {
                diagnosticoPorPaciente[nombrePaciente] = campos["condicion"];
            }
            if (!campos["medicamento"].empty()) {
                medicamentoPorPaciente[nombrePaciente] = campos["medicamento"];
            }
            continue;
        }
    }

    for (auto& p : pacientes) {
        auto itDiag = diagnosticoPorPaciente.find(p.paciente);
        if (itDiag != diagnosticoPorPaciente.end()) {
            p.diagnostico = itDiag->second;
            p.estado = "en tratamiento";
        }
        auto itMed = medicamentoPorPaciente.find(p.paciente);
        if (itMed != medicamentoPorPaciente.end()) {
            p.medicamento = itMed->second;
            p.estado = "en tratamiento";
        }
    }

    std::unordered_map<std::string, int> citasPorMedico;
    std::unordered_map<std::string, std::unordered_set<std::string>> pacientesPorMedico;
    for (const auto& c : citas) {
        citasPorMedico[c.medico]++;
        pacientesPorMedico[c.medico].insert(c.paciente);
    }

    for (auto& m : medicos) {
        int citasPorNombre = citasPorMedico[m.medico];
        int citasPorCodigo = citasPorMedico[m.codigo];
        m.citas = (citasPorCodigo > 0) ? citasPorCodigo : citasPorNombre;

        int pacientesPorNombreCount = static_cast<int>(pacientesPorMedico[m.medico].size());
        int pacientesPorCodigoCount = static_cast<int>(pacientesPorMedico[m.codigo].size());
        m.pacientes = (pacientesPorCodigoCount > 0) ? pacientesPorCodigoCount : pacientesPorNombreCount;

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
    tablaErrores->setHorizontalHeaderLabels({"Errores"});
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
    std::vector<Token> tokensValidos;

    while (true) {
        Token token = analizador.nextToken();
        if (token.getTipo() == TokenType::EOF_TOKEN) {
            break;
        }

        if (token.getTipo() == TokenType::ERROR_TOKEN) {
            continue;
        }

        tokensValidos.push_back(token);

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

    std::vector<PacienteFila> pacientesTmp;
    std::vector<MedicoFila> medicosTmp;
    std::vector<CitaFila> citasTmp;
    std::vector<std::string> erroresSemanticos;
    extraerDatosDesdeTokens(tokensValidos, pacientesTmp, medicosTmp, citasTmp, &erroresSemanticos);

    for (const auto& err : erroresSemanticos) {
        int fila = tablaErrores->rowCount();
        tablaErrores->insertRow(fila);
        tablaErrores->setItem(fila, 0, new QTableWidgetItem(QString::fromStdString(err)));
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
    std::vector<std::string> erroresSemanticos;
    extraerDatosDesdeTokens(tokensValidos, pacientes, medicos, citas, &erroresSemanticos);

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
