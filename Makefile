FILES= ./build/server/server.o
all: $(FILES)

./build/server/server.o:
	gcc -c ./server/server.c -o ./build/server/server.o

clean:
	rm -rf $(FILES)