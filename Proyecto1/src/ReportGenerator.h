#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include <string>
#include <vector>

struct PacienteFila {
    std::string paciente;
    int edad;
    std::string sangre;
    std::string diagnostico;
    std::string medicamento;
    std::string estado;
};

struct MedicoFila {
    std::string medico;
    std::string codigo;
    std::string especialidad;
    int citas;
    int pacientes;
    std::string nivelCarga;
};

struct CitaFila {
    std::string codigo;
    std::string paciente;
    std::string medico;
    std::string fecha;
    std::string hora;
};

class ReportGenerator {
private:
    std::string outputDir;

    std::string buildPath(const std::string& fileName) const;

public:
    explicit ReportGenerator(const std::string& outputDirectory = "docs");

    bool generarReportePacientes(const std::vector<PacienteFila>& filas);
    bool generarReporteMedicos(const std::vector<MedicoFila>& filas);
    bool generarReporteCitas(const std::vector<CitaFila>& filas);
    bool generarReporteEstadisticas(
        const std::vector<PacienteFila>& pacientes,
        const std::vector<MedicoFila>& medicos,
        const std::vector<CitaFila>& citas);
    bool generarDiagramaGraphviz(
        const std::vector<PacienteFila>& pacientes,
        const std::vector<MedicoFila>& medicos,
        const std::vector<CitaFila>& citas);
};

#endif // REPORT_GENERATOR_H
