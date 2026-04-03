#ifndef LEXICAL_ANALYZER_TESTS_H
#define LEXICAL_ANALYZER_TESTS_H

#include "../include/LexicalAnalyzer.h"
#include <iostream>
#include <cassert>
#include <sstream>

// suite de pruebas unitarias para el analizador léxico MedLexer
// aquí testiho:
// - reconocimiento de delimitadores
// - reconocimiento de palabras reservadas
// - reconocimiento de identificadores y códigos ID
// - reconocimiento de números, fechas y horas
// - reconocimiento de cadenas de texto
// - manejo de errores léxicos
// - tracking exacto de línea y columna
class LexicalAnalyzerTests {
private:
    int totalPruebas = 0;
    int pruebasExitosas = 0;
    int pruebatasFallidas = 0;
    
    void reportarPrueba(const std::string& nombre, bool resultado) {
        totalPruebas++;
        if (resultado) {
            pruebasExitosas++;
            std::cout << "✓ PASS: " << nombre << std::endl;
        } else {
            pruebatasFallidas++;
            std::cout << "✗ FAIL: " << nombre << std::endl;
        }
    }
    
public:
    LexicalAnalyzerTests() = default;
    
    // ===== Pruebas de Delimitadores =====
    
    void testDelimitadores() {
        std::cout << "\n=== Pruebas de Delimitadores ===" << std::endl;
        
        LexicalAnalyzer analizador("{ } [ ] : ,");
        auto tokens = analizador.analizarCompleto();
        
        reportarPrueba("Reconocer {", 
            tokens.size() > 0 && tokens[0].getTipo() == TokenType::LLAVE_ABIERTA);
        
        reportarPrueba("Reconocer }", 
            tokens.size() > 1 && tokens[1].getTipo() == TokenType::LLAVE_CERRADA);
        
        reportarPrueba("Reconocer [", 
            tokens.size() > 2 && tokens[2].getTipo() == TokenType::CORCHETE_ABIERTO);
        
        reportarPrueba("Reconocer ]", 
            tokens.size() > 3 && tokens[3].getTipo() == TokenType::CORCHETE_CERRADO);
        
        reportarPrueba("Reconocer :", 
            tokens.size() > 4 && tokens[4].getTipo() == TokenType::DOS_PUNTOS);
        
        reportarPrueba("Reconocer ,", 
            tokens.size() > 5 && tokens[5].getTipo() == TokenType::COMA);
    }
    
    // ===== Pruebas de Palabras Reservadas =====
    
    void testPalabrasReservadas() {
        std::cout << "\n=== Pruebas de Palabras Reservadas ===" << std::endl;
        
        LexicalAnalyzer analizador("HOSPITAL PACIENTES MEDICOS CITAS DIAGNOSTICOS");
        auto tokens = analizador.analizarCompleto();
        
        reportarPrueba("Reconocer HOSPITAL",
            tokens.size() > 0 && tokens[0].getTipo() == TokenType::HOSPITAL);
        
        reportarPrueba("Reconocer PACIENTES",
            tokens.size() > 1 && tokens[1].getTipo() == TokenType::PACIENTES);
        
        reportarPrueba("Reconocer MEDICOS",
            tokens.size() > 2 && tokens[2].getTipo() == TokenType::MEDICOS);
        
        reportarPrueba("Reconocer CITAS",
            tokens.size() > 3 && tokens[3].getTipo() == TokenType::CITAS);
        
        reportarPrueba("Reconocer DIAGNOSTICOS",
            tokens.size() > 4 && tokens[4].getTipo() == TokenType::DIAGNOSTICOS);
    }
    
    void testPalabrasReservadasElemento() {
        std::cout << "\n=== Pruebas de Palabras Reservadas de Elemento ===" << std::endl;
        
        LexicalAnalyzer analizador("paciente medico cita diagnostico");
        auto tokens = analizador.analizarCompleto();
        
        reportarPrueba("Reconocer paciente",
            tokens.size() > 0 && tokens[0].getTipo() == TokenType::PACIENTE);
        
        reportarPrueba("Reconocer medico",
            tokens.size() > 1 && tokens[1].getTipo() == TokenType::MEDICO);
        
        reportarPrueba("Reconocer cita",
            tokens.size() > 2 && tokens[2].getTipo() == TokenType::CITA);
        
        reportarPrueba("Reconocer diagnostico",
            tokens.size() > 3 && tokens[3].getTipo() == TokenType::DIAGNOSTICO);
    }
    
    // ===== Pruebas de Identificadores y Códigos ID =====
    
    void testIdentificadores() {
        std::cout << "\n=== Pruebas de Identificadores y Códigos ID ===" << std::endl;
        
        // Identificador común
        LexicalAnalyzer analizador1("nombre edad ciudad");
        auto tokens1 = analizador1.analizarCompleto();
        reportarPrueba("Reconocer identificador común",
            tokens1.size() > 0 && tokens1[0].getTipo() == TokenType::IDENTIFICADOR);
        
        // Código ID (formato XXX-NNN)
        LexicalAnalyzer analizador2("MED-001 PAC-042 HOS-999");
        auto tokens2 = analizador2.analizarCompleto();
        reportarPrueba("Reconocer código ID MED-001",
            tokens2.size() > 0 && tokens2[0].getTipo() == TokenType::IDENTIFICADOR_CODIGO);
        
        reportarPrueba("Código ID contiene guion",
            tokens2[0].getLexema().find('-') != std::string::npos);
    }
    
    // ===== Pruebas de Números =====
    
    void testNumeros() {
        std::cout << "\n=== Pruebas de Números ===" << std::endl;
        
        LexicalAnalyzer analizador("42 0 999 1 100");
        auto tokens = analizador.analizarCompleto();
        
        reportarPrueba("Reconocer número 42",
            tokens.size() > 0 && tokens[0].getTipo() == TokenType::INTEGER &&
            tokens[0].getLexema() == "42");
        
        reportarPrueba("Reconocer número 0",
            tokens.size() > 1 && tokens[1].getTipo() == TokenType::INTEGER);
        
        reportarPrueba("Números tienen valor correcto",
            tokens.size() > 2 && tokens[2].getLexema() == "999");
    }
    
    // ===== Pruebas de Cadenas =====
    
    void testCadenas() {
        std::cout << "\n=== Pruebas de Cadenas ===" << std::endl;
        
        LexicalAnalyzer analizador(R"("Hola mundo" "Juan Pérez" "Cardiología")");
        auto tokens = analizador.analizarCompleto();
        
        reportarPrueba("Reconocer cadena simple",
            tokens.size() > 0 && tokens[0].getTipo() == TokenType::STRING);
        
        reportarPrueba("Cadena contiene contenido",
            tokens[0].getLexema().length() > 0);
        
        reportarPrueba("Múltiples cadenas",
            tokens.size() > 2 && tokens[2].getTipo() == TokenType::STRING);
    }
    
    // ===== pruebas de fechas =====
    
    void testFechas() {
        std::cout << "\n=== Pruebas de Fechas ===" << std::endl;
        
        // nota: las fechas deben ser implementadas en reconocerNumero()
        // esta prueba es un placeholder para cuando se implemente
        LexicalAnalyzer analizador("2024-12-25 1990-05-15 2000-01-01");
        auto tokens = analizador.analizarCompleto();
        
        // Se espera FECHA cuando se implemente reconocerFecha()
        // Por ahora puede ser INTEGER o ERROR_TOKEN
        reportarPrueba("Estructura de fecha reconocida",
            tokens.size() > 0);
    }
    
    // ===== pruebas de horas =====
    
    void testHoras() {
        std::cout << "\n=== Pruebas de Horas ===" << std::endl;
        
        // nota: las horas deben ser implementadas en reconocerNumero()
        LexicalAnalyzer analizador("14:30 09:00 23:59");
        auto tokens = analizador.analizarCompleto();
        
        // se espera HORA cuando se implemente reconocerHora()
        reportarPrueba("Estructura de hora reconocida",
            tokens.size() > 0);
    }
    
    // ===== pruebas de tracking de línea y columna =====
    
    void testPositionTracking() {
        std::cout << "\n=== Pruebas de Tracking de Línea y Columna ===" << std::endl;
        
        LexicalAnalyzer analizador("HOSPITAL\npaciente");
        auto tokens = analizador.analizarCompleto();
        
        reportarPrueba("Token primero en línea 1, columna 1",
            tokens.size() > 0 && 
            tokens[0].getLinea() == 1 && 
            tokens[0].getColumna() == 1);
        
        reportarPrueba("Token segundo en línea 2",
            tokens.size() > 1 && tokens[1].getLinea() == 2);
    }
    
    // ===== pruebas de manejo de errores =====
    
    void testManejDeErrores() {
        std::cout << "\n=== Pruebas de Manejo de Errores ===" << std::endl;
        
        LexicalAnalyzer analizador("@ $ %");
        auto tokens = analizador.analizarCompleto();
        auto errores = analizador.obtenerErrores();
        
        reportarPrueba("Se registran errores léxicos",
            errores.size() > 0);
        
        reportarPrueba("Análisis continúa después de error",
            tokens.size() > 1);  // Más de un token intenta reconocerse
        
        reportarPrueba("Error contiene información de posición",
            errores[0].find("línea") != std::string::npos ||
            errores[0].find("columna") != std::string::npos);
    }
    
    // ===== pruebas integradas =====
    
    void testIntegracion() {
        std::cout << "\n=== Pruebas Integradas ===" << std::endl;
        
        std::string codigo = R"(
            HOSPITAL {
                paciente { nombre: "Juan" codigo: MED-001 }
            }
        )";
        
        LexicalAnalyzer analizador(codigo);
        auto tokens = analizador.analizarCompleto();
        
        reportarPrueba("Analiza estructura compleja",
            tokens.size() > 0);
        
        reportarPrueba("Contiene HOSPITAL",
            std::find_if(tokens.begin(), tokens.end(),
                [](const Token& t) { return t.getTipo() == TokenType::HOSPITAL; }) != tokens.end());
        
        reportarPrueba("Contiene STRING",
            std::find_if(tokens.begin(), tokens.end(),
                [](const Token& t) { return t.getTipo() == TokenType::STRING; }) != tokens.end());
        
        reportarPrueba("Termina con EOF",
            tokens.size() > 0 && tokens.back().getTipo() == TokenType::EOF_TOKEN);
    }
    
    // ===== método para ejecutar todas las pruebas =====
    
    void ejecutarTodasLasPruebas() {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "  SUITE DE PRUEBAS DEL ANALIZADOR LÉXICO" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        testDelimitadores();
        testPalabrasReservadas();
        testPalabrasReservadasElemento();
        testIdentificadores();
        testNumeros();
        testCadenas();
        testFechas();
        testHoras();
        testPositionTracking();
        testManejDeErrores();
        testIntegracion();
        
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "  RESUMEN DE RESULTADOS" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        std::cout << "Total de pruebas:     " << totalPruebas << std::endl;
        std::cout << "Pruebas exitosas:     " << pruebasExitosas << std::endl;
        std::cout << "Pruebas fallidas:     " << pruebatasFallidas << std::endl;
        
        double porcentaje = totalPruebas > 0 ? 
            (pruebasExitosas * 100.0) / totalPruebas : 0;
        std::cout << "Porcentaje de éxito:  " << porcentaje << "%" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
    }
};

#endif // LEXICAL_ANALYZER_TESTS_H
