
CC=gcc -g 
CFLAGS=`pkg-config --cflags gtk+-3.0`
#LIBS=`pkg-config --libs gtk+-3.0` -lm


.PHONY :all_menus
.PHONY :clean_menus



all_menus:sc-colorchooser.o sc-widthchooser.o sc-fontsizechooser.o\
	sc-shapechooser.o sc-widthsetter.o



sc-colorchooser.o: sc-colorchooser.c sc-colorchooser.h ../../icons/icons.h
	${CC} ${CFLAGS} -c $< -o $@

sc-widthchooser.o: sc-widthchooser.c sc-widthchooser.h ../../icons/icons.h
	${CC} ${CFLAGS} -c $< -o $@

sc-shapechooser.o: sc-shapechooser.c sc-shapechooser.h ../../icons/icons.h
	${CC} ${CFLAGS} -c $< -o $@

sc-fontsizechooser.o: sc-fontsizechooser.c sc-fontsizechooser.h ../../icons/icons.h
	${CC} ${CFLAGS} -c $< -o $@

sc-widthsetter.o: sc-widthsetter.c sc-widthsetter.h ../../icons/icons.h
	${CC} ${CFLAGS} -c $< -o $@


clean_menus:
	-rm *.o


