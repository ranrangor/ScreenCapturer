
CC=clang -g
CFLAGS=`pkg-config --cflags gtk+-3.0`
LIBS=`pkg-config --libs gtk+-3.0`


.PHONY:clean

test-floatBorder:test-floatBorder.c floatBorder.o
	${CC} ${CFLAGS} $^ ${LIBS} -o $@

floatBorder.o:floatBorder.c floatBorder.h
	${CC} ${CFLAGS} -c $< -o $@


chean:
	-rm *.o
	-rm test-floatBorder

