CC = g++
INCLUDE_PATHS = -Iinclude
LIBRARY_PATHS = 
COMPILER_FLAGS = 
#COMPILER_FLAGS = -std=c11 -static-libgcc -Wl,-Bstatic -lgcc
LINKER_FLAGS = 
EXE = trainer.exe
SRC := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c, build/%.o, $(SRC))

all : $(EXE)

$(EXE) : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(EXE)

build/%.o : src/%.c
	mkdir -p build
	$(CC) -c $< $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $@

clean:
	rm -rf trainer.exe build/*.o
