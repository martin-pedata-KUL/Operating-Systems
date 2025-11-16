all: main.c sensor_db.h sensor_db.c logger.h logger.c
	gcc -g -Wall main.c sensor_db.c logger.c -o main.out

test: testing.c sensor_db.h sensor_db.c logger.h logger.c
	gcc -g -Wall testing.c sensor_db.c logger.c -o testing.out

run: all
	./main.out

runTest: all
	./testing.out

memtest: all
	valgrind --leak-check=yes ./main.out

clean:
	rm -f *.out sensor_db.csv gateway.log

zip:
	zip milestone2.zip *.c *.h Makefile
