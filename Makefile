compiler_options = -Wall
libraries = -lglfw -lGL -lGLEW
exec_name = voxelsomething

objs = main.o

default: all

all: $(objs)
	g++ $(compiler_options) -o $(exec_name) $(objs) $(libraries)
	echo $(exec_name)

clean:
	-rm -f $(objs) $(exec_name)
