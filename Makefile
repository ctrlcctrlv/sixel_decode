all:
	gcc -Og -g `libsixel-config --libs` -o sixel_decode sixel_decode.c
