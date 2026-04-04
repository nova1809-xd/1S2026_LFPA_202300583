#include "ReportGenerator.h"

#include <fstream>
#include <unordered_map>

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

bool ReportGenerator::generarReportePacientes(const std::vector<PacienteFila>& filas) {
    std::ofstream out(buildPath("reporte_pacientes.html"));
    if (!out.is_open()) return false;

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

bool ReportGenerator::generarReporteMedicos(const std::vector<MedicoFila>& filas) {
    std::ofstream out(buildPath("reporte_medicos.html"));
    if (!out.is_open()) return false;

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

bool ReportGenerator::generarReporteCitas(const std::vector<CitaFila>& filas) {
    std::ofstream out(buildPath("reporte_citas.html"));
    if (!out.is_open()) return false;

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

bool ReportGenerator::generarReporteEstadisticas(
    const std::vector<PacienteFila>& pacientes,
    const std::vector<MedicoFila>& medicos,
    const std::vector<CitaFila>& citas) {

    std::ofstream out(buildPath("reporte_estadisticas.html"));
    if (!out.is_open()) return false;

    int conflictos = 0;
    std::unordered_map<std::string, int> agenda;
    for (const auto& c : citas) {
        std::string clave = c.medico + "|" + c.fecha + "|" + c.hora;
        agenda[clave]++;
    }
    for (const auto& entry : agenda) {
        if (entry.second > 1) {
            conflictos += entry.second;
        }
    }

    std::unordered_map<std::string, int> medicosPorEspecialidad;
    std::unordered_map<std::string, int> citasPorEspecialidad;
    std::unordered_map<std::string, int> pacientesPorEspecialidad;
    std::unordered_map<std::string, std::string> especialidadPorCodigo;

    for (const auto& m : medicos) {
        medicosPorEspecialidad[m.especialidad]++;
        especialidadPorCodigo[m.codigo] = m.especialidad;
    }
    for (const auto& c : citas) {
        auto it = especialidadPorCodigo.find(c.medico);
        if (it != especialidadPorCodigo.end()) {
            citasPorEspecialidad[it->second]++;
            pacientesPorEspecialidad[it->second]++;
        }
    }

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
    html += "<div class='card'><strong>Pacientes</strong><br>" + std::to_string(pacientes.size()) + "</div>";
    html += "<div class='card'><strong>Medicos</strong><br>" + std::to_string(medicos.size()) + "</div>";
    html += "<div class='card'><strong>Citas</strong><br>" + std::to_string(citas.size()) + "</div>";
    html += "<div class='card'><strong>Conflictos</strong><br>" + std::to_string(conflictos) + "</div>";
    html += "</div>";

    html += "<h2>Tabla de especialidades</h2>";
    html += "<table><thead><tr><th>Especialidad</th><th>Medicos</th><th>Citas</th><th>Pacientes Atendidos</th></tr></thead><tbody>";
    for (const auto& entry : medicosPorEspecialidad) {
        const std::string& especialidad = entry.first;
        html += "<tr>";
        html += "<td>" + especialidad + "</td>";
        html += "<td>" + std::to_string(entry.second) + "</td>";
        html += "<td>" + std::to_string(citasPorEspecialidad[especialidad]) + "</td>";
        html += "<td>" + std::to_string(pacientesPorEspecialidad[especialidad]) + "</td>";
        html += "</tr>";
    }
    html += "</tbody></table></body></html>";

    out << html;
    out.close();
    return true;
}

bool ReportGenerator::generarDiagramaGraphviz(
    const std::vector<PacienteFila>& pacientes,
    const std::vector<MedicoFila>& medicos,
    const std::vector<CitaFila>& citas) {

    std::ofstream out(buildPath("diagrama.dot"));
    if (!out.is_open()) return false;

    std::string dot;
    dot += "digraph MedLexer {\n";
    dot += "  rankdir=LR;\n";
    dot += "  node [shape=box, style=filled, fillcolor=lightblue, fontname=Helvetica];\n";
    dot += "  HOSPITAL [label=\"HOSPITAL\"];\n";
    dot += "  PACIENTES [label=\"PACIENTES (" + std::to_string(pacientes.size()) + ")\"];\n";
    dot += "  MEDICOS [label=\"MEDICOS (" + std::to_string(medicos.size()) + ")\"];\n";
    dot += "  CITAS [label=\"CITAS (" + std::to_string(citas.size()) + ")\"];\n";
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
