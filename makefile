CC = g++
INCLUDE_PATHS = -Iinc
LIBRARY_PATHS = 
#COMPILER_FLAGS = 
COMPILER_FLAGS = -std=c11 -static-libgcc -Wl,-Bstatic -lgcc
LINKER_FLAGS = 
EXE = trainer.exe
SRC := $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp, build/%.o, $(SRC))

all : $(EXE)

$(EXE) : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(EXE)

build/%.o : src/%.cpp
	mkdir -p build
	$(CC) -c $< $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $@

clean:
	rm -rf trainer.exe build/*.o
