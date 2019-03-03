# Makefile

main: cl_utils.o parser.o cl_assert.o asm.o main.o
	cc -o main cl_utils.o cl_assert.o parser.o asm.o main.o

cl_utils.o: cl_utils.c
	cc -c cl_utils.c

cl_assert.o: cl_assert.c
	cc -c cl_assert.c

cl_parser.o: parser.c
	cc -c parser.c

asm.o: asm.c
	cc -c asm.c
	
main.o: main.c
	cc -c main.c

clean:
	rm -f cl_utils.o parser.o cl_assert.o asm.o