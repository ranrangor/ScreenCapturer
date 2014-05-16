#!/bin/sh


rm icons.h 

for FILENAME in *.png;do

    gdk-pixbuf-csource --raw --name icon_${FILENAME%%.*}  ${FILENAME} >>icons.h

done

sed -i "1 i#ifndef _SC_ICONS_H_\n#define _SC_ICONS_H_" icons.h
sed -i "$ a#endif // _SC_ICONS_H_" icons.h

