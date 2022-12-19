CFLAGS=-g #-Wall -Werror

all: tests lib_tar.o  print_function.o #include <stdlib.h>

lib_tar.o: lib_tar.c lib_tar.h

print_function.o : print_function.c print_function.h

tests: tests.c lib_tar.o print_function.o

clean:
	rm -f lib_tar.o print_function.o tests soumission.tar

submit: all
	tar --posix --pax-option delete=".*" --pax-option delete="*time*" --no-xattrs --no-acl --no-selinux -c *.h *.c Makefile > soumission.tar
