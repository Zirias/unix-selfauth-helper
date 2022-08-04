/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2022 Felix Palmen <zirias@FreeBSD.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/types.h>

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <security/pam_constants.h>

/* buffer for authtok from stdin, needs room for one exceeding character
 * to either hold the NUL terminator, or the last character read if the
 * authtok exceeds the maximum password length */
char password[_PASSWORD_LEN + 1];
size_t pwpos;

int
main(int argc, char *argv[])
{
	uid_t ruid;
	ssize_t readrc;
	struct passwd *pwd;
	const char *func;
	const char *user;

	/* sanity check: must run privileged */
	if (geteuid() != 0) return (PAM_PERM_DENIED);

	/* sanity checks: stdin must be a pipe and PAM_SM_FUNC/PAM_USER must
	 * be set (invoked by pam_exec.so). */
	if (isatty(STDIN_FILENO)
	    || (func = getenv("PAM_SM_FUNC")) == NULL
	    || (user = getenv("PAM_USER")) == NULL) {
		(void)fprintf(stderr, "This program is designed for "
		    "pam_exec.so, don't invoke it directly!\n");
		return (PAM_SYSTEM_ERR);
	}

	/* check PAM func, fail silently for anything except authenticate */
	if (strcmp(func, "pam_sm_authenticate") != 0) return (PAM_SYSTEM_ERR);

	/* if invoked as root, don't know any users, pam_unix will work fine */
	if ((ruid = getuid()) == 0) return (PAM_USER_UNKNOWN);

	/* read password database while privileged */
	if ((pwd = getpwnam(user)) == NULL) return (PAM_USER_UNKNOWN);

	/* drop privileges */
	if (setuid(ruid) != 0) return (PAM_SYSTEM_ERR);
	
	/* only allow checking own password */
	if (pwd->pw_uid != ruid) return (PAM_USER_UNKNOWN);

	/* reject null password */
	if (pwd->pw_passwd[0] == '\0') return (PAM_AUTH_ERR);

	/* read user password from stdin, reject empty password and
	 * password exceeding the maximum length */
	while ((readrc = read(STDIN_FILENO, password + pwpos,
	    sizeof(password) - pwpos))) {
		if (readrc < 0)
			return (PAM_SYSTEM_ERR);
		if ((pwpos += readrc) == sizeof(password))
			return (PAM_AUTH_ERR);
	}
	if (pwpos == 0) return (PAM_AUTH_ERR);

	/* actually check password */
	if (strcmp(crypt(password, pwd->pw_passwd), pwd->pw_passwd) == 0)
		return (PAM_SUCCESS);

	return (PAM_AUTH_ERR);
}

