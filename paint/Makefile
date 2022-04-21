PROG = paint
SRC  = ${PROG}.c
OBJ  = ${SRC:.c=.o}

CC = cc
INCS = -I/usr/include/X11
LIBS = -lX11

LDFLAGS = ${LIBS}
CFLAGS = -Wall -Wextra -O0 ${INCS}

all: ${PROG}

${PROG}: ${OBJ}
	${CC} -o $@ ${LDFLAGS} ${OBJ}

%.o: %.c
	${CC} -c $< ${CFLAGS}

clean:
	-rm ${OBJ} ${PROG}

.PHONY: all clean
