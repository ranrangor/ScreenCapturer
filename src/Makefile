
CC=clang -g
CFLAGS=`pkg-config --cflags gtk+-3.0`
LIBS=`pkg-config --libs gtk+-3.0`



.PHONY :clean



scapturer: main.c sc-app.o sc-window.o
	${CC} ${CFLAGS} $^ ${LIBS} -o $@

sc-app.o: sc-app.c sc-app.h
	${CC} ${CFLAGS} -c $< -o $@

sc-window.o: sc-window.c sc-window.h
	${CC} ${CFLAGS} -c $< -o $@



clean:
	-rm *.o
	-rm scapturer


