export CC=gcc
export CFLAGS=`pkg-config --cflags --libs gtk+-2.0` -g -O3 -Wall
EXEC=sdk_resolver

all:sdk_resolver

sdk_gui:
	@echo "## Building sdk_gui"
	@cd gui/ && make gui

sdk_grid:
	@echo "## Building sdk_grid"
	@cd gui/ && make grid

sdk_resolver: sdk_gui sdk_resolver.c sdk_resolver.h main.c
	$(CC) $(CFLAGS) sdk_resolver.c main.c gui/sdk_gui.o -o $(EXEC) -I./gui/

clean:
	cd gui/ && make clean
	@rm -f $(EXEC)

