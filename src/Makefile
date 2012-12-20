CFLAGS=`pkg-config --cflags libpulse ncurses` -Wall -I../include
LIBS=`pkg-config --libs libpulse ncurses`

all: pulsemixer

pulsemixer: main.o model.o view.o
	gcc main.o model.o view.o $(LIBS) -o pulsemixer

.c.o: $<
	gcc $(CFLAGS) -c $<

install: pulsemixer
	install ./pulsemixer /usr/local/bin/

clean:
	rm -f *.o pulsemixer
