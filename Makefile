SRC_FILES = src/main.c
CC_FLAGS = -Wall -Wextra -g -std=c11 -Wall
CC = clang

all:
	${CC} ${SRC_FILES} ${CC_FLAGS} -ledit -o little-ghost
