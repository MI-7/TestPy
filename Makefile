#makefile contains a set of rules.
#a rule consists of 3 parts: target; prerequisites; and the command
#  target: prereq1, prereq2
#          commands
#  foo.o: foo.c foo.h
#         gcc -c foo.c

CC = cc
src_dir = src
include_dir = include
CFLAGS = -g -Wall -I. -I$(include_dir)/

mypython:	main.o object.o typeobject.o intobject.o listobject.o
			$(CC) -o mypython main.o object.o typeobject.o intobject.o listobject.o

main.o:	$(src_dir)/main.c \
		$(include_dir)/python.h
		$(CC) -c $(CFLAGS) $(src_dir)/main.c

object.o:	$(src_dir)/object.c \
			$(include_dir)/python.h
			$(CC) -c $(CFLAGS) $(src_dir)/object.c

typeobject.o:	$(src_dir)/typeobject.c \
				$(include_dir)/python.h
				$(CC) -c $(CFLAGS) $(src_dir)/typeobject.c

intobject.o:	$(src_dir)/intobject.c \
				$(include_dir)/python.h
				$(CC) -c $(CFLAGS) $(src_dir)/intobject.c

listobject.o:	$(src_dir)/listobject.c \
				$(include_dir)/python.h
				$(CC) -c $(CFLAGS) $(src_dir)/listobject.c

clean:
	find . -name '*.o' -exec rm -f {} ';'
	find . -name 'mypython' -exec rm -f {} ';'