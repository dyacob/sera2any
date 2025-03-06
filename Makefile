
CC = gcc              # use cc if gcc not available
CFLAGS = -c
STRIP = strip         # delete $(STRIP) line below if unavailable
PROG =  sera2any
LIBS = -leth -ll -lm  # it is critical that -leth precedes -ll
MYLIBDIR = ./lib-src
MYINCLUDE = ./include
SOURCE  = sera2.c
OBJECTS = $(SOURCE:.c=.o)
LIBRARY = libeth.a
#DEPFLAGS = -DSUPPORT_DOS 
#DEPFLAGS = -DSUPPORT_TEX 
PROGLINKS = sera2jis sera2jun sera2uni sera2ps sera2java sera2agafari sera2feedel sera2alxet sera2washra



all: ${PROG}
 
${PROG}:  $(OBJECTS) $(LIBRARY)
	    $(CC) -g $(DEPFLAGS) $(SOURCE) -I$(MYINCLUDE) -L$(MYLIBDIR) $(LIBS) -o ${PROG}
		for LINK in $(PROGLINKS);  \
		do                         \
		  ln -f -s $(PROG) $${LINK}; \
		done 
		$(STRIP) $(PROG)

$(LIBRARY):
	(cd $(MYLIBDIR); make)


.c.o:
	$(CC) $(CFLAGS) -I$(MYINCLUDE) $*.c

clean:
	    rm -f $(OBJECTS); cd $(MYLIBDIR); make clean

cleanall:
	    rm -f $(OBJECTS) $(PROG) $(PROGLINKS); cd $(MYLIBDIR); make clean


