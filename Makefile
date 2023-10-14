run: bin/main
	./bin/main

bin/main: main.c
	mkdir -p ./bin && gcc main.c -o ./bin/main
