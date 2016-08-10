#
# makefile
#

all: alsa-stream alsa-switch

alsa-stream:	alsa-stream.o util.o
	$(CC) ${LDFLAGS} -o alsa-stream alsa-stream.o util.o -lasound

alsa-switch:	alsa-switch.o util.o
	$(CC) ${LDFLAGS} -o alsa-switch alsa-switch.o util.o -lasound

alsa-stream.o:	alsa-stream.c
	$(CC) $(CFLAGS) -c alsa-stream.c -o alsa-stream.o

alsa-switch.o:	alsa-switch.c
	$(CC) $(CFLAGS) -c alsa-switch.c -o alsa-switch.o

util.o:	util.c
	$(CC) $(CFLAGS) -c util.c -o util.o

install: alsa-stream alsa-switch
	install -o root -g root -m 755 alsa-stream $(DESTDIR)/usr/local/bin/alsa-stream
	install -o root -g root -m 755 alsa-switch ${DESTDIR}/usr/local/bin/alsa-switch

