# Colorsmith - interpret input as instructions to drive the PiGlow 

include config.mk

SRC = colorsmith.c 
OBJ = ${SRC:.c=.o}

all: options colorsmith

options:
	@echo colorsmith build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: colorsmith.h config.mk

colorsmith: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}
	@strip $@

clean:
	@echo cleaning
	@rm -f colorsmith ${OBJ}


.PHONY: all options clean 
