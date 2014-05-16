
.PHONY:clean
.PHONY:all


all:
	cd icons && ${MAKE}	
	cd src && ${MAKE}   
	cp src/scapturer .



clean:
	cd src && ${MAKE} clean_src
	cd icons && ${MAKE} clean_iconsdata
