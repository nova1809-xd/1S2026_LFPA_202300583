// error.h
// clase para almacenar errores lexico y sintacticos
#ifndef PROYECTO2_ERROR_H
#define PROYECTO2_ERROR_H

#include <string>

struct error_t {
    int number; // id del error
    std::string lexeme; // texto asociado
    std::string tipo; // "lexico" o "sintactico"
    std::string descripcion; // descripcion sencilla
    int line;
    int column;

    error_t(): number(0), lexeme(""), tipo(""), descripcion(""), line(0), column(0) {}
    error_t(int n, const std::string &lex, const std::string &t, const std::string &d, int ln, int col)
        : number(n), lexeme(lex), tipo(t), descripcion(d), line(ln), column(col) {}
};

#endif // PROYECTO2_ERROR_H
