// main.cpp
// prueba para correr el lexer y el parser de taskscript
// validamos arbolito, panic mode y generamos reportes
#include "syntax_analyzer.h"
#include "report_generator.h"
#include <fstream>
#include <iostream>
#include <string>

// imprime el arbol con indentacion para ver la jerarquia
static void printAST(const AstNode *nodo, int nivel = 0) {
    if (!nodo) return;

    std::string indent(nivel * 2, ' ');

    if (auto prog = dynamic_cast<const ProgramNode*>(nodo)) {
        std::cout << indent << "PROGRAMA\n";
        std::cout << indent << "  tablero: " << prog->tableroName << '\n';
        for (const auto &col : prog->columns) {
            printAST(col.get(), nivel + 1);
        }
    } else if (auto col = dynamic_cast<const ColumnNode*>(nodo)) {
        std::cout << indent << "COLUMNA: " << col->name << '\n';
        for (const auto &tarea : col->tasks) {
            printAST(tarea.get(), nivel + 1);
        }
    } else if (auto tarea = dynamic_cast<const TaskNode*>(nodo)) {
        std::cout << indent << "TAREA: " << tarea->name << '\n';
        for (const auto &attr : tarea->attributes) {
            printAST(attr.get(), nivel + 1);
        }
    } else if (auto attr = dynamic_cast<const AttributeNode*>(nodo)) {
        std::cout << indent << "ATRIBUTO: " << attr->keyLexeme << " = " << attr->valueLexeme << '\n';
    }
}

int main() {
    // primero procesamos prueba.task (sin errores) para generar reportes de contenido
    std::cout << "\n=== procesando: prueba.task (para reportes) ===\n";

    std::ifstream archivo1("prueba.task");
    if (!archivo1.is_open()) {
        std::cerr << "no se pudo abrir prueba.task\n";
        return 1;
    }

    errores.clear();
    lexicalanalyzer lexer1(archivo1);
    syntaxanalyzer parser1(lexer1);
    std::unique_ptr<ProgramNode> arbol1 = parser1.parse();

    if (arbol1) {
        std::cout << "arbolito parseado (para reportes)\n";
        
        // generamos los 3 reportes del contenido
        reportgenerator gen;
        gen.generarKanban(arbol1.get());
        std::cout << "  -> generado tablero.html\n";
        
        gen.generarCargaResponsables(arbol1.get());
        std::cout << "  -> generado responsables.html\n";
        
        gen.generarArbolDot(arbol1.get());
        std::cout << "  -> generado arbol.dot\n";
    } else {
        std::cout << "no se armo el arbol, no se generan reportes\n";
    }

    // ahora procesamos prueba_error.task para capturar los errores
    std::cout << "\n=== procesando: prueba_error.task (para reporte de errores) ===\n";

    std::ifstream archivo2("prueba_error.task");
    if (!archivo2.is_open()) {
        std::cerr << "no se pudo abrir prueba_error.task\n";
        return 1;
    }

    errores.clear();
    lexicalanalyzer lexer2(archivo2);
    syntaxanalyzer parser2(lexer2);
    std::unique_ptr<ProgramNode> arbol2 = parser2.parse();

    if (!errores.empty()) {
        std::cout << "errores capturados: " << errores.size() << '\n';
        for (const auto &err : errores) {
            std::cout << "  error #" << err.number
                      << " [" << err.tipo << "]"
                      << " linea " << err.line
                      << ", col " << err.column
                      << " -> '" << err.lexeme << "'"
                      << " | " << err.descripcion << '\n';
        }
    } else {
        std::cout << "parseo sin errores\n";
    }

    // generamos el reporte de errores (aunque este vacio, lo creamos igual)
    reportgenerator gen;
    gen.generarHtmlErrores(errores);
    std::cout << "  -> generado errores.html\n";

    std::cout << "\nreportes generados en la carpeta actual:\n"
              << "  - tablero.html (vista kanban)\n"
              << "  - responsables.html (carga de trabajo)\n"
              << "  - arbol.dot (diagrama graphviz)\n"
              << "  - errores.html (reporte de errores)\n";

    return 0;
}
