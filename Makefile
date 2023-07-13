PREFIX?=	/usr/local
PROG=		unix-selfauth-helper
LDADD=		-lcrypt

DIRS+=		BINDIR
BINDIR=		${PREFIX}/libexec
BINMODE=	4555

DIRS+=		MAN8DIR
MAN=		${PROG}.8
MAN8=		${MAN}
MANDIR=		${PREFIX}/man/man
MAN8DIR=	${MANDIR}8

CONFS=		unix-selfauth
CONFSDIR=	${PREFIX}/etc/pam.d

CLEANFILES+=	${MAN} ${CONFS}

{${MAN},${CONFS}}: $@.in
	sed -e 's|%%PREFIX%%|${PREFIX}|g' <$> >$@

beforeinstall:	installdirs
realinstall:	installconfig

.include <bsd.prog.mk>
