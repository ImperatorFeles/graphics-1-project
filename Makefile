GL_FLAGS=-lSOIL -lGL -lGLU -lglut -lGLEW -lopenal
MAC_FLAGS=-framework OpenGL -framework Glut -framework Carbon -lSOILMac -framework OpenAL
C_FLAGS=-g -Wall
DEPS=
OBJ=main.o InitShader.o SceneObject.o ModelObject.o OBJParser.o

%.o: %.cpp $(DEPS)
	g++ $(C_FLAGS) -c -o $@ $< $(GL_FLAGS)

project: $(OBJ)
	g++ $(C_FLAGS) -o $@ $^ -L. $(GL_FLAGS)

mac: $(OBJ)
	g++ $(C_FLAGS) -o $@ $^ -L. $(MAC_FLAGS)

clean:
	rm *.o project
