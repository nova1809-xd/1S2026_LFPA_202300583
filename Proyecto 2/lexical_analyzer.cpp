// lexical_analyzer.cpp
// implementación del afd manual para tokenizar taskscript
#include "lexical_analyzer.h"
#include <cctype>
#include <sstream>

// constructor
lexicalanalyzer::lexicalanalyzer(std::istream &input)
    : in(input), line(1), column(0), tokenCounter(0), pushChar(-1), hasPush(false) {}

int lexicalanalyzer::getChar() {
    // aqui devolvemos el siguiente caracter y actualizamos linea/columna
    int c;
    if (hasPush) {
        c = pushChar;
        hasPush = false;
    } else {
        c = in.get();
        if (c == EOF) return -1;
    }

    if (c == '\n') {
        line++;
        column = 0;
    } else {
        column++;
    }
    return c;
}

void lexicalanalyzer::putBack(int c) {
    // guardamos un char para el siguiente getChar
    // nota: esto ajusta columna/linea de forma aproximada
    pushChar = c;
    hasPush = true;
    if (c == '\n') {
        if (line > 1) line--;
        // no intentamos recalcular columna exacta, queda aproximado
    } else {
        if (column > 0) column--;
    }
}

int lexicalanalyzer::peekChar() {
    int c = getChar();
    if (c != -1) putBack(c);
    return c;
}

int lexicalanalyzer::getLine() const { return line; }
int lexicalanalyzer::getColumn() const { return column; }

std::string lexicalanalyzer::toLower(const std::string &s) {
    std::string r; r.reserve(s.size());
    for (char ch : s) r.push_back(static_cast<char>(std::tolower((unsigned char)ch)));
    return r;
}

// main: nextToken implementa el afd manual
token lexicalanalyzer::nextToken() {
    // estados: implementamos por bloques con switch/ifs y lectura char a char
    std::string lex;
    int startLine = line;
    int startCol = column;
    int c = getChar();

    // saltamos espacios y comentarios
    while (c != -1) {
        if (c == '/') {
            int p = peekChar();
            if (p == '/') {
                // comentario de linea, consumimos hasta newline
                while (c != -1 && c != '\n') c = getChar();
                if (c == -1) break;
                c = getChar();
                continue;
            } else {
                break; // es un '/', puede ser simbolo o error, lo tratamos abajo
            }
        }

        if (c == '#') {
            // comentario estilo script, saltamos hasta fin de linea
            while (c != -1 && c != '\n') c = getChar();
            if (c == -1) break;
            c = getChar();
            continue;
        }

        if (std::isspace(static_cast<unsigned char>(c))) {
            c = getChar();
            continue;
        }
        break;
    }

    if (c == -1) {
        // eof
        token tok(++tokenCounter, "", tokentype::EOF_TOKEN, startLine, startCol);
        return tok;
    }

    startLine = line;
    startCol = column;

    // identificadores / keywords: letras y guion bajo
    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
        lex.push_back(static_cast<char>(c));
        int nc = getChar();
        while (nc != -1 && (std::isalnum(static_cast<unsigned char>(nc)) || nc == '_' )) {
            lex.push_back(static_cast<char>(nc));
            nc = getChar();
        }
        if (nc != -1) putBack(nc);

        std::string low = toLower(lex);
        tokentype tt = tokentype::ERROR;
        if (low == "tablero") tt = tokentype::TABLERO;
        else if (low == "columna") tt = tokentype::COLUMNA;
        else if (low == "tarea") tt = tokentype::TAREA_KW;
        else if (low == "prioridad") tt = tokentype::PRIORIDAD_KW;
        else if (low == "responsable") tt = tokentype::RESPONSABLE_KW;
        else if (low == "fecha_limite" || low == "fecha-limite") tt = tokentype::FECHA_LIMITE_KW;
        else if (low == "alta") tt = tokentype::ALTA;
        else if (low == "media") tt = tokentype::MEDIA;
        else if (low == "baja") tt = tokentype::BAJA;
        else tt = tokentype::ERROR; // palabras desconocidas las marcamos error por ahora

        token tok(++tokenCounter, lex, tt, startLine, startCol);
        return tok;
    }

    // numeros o fechas: digitos y posibles guiones
    if (std::isdigit(static_cast<unsigned char>(c))) {
        lex.push_back(static_cast<char>(c));
        int nc = getChar();
        while (nc != -1 && (std::isdigit(static_cast<unsigned char>(nc)) || nc == '-')) {
            lex.push_back(static_cast<char>(nc));
            nc = getChar();
        }
        if (nc != -1) putBack(nc);

        // si tiene '-' intentamos validar fecha formato yyyy-mm-dd
        bool isDate = false;
        if (lex.find('-') != std::string::npos) {
            // validar sin usar regex: dividir por '-' y chequear tamaños
            std::istringstream ss(lex);
            std::string part;
            int parts = 0;
            int lens[3] = {0,0,0};
            int idx = 0;
            bool allDigits = true;
            while (std::getline(ss, part, '-')) {
                if (idx < 3) lens[idx] = (int)part.size();
                for (char ch : part) if (!std::isdigit(static_cast<unsigned char>(ch))) allDigits = false;
                parts++; idx++;
            }
            if (parts == 3 && allDigits && lens[0] == 4 && lens[1] == 2 && lens[2] == 2) isDate = true;
        }

        if (isDate) {
            token tok(++tokenCounter, lex, tokentype::FECHA, startLine, startCol);
            return tok;
        } else {
            // si contiene '-' pero no cumple, lo marcamos error
            if (lex.find('-') != std::string::npos) {
                token tok(++tokenCounter, lex, tokentype::ERROR, startLine, startCol);
                return tok;
            }
            token tok(++tokenCounter, lex, tokentype::ENTERO, startLine, startCol);
            return tok;
        }
    }

    // cadenas entre comillas dobles
    if (c == '"') {
        // empezamos string, guardamos contenido sin las comillas
        int nc = getChar();
        while (nc != -1 && nc != '"') {
            if (nc == '\\') {
                // escapado simple, tomamos el siguiente si existe
                int esc = getChar();
                if (esc == -1) break;
                lex.push_back(static_cast<char>(nc));
                lex.push_back(static_cast<char>(esc));
                nc = getChar();
                continue;
            }
            lex.push_back(static_cast<char>(nc));
            nc = getChar();
        }
        if (nc != '"') {
            // eof o no se cerró
            token tok(++tokenCounter, lex, tokentype::ERROR, startLine, startCol);
            return tok;
        }
        // consumimos la comilla de cierre ya realizada
        token tok(++tokenCounter, lex, tokentype::CADENA, startLine, startCol);
        return tok;
    }

    // simbolos simples
    switch (c) {
        case '{': return token(++tokenCounter, "{", tokentype::LLAVE_A, startLine, startCol);
        case '}': return token(++tokenCounter, "}", tokentype::LLAVE_C, startLine, startCol);
        case '[': return token(++tokenCounter, "[", tokentype::CORCHETE_A, startLine, startCol);
        case ']': return token(++tokenCounter, "]", tokentype::CORCHETE_C, startLine, startCol);
        case ':': return token(++tokenCounter, ":", tokentype::DOS_PUNTOS, startLine, startCol);
        case ',': return token(++tokenCounter, ",", tokentype::COMA, startLine, startCol);
        case ';': return token(++tokenCounter, ";", tokentype::PUNTO_COMA, startLine, startCol);
        case '/': {
            // si llegamos aqui y no era comentario, devolvemos '/' como error o simbolo no soportado
            return token(++tokenCounter, "/", tokentype::ERROR, startLine, startCol);
        }
        default: {
            // cualquier otro caracter lo marcamos como error
            std::string s(1, static_cast<char>(c));
            return token(++tokenCounter, s, tokentype::ERROR, startLine, startCol);
        }
    }
}
