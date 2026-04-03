#include "../include/LexicalAnalyzer.h"
#include "../include/Token.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

std::string leerArchivo(const std::string& ruta) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << archivo.rdbuf();
    return buffer.str();
}

int main() {
    const std::string rutaPrueba = "../tests/caso01_valido_basico.med";
    std::string codigoFuente = leerArchivo(rutaPrueba);

    if (codigoFuente.empty()) {
        codigoFuente = leerArchivo("tests/caso01_valido_basico.med");
    }

    if (codigoFuente.empty()) {
        std::cerr << "error: no se pudo leer el archivo de prueba: " << rutaPrueba << std::endl;
        return 1;
    }

    LexicalAnalyzer analizador(codigoFuente);

    std::cout << std::left
              << std::setw(8)  << "linea"
              << std::setw(10) << "columna"
              << std::setw(28) << "tipo_token"
              << std::setw(0)  << "lexema" << std::endl;

    std::cout << std::string(80, '-') << std::endl;

    Token token;
    while (true) {
        token = analizador.nextToken();
        if (token.getTipo() == TokenType::EOF_TOKEN) {
            break;
        }

        std::cout << std::left
                  << std::setw(8)  << token.getLinea()
                  << std::setw(10) << token.getColumna()
                  << std::setw(28) << Token::tipoToString(token.getTipo())
                  << token.getLexema() << std::endl;
    }

    std::cout << "\nanalisis por consola finalizado." << std::endl;
    return 0;
}
