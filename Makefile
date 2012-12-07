GL_FLAGS=-lSOIL -lGL -lGLU -lglut -lGLEW
C_FLAGS=-g -Wall
DEPS=
OBJ=main.o InitShader.o SceneObject.o ModelObject.o OBJParser.o

%.o: %.cpp $(DEPS)
	g++ $(C_FLAGS) -c -o $@ $< $(GL_FLAGS)

project: $(OBJ)
	g++ $(C_FLAGS) -o $@ $^ -L. $(GL_FLAGS)

clean:
	rm *.o project
