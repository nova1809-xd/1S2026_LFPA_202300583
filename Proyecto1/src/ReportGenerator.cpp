#include "ReportGenerator.h"

#include <fstream>
#include <vector>
#include <unordered_map>

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

    std::vector<PacienteFila> filas = {
        {"Ana Lopez", 25, "O+", "Hipertension", "Losartan", "Estable"},
        {"Luis Perez", 61, "A-", "Diabetes", "Metformina", "Controlado"},
        {"Marta Diaz", 39, "AB+", "Migraña", "Sumatriptan", "Seguimiento"},
        {"Iris Mora", 33, "B+", "Asma", "Salbutamol", "Estable"}
    };

    std::string html;
    html += "<!DOCTYPE html><html><head><meta charset='utf-8'>";
    html += "<title>Reporte de Pacientes</title>";
    html += "<style>";
    html += "body{font-family:Segoe UI,Arial,sans-serif;background:#f7fafc;margin:24px;}";
    html += "h1{color:#1a365d;margin-bottom:12px;}";
    html += "table{width:100%;border-collapse:collapse;background:#fff;}";
    html += "th,td{border:1px solid #e2e8f0;padding:10px;text-align:left;}";
    html += "th{background:#2b6cb0;color:#fff;}";
    html += "tr:nth-child(even){background:#edf2f7;}";
    html += "</style></head><body>";
    html += "<h1>Reporte de Pacientes</h1>";
    html += "<table><thead><tr>";
    html += "<th>Paciente</th><th>Edad</th><th>Sangre</th><th>Diagnostico</th><th>Medicamento</th><th>Estado</th>";
    html += "</tr></thead><tbody>";

    for (const auto& f : filas) {
        html += "<tr>";
        html += "<td>" + f.paciente + "</td>";
        html += "<td>" + std::to_string(f.edad) + "</td>";
        html += "<td>" + f.sangre + "</td>";
        html += "<td>" + f.diagnostico + "</td>";
        html += "<td>" + f.medicamento + "</td>";
        html += "<td>" + f.estado + "</td>";
        html += "</tr>";
    }

    html += "</tbody></table></body></html>";
    out << html;
    out.close();
    return true;
}

bool ReportGenerator::generarReporteMedicos() {
    std::ofstream out(buildPath("reporte_medicos.html"));
    if (!out.is_open()) return false;

    std::vector<MedicoFila> filas = {
        {"Dr. Ruiz", "MED-001", "CARDIOLOGIA", 18, 15, "Alta"},
        {"Dra. Solis", "MED-010", "NEUROLOGIA", 11, 9, "Media"},
        {"Dr. Vega", "MED-101", "ONCOLOGIA", 7, 6, "Media"},
        {"Dra. Neri", "MED-003", "PEDIATRIA", 4, 4, "Baja"}
    };

    std::string html;
    html += "<!DOCTYPE html><html><head><meta charset='utf-8'>";
    html += "<title>Reporte de Medicos</title>";
    html += "<style>";
    html += "body{font-family:Segoe UI,Arial,sans-serif;background:#f7fafc;margin:24px;}";
    html += "h1{color:#234e52;margin-bottom:12px;}";
    html += "table{width:100%;border-collapse:collapse;background:#fff;}";
    html += "th,td{border:1px solid #e2e8f0;padding:10px;text-align:left;}";
    html += "th{background:#2c7a7b;color:#fff;}";
    html += "tr:nth-child(even){background:#edf2f7;}";
    html += "</style></head><body>";
    html += "<h1>Reporte de Medicos</h1>";
    html += "<table><thead><tr>";
    html += "<th>Medico</th><th>Codigo</th><th>Especialidad</th><th>Citas</th><th>Pacientes</th><th>Nivel de Carga</th>";
    html += "</tr></thead><tbody>";

    for (const auto& f : filas) {
        html += "<tr>";
        html += "<td>" + f.medico + "</td>";
        html += "<td>" + f.codigo + "</td>";
        html += "<td>" + f.especialidad + "</td>";
        html += "<td>" + std::to_string(f.citas) + "</td>";
        html += "<td>" + std::to_string(f.pacientes) + "</td>";
        html += "<td>" + f.nivelCarga + "</td>";
        html += "</tr>";
    }

    html += "</tbody></table></body></html>";
    out << html;
    out.close();
    return true;
}

bool ReportGenerator::generarReporteCitas() {
    std::ofstream out(buildPath("reporte_citas.html"));
    if (!out.is_open()) return false;

    std::vector<CitaFila> filas = {
        {"CIT-001", "PAC-001", "MED-001", "2026-04-10", "09:30"},
        {"CIT-002", "PAC-010", "MED-001", "2026-04-10", "09:30"},
        {"CIT-003", "PAC-101", "MED-010", "2026-04-11", "14:05"},
        {"CIT-004", "PAC-003", "MED-003", "2026-04-12", "11:00"}
    };

    std::unordered_map<std::string, int> agenda;
    for (const auto& f : filas) {
        std::string clave = f.medico + "|" + f.fecha + "|" + f.hora;
        agenda[clave]++;
    }

    std::string html;
    html += "<!DOCTYPE html><html><head><meta charset='utf-8'>";
    html += "<title>Reporte de Citas</title>";
    html += "<style>";
    html += "body{font-family:Segoe UI,Arial,sans-serif;background:#fffaf0;margin:24px;}";
    html += "h1{color:#7b341e;margin-bottom:12px;}";
    html += "table{width:100%;border-collapse:collapse;background:#fff;}";
    html += "th,td{border:1px solid #fbd38d;padding:10px;text-align:left;}";
    html += "th{background:#dd6b20;color:#fff;}";
    html += "tr:nth-child(even){background:#fff5f0;}";
    html += "tr.conflicto{background:#fed7d7 !important;}";
    html += "</style></head><body>";
    html += "<h1>Reporte de Citas</h1>";
    html += "<table><thead><tr>";
    html += "<th>Codigo</th><th>Paciente</th><th>Medico</th><th>Fecha</th><th>Hora</th><th>Conflicto</th>";
    html += "</tr></thead><tbody>";

    for (const auto& f : filas) {
        std::string clave = f.medico + "|" + f.fecha + "|" + f.hora;
        bool conflicto = agenda[clave] > 1;
        html += conflicto ? "<tr class='conflicto'>" : "<tr>";
        html += "<td>" + f.codigo + "</td>";
        html += "<td>" + f.paciente + "</td>";
        html += "<td>" + f.medico + "</td>";
        html += "<td>" + f.fecha + "</td>";
        html += "<td>" + f.hora + "</td>";
        html += "<td>" + std::string(conflicto ? "SI" : "NO") + "</td>";
        html += "</tr>";
    }

    html += "</tbody></table></body></html>";
    out << html;
    out.close();
    return true;
}

bool ReportGenerator::generarReporteEstadisticas() {
    std::ofstream out(buildPath("reporte_estadisticas.html"));
    if (!out.is_open()) return false;

    std::string html;
    html += "<!DOCTYPE html><html><head><meta charset='utf-8'>";
    html += "<title>Reporte de Estadisticas</title>";
    html += "<style>";
    html += "body{font-family:Segoe UI,Arial,sans-serif;background:#f0fff4;margin:24px;}";
    html += "h1,h2{color:#22543d;}";
    html += ".cards{display:flex;gap:12px;flex-wrap:wrap;margin-bottom:18px;}";
    html += ".card{background:#fff;border:1px solid #9ae6b4;padding:12px 16px;border-radius:8px;min-width:170px;}";
    html += "table{width:100%;border-collapse:collapse;background:#fff;}";
    html += "th,td{border:1px solid #c6f6d5;padding:10px;text-align:left;}";
    html += "th{background:#38a169;color:#fff;}";
    html += "</style></head><body>";
    html += "<h1>Reporte de Estadisticas</h1>";

    html += "<h2>Indicadores clave</h2>";
    html += "<div class='cards'>";
    html += "<div class='card'><strong>Pacientes</strong><br>124</div>";
    html += "<div class='card'><strong>Medicos</strong><br>21</div>";
    html += "<div class='card'><strong>Citas</strong><br>318</div>";
    html += "<div class='card'><strong>Conflictos</strong><br>3</div>";
    html += "</div>";

    html += "<h2>Tabla de especialidades</h2>";
    html += "<table><thead><tr><th>Especialidad</th><th>Medicos</th><th>Citas</th><th>Pacientes Atendidos</th></tr></thead><tbody>";
    html += "<tr><td>CARDIOLOGIA</td><td>5</td><td>82</td><td>61</td></tr>";
    html += "<tr><td>NEUROLOGIA</td><td>3</td><td>41</td><td>36</td></tr>";
    html += "<tr><td>ONCOLOGIA</td><td>4</td><td>57</td><td>40</td></tr>";
    html += "<tr><td>PEDIATRIA</td><td>6</td><td>96</td><td>73</td></tr>";
    html += "</tbody></table></body></html>";

    out << html;
    out.close();
    return true;
}

bool ReportGenerator::generarDiagramaGraphviz() {
    std::ofstream out(buildPath("diagrama.dot"));
    if (!out.is_open()) return false;

    std::string dot;
    dot += "digraph MedLexer {\n";
    dot += "  rankdir=LR;\n";
    dot += "  node [shape=box, style=filled, fillcolor=lightblue, fontname=Helvetica];\n";
    dot += "  HOSPITAL [label=\"HOSPITAL\"];\n";
    dot += "  PACIENTES [label=\"PACIENTES\"];\n";
    dot += "  MEDICOS [label=\"MEDICOS\"];\n";
    dot += "  CITAS [label=\"CITAS\"];\n";
    dot += "  DIAGNOSTICOS [label=\"DIAGNOSTICOS\"];\n";
    dot += "  HOSPITAL -> PACIENTES;\n";
    dot += "  HOSPITAL -> MEDICOS;\n";
    dot += "  HOSPITAL -> CITAS;\n";
    dot += "  HOSPITAL -> DIAGNOSTICOS;\n";
    dot += "  PACIENTES -> CITAS;\n";
    dot += "  MEDICOS -> CITAS;\n";
    dot += "  CITAS -> DIAGNOSTICOS;\n";
    dot += "}\n";

    out << dot;
    out.close();
    return true;
}
