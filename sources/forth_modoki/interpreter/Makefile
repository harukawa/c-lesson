# Makefile
main: cl_getc.o parser.o stack.o eval.o dict.o main.o continuation.o primitives.o
	cc -o main cl_getc.o parser.o stack.o eval.o dict.o main.o continuation.o primitives.o
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
main.o: main.c
	cc -c main.c
continuation.o: continuation.c
	cc -c continuation.c
primitives.o: primitives.c
	cc -c primitives.c

clean:
	rm -f cl_getc.o parser.o stack.o eval.o dict.o main.o continuation.o primitives.o

