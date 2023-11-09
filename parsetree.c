#include <stdlib.h>

#ifndef COMPILER_PARSETREE
#define COMPILER_PARSETREE

typedef enum { INT, STRING, BINOP, UNOP } ParseTreeType;
typedef enum { ADDITION, SUBTRACTION, MULTIPLICATION, DIVIDE } ParseTreeBinOp;

// new enum for Unary operator
typedef enum { NEGATION, ASSIGN } ParseTreeUnOp;
// new enum end

typedef struct parseTree ParseTree;

typedef struct BinOpExpr {
    ParseTreeBinOp BinOpType;
    ParseTree *lint;
    ParseTree *rint;
} BinOpExpr;

// new expression struct for unary operator NOT
typedef struct UnOpExpr {
    ParseTreeUnOp UnOpType;
    ParseTree *rint;
} UnOpExpr;

struct parseTree {
    ParseTreeType type;
    union {
        int constantValue;
        char* stringValue; // is this right?
        BinOpExpr *binExpr;
        UnOpExpr *unExpr;
    };
};

ParseTree *int_type(int intValue) {
    ParseTree *parseTree = malloc(sizeof(parseTree));
    parseTree->type = INT;
    parseTree->constantValue = intValue;
    return parseTree;
}

// Symbol Table: create string function
ParseTree *string_type(char* stringValue) {
    ParseTree *parseTree = malloc(sizeof(parseTree));
    parseTree->type = STRING;
    parseTree->stringValue = stringValue;
    return parseTree;
}

ParseTree *add(ParseTree *lint, ParseTree *rint) {
    ParseTree *parseTree = malloc(sizeof(parseTree));
    BinOpExpr *binOpExpr = malloc(sizeof(binOpExpr));
    binOpExpr->BinOpType = ADDITION;
    binOpExpr->lint = lint;
    binOpExpr->rint = rint;
    parseTree->type = BINOP;
    parseTree->binExpr = binOpExpr;
    return parseTree;
}

ParseTree *subtract(ParseTree *lint, ParseTree *rint) {
    ParseTree *parseTree = malloc(sizeof(parseTree));
    BinOpExpr *binOpExpr = malloc(sizeof(binOpExpr));
    binOpExpr->BinOpType = SUBTRACTION;
    binOpExpr->lint = lint;
    binOpExpr->rint = rint;
    parseTree->type = BINOP;
    parseTree->binExpr = binOpExpr;
    return parseTree;
}

ParseTree *multiply(ParseTree *lint, ParseTree *rint) {
    ParseTree *parseTree = malloc(sizeof(parseTree));
    BinOpExpr *binOpExpr = malloc(sizeof(binOpExpr));
    binOpExpr->BinOpType = MULTIPLICATION;
    binOpExpr->lint = lint;
    binOpExpr->rint = rint;
    parseTree->type = BINOP;
    parseTree->binExpr = binOpExpr;
    return parseTree;
    
}

ParseTree *divide(ParseTree *lint, ParseTree *rint) {
    ParseTree *parseTree = malloc(sizeof(parseTree));
    BinOpExpr *binOpExpr = malloc(sizeof(binOpExpr));
    binOpExpr->BinOpType = DIVIDE;
    binOpExpr->lint = lint;
    binOpExpr->rint = rint;
    parseTree->type = BINOP;
    parseTree->binExpr = binOpExpr;
    return parseTree;
}

ParseTree *negate(ParseTree *rint) {
    ParseTree *parseTree = malloc(sizeof(parseTree));
    UnOpExpr *unOpExpr = malloc(sizeof(unOpExpr));
    unOpExpr->UnOpType = NEGATION;
    unOpExpr->rint = rint;
    parseTree->type = UNOP;
    parseTree->unExpr = unOpExpr;
    return parseTree;
}

ParseTree *assign(ParseTree *rint) {
    ParseTree *parseTree = malloc(sizeof(parseTree));
    UnOpExpr *unOpExpr = malloc(sizeof(unOpExpr));
    unOpExpr->UnOpType = ASSIGN;
    unOpExpr->rint = rint;
    parseTree->type = UNOP;
    parseTree->unExpr = unOpExpr;
    return parseTree;
}

#endif