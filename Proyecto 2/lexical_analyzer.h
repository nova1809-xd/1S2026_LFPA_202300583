// lexical_analyzer.h
// analizador lexico que implementa un afd manualmente
#ifndef PROYECTO2_LEXICAL_ANALYZER_H
#define PROYECTO2_LEXICAL_ANALYZER_H

#include "token.h"
#include "error.h"
#include <istream>
#include <memory>
#include <vector>

class lexicalanalyzer {
public:
    // constructor con stream de entrada
    lexicalanalyzer(std::istream &in);

    // devuelve el siguiente token
    token nextToken();

    // tokenizacion parcial: obten los tokens ya producidos por el lexer
    const std::vector<token>& getTokens() const;

    // obtener ultima posicion (util para reportes)
    int getLine() const;
    int getColumn() const;

private:
    std::istream &in; // fuente del texto
    int line; // linea actual (1-based)
    int column; // columna actual (1-based)
    int tokenCounter; // para numerar tokens

    int pushChar; // si tenemos pushback
    bool hasPush;

    int getChar();
    void putBack(int c);
    int peekChar();
    static std::string toLower(const std::string &s);

    std::vector<token> tokens; // guardamos los tokens que el lexer va produciendo
};

#endif // PROYECTO2_LEXICAL_ANALYZER_H
