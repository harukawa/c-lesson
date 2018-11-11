# Makefile
eval: cl_getc.o parser.o stack.o eval.o dict.o
	cc -o eval cl_getc.o parser.o stack.o eval.o dict.o
cl_getc.o: cl_getc.c
	cc -c cl_getc.c
parser.o: parser.c
	cc -c parser.c
stack.o: stack.c
	cc -c stack.c
eval.o: eval.c
	cc -c eval.c
dict.o: dict.c
	cc -c dict.c
clean:
	rm -f cl_getc.o parser.o stack.o eval.o dict.o

