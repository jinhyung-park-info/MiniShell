CFLAGS = -g -Wall
DEPS = miniShell.h parser.h execute.h

miniShell: miniShell.o parser.o execute.o
	gcc  $(CFLAGS) -o miniShell miniShell.o parser.o execute.o

%.o: %.c $(DEPS)
	gcc  $(CFLAGS) -c -o $@ $< 

clean:
	rm -f miniShell *.o
