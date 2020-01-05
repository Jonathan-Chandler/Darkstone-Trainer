#OBJS specifies which files to compile as part of the project
OBJS = main.cpp .\src\Trainer.cpp

#CC specifies which compiler we're using
CC = g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = 

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = 

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
#COMPILER_FLAGS = -w -Wl,-subsystem,windows
# COMPILER_FLAGS = -Wall -std=c++11 -static-libgcc
COMPILER_FLAGS = -static-libgcc -static-libstdc++ -Wl,-Bstatic -lgcc -lstdc++

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = 

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = trainer

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) -DSTEAM $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)-Steam
	$(CC) -DGOG $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)-GOG

clean:
	rm *.exe *.o
