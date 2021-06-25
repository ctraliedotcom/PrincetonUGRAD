CC = gcc
CPP = g++

#CFLAGS = -Wall -ansi -pedantic
#CFLAGS = -Wall -pedantic -g
CFLAGS = -g

LIBS = -I /usr/include/sphinx2 -I /usr/include/mrss -I/usr/include/panel-2.0 -I/usr/include/gtk-2.0 -I/usr/include/libgnomeui-2.0 -I/usr/include/libbonoboui-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -I/usr/include/libart-2.0 -I/usr/include/gconf/2 -I/usr/include/gnome-keyring-1 -I/usr/include/libgnome-2.0 -I/usr/include/libgnomecanvas-2.0 -I/usr/include/gnome-vfs-2.0 -I/usr/lib/gnome-vfs-2.0/include -I/usr/include/orbit-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/libbonobo-2.0 -I/usr/include/bonobo-activation-2.0 -I/usr/include/libxml2 -I/usr/include/gail-1.0 -I/usr/include/gstreamer-0.10 -I/usr/include/libwnck-1.0 -I/usr/include/startup-notification-1.0 -lpanel-applet-2 -lgnomeui-2 -lSM -lICE -lbonoboui-2 -lgnomevfs-2 -lgnomecanvas-2 -lgnome-2 -lpopt -lbonobo-2 -lbonobo-activation -lORBit-2 -lart_lgpl_2 -lgconf-2 -lgstbase-0.10 -lgstreamer-0.10 -lgthread-2.0 -lrt -lxml2 -lwnck-1 -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lpangoft2-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lgio-2.0 -lcairo -lpango-1.0 -lfreetype -lz -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lglib-2.0 -lmrss -lsphinx2 -lsphinx2fe -lsphinx2ad


LIBSPP = -I /usr/include/estools -I /usr/include/festival/ -lestools -lestbase -leststring -lesd -lncurses -ltermcap -lstdc++

##### Build rules for non-file targets #####
all: main

clobber: clean
	rm -f *~ \#*\# core

clean:
	rm -f main *.o

##### Build rules for programs #####
main: main.o news.o speak.o weather.o number_game.o sphinx_recognition.o alarmconfig.o play.o
	$(CC) $(CFLAGS) $(LIBS) $(LIBSPP) -o TAB main.o news.o speak.o weather.o sphinx_recognition.o alarmconfig.o play.o number_game.o /usr/lib/libFestival.a

test: test.o news.o speak.o weather.o sphinx_recognition.o alarmconfig.o play.o
	$(CC) $(CFLAGS) $(LIBS) $(LIBSPP) -o test test.o news.o speak.o weather.o sphinx_recognition.o alarmconfig.o play.o /usr/lib/libFestival.a

##### Build rules for .o files #####
main.o: main.c
	$(CC) $(CFLAGS) $(LIBS) -c main.c
	
test.o: test.c
	$(CC) $(CFLAGS) $(LIBS) -c test.c

news.o: news.c news.h
	$(CC) $(CFLAGS) $(LIBS) -c news.c	
	
speak.o: speak.cpp speak.h
	$(CPP) $(CFLAGS) $(LIBSPP) -c speak.cpp

weather.o: weather.c weather.h
	$(CC) $(CFLAGS) $(LIBS) -c weather.c

number_game.o: number_game.c number_game.h
	$(CC) $(CFLAGS) $(LIBS) -c number_game.c

sphinx_recognition.o: sphinx_recognition.c sphinx_recognition.h
	$(CC) $(CFLAGS) $(LIBS) -c sphinx_recognition.c

alarmconfig.o: alarmconfig.c alarmconfig.h
	$(CC) $(CFLAGS) $(LIBS) -c alarmconfig.c

play.o: play.c play.h
	$(CC) $(CFLAGS) $(LIBS) -c play.c
