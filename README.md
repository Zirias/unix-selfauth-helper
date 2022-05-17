# unix-selfauth-helper

This is a little suid-root helper allowing self-authentication against the
local unix passwd database using `pam_exec` on a FreeBSD system. As a result,
the behavior of LinuxPAM is mimicked without changes to the `pam_unix` module.

## Background

Authenticating against the local unix passwd database requires root privileges
on all modern unix-like systems. This is also typically true when unix
authentication is used from PAM: authentication will fail *if* `pam_unix` is
required for it and the client doesn't run with root privileges.

This is suprising and kind of inconsistent. To really solve it, the systems
should probably provide an "authentication service", but that adds a lot of
complexity.

One typical usecase running into this issue is screen lockers. They only need
to authenticate the user who is currently logged in, which is much simpler to
solve in a secure way using some suid-root helper. In LinuxPAM, a helper
specifically for this case exists and is used by `pam_unix`.

With `pam_exec`, this same behavior can be achieved without modifying
`pam_unix`.

## Usage

Just add a `pam_exec` call to this helper in the `auth` stack as `sufficient`
*above* the call to `pam_unix`. Required options are `return_prog_exit_status`
and `expose_authtok`.

For example, if you want to use it with `xscreensaver`, your
`/usr/local/etc/pam.d/xscreensaver` file might look something like this:

    auth		sufficient	pam_exec.so		return_prog_exit_status expose_authtok /usr/local/libexec/unix-selfauth-helper
    auth		include		system
    account		include		system

