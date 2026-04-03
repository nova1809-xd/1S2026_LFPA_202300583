#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include "Token.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// mi analizador léxico: lo que hago es reconocer tokens del lenguaje MedLang
// implemento un autómata finito determinista que evalúa carácter por carácter
// sigo pista de la línea y columna para poder reportar errores con posición exacta
class LexicalAnalyzer {
private:
    std::string codigoFuente;       // el código que estoy analizando
    size_t posicion;                // dónde estoy ahora en el código
    int linea;                      // en qué línea estoy (empiezo en 1)
    int columna;                    // en qué columna estoy (empiezo en 1)
    
    std::vector<Token> tokens;      // los tokens que reconozco
    std::vector<std::string> errores; // los errores que encuentro
    
    // tabla de hash con las palabras reservadas
    // la uso para búsquedas rápidas sin tener que chequear listas
    std::unordered_map<std::string, TokenType> palabrasReservadas;
    
    // cargo todas las palabras reservadas en la tabla
    void inicializarPalabrasReservadas();
    
    // métodos auxiliares del autómata
    
    // chequea si es un dígito (0-9)
    bool esDigito(char c) const;
    
    // chequea si es una letra (minúscula o mayúscula)
    bool esLetra(char c) const;
    
    // chequea si es mayúscula nada más (A-Z)
    bool esLetraMayuscula(char c) const;
    
    // chequea si es un espacio o algo similar
    bool esEspacio(char c) const;
    
    // me devuelve el carácter donde estoy ahora
    char caracterActual() const;
    
    // me devuelve el carácter a una cierta distancia de donde estoy
    char caracterEn(int offset) const;
    
    // avanzo una posición y actualizo línea y columna
    void avanzar();
    
    // salto todos los espacios en blanco y saltos de línea
    void saltarEspacios();
    
    // reconozco un identificador o una palabra reservada
    Token reconocerIdentificador(int linea, int columna);
    
    // valido un código ID con formato XXX-NNN
    Token reconocerCodigoID(int linea, int columna);
    
    // reconozco números (secuencias de dígitos)
    Token reconocerNumero(int linea, int columna);
    
    // reconozco cadenas entre comillas dobles
    Token reconocerCadena(int linea, int columna);
    
    // valido fechas en formato AAAA-MM-DD
    Token reconocerFecha(int linea, int columna);
    
    // valido horas en formato HH:MM (24 horas)
    Token reconocerHora(int linea, int columna);
    
    // reconozco tipos de sangre (A+, O-, AB+, etc.)
    Token reconocerTipoSangre(int linea, int columna);
    
    // guardo errores que encuentro
    void registrarError(const std::string& mensaje, int linea, int columna);
    
public:
    // constructor: genero el analizador con el código
    explicit LexicalAnalyzer(const std::string& codigo);
    
    // destructor
    ~LexicalAnalyzer() = default;
    
    // obtengo el siguiente token
    // este es el método principal que implementa el autómata
    Token nextToken();
    
    // analizo todo el código de una vez y me devuelve todos los tokens
    std::vector<Token> analizarCompleto();
    
    // retorna la lista de errores que encontré
    const std::vector<std::string>& obtenerErrores() const;
    
    // retorna los tokens que reconocí
    const std::vector<Token>& obtenerTokens() const;
    
    // chequea si ya llegué al final del archivo
    bool esFinDeArchivo() const;
    
    // me devuelve dónde estoy en el código
    size_t obtenerPosicion() const;
    
    // me devuelve el número de línea donde estoy
    int obtenerLinea() const;
    
    // me devuelve el número de columna donde estoy
    int obtenerColumna() const;
};

#endif // LEXICAL_ANALYZER_H
