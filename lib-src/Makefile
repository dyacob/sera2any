
CC = gcc              # use cc if gcc not available
CFLAGS = -c
LEX = flex            # use lex if flex not available
RANLIB = ranlib       # delete $(RANLIB) line below if unavailable
MYINCLUDE = ../include
LIBRARY = libeth.a
LIBSOURCES = get_fchar.c libeth.c
MEMBERS = $(LIBSOURCES:.c=.o)
ARFLAGS = rcv
#DEPFLAGS = -DSUPPORT_DOS
#DEPFLAGS = -DSUPPORT_TEX

 
 

all: ${LIBRARY}
 
 

$(LIBRARY): $(MEMBERS)
	    $(AR) $(ARFLAGS) $@ $(MEMBERS) 
	    $(RANLIB) $(LIBRARY)

get_fchar.c:	get_fchar.l
	${LEX} -t get_fchar.l > get_fchar.c


.c.o:  fidel.map fidel.h libeth.h
	$(CC) $(CFLAGS) $(DEPFLAGS) -I$(MYINCLUDE) $*.c

clean:
	    rm -f $(MEMBERS) $(LIBRARY) get_fchar.c 


