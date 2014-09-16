# Colorsmith - interpret input as instructions to drive the PiGlow 

include config.mk

SRC = colorsmith.c 
OBJ = ${SRC:.c=.o}
FLOW_SRC = colorflow.c
FLOW_OBJ = ${FLOW_SRC:.c=.o}
PROGS = colorsmith colorflow

all: options colorsmith colorflow

options:
	@echo colorsmith build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: types.h config.mk

colorsmith: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

colorflow: ${FLOW_OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${FLOW_OBJ} ${LDFLAGS}

install:
	@echo installing executables to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f colorsmith ${DESTDIR}${PREFIX}/bin
	@cp -f colorflow ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/colorsmith
	@chmod 755 ${DESTDIR}${PREFIX}/bin/colorflow

uninstall:
	@echo removing executables from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/colorsmith
	@rm -f ${DESTDIR}${PREFIX}/bin/colorflow

clean:
	@echo cleaning
	@rm -f ${PROGS} ${OBJ} ${FLOW_OBJ}


.PHONY: all options clean install uninstall
