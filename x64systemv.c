#include "stdio.h"
#include "parsetree.c"
#include "string.h"

int funcCodeSymbolIndex = 0;
int funcCodePrintIndex = 0;
int funcCodeErrorIndex = 0;

void dataSectionHeader(FILE *prog, ParseTree *parseTree) {
  fprintf(prog, "section .data\n\n");
  fprintf(prog, "\towl db '<(o.O)>', 0xa, 'Wan Shi Tong is watching for mistakes', 0xa, 0x0\n");
  fprintf(prog, "\t\towlLen equ $ - owl\n");
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
  fprintf(prog, "\txor rdi, rdi\n");
  fprintf(prog, "\tsyscall\n");
}


void funcCode(FILE *prog, ParseTree *parseTree, char symbolName[50][20], char symbolType[50][10], int symbolLocation[50], int stackDepth, int symbolTableIndex) {
  
  // TODO: change these to match the enum types
  if (parseTree->type == INT) {
    printf("x64systemv.c intValue: %d\n", parseTree->constantValue);
    fprintf(prog, "\tmov edi, %d\n", parseTree->constantValue);
    fprintf(prog, "\tmov eax, edi\n");
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
    else if (unOpExpr->UnOpType == ASSIGN) {
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
    else if (unOpExpr->UnOpType == PRINTLN) {
      printf("x64systemv.c: print called\n");

      funcCode(prog, unOpExpr->rint, symbolName, symbolType, symbolLocation, stackDepth, symbolTableIndex);

      fprintf(prog, "\tmov rcx, 10\t; set the divisor to 10\n");
      fprintf(prog, "\txor rbx, rbx\t; clear rbx to use as counter\n");
      fprintf(prog, "\tpush 0xa\t\t; push new line character to the stack\n");

      // setup the divide loop
      fprintf(prog, "divide_loop%d:\n", funcCodePrintIndex);
      fprintf(prog, "\txor rdx, rdx\t; clear remainder\n");
      fprintf(prog, "\tdiv rcx\t\t; divide rax by 10, remainder in rdx\n");
      fprintf(prog, "\tadd rdx, '0'\t; convert integer to ascii\n");
      fprintf(prog, "\tpush rdx\t; push the remainder to the stack\n");
      fprintf(prog, "\tinc rbx\t\t; increment the number of pushes to the stack\n\n");
      fprintf(prog, "\tcmp rax, 0\t; check to make sure quotient is not 0\n");
      fprintf(prog, "\tjne divide_loop%d\t; if != 0, continue loop\n", funcCodePrintIndex);

      // setup the print result loop
      fprintf(prog, "print_result%d:\n", funcCodePrintIndex);
      fprintf(prog, "; prepare the registers for syswrite\n");
      fprintf(prog, "\tmov rax, 1\t; syscall number for syswrite\n");
      fprintf(prog, "\tmov rdi, 1\t; file descriptor for stdout\n");
      fprintf(prog, "\tmov rdx, 1\t; the number of bytes to write to the console\n\n");
      fprintf(prog, "\tmov rsi, rsp\t; move address of first character into the buffer register\n");
      fprintf(prog, "\tadd rsp, 8\t; add 8 bytes to rsp register to simulate a pop\n");
      fprintf(prog, "\tsyscall\t; syswrite syscall\n");
      fprintf(prog, "\tdec rbx\t\t; decrement counter register\n");
      fprintf(prog, "\tcmp rbx, 0\t; if rbx > 0, there are more bytes to print\n");
      fprintf(prog, "\tjne print_result%d\t; loop back\n", funcCodePrintIndex);

      
      // printing the new line in assembly
      fprintf(prog, "\tmov rsi, rsp\t; add the new line character to be printed\n");
      fprintf(prog, "\tadd rsp, 8\t; simulate a pop\n");
      fprintf(prog, "\tsyscall\t; print the new line\n");

      funcCodePrintIndex++;

    }
    else if (unOpExpr->UnOpType == PRINT) {
      printf("\n\n\nAre you sure you don't want to print a new line???\n\n\n");

      funcCode(prog, unOpExpr->rint, symbolName, symbolType, symbolLocation, stackDepth, symbolTableIndex);

      fprintf(prog, "error%d: \n", funcCodeErrorIndex++);
      fprintf(prog, "\tmov rax, 1\t; syswrite\n");
      fprintf(prog, "\tmov rsi, owl\t; move address to rsi\n");
      fprintf(prog, "\tmov rdi, 1\t; file descriptor for stdout\n");
      fprintf(prog, "\tmov rdx, owlLen\t; move the string length to rdx\n");
      fprintf(prog, "\tsyscall\t; print the error message\n");

      fprintf(prog, "\tmov rcx, 10\t; set the divisor to 10\n");
      fprintf(prog, "\txor rbx, rbx\t; clear out rbx to use as the counter\n");
      

      // setup the divide loop
      fprintf(prog, "divide_loop%d:\n", funcCodePrintIndex);
      fprintf(prog, "\txor rdx, rdx\t; clear remainder\n");
      fprintf(prog, "\tdiv rcx\t\t; divide rax by 10, remainder in rdx\n");
      fprintf(prog, "\tadd rdx, '0'\t; convert integer to ascii\n");
      fprintf(prog, "\tpush rdx\t; push the remainder to the stack\n");
      fprintf(prog, "\tinc rbx\t\t; increment the number of pushes to the stack\n\n");
      fprintf(prog, "\tcmp rax, 0\t; check to make sure quotient is not 0\n");
      fprintf(prog, "\tjne divide_loop%d\t; if != 0, continue loop\n", funcCodePrintIndex);

      // setup the print result loop
      fprintf(prog, "print_result%d:\n", funcCodePrintIndex);
      fprintf(prog, "; prepare the registers for syswrite\n");
      fprintf(prog, "\tmov rax, 1\t; syscall number for syswrite\n");
      fprintf(prog, "\tmov rdi, 1\t; file descriptor for stdout\n");
      fprintf(prog, "\tmov rdx, 1\t; the number of bytes to write to the console\n\n");
      fprintf(prog, "\tmov rsi, rsp\t; move address of first character into the buffer register\n");
      fprintf(prog, "\tadd rsp, 8\t; add 8 bytes to rsp register to simulate a pop\n");
      fprintf(prog, "\tsyscall\t; syswrite syscall\n");
      fprintf(prog, "\tdec rbx\t\t; decrement counter register\n");
      fprintf(prog, "\tcmp rbx, 0\t; if rbx > 0, there are more bytes to print\n");
      fprintf(prog, "\tjne print_result%d\t; loop back\n", funcCodePrintIndex);

      funcCodePrintIndex++;

    } 
  } else if (parseTree->type == ERROR) {
    ErrorExpr *errExpr = parseTree->errExpr;
    
    if (errExpr->ErrorOpType == COUTERROR) {
      printf("\n\n\nERROR: There is something missing\n\n\n");

      fprintf(prog, "error%d: \n", funcCodeErrorIndex++);
      fprintf(prog, "\tmov rax, 1\t; syswrite\n");
      fprintf(prog, "\tmov rsi, owl\t; move address to rsi\n");
      fprintf(prog, "\tmov rdi, 1\t; file descriptor for stdout\n");
      fprintf(prog, "\tmov rdx, owlLen\t; move the string length to rdx\n");
      fprintf(prog, "\tsyscall\t; print the error message\n");

      funcCode(prog, println(errExpr->rint), symbolName, symbolType, symbolLocation, stackDepth, symbolTableIndex);

    }
    else if (errExpr->ErrorOpType == ASSIGNERROR) {
      printf("\n\n\nERROR: There is something missing\n\n\n");

      fprintf(prog, "error%d: \n", funcCodeErrorIndex++);
      fprintf(prog, "\tmov rax, 1\t; syswrite\n");
      fprintf(prog, "\tmov rsi, owl\t; move address to rsi\n");
      fprintf(prog, "\tmov rdi, 1\t; file descriptor for stdout\n");
      fprintf(prog, "\tmov rdx, owlLen\t; move the string length to rdx\n");
      fprintf(prog, "\tsyscall\t; print the error message\n");

      funcCode(prog, assign(errExpr->rint), symbolName, symbolType, symbolLocation, stackDepth, symbolTableIndex);

    }


  }
}
