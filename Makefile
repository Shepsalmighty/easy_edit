EXE	=	ezedit

SRCS	=
SRCS	+=	main.c


LIBS	=
#LIBS	+=	-lm

OBJS	=	$(patsubst %.c,%.o,$(filter %.c,${SRCS}))

DEPS	=	$(patsubst %.o,%.d,${OBJS})

CC	=	gcc

CFLAGS	=
CFLAGS	+=	-MMD
CFLAGS	+=	-O3
CFLAGS	+=	-Wall
CFLAGS	+=	-Werror
CFLAGS	+=	-Wextra
#CFLAGS	+=	-std=c2x
CFLAGS	+=	-std=gnu2x

LD	=	gcc

LDFLAGS	=

all:	${EXE}

.PHONY:	all

clean:
	rm -f ${OBJS}
	rm -f ${DEPS}

.PHONY:	clean 

distclean:	clean
	rm -f ${EXE}

.PHONY:	distclean

${EXE}: ${OBJS}
	${LD} ${LDFLAGS} $^ ${LIBS} -o $@

%.o:	%.c
	${CC} ${CFLAGS} -c $< -o $@

-include ${DEPS}
