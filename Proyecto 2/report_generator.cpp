// report_generator.cpp
// implementacion de generadores de reportes
#include "report_generator.h"
#include <fstream>
#include <map>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <functional>

// helpers para manejo de texto (minusculas/Mayusculas y escape para dot)
static std::string toLowerStr(const std::string &s) {
    std::string r; r.reserve(s.size());
    for (char ch : s) r.push_back(static_cast<char>(std::tolower((unsigned char)ch)));
    return r;
}

static std::string toUpperStr(const std::string &s) {
    std::string r; r.reserve(s.size());
    for (char ch : s) r.push_back(static_cast<char>(std::toupper((unsigned char)ch)));
    return r;
}

static std::string escapeDotLabel(const std::string &s) {
    std::string out;
    for (char c : s) {
        if (c == '\\') out += "\\\\";
        else if (c == '"') out += "\\\"";
        else if (c == '\n') out += "\\n";
        else out.push_back(c);
    }
    return out;
}

void reportgenerator::generarKanban(const ProgramNode *raiz) {
    if (!raiz) return;

    std::ofstream out("tablero.html");
    out << "<!DOCTYPE html>\n<html lang=\"es\">\n<head>\n"
        << "  <meta charset=\"UTF-8\">\n"
        << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        << "  <title>Kanban - " << raiz->tableroName << "</title>\n"
        << "  <style>\n"
        << "    * { margin: 0; padding: 0; box-sizing: border-box; }\n"
        << "    body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #f5f5f5; padding: 20px; }\n"
        << "    .container { max-width: 1400px; margin: 0 auto; }\n"
        << "    h1 { color: #333; margin-bottom: 30px; text-align: center; }\n"
        << "    .kanban-board { display: flex; gap: 20px; overflow-x: auto; padding: 10px; }\n"
        << "    .column { flex: 0 0 300px; background: #ecf0f1; border-radius: 8px; padding: 15px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }\n"
        << "    .column-title { font-weight: bold; color: #2c3e50; margin-bottom: 15px; font-size: 16px; }\n"
        << "    .task-card { background: white; border-radius: 6px; padding: 12px; margin-bottom: 10px; box-shadow: 0 1px 3px rgba(0,0,0,0.1); border-left: 4px solid #3498db; }\n"
        << "    .task-name { font-weight: 600; color: #2c3e50; margin-bottom: 8px; font-size: 14px; }\n"
        << "    .task-priority { display: inline-block; padding: 4px 8px; border-radius: 4px; font-size: 12px; font-weight: bold; margin-bottom: 6px; }\n"
        << "    .priority-alta { background: #e74c3c; color: white; }\n"
        << "    .priority-media { background: #f39c12; color: white; }\n"
        << "    .priority-baja { background: #27ae60; color: white; }\n"
        << "    .task-info { font-size: 12px; color: #555; margin: 4px 0; }\n"
        << "    .task-info strong { color: #2c3e50; }\n"
        << "  </style>\n"
        << "</head>\n<body>\n"
        << "  <div class=\"container\">\n"
        << "    <h1>Tablero Kanban: " << raiz->tableroName << "</h1>\n"
        << "    <div class=\"kanban-board\">\n";

    // recorremos columnas y tareas
    for (const auto &columna : raiz->columns) {
        out << "      <div class=\"column\">\n"
            << "        <div class=\"column-title\">" << columna->name << "</div>\n";

        for (const auto &tarea : columna->tasks) {
            out << "        <div class=\"task-card\">\n"
                << "          <div class=\"task-name\">" << tarea->name << "</div>\n";

            // aqui recorremos los atributos y agarramos la info real
            std::string prioridad = "BAJA"; // default
            std::string responsable = "N/A";
            std::string fecha = "N/A";

            for (const auto &attr : tarea->attributes) {
                std::string key = toLowerStr(attr->keyLexeme);
                if (key == "prioridad") {
                    prioridad = toUpperStr(attr->valueLexeme);
                } else if (key == "responsable") {
                    responsable = attr->valueLexeme;
                } else if (key == "fecha_limite" || key == "fecha-limite" || key == "fecha") {
                    fecha = attr->valueLexeme;
                }
            }

            std::string priorityClass = "priority-baja";
            if (toUpperStr(prioridad) == "ALTA") priorityClass = "priority-alta";
            else if (toUpperStr(prioridad) == "MEDIA") priorityClass = "priority-media";

            out << "          <div class=\"task-priority " << priorityClass << "\">" << prioridad << "</div>\n"
                << "          <div class=\"task-info\"><strong>Responsable:</strong> " << responsable << "</div>\n"
                << "          <div class=\"task-info\"><strong>Fecha límite:</strong> " << fecha << "</div>\n"
                << "        </div>\n";
        }

        out << "      </div>\n";
    }

    out << "    </div>\n"
        << "  </div>\n"
        << "</body>\n</html>\n";
    out.close();
}

void reportgenerator::generarCargaResponsables(const ProgramNode *raiz) {
    if (!raiz) return;

    // recolectamos carga por responsable: recorremos tareas y leemos atributos
    std::map<std::string, CargaResponsable> carga;

    for (const auto &columna : raiz->columns) {
        for (const auto &tarea : columna->tasks) {
            std::string resp = "";
            std::string prioridad = "BAJA";
            for (const auto &attr : tarea->attributes) {
                std::string key = toLowerStr(attr->keyLexeme);
                if (key == "responsable") resp = attr->valueLexeme;
                else if (key == "prioridad") prioridad = toUpperStr(attr->valueLexeme);
            }
            if (resp.empty()) continue; // si no tiene responsable lo ignoramos
            if (carga.find(resp) == carga.end()) carga[resp].nombre = resp;
            carga[resp].total++;
            if (prioridad == "ALTA") carga[resp].alta++;
            else if (prioridad == "MEDIA") carga[resp].media++;
            else carga[resp].baja++;
        }
    }

    std::ofstream out("responsables.html");
    out << "<!DOCTYPE html>\n<html lang=\"es\">\n<head>\n"
        << "  <meta charset=\"UTF-8\">\n"
        << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        << "  <title>Carga de Responsables</title>\n"
        << "  <style>\n"
        << "    * { margin: 0; padding: 0; box-sizing: border-box; }\n"
        << "    body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #f5f5f5; padding: 20px; }\n"
        << "    .container { max-width: 900px; margin: 0 auto; }\n"
        << "    h1 { color: #333; margin-bottom: 20px; text-align: center; }\n"
        << "    table { width: 100%; border-collapse: collapse; background: white; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }\n"
        << "    th { background: #2c3e50; color: white; padding: 12px; text-align: left; font-weight: 600; }\n"
        << "    td { padding: 12px; border-bottom: 1px solid #ecf0f1; }\n"
        << "    tr:hover { background: #f9f9f9; }\n"
        << "    .responsable-name { font-weight: 600; color: #2c3e50; }\n"
        << "    .priority-alta { color: #e74c3c; font-weight: bold; }\n"
        << "    .priority-media { color: #f39c12; font-weight: bold; }\n"
        << "    .priority-baja { color: #27ae60; font-weight: bold; }\n"
        << "    .bar-container { width: 200px; height: 20px; background: #ecf0f1; border-radius: 4px; overflow: hidden; display: flex; }\n"
        << "    .bar-alta { background: #e74c3c; height: 100%; display: flex; align-items: center; justify-content: center; color: white; font-size: 11px; }\n"
        << "    .bar-media { background: #f39c12; height: 100%; display: flex; align-items: center; justify-content: center; color: white; font-size: 11px; }\n"
        << "    .bar-baja { background: #27ae60; height: 100%; display: flex; align-items: center; justify-content: center; color: white; font-size: 11px; }\n"
        << "  </style>\n"
        << "</head>\n<body>\n"
        << "  <div class=\"container\">\n"
        << "    <h1>Carga de Trabajo por Responsable</h1>\n"
        << "    <table>\n"
        << "      <thead>\n"
        << "        <tr>\n"
        << "          <th>Responsable</th>\n"
        << "          <th>Total Tareas</th>\n"
        << "          <th>ALTA</th>\n"
        << "          <th>MEDIA</th>\n"
        << "          <th>BAJA</th>\n"
        << "          <th>Distribución</th>\n"
        << "        </tr>\n"
        << "      </thead>\n"
        << "      <tbody>\n";

    int totalTareas = 0;
    for (const auto &p : carga) totalTareas += p.second.total;

    for (const auto &p : carga) {
        const auto &r = p.second;
        int porcAlta = totalTareas > 0 ? (r.alta * 100) / totalTareas : 0;
        int porcMedia = totalTareas > 0 ? (r.media * 100) / totalTareas : 0;
        int porcBaja = totalTareas > 0 ? (r.baja * 100) / totalTareas : 0;

        out << "      <tr>\n"
            << "        <td class=\"responsable-name\">" << r.nombre << "</td>\n"
            << "        <td>" << r.total << "</td>\n"
            << "        <td><span class=\"priority-alta\">" << r.alta << "</span></td>\n"
            << "        <td><span class=\"priority-media\">" << r.media << "</span></td>\n"
            << "        <td><span class=\"priority-baja\">" << r.baja << "</span></td>\n"
            << "        <td>\n"
            << "          <div class=\"bar-container\">\n";

        if (porcAlta > 0)
            out << "            <div class=\"bar-alta\" style=\"width: " << porcAlta << "%; title=\"ALTA " << r.alta << "\"></div>\n";
        if (porcMedia > 0)
            out << "            <div class=\"bar-media\" style=\"width: " << porcMedia << "%; title=\"MEDIA " << r.media << "\"></div>\n";
        if (porcBaja > 0)
            out << "            <div class=\"bar-baja\" style=\"width: " << porcBaja << "%; title=\"BAJA " << r.baja << "\"></div>\n";

        out << "          </div>\n"
            << "        </td>\n"
            << "      </tr>\n";
    }

    out << "      </tbody>\n"
        << "    </table>\n"
        << "  </div>\n"
        << "</body>\n</html>\n";
    out.close();
}

// generamos un arbol de derivacion puro en dot (producciones y tokens)
void reportgenerator::generarArbolDot(const ProgramNode *raiz) {
    if (!raiz) return;

    // cabecera del dot y estilo global
    std::string contenido = "digraph Derivacion {\n  rankdir=TB;\n  node[shape=box];\n";
    int contador = 0;

    // funciones lambda para crear nodos y aristas con colores obligatorios
    auto addNode = [&](int id, const std::string &label, bool terminal) {
        std::string lbl = escapeDotLabel(label);
        if (terminal) {
            contenido += "  N" + std::to_string(id) + " [label=\"" + lbl + "\", style=filled, fillcolor=\"#D6EAF8\"];\n";
        } else {
            contenido += "  N" + std::to_string(id) + " [label=\"" + lbl + "\", style=filled, fillcolor=\"#2E75B6\", fontcolor=white];\n";
        }
    };

    std::function<int(const ProgramNode*)> genProgram;
    std::function<int(const ColumnNode*)> genColumn;
    std::function<int(const TaskNode*)> genTask;
    std::function<int(const std::vector<std::unique_ptr<AttributeNode>>&)> genAtributos;
    std::function<int(const AttributeNode*)> genAtributo;

    // generador de programa (produccion exacta)
    genProgram = [&](const ProgramNode *p) -> int {
        int id = contador++;
        addNode(id, "<programa>", false); // nodo no terminal

        // TABLERO terminal
        int t1 = contador++; addNode(t1, "TABLERO", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(t1) + ";\n";

        // CADENA (nombre tablero)
        int tname = contador++; addNode(tname, "\"" + p->tableroName + "\"", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(tname) + ";\n";

        // '{'
        int tLBr = contador++; addNode(tLBr, "{", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(tLBr) + ";\n";

        // <columnas>
        int colNode = contador++; addNode(colNode, "<columnas>", false); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(colNode) + ";\n";
        // generamos cada columna y la conectamos al nodo <columnas>
        for (const auto &col : p->columns) {
            int childCol = genColumn(col.get());
            contenido += "  N" + std::to_string(colNode) + " -> N" + std::to_string(childCol) + ";\n";
        }

        // '}'
        int tRBr = contador++; addNode(tRBr, "}", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(tRBr) + ";\n";

        // ';'
        int tSemi = contador++; addNode(tSemi, ";", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(tSemi) + ";\n";

        return id;
    };

    // columna ::= COLUMNA CADENA '{' <tareas> '}' ';'
    genColumn = [&](const ColumnNode *c) -> int {
        int id = contador++;
        addNode(id, "<columna>", false);

        int tCol = contador++; addNode(tCol, "COLUMNA", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(tCol) + ";\n";
        int tName = contador++; addNode(tName, "\"" + c->name + "\"", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(tName) + ";\n";
        int tL = contador++; addNode(tL, "{", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(tL) + ";\n";

        int tareasNode = contador++; addNode(tareasNode, "<tareas>", false); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(tareasNode) + ";\n";
        // cada tarea y comas entre ellas
        for (size_t i = 0; i < c->tasks.size(); ++i) {
            int tchild = genTask(c->tasks[i].get());
            contenido += "  N" + std::to_string(tareasNode) + " -> N" + std::to_string(tchild) + ";\n";
            if (i + 1 < c->tasks.size()) {
                int comma = contador++; addNode(comma, ",", true);
                contenido += "  N" + std::to_string(tareasNode) + " -> N" + std::to_string(comma) + ";\n";
            }
        }

        int tR = contador++; addNode(tR, "}", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(tR) + ";\n";
        int tS = contador++; addNode(tS, ";", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(tS) + ";\n";

        return id;
    };

    // tarea ::= TAREA_KW ':' CADENA '[' <atributos> ']'
    genTask = [&](const TaskNode *t) -> int {
        int id = contador++;
        addNode(id, "<tarea>", false);

        int tkw = contador++; addNode(tkw, "TAREA", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(tkw) + ";\n";
        int td = contador++; addNode(td, ":", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(td) + ";\n";
        int tname = contador++; addNode(tname, "\"" + t->name + "\"", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(tname) + ";\n";
        int tL = contador++; addNode(tL, "[", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(tL) + ";\n";

        int attrsNode = genAtributos(t->attributes);
        contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(attrsNode) + ";\n";

        int tR = contador++; addNode(tR, "]", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(tR) + ";\n";

        return id;
    };

    // atributos ::= atributo ("," atributos)?  -> representamos como lista de atributos
    genAtributos = [&](const std::vector<std::unique_ptr<AttributeNode>> &attrs) -> int {
        int id = contador++;
        addNode(id, "<atributos>", false);
        for (size_t i = 0; i < attrs.size(); ++i) {
            int child = genAtributo(attrs[i].get());
            contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(child) + ";\n";
            if (i + 1 < attrs.size()) {
                int comma = contador++; addNode(comma, ",", true);
                contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(comma) + ";\n";
            }
        }
        return id;
    };

    // atributo ::= PRIORIDAD_KW ':' (ALTA|MEDIA|BAJA) | RESPONSABLE_KW ':' CADENA | FECHA_LIMITE_KW ':' FECHA
    genAtributo = [&](const AttributeNode *a) -> int {
        int id = contador++;
        addNode(id, "<atributo>", false);
        std::string key = toLowerStr(a->keyLexeme);
        if (key == "prioridad") {
            int k = contador++; addNode(k, "PRIORIDAD", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(k) + ";\n";
            int d = contador++; addNode(d, ":", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(d) + ";\n";
            int val = contador++; addNode(val, toUpperStr(a->valueLexeme), true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(val) + ";\n";
        } else if (key == "responsable") {
            int k = contador++; addNode(k, "RESPONSABLE", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(k) + ";\n";
            int d = contador++; addNode(d, ":", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(d) + ";\n";
            int val = contador++; addNode(val, "\"" + a->valueLexeme + "\"", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(val) + ";\n";
        } else { // fecha_limite
            int k = contador++; addNode(k, "FECHA_LIMITE", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(k) + ";\n";
            int d = contador++; addNode(d, ":", true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(d) + ";\n";
            int val = contador++; addNode(val, a->valueLexeme, true); contenido += "  N" + std::to_string(id) + " -> N" + std::to_string(val) + ";\n";
        }
        return id;
    };

    // arrancamos por el programa
    genProgram(raiz);

    contenido += "}\n";

    std::ofstream out("arbol.dot");
    out << contenido;
    out.close();
}

void reportgenerator::generarHtmlErrores(const std::vector<error_t> &errores) {
    std::ofstream out("errores.html");
    out << "<!DOCTYPE html>\n<html lang=\"es\">\n<head>\n"
        << "  <meta charset=\"UTF-8\">\n"
        << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        << "  <title>Reporte de Errores</title>\n"
        << "  <style>\n"
        << "    * { margin: 0; padding: 0; box-sizing: border-box; }\n"
        << "    body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #f5f5f5; padding: 20px; }\n"
        << "    .container { max-width: 1000px; margin: 0 auto; }\n"
        << "    h1 { color: #333; margin-bottom: 20px; text-align: center; }\n"
        << "    .error-count { background: #e74c3c; color: white; padding: 10px 15px; border-radius: 4px; display: inline-block; margin-bottom: 15px; font-weight: bold; }\n"
        << "    table { width: 100%; border-collapse: collapse; background: white; box-shadow: 0 2px 4px rgba(0,0,0,0.1); margin-top: 10px; }\n"
        << "    th { background: #2c3e50; color: white; padding: 12px; text-align: left; font-weight: 600; }\n"
        << "    td { padding: 12px; border-bottom: 1px solid #ecf0f1; font-size: 14px; }\n"
        << "    tr:hover { background: #f9f9f9; }\n"
        << "    .tipo-lexico { background: #ffffcc; color: #333; padding: 4px 8px; border-radius: 4px; font-size: 12px; font-weight: bold; }\n"
        << "    .tipo-sintactico { background: #ffcccc; color: #333; padding: 4px 8px; border-radius: 4px; font-size: 12px; font-weight: bold; }\n"
        << "  </style>\n"
        << "</head>\n<body>\n"
        << "  <div class=\"container\">\n"
        << "    <h1>Reporte de Errores</h1>\n";

    if (errores.empty()) {
        out << "    <p style=\"color: #27ae60; font-weight: bold; font-size: 16px;\">No hay errores. Compilación exitosa.</p>\n";
    } else {
        out << "    <div class=\"error-count\">Total: " << errores.size() << " errores</div>\n"
            << "    <table>\n"
            << "      <thead>\n"
            << "        <tr>\n"
            << "          <th>No.</th>\n"
            << "          <th>Lexema</th>\n"
            << "          <th>Tipo</th>\n"
            << "          <th>Descripción</th>\n"
            << "          <th>Línea</th>\n"
            << "          <th>Columna</th>\n"
            << "        </tr>\n"
            << "      </thead>\n"
            << "      <tbody>\n";

        for (const auto &err : errores) {
            std::string tipoClass = (err.tipo == "lexico") ? "tipo-lexico" : "tipo-sintactico";
            out << "      <tr>\n"
                << "        <td>" << err.number << "</td>\n"
                << "        <td><code>" << err.lexeme << "</code></td>\n"
                << "        <td><span class=\"" << tipoClass << "\">" << err.tipo << "</span></td>\n"
                << "        <td>" << err.descripcion << "</td>\n"
                << "        <td>" << err.line << "</td>\n"
                << "        <td>" << err.column << "</td>\n"
                << "      </tr>\n";
        }

        out << "      </tbody>\n"
            << "    </table>\n";
    }

    out << "  </div>\n"
        << "</body>\n</html>\n";
    out.close();
}
