all: gui
EXEC=sdk_gui

gui: sdk_gui.c sdk_gui.h
	$(CC) $(CFLAGS) -Wall sdk_gui.c -c -I../

clean:
	@rm -f $(EXEC).o
