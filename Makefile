
PROG_FILES_DIR = .program_cfiles/

.PHONY: clean all run add sub mul div neg

all: compiler prog

lex.yy.c: lex.l
	flex lex.l

y.outout y.tab.c y.tab.h: parser.y
	yacc -d -v parser.y

parserstack.o: parserstack.c
	clang -c parserstack.c

compiler: lex.yy.c y.tab.c parserstack.o
	clang -DYYDEBUG $^ -o $@

prog.s: prog.c
	./compiler $<

prog.o: prog.s
	nasm -felf64 $<

prog: prog.o
	ld -static $< -o $@


run:
	gdb ./prog

clean:
	rm -f y.tab.c
	rm -f y.tab.h
	rm -f parserstack.o
	rm -f lex.yy.c
	rm -f y.output
	rm -f prog.s
	rm -f prog
	rm -f prog.o
	rm -f compiler

add:
	cp $(PROG_FILES_DIR)prog-addition.c prog.c

sub:
	cp $(PROG_FILES_DIR)prog-subtraction.c prog.c

mul:
	cp $(PROG_FILES_DIR)prog-multiplication.c prog.c

div:
	cp $(PROG_FILES_DIR)prog-division.c prog.c

neg:
	cp $(PROG_FILES_DIR)prog-negation.c prog.c