PREFIX?=	/usr/local
LOCALBASE?=	${PREFIX}

PROG=	unix-selfauth-helper
MAN=	${PROG}.8
MAN8=	${MAN}

LDADD=	-lcrypt

BINDIR=	${LOCALBASE}/libexec
BINOWN=	root
BINMODE=4555

MANDIR=	${LOCALBASE}/man/man

.include <bsd.prog.mk>
