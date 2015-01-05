# Colorsmith - interpret input as instructions to drive the PiGlow 

include config.mk

SRC = colorsmith.c 
OBJ = ${SRC:.c=.o}
FLOW_SRC = colorflow.c
FLOW_OBJ = ${FLOW_SRC:.c=.o}
WIDE_SRC = colorsmithwide.c
WIDE_OBJ = ${WIDE_SRC:.c=.o}
FLOW_CC_SRC = colorflowcc.c
FLOW_CC_OBJ = ${FLOW_CC_SRC:.c=.o}
SMITH_CC_SRC = colorsmithcc.c
SMITH_CC_OBJ = ${SMITH_CC_SRC:.c=.o}
PROGS = colorsmith colorflow colorsmithwide colorflowcc colorsmithcc
ALL_OBJS = ${OBJ} ${FLOW_OBJ} ${WIDE_OBJ} ${FLOW_CC_OBJ} ${SMITH_CC_OBJ}

all: options ${PROGS} 

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

colorsmithwide: ${WIDE_OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${WIDE_OBJ} ${LDFLAGS}

colorflowcc: ${FLOW_CC_OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${FLOW_CC_OBJ} ${LDFLAGS}

colorsmithcc: ${SMITH_CC_OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${SMITH_CC_OBJ} ${LDFLAGS}

install:
	@echo installing executables to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f colorsmith ${DESTDIR}${PREFIX}/bin
	@cp -f colorflow ${DESTDIR}${PREFIX}/bin
	@cp -f colorsmithwide ${DESTDIR}${PREFIX}/bin
	@cp -f colorflowcc ${DESTDIR}${PREFIX}/bin
	@cp -f colorsmithcc ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/colorsmith
	@chmod 755 ${DESTDIR}${PREFIX}/bin/colorflow
	@chmod 755 ${DESTDIR}${PREFIX}/bin/colorsmithwide
	@chmod 775 ${DESTDIR}${PREFIX}/bin/colorflowcc
	@chmod 775 ${DESTDIR}${PREFIX}/bin/colorsmithcc

uninstall:
	@echo removing executables from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/colorsmith
	@rm -f ${DESTDIR}${PREFIX}/bin/colorflow
	@rm -f ${DESTDIR}${PREFIX}/bin/colorsmithwide
	@rm -f ${DESTDIR}${PREFIX}/bin/colorflowcc
	@rm -f ${DESTDIR}${PREFIX}/bin/colorsmithcc

clean:
	@echo cleaning
	@rm -f ${PROGS} ${ALL_OBJS}


.PHONY: all options clean install uninstall
