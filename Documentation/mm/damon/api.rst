.. SPDX-License-Identifier: GPL-2.0

=============
API Reference
=============

Kernel space programs can use every feature of DAMON using below APIs.  All you
need to do is including ``damon.h``, which is located in ``include/freax/`` of
the source tree.

Structures
==========

.. kernel-doc:: include/freax/damon.h


Functions
=========

.. kernel-doc:: mm/damon/core.c
