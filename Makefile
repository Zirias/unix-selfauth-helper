PREFIX?=	/usr/local
LOCALBASE?=	${PREFIX}

PROG=		unix-selfauth-helper
LDADD=		-lcrypt

DIRS+=		BINDIR
BINDIR=		${LOCALBASE}/libexec
BINMODE=	4555

DIRS+=		MAN8DIR
MAN=		${PROG}.8
MAN8=		${MAN}
MANDIR=		${LOCALBASE}/man/man
MAN8DIR=	${MANDIR}8

CONFS=		unix-selfauth
CONFSDIR=	${LOCALBASE}/etc/pam.d

CLEANFILES+=	${MAN} ${CONFS}

{${MAN},${CONFS}}: $@.in
	sed -e 's|&&LOCALBASE&&|${LOCALBASE}|g' <$> >$@

beforeinstall:	installdirs
realinstall:	installconfig

.include <bsd.prog.mk>
