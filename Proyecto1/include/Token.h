#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>

// tipos de tokens que reconozco en el análisis léxico
// tengo todos los tipos que necesito para MedLang aquí
enum class TokenType {
    // palabras clave principales del lenguaje
    HOSPITAL,
    PACIENTES,
    MEDICOS,
    CITAS,
    DIAGNOSTICOS,
    
    // palabras para los elementos individuales
    PACIENTE,
    MEDICO,
    CITA,
    DIAGNOSTICO,
    CON,                    // palabra clave "con" usada en citas
    
    // las especialidades médicas que soporto
    CARDIOLOGIA,
    NEUROLOGIA,
    OFTALMOLOGIA,
    ORTOPEDIA,
    PEDIATRIA,
    PSIQUIATRIA,
    GASTROENTEROLOGIA,
    UROLOGIA,
    DERMATOLOGIA,
    ONCOLOGIA,
    
    // opciones de frecuencia para medicamentos
    DIARIA,
    CADA_8_HORAS,
    CADA_12_HORAS,
    CADA_6_HORAS,
    CADA_4_HORAS,
    SEGUN_SEA_NECESARIO,
    
    // tipos de sangre posibles
    TIPO_SANGRE_A_POS,      // A+
    TIPO_SANGRE_A_NEG,      // A-
    TIPO_SANGRE_B_POS,      // B+
    TIPO_SANGRE_B_NEG,      // B-
    TIPO_SANGRE_AB_POS,     // AB+
    TIPO_SANGRE_AB_NEG,     // AB-
    TIPO_SANGRE_O_POS,      // O+
    TIPO_SANGRE_O_NEG,      // O-
    
    // tipos de datos básicos del lenguaje
    INTEGER,                // números (ej: 42, 100)
    STRING,                 // texto entre comillas (ej: "Juan")
    IDENTIFICADOR_CODIGO,   // códigos como MED-001
    FECHA,                  // fechas tipo 2024-12-25
    HORA,                   // horas 14:30
    IDENTIFICADOR,          // nombres de variables
    
    // símbolos de puntuación
    LLAVE_ABIERTA,          // {
    LLAVE_CERRADA,          // }
    CORCHETE_ABIERTO,       // [
    CORCHETE_CERRADO,       // ]
    DOS_PUNTOS,             // :
    COMA,                   // ,
    PUNTO_Y_COMA,           // ;
    
    // casos especiales
    EOF_TOKEN,              // fin del archivo
    ERROR_TOKEN,             // algo anduvo mal en el análisis
    CON
};

// representa cada token que reconozco en el análisis
// guarda el texto original, qué tipo es, y dónde estaba en el código
class Token {
private:
    std::string lexema;      // la cadena original que reconocí
    TokenType tipo;          // qué tipo de token es
    int linea;               // en qué línea estaba
    int columna;             // en qué posición de la línea

public:
    // constructor cuando conozco todo del token
    Token(const std::string& lex, TokenType t, int lin, int col);
    
    // constructor vacío para cuando no tengo nada todavía
    Token();
    
    // para leer los datos
    std::string getLexema() const;
    TokenType getTipo() const;
    int getLinea() const;
    int getColumna() const;
    
    // para cambiar los datos si necesito
    void setLexema(const std::string& lex);
    void setTipo(TokenType t);
    void setLinea(int lin);
    void setColumna(int col);
    
    // convierte el tipo a un texto legible
    static std::string tipoToString(TokenType tipo);
    
    // muestra el token de una forma legible para debugging
    std::string toString() const;
};

#endif // TOKEN_H
