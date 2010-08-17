all: gui grid
EXEC=sdk_gui

gui: sdk_gui.c sdk_gui.h
	$(CC) $(CFLAGS) sdk_gui.c -c -I../

grid: sdk_grid.c sdk_grid.h
	$(CC) $(CFLAGS) sdk_grid.c -c -I../

clean:
	@rm -f $(EXEC).o
