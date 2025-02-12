# compiler
CC = g++
LL = ld

#path to raylib
MAINFILE = main.cpp
NAME = game.exe

#compiler flags:
#	-g = debugging info
#	-Wall = all warnings
CPPFLAGS = $(MAINFILE) -o $(NAME) -O0 -Wall -I include/ -lm -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm -lraylibdll -lmariadb -lws2_32 -lbthprops -lcurl-x64
NOWARN = $(MAINFILE) -o $(NAME) -O0 -I include/ -lm -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm -lraylibdll -lmariadb -lws2_32 -lbthprops -lcurl-x64
DEBUG = -g -DDEBUG
O3 = $(MAINFILE) -o $(NAME) -O3 -Wall -I include/ -lm -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm -lraylibdll -lmariadb -mwindows -lws2_32 -lbthprops -lcurl-x64
main:
	$(CC) $(CPPFLAGS)
nowarn:
	$(CC) $(NOWARN)
o3:
	$(CC) $(O3)
nowrun:
	$(CC) $(NOWARN)
	cls
	$(NAME)
runo3:
	$(CC) $(O3)
	cls
	$(NAME)
debug:
	$(CC) $(CPPFLAGS) $(DEBUG)