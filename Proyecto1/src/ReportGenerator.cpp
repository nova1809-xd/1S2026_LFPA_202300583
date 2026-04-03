#include "ReportGenerator.h"

#include <fstream>

ReportGenerator::ReportGenerator(const std::string& outputDirectory)
    : outputDir(outputDirectory) {
}

std::string ReportGenerator::buildPath(const std::string& fileName) const {
    if (outputDir.empty()) {
        return fileName;
    }

    const char last = outputDir.back();
    if (last == '/' || last == '\\') {
        return outputDir + fileName;
    }

    return outputDir + "/" + fileName;
}

bool ReportGenerator::generarReportePacientes() {
    std::ofstream out(buildPath("reporte_pacientes.html"));
    if (!out.is_open()) return false;

    out << "<h1>Reporte de Pacientes</h1>\n";
    out.close();
    return true;
}

bool ReportGenerator::generarReporteMedicos() {
    std::ofstream out(buildPath("reporte_medicos.html"));
    if (!out.is_open()) return false;

    out << "<h1>Reporte de Medicos</h1>\n";
    out.close();
    return true;
}

bool ReportGenerator::generarReporteCitas() {
    std::ofstream out(buildPath("reporte_citas.html"));
    if (!out.is_open()) return false;

    out << "<h1>Reporte de Citas</h1>\n";
    out.close();
    return true;
}

bool ReportGenerator::generarReporteEstadisticas() {
    std::ofstream out(buildPath("reporte_estadisticas.html"));
    if (!out.is_open()) return false;

    out << "<h1>Reporte de Estadisticas</h1>\n";
    out.close();
    return true;
}

bool ReportGenerator::generarDiagramaGraphviz() {
    std::ofstream out(buildPath("diagrama.dot"));
    if (!out.is_open()) return false;

    out << "digraph MedLexer {\n";
    out << "  rankdir=LR;\n";
    out << "}\n";
    out.close();
    return true;
}
