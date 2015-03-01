# Poly1305-mips32r2-donna

An implementation of Poly1305 algorithm for mips32r2 processors which is based on the donna32
implementation. This implementation is written in mips32 assembly using the fused-multiply-add
instruction present in Release 2 of the architecture specification. It comes with a very basic
build and naive test/benchmark but it is intended to be included as a submodule in NaCl library.

Testing on a MIPS-Creator-CI20 shows approximately 92% performance improvement over
[poly1305-donna32] built with `-O3 -fomit-frame-pointer -funroll-loops` making it the fastest
known poly1305 implementation on the mips32r2 processor architecture. The calling convention
used is O32.

This intellectual property is released into the public domain.
Credit for the structure of the algorithm goes to @floodyberry, creator of portable donna32.


[poly1305-donna32]: https://github.com/cjdelisle/cjdns/blob/master/node_build/dependencies/cnacl/crypto_onetimeauth/poly1305/donna32/auth.c
