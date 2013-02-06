GL_FLAGS=-lSOIL -lGL -lGLU -lglut #-lGLEW
MAC_FLAGS=-framework OpenGL -framework Glut -framework Carbon -lSOILMac
C_FLAGS=-g -Wall
DEPS=
OBJ=main.o InitShader.o SceneObject.o ModelObject.o LightObject.o OBJParser.o World.o CameraObject.o Train.o
UNAME := $(shell uname)

%.o: %.cpp $(DEPS)
	g++ $(C_FLAGS) -c -o $@ $< $(GL_FLAGS)

ifeq ($(UNAME), Linux)
project: $(OBJ)	
	g++ $(C_FLAGS) -o $@ $^ -L. $(GL_FLAGS)
endif
ifeq ($(UNAME), Darwin)
project: $(OBJ)
	g++ $(C_FLAGS) -o $@ $^ -L. $(MAC_FLAGS)
endif

clean:
	rm *.o project
