// syntax_analyzer.h
// parser recursivo descendente para taskscript
#ifndef PROYECTO2_SYNTAX_ANALYZER_H
#define PROYECTO2_SYNTAX_ANALYZER_H

#include "lexical_analyzer.h"
#include "ast.h"
#include "error.h"
#include <vector>
#include <memory>

// vector global de errores sintacticos
extern std::vector<error_t> errores;

class syntaxanalyzer {
public:
    syntaxanalyzer(lexicalanalyzer &lex);
    std::unique_ptr<ProgramNode> parse();

    // errores accesibles desde fuera
    // usamos el vector global `errores`

private:
    lexicalanalyzer &lex;
    token current; // token actual

    void advance();
    void synchronize();
    void panic(const std::string &msg);

    // funciones de parsing por la gramática proporcionada
    std::unique_ptr<ProgramNode> parseProgram();
    std::unique_ptr<ColumnNode> parseColumn();
    std::vector<std::unique_ptr<ColumnNode>> parseColumns();
    std::unique_ptr<TaskNode> parseTask();
    std::vector<std::unique_ptr<TaskNode>> parseTasks();
    std::unique_ptr<AttributeNode> parseAttribute();
    std::vector<std::unique_ptr<AttributeNode>> parseAttributes();

    int errorCount;
};

#endif // PROYECTO2_SYNTAX_ANALYZER_H
