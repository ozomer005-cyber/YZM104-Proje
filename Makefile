CC = gcc
CFLAGS = -Wall
LIBS = -lallegro -lallegro_main -lallegro_font -lallegro_primitives -lallegro_image
HEDEF = oyun.exe

all:
	$(CC) main.c -o $(HEDEF) $(CFLAGS) $(LIBS)

run: all
	./$(HEDEF)

clean:
	rm -f $(HEDEF) 