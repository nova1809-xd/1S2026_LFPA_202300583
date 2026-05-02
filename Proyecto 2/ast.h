// ast.h
// definimos nodos para el arbol de derivacion de taskscript
#ifndef PROYECTO2_AST_H
#define PROYECTO2_AST_H

#include <string>
#include <vector>
#include <memory>
#include "token.h"

// tipos de nodo
enum class AstNodeType { PROGRAM, COLUMN, TASK, ATTRIBUTE };

struct AstNode {
    AstNodeType type;
    int line;
    int column;
    AstNode(AstNodeType t, int l, int c): type(t), line(l), column(c) {}
    virtual ~AstNode() = default;
};

struct AttributeNode : public AstNode {
    // tipo: prioridad/responsable/fecha
    std::string keyLexeme; // lexema de la keyword (ej: prioridad)
    std::string valueLexeme; // valor asociado
    tokentype valueType; // tipo del valor (ALTA/MEDIA/BAJA o CADENA o FECHA)
    AttributeNode(const std::string &k, const std::string &v, tokentype vt, int l, int c)
        : AstNode(AstNodeType::ATTRIBUTE, l, c), keyLexeme(k), valueLexeme(v), valueType(vt) {}
};

struct TaskNode : public AstNode {
    std::string name; // lexema de la tarea
    std::vector<std::unique_ptr<AttributeNode>> attributes;
    TaskNode(const std::string &n, int l, int c) : AstNode(AstNodeType::TASK, l, c), name(n) {}
};

struct ColumnNode : public AstNode {
    std::string name; // lexema de la columna
    std::vector<std::unique_ptr<TaskNode>> tasks;
    ColumnNode(const std::string &n, int l, int c) : AstNode(AstNodeType::COLUMN, l, c), name(n) {}
};

struct ProgramNode : public AstNode {
    std::string tableroName; // nombre del tablero
    std::vector<std::unique_ptr<ColumnNode>> columns;
    ProgramNode(const std::string &n, int l, int c) : AstNode(AstNodeType::PROGRAM, l, c), tableroName(n) {}
};

#endif // PROYECTO2_AST_H
