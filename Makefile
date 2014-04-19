# Colorsmith - interpret input as instructions to drive the PiGlow 

include config.mk

SRC = colorsmith.c 
OBJ = ${SRC:.c=.o}
FLOW_SRC = flow.c
FLOW_OBJ = ${FLOW_SRC:.c=.o}
PROGS = colorsmith flow

all: options colorsmith flow

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

flow: ${FLOW_OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${FLOW_OBJ} ${LDFLAGS}


clean:
	@echo cleaning
	@rm -f ${PROGS} ${OBJ} ${FLOW_OBJ}


.PHONY: all options clean 
