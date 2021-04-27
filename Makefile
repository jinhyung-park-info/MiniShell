CFLAGS = -g -Wall
DEPS = miniShell.h parser.h execute.h

all: miniShell merge_sort_serial merge_sort_multiprocess merge_sort_multithread

miniShell: miniShell.o parser.o execute.o
	gcc  $(CFLAGS) -o miniShell miniShell.o parser.o execute.o

merge_sort_serial: merge_sort_serial.o
	gcc  $(CFLAGS) -o merge_sort_serial merge_sort_serial.o

merge_sort_multiprocess: merge_sort_multiprocess.o
	gcc  $(CFLAGS) -o merge_sort_multiprocess merge_sort_multiprocess.o

merge_sort_multithread: merge_sort_multithread.o
	gcc  $(CFLAGS) -pthread -o merge_sort_multithread merge_sort_multithread.o


%.o: %.c $(DEPS)
	gcc  $(CFLAGS) -c -o $@ $< 

clean:
	rm -f miniShell merge_sort_serial merge_sort_multiprocess merge_sort_multithread *.o