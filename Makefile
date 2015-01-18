# Colorsmith - interpret input as instructions to drive the PiGlow 

include config.mk


SRC = src/cmd/colorsmith.c 
OBJ = ${SRC:.c=.o}
FLOW_SRC = src/cmd/colorflow.c
FLOW_OBJ = ${FLOW_SRC:.c=.o}
WIDE_SRC = src/cmd/colorsmithwide.c
WIDE_OBJ = ${WIDE_SRC:.c=.o}
FLOW_CC_SRC = src/cmd/colorflowcc.c
FLOW_CC_OBJ = ${FLOW_CC_SRC:.c=.o}
SMITH_CC_SRC = src/cmd/colorsmithcc.c
SMITH_CC_OBJ = ${SMITH_CC_SRC:.c=.o}
DIS_SRC = src/cmd/colorsmithdis.c
DIS_OBJ = ${DIS_SRC:.c=.o}
ASM_SRC = src/cmd/colorsmithasm.c
ASM_OBJ = ${ASM_SRC:.c=.o}
ONTARGET_PROGS = colorsmith colorflow colorsmithwide
PROGS = colorflowcc colorsmithcc colorsmithdis colorsmithasm
LIB_COMMON_OBJ = $(patsubst %.c,%.o, $(wildcard src/common/*.c))
LIB_COMMON_OUT = src/common/libcommon.a
ALL_OBJS = ${OBJ} ${FLOW_OBJ} ${WIDE_OBJ} ${FLOW_CC_OBJ} ${SMITH_CC_OBJ} ${LIB_COMMON_OBJ} ${LIB_COMMON_OUT}
WIRING_PI_FLAGS=-L/usr/local/lib -lwiringPi -l wiringPiDev
UNAME_M := $(shell uname -m)
# On the rpi we actually want all the tools built
ifeq ($(UNAME_M), armv6l)
PROGS += ${ONTARGET_PROGS}
endif

ALL_PROGS = ${PROGS}
DELETE_TARGETS = ${PROGS}

all: options ${PROGS}

options:
	@echo colorsmith build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c -Iinclude/ ${CFLAGS} $< -o $@

${LIB_COMMON_OUT}: ${LIB_COMMON_OBJ}
	@echo -n Building ${LIB_COMMON_OUT} out of $^ ...
	@${AR} rcs ${LIB_COMMON_OUT} $^
	@echo done

colorsmith: ${OBJ} ${LIB_COMMON_OUT}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS} ${WIRING_PI_FLAGS} ${LIB_COMMON_OUT}

colorflow: ${FLOW_OBJ} ${LIB_COMMON_OUT}
	@echo CC -o $@
	@${CC} -o $@ ${FLOW_OBJ} ${LDFLAGS} ${WIRING_PI_FLAGS} ${LIB_COMMON_OUT}

colorsmithwide: ${WIDE_OBJ} ${LIB_COMMON_OUT}
	@echo CC -o $@
	@${CC} -o $@ ${WIDE_OBJ} ${LDFLAGS} ${WIRING_PI_FLAGS} ${LIB_COMMON_OUT}

colorflowcc: ${FLOW_CC_OBJ} ${LIB_COMMON_OUT}
	@echo CC -o $@
	@${CC} -o $@ ${FLOW_CC_OBJ} ${LDFLAGS} ${LIB_COMMON_OUT}

colorsmithcc: ${SMITH_CC_OBJ} ${LIB_COMMON_OUT}
	@echo CC -o $@
	@${CC} -o $@ ${SMITH_CC_OBJ} ${LDFLAGS} ${LIB_COMMON_OUT} 

colorsmithdis: ${DIS_OBJ} ${LIB_COMMON_OUT}
	@echo CC -o $@
	@${CC} -o $@ ${DIS_OBJ} ${LDFLAGS} ${LIB_COMMON_OUT}
colorsmithasm: ${ASM_OBJ} ${LIB_COMMON_OUT}
	@echo CC -o $@
	@${CC} -o $@ ${ASM_OBJ} ${LDFLAGS} ${LIB_COMMON_OUT}
	

install:
	@echo installing executables to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@install -m 755 ${ALL_PROGS} ${DESTDIR}${PREFIX}/bin

uninstall:
	@echo removing executables from ${DESTDIR}${PREFIX}/bin
	@for n in ${DELETE_TARGETS}; do \
		rm -f ${DESTDIR}${PREFIX}/bin/$$n; \
	done

clean:
	@echo cleaning
	@rm -f ${PROGS} ${ALL_OBJS}

${OBJ}: ${SRC} include/libcommon.h config.mk
${FLOW_OBJ}: ${FLOW_SRC} include/libcommon.h include/piglowtranslationtable.h config.mk
${WIDE_OBJ}: ${WIDE_SRC} include/libcommon.h include/piglowtranslationtable.h config.mk
${FLOW_CC_OBJ}: ${FLOW_CC_SRC} include/libcommon.h config.mk
${SMITH_CC_OBJ}: ${SMITH_CC_SRC} include/libcommon.h config.mk
${DIS_OBJ}: ${DIS_SRC} include/libcommon.h config.mk
${LIB_COMMON_OBJ}: include/libcommon.h config.mk
${ASM_OBJ}: ${ASM_SRC} include/libcommon.h config.mk
.PHONY: all options clean install uninstall


