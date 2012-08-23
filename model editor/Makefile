compiler_options = -Wall -I"C:/MinGW/include"

libraries = -lmingw32 -lglfw -lopengl32 -lglew32
source_files = main.cpp

exec_name = voxelsomethingeditor

default: all

all:
	g++ $(compiler_options) -o $(exec_name) $(source_files) $(libraries)

clean:
	-rm -f $(exec_name)