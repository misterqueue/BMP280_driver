CC=gcc
CFLAGS=-c -O2 -Wall -Wextra -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -pie -fPIE -ftrapv -D_FORTIFY_SOURCE=2 -Wl,-z,relro,-z,now
LDFLAGS=-l wiringPi -lm
SOURCES_MAIN=get.c pi.c bmp280.c
HEADERS_MAIN=bmp280.h pi.h
OBJECTS_MAIN=$(SOURCES_MAIN:.c=.o)
EXECUTABLE_MAIN=get

all: $(SOURCES_MAIN) $(EXECUTABLE_MAIN)

$(EXECUTABLE_MAIN): $(OBJECTS_MAIN) $(HEADERS_MAIN)
	$(CC) $(LDFLAGS) $(OBJECTS_MAIN) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
	rm $(EXECUTABLE_MAIN)

