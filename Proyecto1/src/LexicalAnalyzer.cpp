#include "../include/LexicalAnalyzer.h"
#include <cctype>

// constructor: guardo el código y cargo las palabras reservadas
LexicalAnalyzer::LexicalAnalyzer(const std::string& codigo)
    : codigoFuente(codigo), posicion(0), linea(1), columna(1) {
    inicializarPalabrasReservadas();
}

// lleno la tabla hash con las palabras reservadas
void LexicalAnalyzer::inicializarPalabrasReservadas() {
    // palabras clave principales
    palabrasReservadas["HOSPITAL"] = TokenType::HOSPITAL;
    palabrasReservadas["PACIENTES"] = TokenType::PACIENTES;
    palabrasReservadas["MEDICOS"] = TokenType::MEDICOS;
    palabrasReservadas["CITAS"] = TokenType::CITAS;
    palabrasReservadas["DIAGNOSTICOS"] = TokenType::DIAGNOSTICOS;
    
    // palabras para los elementos (minúsculas - formato antiguo)
    palabrasReservadas["paciente"] = TokenType::PACIENTE;
    palabrasReservadas["medico"] = TokenType::MEDICO;
    palabrasReservadas["cita"] = TokenType::CITA;
    palabrasReservadas["diagnostico"] = TokenType::DIAGNOSTICO;
    palabrasReservadas["con"] = TokenType::CON;
    
    // palabras para los elementos (PascalCase - formato nuevo)
    palabrasReservadas["Paciente"] = TokenType::PACIENTE;
    palabrasReservadas["Medico"] = TokenType::MEDICO;
    palabrasReservadas["Cita"] = TokenType::CITA;
    palabrasReservadas["Diagnostico"] = TokenType::DIAGNOSTICO;
    
    // las especialidades
    palabrasReservadas["CARDIOLOGIA"] = TokenType::CARDIOLOGIA;
    palabrasReservadas["NEUROLOGIA"] = TokenType::NEUROLOGIA;
    palabrasReservadas["OFTALMOLOGIA"] = TokenType::OFTALMOLOGIA;
    palabrasReservadas["ORTOPEDIA"] = TokenType::ORTOPEDIA;
    palabrasReservadas["PEDIATRIA"] = TokenType::PEDIATRIA;
    palabrasReservadas["PSIQUIATRIA"] = TokenType::PSIQUIATRIA;
    palabrasReservadas["GASTROENTEROLOGIA"] = TokenType::GASTROENTEROLOGIA;
    palabrasReservadas["UROLOGIA"] = TokenType::UROLOGIA;
    palabrasReservadas["DERMATOLOGIA"] = TokenType::DERMATOLOGIA;
    palabrasReservadas["ONCOLOGIA"] = TokenType::ONCOLOGIA;
    
    // frecuencias de medicamentos
    palabrasReservadas["DIARIA"] = TokenType::DIARIA;
    palabrasReservadas["CADA_8_HORAS"] = TokenType::CADA_8_HORAS;
    palabrasReservadas["CADA_12_HORAS"] = TokenType::CADA_12_HORAS;
    palabrasReservadas["CADA_6_HORAS"] = TokenType::CADA_6_HORAS;
    palabrasReservadas["CADA_4_HORAS"] = TokenType::CADA_4_HORAS;
    palabrasReservadas["SEGUN_SEA_NECESARIO"] = TokenType::SEGUN_SEA_NECESARIO;
}

// ===== métodos auxiliares del autómata =====

// chequeo si es un dígito
bool LexicalAnalyzer::esDigito(char c) const {
    return c >= '0' && c <= '9';
}

// chequeo si es una letra
bool LexicalAnalyzer::esLetra(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// chequeo si es mayúscula
bool LexicalAnalyzer::esLetraMayuscula(char c) const {
    return c >= 'A' && c <= 'Z';
}

// chequeo si es un espacio o similar
bool LexicalAnalyzer::esEspacio(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

// me devuelve el carácter donde estoy
char LexicalAnalyzer::caracterActual() const {
    if (posicion < codigoFuente.length()) {
        return codigoFuente.at(posicion);
    }
    return '\0';
}

// me devuelve el carácter a una distancia
char LexicalAnalyzer::caracterEn(int offset) const {
    if (posicion + offset < codigoFuente.length()) {
        return codigoFuente.at(posicion + offset);
    }
    return '\0';
}

// me muevo a la siguiente posición
void LexicalAnalyzer::avanzar() {
    if (posicion < codigoFuente.length()) {
        if (codigoFuente.at(posicion) == '\n') {
            linea++;
            columna = 1;
        } else {
            columna++;
        }
        posicion++;
    }
}

// salto todos los espacios en blanco
void LexicalAnalyzer::saltarEspacios() {
    while (posicion < codigoFuente.length() && 
           (esEspacio(caracterActual()) || caracterActual() == '\n')) {
        avanzar();
    }
}

// reconozco identificadores y palabras clave
Token LexicalAnalyzer::reconocerIdentificador(int linea, int columna) {
    size_t inicio = posicion;

    if (!esLetra(caracterActual())) {
        std::string lexemaError = std::string(1, caracterActual());
        registrarError("identificador inválido: '" + lexemaError + "'", linea, columna);
        if (posicion < codigoFuente.length()) {
            avanzar();
        }
        return Token(lexemaError, TokenType::ERROR_TOKEN, linea, columna);
    }

    while (posicion < codigoFuente.length()) {
        char c = caracterActual();
        if (esLetra(c) || esDigito(c) || c == '_') {
            avanzar();
        } else {
            break;
        }
    }

    std::string lexema = codigoFuente.substr(inicio, posicion - inicio);
    auto it = palabrasReservadas.find(lexema);
    if (it != palabrasReservadas.end()) {
        return Token(lexema, it->second, linea, columna);
    }

    return Token(lexema, TokenType::IDENTIFICADOR, linea, columna);
}

// valido un código ID
Token LexicalAnalyzer::reconocerCodigoID(int linea, int columna) {
    size_t inicio = posicion;

    auto esSeparador = [this](char c) -> bool {
        return c == '\0' || esEspacio(c) || c == '\n' ||
               c == '{' || c == '}' || c == '[' || c == ']' ||
               c == ':' || c == ',' || c == ';';
    };

    auto consumirLexemaCorrupto = [&]() {
        size_t antes = posicion;
        while (posicion < codigoFuente.length() && !esSeparador(caracterActual())) {
            avanzar();
        }
        if (posicion == antes && posicion < codigoFuente.length()) {
            avanzar();
        }
    };

    bool formatoValido =
        posicion + 7 <= codigoFuente.length() &&
        esLetraMayuscula(caracterEn(0)) &&
        esLetraMayuscula(caracterEn(1)) &&
        esLetraMayuscula(caracterEn(2)) &&
        caracterEn(3) == '-' &&
        esDigito(caracterEn(4)) &&
        esDigito(caracterEn(5)) &&
        esDigito(caracterEn(6));

    bool prefijoValido =
        (caracterEn(0) == 'P' && caracterEn(1) == 'A' && caracterEn(2) == 'C') ||
        (caracterEn(0) == 'M' && caracterEn(1) == 'E' && caracterEn(2) == 'D') ||
        (caracterEn(0) == 'C' && caracterEn(1) == 'I' && caracterEn(2) == 'T');

    bool valido = formatoValido && prefijoValido;

    if (valido && !esSeparador(caracterEn(7))) {
        valido = false;
    }

    if (!valido) {
        consumirLexemaCorrupto();
        std::string lexemaError = codigoFuente.substr(inicio, posicion - inicio);
        registrarError("código ID inválido: '" + lexemaError + "'", linea, columna);
        return Token(lexemaError, TokenType::ERROR_TOKEN, linea, columna);
    }

    std::string lexema = codigoFuente.substr(posicion, 7);
    for (int i = 0; i < 7; ++i) {
        avanzar();
    }
    return Token(lexema, TokenType::IDENTIFICADOR_CODIGO, linea, columna);
}

// reconozco números enteros
Token LexicalAnalyzer::reconocerNumero(int linea, int columna) {
    size_t inicio = posicion;

    if (!esDigito(caracterActual())) {
        std::string lexemaError = std::string(1, caracterActual());
        registrarError("número inválido: '" + lexemaError + "'", linea, columna);
        if (posicion < codigoFuente.length()) {
            avanzar();
        }
        return Token(lexemaError, TokenType::ERROR_TOKEN, linea, columna);
    }

    while (posicion < codigoFuente.length() && esDigito(caracterActual())) {
        avanzar();
    }

    std::string lexema = codigoFuente.substr(inicio, posicion - inicio);
    return Token(lexema, TokenType::INTEGER, linea, columna);
}

// reconozco cadenas entre comillas
Token LexicalAnalyzer::reconocerCadena(int linea, int columna) {
    size_t inicio = posicion;

    if (caracterActual() != '"') {
        std::string lexemaError = std::string(1, caracterActual());
        registrarError("cadena inválida: '" + lexemaError + "'", linea, columna);
        if (posicion < codigoFuente.length()) {
            avanzar();
        }
        return Token(lexemaError, TokenType::ERROR_TOKEN, linea, columna);
    }

    avanzar();

    while (posicion < codigoFuente.length()) {
        char c = caracterActual();

        if (c == '\\' && caracterEn(1) == '"') {
            avanzar();
            avanzar();
            continue;
        }

        if (c == '"') {
            avanzar();
            std::string lexema = codigoFuente.substr(inicio, posicion - inicio);
            return Token(lexema, TokenType::STRING, linea, columna);
        }

        if (c == '\n') {
            std::string lexemaError = codigoFuente.substr(inicio, posicion - inicio);
            registrarError("cadena sin cierre: '" + lexemaError + "'", linea, columna);
            return Token(lexemaError, TokenType::ERROR_TOKEN, linea, columna);
        }

        avanzar();
    }

    std::string lexemaError = codigoFuente.substr(inicio, posicion - inicio);
    registrarError("cadena sin cierre: '" + lexemaError + "'", linea, columna);
    return Token(lexemaError, TokenType::ERROR_TOKEN, linea, columna);
}

// valido fechas
Token LexicalAnalyzer::reconocerFecha(int linea, int columna) {
    size_t inicio = posicion;

    auto esSeparador = [this](char c) -> bool {
        return c == '\0' || esEspacio(c) || c == '\n' ||
               c == '{' || c == '}' || c == '[' || c == ']' ||
               c == ':' || c == ',' || c == ';';
    };

    auto consumirLexemaCorrupto = [&]() {
        size_t antes = posicion;
        while (posicion < codigoFuente.length() && !esSeparador(caracterActual())) {
            avanzar();
        }
        if (posicion == antes && posicion < codigoFuente.length()) {
            avanzar();
        }
    };

    bool valido =
        posicion + 10 <= codigoFuente.length() &&
        esDigito(caracterEn(0)) && esDigito(caracterEn(1)) &&
        esDigito(caracterEn(2)) && esDigito(caracterEn(3)) &&
        caracterEn(4) == '-' &&
        esDigito(caracterEn(5)) && esDigito(caracterEn(6)) &&
        caracterEn(7) == '-' &&
        esDigito(caracterEn(8)) && esDigito(caracterEn(9));

    if (valido && !esSeparador(caracterEn(10))) {
        valido = false;
    }

    if (valido) {
        int mes = (caracterEn(5) - '0') * 10 + (caracterEn(6) - '0');
        int dia = (caracterEn(8) - '0') * 10 + (caracterEn(9) - '0');
        if (mes < 1 || mes > 12 || dia < 1 || dia > 31) {
            valido = false;
        }
    }

    if (!valido) {
        consumirLexemaCorrupto();
        std::string lexemaError = codigoFuente.substr(inicio, posicion - inicio);
        registrarError("fecha inválida: '" + lexemaError + "'", linea, columna);
        return Token(lexemaError, TokenType::ERROR_TOKEN, linea, columna);
    }

    std::string lexema = codigoFuente.substr(posicion, 10);
    for (int i = 0; i < 10; ++i) {
        avanzar();
    }
    return Token(lexema, TokenType::FECHA, linea, columna);
}

// valido horas
Token LexicalAnalyzer::reconocerHora(int linea, int columna) {
    size_t inicio = posicion;

    auto esSeparador = [this](char c) -> bool {
        return c == '\0' || esEspacio(c) || c == '\n' ||
               c == '{' || c == '}' || c == '[' || c == ']' ||
               c == ':' || c == ',' || c == ';';
    };

    auto consumirLexemaCorrupto = [&]() {
        size_t antes = posicion;
        while (posicion < codigoFuente.length() && !esSeparador(caracterActual())) {
            avanzar();
        }
        if (posicion == antes && posicion < codigoFuente.length()) {
            avanzar();
        }
    };

    bool valido =
        posicion + 5 <= codigoFuente.length() &&
        esDigito(caracterEn(0)) && esDigito(caracterEn(1)) &&
        caracterEn(2) == ':' &&
        esDigito(caracterEn(3)) && esDigito(caracterEn(4));

    if (valido && !esSeparador(caracterEn(5))) {
        valido = false;
    }

    if (valido) {
        int hh = (caracterEn(0) - '0') * 10 + (caracterEn(1) - '0');
        int mm = (caracterEn(3) - '0') * 10 + (caracterEn(4) - '0');
        if (hh < 0 || hh > 23 || mm < 0 || mm > 59) {
            valido = false;
        }
    }

    if (!valido) {
        consumirLexemaCorrupto();
        std::string lexemaError = codigoFuente.substr(inicio, posicion - inicio);
        registrarError("hora inválida: '" + lexemaError + "'", linea, columna);
        return Token(lexemaError, TokenType::ERROR_TOKEN, linea, columna);
    }

    std::string lexema = codigoFuente.substr(posicion, 5);
    for (int i = 0; i < 5; ++i) {
        avanzar();
    }
    return Token(lexema, TokenType::HORA, linea, columna);
}

// reconozco tipos de sangre
Token LexicalAnalyzer::reconocerTipoSangre(int linea, int columna) {
    size_t inicio = posicion;

    std::string lexema;
    TokenType tipo = TokenType::ERROR_TOKEN;

    if (caracterEn(0) == 'A' && caracterEn(1) == 'B' && (caracterEn(2) == '+' || caracterEn(2) == '-')) {
        lexema = codigoFuente.substr(posicion, 3);
        tipo = (caracterEn(2) == '+') ? TokenType::TIPO_SANGRE_AB_POS : TokenType::TIPO_SANGRE_AB_NEG;
        avanzar();
        avanzar();
        avanzar();
        return Token(lexema, tipo, linea, columna);
    }

    if ((caracterEn(0) == 'A' || caracterEn(0) == 'B' || caracterEn(0) == 'O') &&
        (caracterEn(1) == '+' || caracterEn(1) == '-')) {
        lexema = codigoFuente.substr(posicion, 2);
        if (caracterEn(0) == 'A' && caracterEn(1) == '+') tipo = TokenType::TIPO_SANGRE_A_POS;
        if (caracterEn(0) == 'A' && caracterEn(1) == '-') tipo = TokenType::TIPO_SANGRE_A_NEG;
        if (caracterEn(0) == 'B' && caracterEn(1) == '+') tipo = TokenType::TIPO_SANGRE_B_POS;
        if (caracterEn(0) == 'B' && caracterEn(1) == '-') tipo = TokenType::TIPO_SANGRE_B_NEG;
        if (caracterEn(0) == 'O' && caracterEn(1) == '+') tipo = TokenType::TIPO_SANGRE_O_POS;
        if (caracterEn(0) == 'O' && caracterEn(1) == '-') tipo = TokenType::TIPO_SANGRE_O_NEG;

        avanzar();
        avanzar();
        return Token(lexema, tipo, linea, columna);
    }

    while (posicion < codigoFuente.length()) {
        char c = caracterActual();
        if (esEspacio(c) || c == '\n' || c == '{' || c == '}' || c == '[' || c == ']' || c == ':' || c == ',' || c == ';') {
            break;
        }
        avanzar();
    }

    if (posicion == inicio && posicion < codigoFuente.length()) {
        avanzar();
    }

    std::string lexemaError = codigoFuente.substr(inicio, posicion - inicio);
    registrarError("tipo de sangre inválido: '" + lexemaError + "'", linea, columna);
    return Token(lexemaError, TokenType::ERROR_TOKEN, linea, columna);
}

// guardo los errores que me encuentro
void LexicalAnalyzer::registrarError(const std::string& mensaje, int linea, int columna) {
    std::string error = "Error léxico en línea " + std::to_string(linea) + 
                       ", columna " + std::to_string(columna) + ": " + mensaje;
    errores.push_back(error);
}

// metodo principal: obtengo el siguiente token
Token LexicalAnalyzer::nextToken() {
    // salto espacios en blanco
    saltarEspacios();
    
    // verifico si llegue al fin
    if (posicion >= codigoFuente.length()) {
        return Token("", TokenType::EOF_TOKEN, linea, columna);
    }
    
    // guardo dónde empieza el token
    int tokenLinea = linea;
    int tokenColumna = columna;
    
    char actual = caracterActual();
    
    // ===== reconocimiento de delimitadores =====
    if (actual == '{') {
        avanzar();
        return Token("{", TokenType::LLAVE_ABIERTA, tokenLinea, tokenColumna);
    }
    if (actual == '}') {
        avanzar();
        return Token("}", TokenType::LLAVE_CERRADA, tokenLinea, tokenColumna);
    }
    if (actual == '[') {
        avanzar();
        return Token("[", TokenType::CORCHETE_ABIERTO, tokenLinea, tokenColumna);
    }
    if (actual == ']') {
        avanzar();
        return Token("]", TokenType::CORCHETE_CERRADO, tokenLinea, tokenColumna);
    }
    if (actual == ':') {
        avanzar();
        return Token(":", TokenType::DOS_PUNTOS, tokenLinea, tokenColumna);
    }
    if (actual == ',') {
        avanzar();
        return Token(",", TokenType::COMA, tokenLinea, tokenColumna);
    }
    if (actual == ';') {
        avanzar();
        return Token(";", TokenType::PUNTO_Y_COMA, tokenLinea, tokenColumna);
    }
    
    // ===== reconocimiento de cadena de texto =====
    if (actual == '"') {
        size_t antes = posicion;
        Token t = reconocerCadena(tokenLinea, tokenColumna);
        if (t.getTipo() == TokenType::ERROR_TOKEN && posicion == antes) {
            avanzar();
        }
        return t;
    }
    
    // ===== reconocimiento de números, fechas y horas =====
    if (esDigito(actual)) {
        size_t antes = posicion;

        bool posibleFecha =
            esDigito(caracterEn(0)) && esDigito(caracterEn(1)) &&
            esDigito(caracterEn(2)) && esDigito(caracterEn(3)) &&
            caracterEn(4) == '-' && esDigito(caracterEn(5)) &&
            esDigito(caracterEn(6)) && caracterEn(7) == '-' &&
            esDigito(caracterEn(8)) && esDigito(caracterEn(9));

        if (posibleFecha) {
            Token t = reconocerFecha(tokenLinea, tokenColumna);
            if (t.getTipo() == TokenType::ERROR_TOKEN && posicion == antes) {
                avanzar();
            }
            return t;
        }

        bool posibleHora =
            esDigito(caracterEn(0)) && esDigito(caracterEn(1)) &&
            caracterEn(2) == ':' && esDigito(caracterEn(3)) && esDigito(caracterEn(4));

        if (posibleHora) {
            Token t = reconocerHora(tokenLinea, tokenColumna);
            if (t.getTipo() == TokenType::ERROR_TOKEN && posicion == antes) {
                avanzar();
            }
            return t;
        }

        Token t = reconocerNumero(tokenLinea, tokenColumna);
        if (t.getTipo() == TokenType::ERROR_TOKEN && posicion == antes) {
            avanzar();
        }
        return t;
    }
    
    // ===== reconocimiento de identificadores, palabras reservadas, códigos ID o sangre =====
    if (esLetra(actual)) {
        size_t antes = posicion;

        if (esLetraMayuscula(actual)) {
            bool posibleTipoSangre =
                ((actual == 'A') && (caracterEn(1) == '+' || caracterEn(1) == '-')) ||
                ((actual == 'B') && (caracterEn(1) == '+' || caracterEn(1) == '-')) ||
                ((actual == 'O') && (caracterEn(1) == '+' || caracterEn(1) == '-')) ||
                ((actual == 'A') && caracterEn(1) == 'B' &&
                 (caracterEn(2) == '+' || caracterEn(2) == '-'));

            if (posibleTipoSangre) {
                Token t = reconocerTipoSangre(tokenLinea, tokenColumna);
                if (t.getTipo() == TokenType::ERROR_TOKEN && posicion == antes) {
                    avanzar();
                }
                return t;
            }

                        bool posibleCodigoID =
                                (esLetraMayuscula(caracterEn(0)) && esLetraMayuscula(caracterEn(1)) &&
                                 esLetraMayuscula(caracterEn(2)) && caracterEn(3) == '-' &&
                                 esDigito(caracterEn(4)) && esDigito(caracterEn(5)) &&
                                 esDigito(caracterEn(6)));

            if (posibleCodigoID) {
                Token t = reconocerCodigoID(tokenLinea, tokenColumna);
                if (t.getTipo() == TokenType::ERROR_TOKEN && posicion == antes) {
                    avanzar();
                }
                return t;
            }
        }

        Token t = reconocerIdentificador(tokenLinea, tokenColumna);
        if (t.getTipo() == TokenType::ERROR_TOKEN && posicion == antes) {
            avanzar();
        }
        return t;
    }
    
    // ===== carácter inválido =====
    std::string mensaje = "carácter inválido: '" + std::string(1, actual) + "'";
    registrarError(mensaje, tokenLinea, tokenColumna);
    avanzar();
    
    return Token(std::string(1, actual), TokenType::ERROR_TOKEN, tokenLinea, tokenColumna);
}

// analizo todo el código de una vez
std::vector<Token> LexicalAnalyzer::analizarCompleto() {
    Token t;
    do {
        t = nextToken();
        if (t.getTipo() != TokenType::EOF_TOKEN) {
            tokens.push_back(t);
        }
    } while (t.getTipo() != TokenType::EOF_TOKEN);
    
    // agregar token EOF al final
    tokens.push_back(t);
    
    return tokens;
}

// retorna los errores
const std::vector<std::string>& LexicalAnalyzer::obtenerErrores() const {
    return errores;
}

// retorna los tokens
const std::vector<Token>& LexicalAnalyzer::obtenerTokens() const {
    return tokens;
}

// chequea si esta al fin
bool LexicalAnalyzer::esFinDeArchivo() const {
    return posicion >= codigoFuente.length();
}

// posición donde estoy
size_t LexicalAnalyzer::obtenerPosicion() const {
    return posicion;
}

// línea donde estoy
int LexicalAnalyzer::obtenerLinea() const {
    return linea;
}

// columna donde estoy
int LexicalAnalyzer::obtenerColumna() const {
    return columna;
}
