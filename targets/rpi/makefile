CPP=arm-linux-gnueabihf-g++
CC=arm-linux-gnueabihf-gcc
LD=arm-linux-gnueabihf-ld
AS=arm-linux-gnueabihf-as

# Remote user name and client
USERNAME=root
CLIENT=hallonpaj
CLIENTLOCATION=/mnt/usb/quake

# Quiet som warnings we don't really care about atm
CSHUTUP= -Wno-trigraphs -Wno-dangling-pointer

ENGFLAGS=-DTRACE_STACK 



CFLAGS= -std=gnu99 -fno-short-enums -Og -ggdb -trigraphs -Wall -Wpedantic -Werror  $(ENGFLAGS) $(CSHUTUP)
LDFLAGS= -Wl,-Map=quake.map
LIBS= -lm

.PHONY: all upload clean showsz

all: gendir saybuild quake

gendir:
	@mkdir -p out/rend

upload:
	@echo "Uploading.."
	@scp quake $(USERNAME)@$(CLIENT):$(CLIENTLOCATION)

saybuild:
	@echo "Building.."

HOSTOBJECTS := $(patsubst %.c, out/%.o, $(wildcard *.c))
out/%.o: %.c
	@$(CC) $(CFLAGS) $< -c -o $@

COREOBJECTS := $(patsubst ../../quake/%.c, ./out/%.o, $(wildcard ../../quake/*.c))
out/%.o: ../../quake/%.c
	@$(CC) $(CFLAGS) $< -c -o $@

RENDOBJS := $(patsubst ../../quake/render/%.c, ./out/rend/%.o, $(wildcard ../../quake/render/*.c))
out/rend/%.o: ../../quake/render/%.c
	@$(CC) $(CFLAGS) $< -c -o $@

QUAKEOBJS=\
	$(HOSTOBJECTS) \
	$(COREOBJECTS) \
	$(RENDOBJS) \


quake: $(QUAKEOBJS)
	@echo "Linking..."
	@$(CC) $(LDFLAGS) -o $@ $(QUAKEOBJS) $(LIBS)
	@echo
#	@./sizehelp

showsz:
	@./sizehelp
	@echo

clean:
	@echo "Cleaning.."
	@rm -f out/*.o
	@rm -f out/rend/*.o
	@rm -f quake
