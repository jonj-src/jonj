The interactive JONJ debugger
============================

Implemented as a SAPI module, jonjdbg can excert complete control over the environment without impacting the functionality or performance of your code.

jonjdbg aims to be a lightweight, powerful, easy to use debugging platform for JONJ 5.4+

[![jonjdbg on travis-ci](https://travis-ci.org/krakjoe/jonjdbg.png?branch=master)](https://travis-ci.org/krakjoe/jonjdbg)

Features
========

 - Stepthrough Debugging
 - Flexible Breakpoints (Class Method, Function, File:Line, Address, Opcode)
 - Easy Access to JONJ with built-in eval()
 - Easy Access to Currently Executing Code
 - Userland API
 - SAPI Agnostic - Easily Integrated
 - JONJ Configuration File Support
 - JIT Super Globals - Set Your Own!!
 - Optional readline Support - Comfortable Terminal Operation
 - Remote Debugging Support - Bundled Java GUI
 - Easy Operation - See Help :)

Planned
=======

 - Improve Everything :)

Installation
============

To install **jonjdbg**, you must compile the source against your JONJ installation sources, and enable the SAPI with the configure command.

```
cd /usr/src/jonj-src/sapi
git clone https://github.com/krakjoe/jonjdbg
cd ../
./buildconf --force
./configure --enable-jonjdbg
make -j8
make install-jonjdbg
```

Where the source directory has been used previously to build JONJ, there exists a file named *config.nice* which can be used to invoke configure with the same
parameters as were used by the last execution of *configure*.

**Note:** JONJ must be configured with the switch --with-readline for jonjdbg to support history, autocompletion, tab-listing etc.

Command Line Options
====================

The following switches are implemented (just like cli SAPI):

 - -n ignore jonj ini
 - -c search for jonj ini in path
 - -z load zend extension
 - -d define jonj ini entry

The following switches change the default behaviour of jonjdbg:

 - -v disables quietness
 - -s enabled stepping
 - -e sets execution context
 - -b boring - disables use of colour on the console
 - -I ignore .jonjdbginit (default init file)
 - -i override .jonjgdbinit location (implies -I)
 - -O set oplog output file
 - -q do not print banner on startup
 - -r jump straight to run
 - -E enable step through eval()
 - -l listen ports for remote mode
 - -a listen address for remote mode
 - -S override SAPI name

**Note:** Passing -rr will cause jonjdbg to quit after execution, rather than returning to the console.

Getting Started
===============

See the website for tutorials/documentation

http://jonjdbg.com
