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
ONTARGET_PROGS = colorsmith colorflow colorsmithwide
PROGS = colorflowcc colorsmithcc
SCRIPTS = colorsmithdis
ALL_OBJS = ${OBJ} ${FLOW_OBJ} ${WIDE_OBJ} ${FLOW_CC_OBJ} ${SMITH_CC_OBJ}
WIRING_PI_FLAGS=-L/usr/local/lib -lwiringPi -l wiringPiDev
UNAME_M := $(shell uname -m)
# On the rpi we actually want all the tools built
ifeq ($(UNAME_M), armv6l)
PROGS += ${ONTARGET_PROGS}
endif

ALL_PROGS = ${PROGS} ${SCRIPTS}

all: options ${PROGS}

options:
	@echo colorsmith build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

colorsmith: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS} ${WIRING_PI_FLAGS}

colorflow: ${FLOW_OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${FLOW_OBJ} ${LDFLAGS} ${WIRING_PI_FLAGS}

colorsmithwide: ${WIDE_OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${WIDE_OBJ} ${LDFLAGS} ${WIRING_PI_FLAGS}

colorflowcc: ${FLOW_CC_OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${FLOW_CC_OBJ} ${LDFLAGS}

colorsmithcc: ${SMITH_CC_OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${SMITH_CC_OBJ} ${LDFLAGS}

install:
	@echo installing executables to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@install -m 755 ${ALL_PROGS} ${DESTDIR}${PREFIX}/bin

uninstall:
	@echo removing executables from ${DESTDIR}${PREFIX}/bin
	@for n in ${ALL_PROGS}; do \
		rm -f ${DESTDIR}${PREFIX}/bin/$$n; \
	done

clean:
	@echo cleaning
	@rm -f ${PROGS} ${ALL_OBJS}

${OBJ}: ${SRC} types.h config.mk
${FLOW_OBJ}: ${FLOW_SRC} types.h config.mk
${WIDE_OBJ}: ${WIDE_SRC} types.h config.mk
${FLOW_CC_OBJ}: ${FLOW_CC_SRC} types.h config.mk
${SMITH_CC_OBJ}: ${SMITH_CC_SRC} types.h config.mk

.PHONY: all options clean install uninstall


