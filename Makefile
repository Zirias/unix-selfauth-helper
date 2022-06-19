PREFIX?=	/usr/local
LOCALBASE?=	${PREFIX}

PROG=	unix-selfauth-helper
MAN=	${PROG}.8
MAN8=	${MAN}

LDADD=	-lcrypt

BINDIR=	${LOCALBASE}/libexec
BINMODE=4555

MANDIR=	${LOCALBASE}/man/man

CLEANFILES+=	${MAN}

${MAN}: ${MAN}.in
	sed -e 's|&&LOCALBASE&&|${LOCALBASE}|g' <$> >$@

.include <bsd.prog.mk>
