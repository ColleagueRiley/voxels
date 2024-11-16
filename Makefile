run: build
	./build/voxels.out

build: src
	mkdir build -p
	${CC} ${CFLAGS} -o ./build/voxels.out src/*.c -lGLEW -lglfw -lGL -lm -I./include -std=c2x -Wall -Wextra -Wpedantic
