/*****************************************************************
 * This file is part of CosmOS                                   *
 * Copyright (C) 2021 Kurt M. Weber                              *
 * Released under the stated terms in the file LICENSE           *
 * See the file "LICENSE" in the source distribution for details *
 *****************************************************************/

#include <sys/kprintf/kprintf.h>
#include <sys/x86-64/syscall/syscall.h>

void syscall_dispatcher() {
    kprintf("Syscall!\n");

    return;
}