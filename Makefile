CC=i686-w64-mingw32-gcc
CFLAGS=-I.
EXE=min_win32svc.exe

min_win32svc: svc_main.o
	     $(CC) -o $(EXE) svc_main.o -I.

clean:
	rm *.o; rm min_win32svc.exe
