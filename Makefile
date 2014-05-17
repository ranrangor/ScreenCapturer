

#CC=clang -g 
#CFLAGS=`pkg-config --cflags gtk+-3.0`
#LIBS=`pkg-config --libs gtk+-3.0` -lm


.PHONY:clean
.PHONY:all


all:
	cd icons && ${MAKE}	
	cd src && ${MAKE}   
	cp src/scapturer .



clean:
	cd src && ${MAKE} clean_src
	cd icons && ${MAKE} clean_iconsdata
