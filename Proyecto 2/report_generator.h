// report_generator.h
// generador de reportes html y graphviz para taskscript
#ifndef PROYECTO2_REPORT_GENERATOR_H
#define PROYECTO2_REPORT_GENERATOR_H

#include "ast.h"
#include "error.h"
#include <vector>
#include <string>

class reportgenerator {
public:
    // genera tablero.html: vista kanban tipo trello
    void generarKanban(const ProgramNode *raiz);

    // genera responsables.html: carga de trabajo por responsable
    void generarCargaResponsables(const ProgramNode *raiz);

    // genera arbol.dot: diagrama de derivacion en graphviz
    void generarArbolDot(const ProgramNode *raiz);

    // genera errores.html: tabla de errores lexicos y sintacticos
    void generarHtmlErrores(const std::vector<error_t> &errores);

private:
    // helper para recorrer el arbol en arbol.dot
    void generarNodoDot(const AstNode *nodo, int &contador, std::string &contenido);

    // contar tareas y atributos por responsable
    struct CargaResponsable {
        std::string nombre;
        int total = 0;
        int alta = 0;
        int media = 0;
        int baja = 0;
    };
};

#endif // PROYECTO2_REPORT_GENERATOR_H
