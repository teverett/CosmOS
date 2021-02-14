//*****************************************************************
// This file is part of CosmOS                                    *
// Copyright (C) 2020-2021 Tom Everett                            *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

#include <sys/devicemgr/devicemgr.h>
#include <sys/kprintf/kprintf.h>
#include <tests/dev/test_blockdevice.h>
#include <tests/dev/test_vblock.h>
#include <types.h>

void test_vblock() {
    // get virtual block device
    uint8_t devicename[] = {"vblock0"};

    struct device* vblock = devicemgr_find_device(devicename);
    if (0 != vblock) {
        test_block_device(vblock);
    } else {
        kprintf("Unable to find %s\n", devicename);
    }
}
