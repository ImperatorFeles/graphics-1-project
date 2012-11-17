main: main.cpp InitShader.o
	g++ -g -o project main.cpp InitShader.o -Wall -framework OpenGL -framework GLUT

InitShader: InitShader.cpp
	g++ -c InitShader.cpp

