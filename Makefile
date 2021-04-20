CFLAGS = -g -Wall
DEPS = miniShell.h parser.h execute.h

all: miniShell program1 program2 program3

miniShell: miniShell.o parser.o execute.o
	gcc  $(CFLAGS) -o miniShell miniShell.o parser.o execute.o

program1: program1.o
	gcc  $(CFLAGS) -o program1 program1.o

program2: program2.o
	gcc  $(CFLAGS) -o program2 program2.o

program3: program3.o
	gcc  $(CFLAGS) -pthread -o program3 program3.o


%.o: %.c $(DEPS)
	gcc  $(CFLAGS) -c -o $@ $< 

clean:
	rm -f miniShell *.o
	rm -f program1 *.o
	rm -f program2 *.o
	rm -f program3 *.o
