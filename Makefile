GL_FLAGS=-lGL -lGLU -lglut -lGLEW
DEPS=
OBJ=main.o InitShader.o SceneObject.o ModelObject.o

%.o: %.cpp $(DEPS)
	g++ -g -c -o $@ $< $(GL_FLAGS)

project: $(OBJ)
	g++ -g -o $@ $^ $(GL_FLAGS)

clean:
	rm *.o project
