CC=mpic++
LD=mpic++
CFLAGS=-c -Wall
LDFLAGS=-lmpi
SOURCE_DIR=src
BUILD_DIR=build
INC_DIR=inc

SRC=main.cpp $(wildcard $(SOURCE_DIR)/*.cpp)
OBJ=build/main.o $(pathsubst src/%.cpp,build/%.o,$(SRC))
INCLUDES=$(addprefix -I,$(INC_DIR))
