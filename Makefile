CC=g++

CFLAGS=-c -std=c++0x -O3 -Wall
LDFLAGS=-lGLEW -lGL -lglut -lXi -pthread -L/usr/lib64
SOURCES=main.cpp ShaderTools.cpp Mat4f.cpp OpenGLMatrixTools.cpp Vec3f.cpp Boid.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=Tut01

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o $(EXECUTABLE)
