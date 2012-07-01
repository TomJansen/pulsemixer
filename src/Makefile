CFLAGS=`pkg-config --cflags libpulse ncurses` -Wall -I../include
LIBS=`pkg-config --libs libpulse ncurses`

pa-mixer: main.o model.o view.o
	gcc $(LIBS) main.o model.o view.o -o pulsemixer

.c.o: $<
	gcc $(CFLAGS) -c $<

clean:
	rm -f *.o pulsemixer
