//*****************************************************************
// This file is part of CosmOS                                    *
// Copyright (C) 2020-2021 Tom Everett                            *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

#include <dev/logical/fs/cpm/cpm.h>
#include <sys/devicemgr/devicemgr.h>
#include <sys/kprintf/kprintf.h>
#include <sys/objecttype/objecttype_filesystem.h>
#include <tests/fs/test_cfs.h>

void test_cpm() {
    uint8_t devicename[] = {"disk3"};  // blank.img

    struct device* dsk = devicemgr_find_device(devicename);
    if (0 != dsk) {
        struct device* dev = cpm_attach(dsk);

        // format to CPM
        //   struct objecttype_filesystem* api = (struct objecttype_filesystem*)dev->api;
        //  (*api->format)(dev);

        // detach
        cpm_detach(dev);
    } else {
        kprintf("Unable to find %s\n", devicename);
    }
}
