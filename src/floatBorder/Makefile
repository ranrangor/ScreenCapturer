
#CC=clang 
#CFLAGS=`pkg-config --cflags gtk+-3.0`

.PHONY:clean_fb

floatBorder.o:floatBorder.c floatBorder.h
	${CC} ${CFLAGS} -c $< -o $@


clean_fb:
	-rm floatBorder.o


