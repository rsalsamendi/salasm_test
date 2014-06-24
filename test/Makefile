OBJECTS = main.o
LIBS = -lsalsasm ../udis86/libudis86/.libs/libudis86.a -lpthread -lstdc++
LIBPATH := -L.
INCLUDE := -I../gtest-1.6.0 -I../gtest-1.6.0/include -I../udis86 -I../libsalsasm
CFLAGS = -Wall -Wextra -g
LDFLAGS = -Wl,-rpath,\$$ORIGIN
AS = yasm

.PHONY: all
all: udis86 libsalsasm test

udis86:
	cd ../udis86 && $(MAKE) && cd ../test
libsalsasm:
	cd ../libsalsasm && $(MAKE) && cd ../test
	cp ../libsalsasm/libsalsasm.so .

%16.bin: %.asm test.inc
	$(AS) -w -DARCH=16 -o $*16.bin $*.asm

%32.bin: %.asm test.inc
	$(AS) -w -DARCH=32 -o $*32.bin $*.asm

%64.bin: %.asm test.inc
	$(AS) -w -DARCH=64 -o $*64.bin $*.asm

BIOS-bochs-latest.bin: BIOS-bochs-latest
	cp BIOS-bochs-latest BIOS-bochs-latest.bin

%.o: %.cpp
	$(CC) $(CFLAGS) -c $*.cpp $(INCLUDE)

test: libsalsasm udis86 test_one_byte16.bin test_two_byte16.bin test_three_byte16.bin test_one_byte32.bin test_two_byte32.bin test_three_byte32.bin test_one_byte64.bin BIOS-bochs-latest.bin

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
