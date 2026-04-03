#include "../include/Token.h"

// constructor con todos los datos
Token::Token(const std::string& lex, TokenType t, int lin, int col)
    : lexema(lex), tipo(t), linea(lin), columna(col) {}

// constructor vacío que inicializa todo en valores inválidos
Token::Token()
    : lexema(""), tipo(TokenType::ERROR_TOKEN), linea(-1), columna(-1) {}

// para leer los datos del token
std::string Token::getLexema() const {
    return lexema;
}

TokenType Token::getTipo() const {
    return tipo;
}

int Token::getLinea() const {
    return linea;
}

int Token::getColumna() const {
    return columna;
}

// para modificar los datos del token
void Token::setLexema(const std::string& lex) {
    lexema = lex;
}

void Token::setTipo(TokenType t) {
    tipo = t;
}

void Token::setLinea(int lin) {
    linea = lin;
}

void Token::setColumna(int col) {
    columna = col;
}

// convierte los tipos de token a strings para mostrar
std::string Token::tipoToString(TokenType tipo) {
    switch (tipo) {
        // Palabras reservadas principales
        case TokenType::HOSPITAL:           return "HOSPITAL";
        case TokenType::PACIENTES:          return "PACIENTES";
        case TokenType::MEDICOS:            return "MEDICOS";
        case TokenType::CITAS:              return "CITAS";
        case TokenType::DIAGNOSTICOS:       return "DIAGNOSTICOS";
        
        // Palabras reservadas de elemento
        case TokenType::PACIENTE:           return "PACIENTE";
        case TokenType::MEDICO:             return "MEDICO";
        case TokenType::CITA:               return "CITA";
        case TokenType::DIAGNOSTICO:        return "DIAGNOSTICO";
        
        // Especialidades
        case TokenType::CARDIOLOGIA:        return "CARDIOLOGIA";
        case TokenType::NEUROLOGIA:         return "NEUROLOGIA";
        case TokenType::OFTALMOLOGIA:       return "OFTALMOLOGIA";
        case TokenType::ORTOPEDIA:          return "ORTOPEDIA";
        case TokenType::PEDIATRIA:          return "PEDIATRIA";
        case TokenType::PSIQUIATRIA:        return "PSIQUIATRIA";
        case TokenType::GASTROENTEROLOGIA:  return "GASTROENTEROLOGIA";
        case TokenType::UROLOGIA:           return "UROLOGIA";
        case TokenType::DERMATOLOGIA:       return "DERMATOLOGIA";
        case TokenType::ONCOLOGIA:          return "ONCOLOGIA";
        
        // Frecuencia de dosis
        case TokenType::DIARIA:             return "DIARIA";
        case TokenType::CADA_8_HORAS:       return "CADA_8_HORAS";
        case TokenType::CADA_12_HORAS:      return "CADA_12_HORAS";
        case TokenType::CADA_6_HORAS:       return "CADA_6_HORAS";
        case TokenType::CADA_4_HORAS:       return "CADA_4_HORAS";
        case TokenType::SEGUN_SEA_NECESARIO: return "SEGUN_SEA_NECESARIO";
        
        // Tipos de sangre
        case TokenType::TIPO_SANGRE_A_POS:  return "TIPO_SANGRE_A_POS (A+)";
        case TokenType::TIPO_SANGRE_A_NEG:  return "TIPO_SANGRE_A_NEG (A-)";
        case TokenType::TIPO_SANGRE_B_POS:  return "TIPO_SANGRE_B_POS (B+)";
        case TokenType::TIPO_SANGRE_B_NEG:  return "TIPO_SANGRE_B_NEG (B-)";
        case TokenType::TIPO_SANGRE_AB_POS: return "TIPO_SANGRE_AB_POS (AB+)";
        case TokenType::TIPO_SANGRE_AB_NEG: return "TIPO_SANGRE_AB_NEG (AB-)";
        case TokenType::TIPO_SANGRE_O_POS:  return "TIPO_SANGRE_O_POS (O+)";
        case TokenType::TIPO_SANGRE_O_NEG:  return "TIPO_SANGRE_O_NEG (O-)";
        
        // Tipos de datos
        case TokenType::INTEGER:            return "INTEGER";
        case TokenType::STRING:             return "STRING";
        case TokenType::IDENTIFICADOR_CODIGO: return "IDENTIFICADOR_CODIGO";
        case TokenType::FECHA:              return "FECHA";
        case TokenType::HORA:               return "HORA";
        case TokenType::IDENTIFICADOR:      return "IDENTIFICADOR";
        
        // Delimitadores
        case TokenType::LLAVE_ABIERTA:      return "LLAVE_ABIERTA ({)";
        case TokenType::LLAVE_CERRADA:      return "LLAVE_CERRADA (})";
        case TokenType::CORCHETE_ABIERTO:   return "CORCHETE_ABIERTO ([)";
        case TokenType::CORCHETE_CERRADO:   return "CORCHETE_CERRADO (])";
        case TokenType::DOS_PUNTOS:         return "DOS_PUNTOS (:)";
        case TokenType::COMA:               return "COMA (,)";
        
        // Especiales
        case TokenType::EOF_TOKEN:          return "EOF_TOKEN";
        case TokenType::ERROR_TOKEN:        return "ERROR_TOKEN";
        
        default:                             return "UNKNOWN";
    }
}

std::string Token::toString() const {
    return "[" + tipoToString(tipo) + "] \"" + lexema + "\" (" 
         + std::to_string(linea) + ":" + std::to_string(columna) + ")";
}
