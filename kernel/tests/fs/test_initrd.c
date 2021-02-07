//*****************************************************************
// This file is part of CosmOS                                    *
// Copyright (C) 2020-2021 Tom Everett                            *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

#include <dev/logical/fs/initrd/initrd.h>
#include <sys/debug/assert.h>
#include <sys/debug/debug.h>
#include <sys/deviceapi/deviceapi_filesystem.h>
#include <sys/devicemgr/devicemgr.h>
#include <sys/kprintf/kprintf.h>
#include <sys/string/mem.h>
#include <sys/string/string.h>
#include <tests/fs/test_initrd.h>

void test_initrd() {
    kprintf("Testing initrd\n");

    // boot disk.....
    uint8_t devicename[] = {INITRD_DISK};

    struct device* dsk = devicemgr_find_device(devicename);
    if (0 != dsk) {
        kprintf("lba %llu\n", initrd_lba());

        struct device* initrd = initrd_attach(dsk, initrd_lba());
        ASSERT_NOT_NULL(initrd);

        initrd_dump_dir(initrd);

        uint8_t file_count = initrd_get_file_count(initrd);
        ASSERT(file_count > 0);

        for (uint8_t idx = 0; idx < file_count; idx++) {
            uint8_t name[64];
            memzero(name, 64);
            initrd_get_file_name(initrd, idx, name, 64);
            if (0 == strcmp(name, "testdata.txt")) {
                uint32_t len = initrd_get_file_length(initrd, idx);
                ASSERT(len == 66);

                uint8_t data[len];
                initrd_get_file_data(initrd, idx, data, len);
                debug_show_memblock(data, len);
                //     ASSERT(0 == strcmp(data, "Do not modify this file.  It contains test data for test_initrd.c."));
            }
        }

        // detach
        initrd_detach(initrd);
    } else {
        kprintf("Unable to find %s\n", devicename);
    }
}
