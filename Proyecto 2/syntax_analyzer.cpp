// syntax_analyzer.cpp
// implementación del parser recursivo descendente con panic-mode
#include "syntax_analyzer.h"
#include <sstream>

// vector global de errores
std::vector<error_t> errores;

syntaxanalyzer::syntaxanalyzer(lexicalanalyzer &lx) : lex(lx), errorCount(0) {
    // cargamos el primer token
    advance();
}

void syntaxanalyzer::advance() {
    current = lex.nextToken();
}

void syntaxanalyzer::panic(const std::string &msg) {
    // registramos un error sintactico con el token actual
    error_t e(++errorCount, current.lexeme, "sintactico", msg, current.line, current.column);
    errores.push_back(e);
    // aplicamos panic mode: avanzamos hasta token de sincronizacion
    synchronize();
}

void syntaxanalyzer::synchronize() {
    // avanzamos hasta ; , } o ] o EOF
    while (current.type != tokentype::PUNTO_COMA &&
           current.type != tokentype::LLAVE_C &&
           current.type != tokentype::CORCHETE_C &&
           current.type != tokentype::EOF_TOKEN) {
        advance();
    }
    // si estamos en un token de sincronizacion lo consumimos para continuar
    if (current.type != tokentype::EOF_TOKEN) advance();
}

std::unique_ptr<ProgramNode> syntaxanalyzer::parse() {
    return parseProgram();
}

// programa ::= TABLERO CADENA "{" columnas "}" ";"
std::unique_ptr<ProgramNode> syntaxanalyzer::parseProgram() {
    // checamos TABLERO
    if (current.type != tokentype::TABLERO) {
        panic("se esperaba 'TABLERO' al inicio del programa");
        return nullptr;
    }
    // guardamos posicion y avanzamos
    int l = current.line; int c = current.column;
    advance();

    // esperamos CADENA
    if (current.type != tokentype::CADENA) {
        panic("se esperaba un nombre de tablero (CADENA)");
        return nullptr;
    }
    std::string tableroName = current.lexeme;
    int nameLine = current.line; int nameCol = current.column;
    advance();

    // esperamos '{'
    if (current.type != tokentype::LLAVE_A) {
        panic("se esperaba '{' despues del nombre del tablero");
        return nullptr;
    }
    advance();

    // parse columnas
    auto cols = parseColumns();

    // esperamos '}'
    if (current.type != tokentype::LLAVE_C) {
        panic("se esperaba '}' al finalizar las columnas");
        return nullptr;
    }
    advance();

    // esperamos ';'
    if (current.type != tokentype::PUNTO_COMA) {
        panic("se esperaba ';' al final del programa");
        return nullptr;
    }
    advance();

    auto prog = std::make_unique<ProgramNode>(tableroName, nameLine, nameCol);
    for (auto &col : cols) prog->columns.push_back(std::move(col));
    return prog;
}

// columnas ::= columna columnas | columna  => implementado como 1+ columnas
std::vector<std::unique_ptr<ColumnNode>> syntaxanalyzer::parseColumns() {
    std::vector<std::unique_ptr<ColumnNode>> res;
    while (current.type == tokentype::COLUMNA) {
        auto col = parseColumn();
        if (col) res.push_back(std::move(col));
        else {
            // si hubo error, panic() se encargó de sincronizar, continuamos
            if (current.type == tokentype::EOF_TOKEN) break;
        }
    }
    return res;
}

// columna ::= COLUMNA CADENA "{" tareas "}" ";"
std::unique_ptr<ColumnNode> syntaxanalyzer::parseColumn() {
    if (current.type != tokentype::COLUMNA) {
        panic("se esperaba 'COLUMNA' al iniciar una columna");
        return nullptr;
    }
    int startLine = current.line; int startCol = current.column;
    advance();

    if (current.type != tokentype::CADENA) {
        panic("se esperaba nombre de columna (CADENA)");
        return nullptr;
    }
    std::string colName = current.lexeme;
    int nameLine = current.line; int nameCol = current.column;
    advance();

    if (current.type != tokentype::LLAVE_A) {
        panic("se esperaba '{' despues del nombre de la columna");
        return nullptr;
    }
    advance();

    auto tasks = parseTasks();

    if (current.type != tokentype::LLAVE_C) {
        panic("se esperaba '}' al finalizar las tareas de la columna");
        return nullptr;
    }
    advance();

    if (current.type != tokentype::PUNTO_COMA) {
        panic("se esperaba ';' despues de la columna");
        return nullptr;
    }
    advance();

    auto colnode = std::make_unique<ColumnNode>(colName, nameLine, nameCol);
    for (auto &t : tasks) colnode->tasks.push_back(std::move(t));
    return colnode;
}

// tareas ::= tarea "," tareas | tarea  => 1+ separadas por coma
std::vector<std::unique_ptr<TaskNode>> syntaxanalyzer::parseTasks() {
    std::vector<std::unique_ptr<TaskNode>> res;
    auto t = parseTask();
    if (!t) return res; // si primer tarea falla, devolvemos vacio (panic ya aplicado)
    res.push_back(std::move(t));
    while (current.type == tokentype::COMA) {
        advance(); // consumimos la coma
        auto t2 = parseTask();
        if (t2) res.push_back(std::move(t2));
        else break; // si falla, panic ya hecho
    }
    return res;
}

// tarea ::= TAREA_KW ":" CADENA "[" atributos "]"
std::unique_ptr<TaskNode> syntaxanalyzer::parseTask() {
    if (current.type != tokentype::TAREA_KW) {
        panic("se esperaba 'TAREA' al iniciar una tarea");
        return nullptr;
    }
    int tLine = current.line; int tCol = current.column;
    advance();

    if (current.type != tokentype::DOS_PUNTOS) {
        panic("se esperaba ':' despues de TAREA");
        return nullptr;
    }
    advance();

    if (current.type != tokentype::CADENA) {
        panic("se esperaba el nombre de la tarea (CADENA)");
        return nullptr;
    }
    std::string tareaName = current.lexeme;
    int nameLine = current.line; int nameCol = current.column;
    advance();

    if (current.type != tokentype::CORCHETE_A) {
        panic("se esperaba '[' antes de los atributos de la tarea");
        return nullptr;
    }
    advance();

    auto attrs = parseAttributes();

    if (current.type != tokentype::CORCHETE_C) {
        panic("se esperaba ']' al finalizar atributos");
        return nullptr;
    }
    advance();

    auto tasknode = std::make_unique<TaskNode>(tareaName, nameLine, nameCol);
    for (auto &a : attrs) tasknode->attributes.push_back(std::move(a));
    return tasknode;
}

// atributos ::= atributo ("," atributos)?
std::vector<std::unique_ptr<AttributeNode>> syntaxanalyzer::parseAttributes() {
    std::vector<std::unique_ptr<AttributeNode>> res;
    auto a = parseAttribute();
    if (!a) return res;
    res.push_back(std::move(a));
    while (current.type == tokentype::COMA) {
        advance();
        auto a2 = parseAttribute();
        if (a2) res.push_back(std::move(a2));
        else break;
    }
    return res;
}

// atributo ::= PRIORIDAD_KW ":" (ALTA|MEDIA|BAJA) | RESPONSABLE_KW ":" CADENA | FECHA_LIMITE_KW ":" FECHA
std::unique_ptr<AttributeNode> syntaxanalyzer::parseAttribute() {
    if (current.type == tokentype::PRIORIDAD_KW) {
        int l = current.line; int c = current.column;
        std::string key = current.lexeme;
        advance();
        if (current.type != tokentype::DOS_PUNTOS) { panic("se esperaba ':' despues de PRIORIDAD"); return nullptr; }
        advance();
        if (current.type != tokentype::ALTA && current.type != tokentype::MEDIA && current.type != tokentype::BAJA) {
            panic("se esperaba ALTA|MEDIA|BAJA como valor de PRIORIDAD");
            return nullptr;
        }
        std::string val = current.lexeme;
        tokentype vt = current.type;
        advance();
        return std::make_unique<AttributeNode>(key, val, vt, l, c);
    } else if (current.type == tokentype::RESPONSABLE_KW) {
        int l = current.line; int c = current.column;
        std::string key = current.lexeme;
        advance();
        if (current.type != tokentype::DOS_PUNTOS) { panic("se esperaba ':' despues de RESPONSABLE"); return nullptr; }
        advance();
        if (current.type != tokentype::CADENA) { panic("se esperaba CADENA como responsable"); return nullptr; }
        std::string val = current.lexeme;
        tokentype vt = current.type;
        advance();
        return std::make_unique<AttributeNode>(key, val, vt, l, c);
    } else if (current.type == tokentype::FECHA_LIMITE_KW) {
        int l = current.line; int c = current.column;
        std::string key = current.lexeme;
        advance();
        if (current.type != tokentype::DOS_PUNTOS) { panic("se esperaba ':' despues de FECHA_LIMITE"); return nullptr; }
        advance();
        if (current.type != tokentype::FECHA) { panic("se esperaba FECHA como valor de FECHA_LIMITE"); return nullptr; }
        std::string val = current.lexeme;
        tokentype vt = current.type;
        advance();
        return std::make_unique<AttributeNode>(key, val, vt, l, c);
    } else {
        panic("se esperaba un atributo (PRIORIDAD/RESPONSABLE/FECHA_LIMITE)");
        return nullptr;
    }
}
