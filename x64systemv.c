#include "stdio.h"
#include "parsetree.c"
#include "string.h"

int funcCodeSymbolIndex = 0;

void dataSectionHeader(FILE *prog, ParseTree *parseTree) {
  fprintf(prog, "section .data\n\n");
}

void textSectionHeader(FILE *prog, ParseTree *parseTree) {
  fprintf(prog, "section .text\n\n");
  fprintf(prog, "\tglobal _start\n\n");
  fprintf(prog, "_start:\n");
}

void funcPrologue(FILE *prog, int stackDepth) {
  // prepare a generic stackframe suffiently large
  fprintf(prog, "\n\tmov rbp, rsp\n");
  fprintf(prog, "\tsub rsp, %d\n", stackDepth * 12);
}

void sysExit(FILE *prog) {
  fprintf(prog, "\n\tmov rax, 60\n");
  fprintf(prog, "\tsyscall\n");
}


void funcCode(FILE *prog, ParseTree *parseTree, char symbolName[50][20], char symbolType[50][10], int symbolLocation[50], int stackDepth, int symbolTableIndex) {
  
  // TODO: change these to match the enum types
  if (parseTree->type == INT) {
    printf("x64systemv.c intValue: %d\n", parseTree->constantValue);
    fprintf(prog, "\tmov edi, %d\n", parseTree->constantValue);
  } 
  // add string clause to handle string types
  else if (parseTree->type == STRING) {
    printf("x64systemv.c stringValue: %s\n", parseTree->stringValue);

    for (int i = 0; i < symbolTableIndex; i++) {
      printf("x64systemv.c: symbol name index == %s\n", symbolName[i]);
      if (strcmp(symbolName[i], parseTree->stringValue) == 0) { // TODO: stringcompare from clib "string.h" strcmp
        printf("x64systemv: related proxy location %d\n", symbolLocation[i] * 12);
        fprintf(prog, "\tmov edi, [rbp-%d]\t; retrieve variable value from the stack\n", (symbolLocation[i]) * 12); // TODO: this could be the issue
      } 
    }

  } 
  
  else if (parseTree->type == BINOP) {
    BinOpExpr *binOpExpr = parseTree->binExpr;

    if (binOpExpr->BinOpType == ADDITION) {
      
      funcCode(prog, binOpExpr->lint, symbolName, symbolType, symbolLocation, stackDepth, symbolTableIndex);
      fprintf(prog, "\tmov [rbp-%d], edi\t; save argument one to the stack\n", (stackDepth * 12) - 8);

      funcCode(prog, binOpExpr->rint, symbolName, symbolType, symbolLocation, stackDepth, symbolTableIndex);
      fprintf(prog, "\tmov [rbp-%d], edi\t; save argument two to the stack\n", (stackDepth * 12) - 4);
      fprintf(prog, "\tadc edi, [rbp-%d]\t; add the second argument to the first argument from the stack\n", (stackDepth * 12) - 8);
      fprintf(prog, "\tmov [rbp-%d], edi\t; save result to the stack\n", (stackDepth * 12));
      fprintf(prog, "\tmov eax, edi\t; move to rax according to ABI\n"); // return into rax due to system V abi
    } 
    
    
    else if (binOpExpr->BinOpType == SUBTRACTION) {

      funcCode(prog, binOpExpr->lint, symbolName, symbolType, symbolLocation, stackDepth, symbolTableIndex);
      fprintf(prog, "\tmov [rbp-%d], edi\t; save argument one to the stack\n", (stackDepth * 12 - 8));

      funcCode(prog, binOpExpr->rint, symbolName, symbolType, symbolLocation, stackDepth, symbolTableIndex);
      fprintf(prog, "\tmov [rbp-%d], edi\t; save argument two to the stack\n", (stackDepth * 12) - 4);
      fprintf(prog, "\tsub edi, [rbp-%d]\t; subtract\n", (stackDepth * 12) - 8);
      fprintf(prog, "\tmov [rbp-%d], edi\t; save result to the stack\n", (stackDepth * 12));
      fprintf(prog, "\tmov eax, edi\t; move result into rax according to SystemV ABI\n");
    } 
    
    else if (binOpExpr->BinOpType == MULTIPLICATION) {

      funcCode(prog, binOpExpr->lint, symbolName, symbolType, symbolLocation, stackDepth, symbolTableIndex);
      fprintf(prog, "\tmov [rbp-%d], edi\t; save argument one to the stack\n", (stackDepth * 12) - 8);
      fprintf(prog, "\tmov rax, rdi\t; set up the first argument into rax\n");

      funcCode(prog, binOpExpr->rint, symbolName, symbolType, symbolLocation, stackDepth, symbolTableIndex);
      fprintf(prog, "\tmov [rbp-%d], edi\t; save argument two to the stack\n", (stackDepth * 12) - 4);
      fprintf(prog, "\tmul rdi\t; multiply the second argument by what is in rax\n");
      fprintf(prog, "\tmov [rbp-%d], eax\t; save the result to the stack. RAX already holds the answer\n", (stackDepth * 12));
    } 
    
    else if (binOpExpr->BinOpType == DIVIDE) {

      funcCode(prog, binOpExpr->rint, symbolName, symbolType, symbolLocation, stackDepth, symbolTableIndex);
      fprintf(prog, "\tmov [rbp-%d], edi\t; save argument one to the stack\n", (stackDepth * 12) - 8);
      fprintf(prog, "\tmov rax, rdi\t; set up the first argument into rax\n");

      funcCode(prog, binOpExpr->lint, symbolName, symbolType, symbolLocation, stackDepth, symbolTableIndex);
      fprintf(prog, "\tmov [rbp-%d], edi\t; save argument two to the stack\n", (stackDepth * 12) - 4);
      fprintf(prog, "\tdiv edi; divide what is in rax\n");
      fprintf(prog, "\tmov [rbp-%d], eax\t; save the result to the stack\n", (stackDepth * 12));
    }
  } else if (parseTree->type == UNOP) {
    UnOpExpr *unOpExpr = parseTree->unExpr;

    if (unOpExpr->UnOpType == NEGATION) {

      funcCode(prog, unOpExpr->rint, symbolName, symbolType, symbolLocation, stackDepth, symbolTableIndex);
      fprintf(prog, "\tmov [rbp-%d], edi\t; save the first argument to the stack\n", (stackDepth * 12) - 8);
      fprintf(prog, "\tcmp edi, 0\t; check if the register is zero\n");
      fprintf(prog, "\tjz is_zero\t; jump to zero label\n");

      // if edi is > 0, change to zero here
      fprintf(prog, "\tmov rax, 0\t; otherwise make rax == 0\n"); 
      fprintf(prog, "\tjmp done\n");     

      // label for is zero
      fprintf(prog, "is_zero:\t; here is if argument was 0\n");
      fprintf(prog, "\tmov rax, 1\t; make rax == 1\n");

      // label for done
      fprintf(prog, "done:\n");
      //fprintf(prog, "\tmov rax, edi\n"); // return into rax due to system V abi
      fprintf(prog, "\tmov [rbp-%d], rax\n", (stackDepth * 12) - 4); // save the answer in the stack

    } 
    if (unOpExpr->UnOpType == ASSIGN) {
      // then we have to create a variable in assembly
      // first: set up the label in assembly
      funcCode(prog, unOpExpr->rint, symbolName, symbolType, symbolLocation, stackDepth, symbolTableIndex);
      fprintf(prog, "%s: ", symbolName[funcCodeSymbolIndex]);
      printf("x64systemv: assign variable name: %s\n", symbolName[funcCodeSymbolIndex]);
      fprintf(prog, "\tmov [rbp-%d], edi\t; move the variable value to the stack\n", (symbolLocation[funcCodeSymbolIndex]) * 12);
      printf("x64systemv: variable proxy location %d\n", symbolLocation[funcCodeSymbolIndex]);
      printf("x64systemv: stack depth %d\n", stackDepth);
      funcCodeSymbolIndex++;
    }
  }
}
