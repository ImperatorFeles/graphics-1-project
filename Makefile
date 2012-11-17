GCC_OPTIONS=-Wall -pedantic -I ../include
GL_OPTIONS=-framework OpenGL -framework GLUT
OPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)

main: main.cpp InitShader.o
	g++ -g -o project main.cpp InitShader.o $(OPTIONS)

InitShader: InitShader.cpp
	g++ -c InitShader.cpp

