OBJECTS = main.o
LIBS = -lsalsasm ../udis86/libudis86/.libs/libudis86.a -lpthread -lstdc++
LIBPATH := -L.
INCLUDE := -I../gtest-1.6.0 -I../gtest-1.6.0/include -I../udis86 -I../libsalsasm
CFLAGS = -Wall -Wextra
LDFLAGS = -Wl,-rpath,\$$ORIGIN
AS = yasm

.PHONY: all
all: udis86 libsalsasm test

udis86:
	cd ../udis86 && $(MAKE) && cd ../test
libsalsasm:
	cd ../libsalsasm && $(MAKE) && cd ../test
	cp ../libsalsasm/libsalsasm.so .

%.bin: %.asm
	$(AS) -o $*.bin $*.asm

%.bin: BIOS-bochs-latest
	cp BIOS-bochs-latest BIOS-bochs-latest.bin

%.o: %.cpp
	$(CC) $(CFLAGS) -c $*.cpp $(INCLUDE)

test: libsalsasm udis86 test_primary.bin test_secondary.bin BIOS-bochs-latest.bin

test: $(OBJECTS)
	$(CC) -o $@ $(LDFLAGS) $(LIBPATH) $(OBJECTS) $(LIBS)


.PHONY: clean
clean:
	rm -rf *.o
	rm -f test
	rm -f libsalsasm.so
	rm -f *.bin
	rm -f BIOS-bochs-latest.bin
	cd ../udis86 && $(MAKE) clean && cd ../test
	cd ../libsalsasm && $(MAKE) clean && cd ../test
