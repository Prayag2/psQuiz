run: build
	./bin/main

build: main.c
	mkdir -p ./bin && gcc main.c -o ./bin/main
