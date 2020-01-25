all: obj/main.o
	cc obj/main.o -o bin/mbwm -L/usr/local/lib -L/usr/X11R6/lib -lX11

obj/main.o: src/main.c
	cc -c src/main.c -o obj/main.o -I/usr/local/include -I/usr/X11R6/include

clean:
	rm obj/*.o bin/*
