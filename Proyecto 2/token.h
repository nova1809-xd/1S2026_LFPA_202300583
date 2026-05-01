// token.h
// definimos los tipos de token y la estructura para guardar tokens
#ifndef PROYECTO2_TOKEN_H
#define PROYECTO2_TOKEN_H

#include <string>

enum class tokentype {
    TABLERO,
    COLUMNA,
    TAREA_KW,
    PRIORIDAD_KW,
    RESPONSABLE_KW,
    FECHA_LIMITE_KW,
    ALTA,
    MEDIA,
    BAJA,
    FECHA,
    ENTERO,
    CADENA,
    LLAVE_A,
    LLAVE_C,
    CORCHETE_A,
    CORCHETE_C,
    DOS_PUNTOS,
    COMA,
    PUNTO_COMA,
    ERROR,
    EOF_TOKEN
};

struct token {
    int number; // id del token (se puede usar incremental)
    std::string lexeme; // el texto original
    tokentype type; // tipo del token
    int line; // linea donde inicia
    int column; // columna donde inicia

    token(): number(0), lexeme(""), type(tokentype::ERROR), line(0), column(0) {}
    token(int n, const std::string &l, tokentype t, int ln, int col)
        : number(n), lexeme(l), type(t), line(ln), column(col) {}
};

#endif // PROYECTO2_TOKEN_H
