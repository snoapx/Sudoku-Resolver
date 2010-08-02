all: gui
EXEC=sdk_gui

gui: sdk_gui.c sdk_gui.h
	$(CC) $(CFLAGS) -g -Wall sdk_gui.c -c -I../

clean:
	@rm -f $(EXEC).o
