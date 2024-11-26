FILES= ./build/server/webserver
all: $(FILES)
	mv build/server/webserver /home/slobe/HTTP-Webserver
./build/server/webserver:
	gcc ./server/server.c -o ./build/server/webserver

clean:
	rm -rf webserver