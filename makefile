OBJS = .\src\main.c .\src\trainer.c
CC = gcc
INCLUDE_PATHS = -Iinclude -Isrc
LIBRARY_PATHS = 
COMPILER_FLAGS = -std=c11 -static-libgcc -Wl,-Bstatic -lgcc
LINKER_FLAGS = 
OBJ_NAME = trainer

all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

clean:
	rm *.exe *.o
