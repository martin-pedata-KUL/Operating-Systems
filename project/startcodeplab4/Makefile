zip:
	zip milestone3.zip *.c *.h Makefile

build: main.c sbuffer.c
	gcc *.c -o main.out -lpthread

run: build
	./main.out

clean:
	rm -rf *.out *.csv