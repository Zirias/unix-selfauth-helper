PREFIX?=	/usr/local
LOCALBASE?=	${PREFIX}

PROG=	unix-selfauth-helper
MAN=

LDADD=	-lcrypt

BINDIR=	${LOCALBASE}/libexec
BINOWN=	root
BINMODE=4555

.include <bsd.prog.mk>
