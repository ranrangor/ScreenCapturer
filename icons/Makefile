


.PHONY:clean_iconsdata


PICS= arrow.png done.png exit.png hide_menu.png move_handle.png painter.png reselect.png\
   	save.png shape_circle.png shape.png shape_rect.png show_menu.png text.png undo.png




icons.h:${PICS}
	sh png2data.sh


clean_iconsdata:
	-rm icons.h
