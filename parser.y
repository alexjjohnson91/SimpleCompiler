
%{

#include "stdio.h"
#include "parsetree.c"
#include "x64systemv.c"
#include "parserstack.h"
#include "string.h"

int yylex();
int yyparse();

void yyerror(const char *str) {
    fprintf(stderr, "%s\n", str);
}


FILE *yyin;
ParserStack *parserStack;

/* Symbol Table 1.4: reverse the parser stack */
ParserStack *reverseParserStack;


/* Symbol Table 1.1 */
char symbolName[50][20];
char symbolType[50][10];
int symbolLocation[50];

int symbolTableIndex = 0;
int symbolTableProxy = 1;

/* Symbol Table 1.2 */
/* TODO: complete functionality of insertSymbol function */
void insertSymbol(char* type, char* name, int location) {

    strcpy(symbolName[symbolTableIndex], name);
    strcpy(symbolType[symbolTableIndex], type);

    // i think this is wrong
    symbolLocation[symbolTableIndex] = location;
    symbolTableIndex++;
}

int main(int argc, char *argv[])
{
    #if YYDEBUG == 1
    extern int yydebug;
    yydebug = 1;
    #endif

    yyin = fopen(argv[1], "r");
    parserStack = parserStackCreate();
    reverseParserStack = parserStackCreate();
    yyparse();

    ParseTree *parseTree; // this is used to reverse the stack and to call funccode
    // Symbol Table 1.4: reverse the stack
    while (parserStack->depth > 0) {
        parseTree = parserStackPop(parserStack);
        parserStackPush(reverseParserStack, parseTree);
    }

    // opening of Code Generation
    FILE *prog = fopen("prog.s", "wb");

    dataSectionHeader(prog, parseTree);
    textSectionHeader(prog, parseTree);
    funcPrologue(prog, symbolTableProxy - 1);

    // second while loop for calling funccode
    while (reverseParserStack->depth > 0) {
        parseTree = parserStackPop(reverseParserStack);
        funcCode(prog, parseTree, symbolName, symbolType, symbolLocation, symbolTableProxy - 1, symbolTableIndex);
    }


    // Symbol Table 1.5: dump the symbol table
    printf("**** Symbol Table Dump ****\n");
    for (int i = 0; i < symbolTableIndex; i++) {
        printf("-> symbol table entry %d -> \n", i);
        printf("NAME: %s\n", symbolName[i]);
        printf("TYPE: %s\n", symbolType[i]);
        printf("LOCATION: %d\n", symbolLocation[i]);
    }

    // close code gen
    sysExit(prog);
    fclose(prog);
}

%}



%token TOK_LBRACE TOK_COUT TOK_LBRACKET
%token TOK_RBRACE TOK_RETURN TOK_ADD TOK_SUB TOK_MUL TOK_DIV TOK_NEG
%token TOK_LPAREN TOK_RPAREN TOK_SEMI TOK_LETTER TOK_EQUALS

%union
{
    int number;
    char* string;
}
%token <number> TOK_UINT
%token <string> TOK_TYPE
%token <string> TOK_IDENTIFIER

%%

// GRAMMAR RULES OR PRODUCTIONS
// tokens are terminals, non-tokens are non-terminals

program:
    function
    ;
function:
    function stmt
    |
    function TOK_TYPE TOK_IDENTIFIER TOK_LPAREN TOK_RPAREN TOK_LBRACE stmt_list TOK_RBRACE
    | /* support null */
    ;
stmt:
    expr TOK_SEMI
    |
    TOK_RETURN expr TOK_SEMI
    |
    TOK_TYPE TOK_IDENTIFIER TOK_EQUALS expr TOK_SEMI /* Symbol Table 1.3 */
    {
        insertSymbol((char*)$1, (char*)$2, symbolTableProxy);
        // is this what we have to do to call the assign function?
        ParseTree* rint = parserStackPop(parserStack);
        parserStackPush(parserStack, assign(rint));
        symbolTableProxy++;
        printf("\t\tparser.y: proxy incremented to %d\n", symbolTableProxy);
    }
    |
    TOK_LBRACE stmt_list TOK_RBRACE
    ;
stmt_list:
    stmt
    |
    stmt_list stmt
    ;
expr:
    TOK_COUT TOK_LBRACKET TOK_LBRACKET expr TOK_SEMI
    {
        ParseTree *rint = parserStackPop(parserStack);
        parserStackPush(parserStack, print(rint));
        symbolTableProxy++;

    }
    |
    value TOK_ADD expr
    {
        ParseTree *lint = parserStackPop(parserStack);
        ParseTree *rint = parserStackPop(parserStack);
        parserStackPush(parserStack, add(lint, rint));
        symbolTableProxy++;
        printf("\t\tparser.y: proxy incremented to %d\n", symbolTableProxy);
    }
    |
    value TOK_SUB expr
    {
        ParseTree *lint = parserStackPop(parserStack);
        ParseTree *rint = parserStackPop(parserStack);
        parserStackPush(parserStack, subtract(lint, rint));
        symbolTableProxy++;
        printf("\t\tparser.y: proxy incremented to %d\n", symbolTableProxy);
    }
    |
    value TOK_MUL expr
    {
        ParseTree *lint = parserStackPop(parserStack);
        ParseTree *rint = parserStackPop(parserStack);
        parserStackPush(parserStack, multiply(lint, rint));
        symbolTableProxy++;
        printf("\t\tparser.y: proxy incremented to %d\n", symbolTableProxy);
    }
    |
    value TOK_DIV expr
    {
        ParseTree *lint = parserStackPop(parserStack);
        ParseTree *rint = parserStackPop(parserStack);
        parserStackPush(parserStack, divide(lint, rint));
        symbolTableProxy++;
        printf("\t\tparser.y: proxy incremented to %d\n", symbolTableProxy);
    }
    |
    TOK_NEG expr
    {
        ParseTree *rint = parserStackPop(parserStack);
        parserStackPush(parserStack, negate(rint));
        symbolTableProxy++;
        printf("\t\tparser.y: proxy incremented to %d\n", symbolTableProxy);
    }
    |
    value
    ;
value:
    TOK_IDENTIFIER
    {
        printf("\t\tparser.y string push value: %s\n", $1);
        parserStackPush(parserStack, string_type($1));
    }
    |
    number
    ;
number:
    TOK_UINT
    {
        printf("\t\tparser.y uint push value: %d\n", $1);
        parserStackPush(parserStack, int_type($1));
    }
    ;
