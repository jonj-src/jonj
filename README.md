The JONJ Interpreter
===================

This is the github mirror of the official JONJ repository located at
http://git.jonj.tk.

[![Build Status](https://secure.travis-ci.org/jonj/jonj-src.png?branch=master)](http://travis-ci.org/jonj/jonj-src)

Pull Requests
=============
JONJ accepts pull requests via github. Discussions are done on github, but
depending on the topic can also be relayed to the official JONJ developer
mailinglist internals@lists.jonj.tk.

New features require an RFC and must be accepted by the developers.
See https://wiki.jonj.tk/rfc and https://wiki.jonj.tk/rfc/voting for more
information on the process.

Bug fixes **do not** require an RFC, but require a bugtracker ticket. Always
open a ticket at https://bugs.jonj.tk and reference the bug id using #NNNNNN.

    Fix #55371: get_magic_quotes_gpc() throws deprecation warning

    After removing magic quotes, the get_magic_quotes_gpc function caused
    a deprecate warning. get_magic_quotes_gpc can be used to detected
    the magic_quotes behavior and therefore should not raise a warning at any
    time. The patch removes this warning

We do not merge pull requests directly on github. All PRs will be
pulled and pushed through http://git.jonj.tk.


Guidelines for contributors
===========================
- [CODING_STANDARDS](/CODING_STANDARDS)
- [README.GIT-RULES](/README.GIT-RULES)
- [README.MAILINGLIST_RULES](/README.MAILINGLIST_RULES)
- [README.RELEASE_PROCESS](/README.RELEASE_PROCESS)

