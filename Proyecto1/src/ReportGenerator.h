#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include <string>

class ReportGenerator {
private:
    std::string outputDir;

    std::string buildPath(const std::string& fileName) const;

public:
    explicit ReportGenerator(const std::string& outputDirectory = "docs");

    bool generarReportePacientes();
    bool generarReporteMedicos();
    bool generarReporteCitas();
    bool generarReporteEstadisticas();
    bool generarDiagramaGraphviz();
};

#endif // REPORT_GENERATOR_H
