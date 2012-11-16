main: main.cpp InitShader.o
	g++ -o project main.cpp InitShader.o -lGL -lGLU -lglut -lm

InitShader: InitShader.cpp
	g++ -c InitShader.cpp

