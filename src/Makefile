
CC=clang -g
CFLAGS=`pkg-config --cflags gtk+-3.0`
LIBS=`pkg-config --libs gtk+-3.0` -lm



.PHONY :clean



scapturer: main.c sc-app.o sc-window.o sc-canvas.o sc-operable.o sc-arrow.o sc-shape.o sc-painter.o
	${CC} ${CFLAGS} $^ ${LIBS} -o $@

sc-app.o: sc-app.c sc-app.h
	${CC} ${CFLAGS} -c $< -o $@

sc-window.o: sc-window.c sc-window.h
	${CC} ${CFLAGS} -c $< -o $@

sc-canvas.o: sc-canvas.c sc-canvas.h
	${CC} ${CFLAGS} -c $< -o $@

sc-operble.o: sc-operable.c sc-operable.h
	${CC} ${CFLAGS} -c $< -o $@

sc-shape.o: sc-shape.c sc-shape.h
	${CC} ${CFLAGS} -c $< -o $@

#sc-line.o: sc-line.c sc-line.h
#	${CC} ${CFLAGS} -c $< -o $@

sc-arrow.o: sc-arrow.c sc-arrow.h
	${CC} ${CFLAGS} -c $< -o $@

#sc-colorchooser.o: sc-colorchooser.c sc-colorchooser.h
#	${CC} ${CFLAGS} -c $< -o $@

#sc-widthsetter.o: sc-widthsetter.c sc-widthsetter.h
#	${CC} ${CFLAGS} -c $< -o $@

sc-painter.o: sc-painter.c sc-painter.h
	${CC} ${CFLAGS} -c $< -o $@




clean:
	-rm *.o
	-rm scapturer


