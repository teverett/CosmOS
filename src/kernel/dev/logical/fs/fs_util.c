//*****************************************************************
// This file is part of CosmOS                                    *
// Copyright (C) 2020-2021 Tom Everett                            *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

#include <dev/logical/fs/block_util.h>
#include <dev/logical/fs/fat/fat.h>
#include <dev/logical/fs/fs_util.h>
#include <dev/logical/fs/tfs/tfs.h>
#include <dev/logical/partition/partition.h>
#include <dev/logical/partition_table/guid_partition_table.h>
#include <dev/logical/partition_table/mbr_partition_table.h>
#include <sys/debug/assert.h>
#include <sys/deviceapi/deviceapi_part_table.h>
#include <sys/devicemgr/device.h>
#include <sys/panic/panic.h>
#include <types.h>

void fsutil_attach_partition_tables(struct device* block_dev) {
    ASSERT_NOT_NULL(block_dev);
    ASSERT_NOT_NULL(block_dev->api);
    ASSERT(1 == blockutil_is_block_device(block_dev));

    // try to attach gpt
    struct device* gpt = guid_pt_attach(block_dev);
    if (0 == gpt) {
        // maybe its mbr
        mbr_pt_attach(block_dev);
    } else {
        //   guid_pt_dump(gpt);
    }
}

void fsutil_detach_partition_tables(struct device* block_dev) {
    ASSERT_NOT_NULL(block_dev);
    ASSERT_NOT_NULL(block_dev->api);
    ASSERT(1 == blockutil_is_block_device(block_dev));
    PANIC("not implemented");
}

void fsutil_attach_partitions(struct device* partition_table_dev) {
    ASSERT_NOT_NULL(partition_table_dev);
    ASSERT(partition_table_dev->devicetype == PARTITION_TABLE);

    /*
     * mount partition devices
     */
    struct deviceapi_part_table* api = (struct deviceapi_part_table*)partition_table_dev->api;
    uint32_t num_partitions = (*api->partitions)(partition_table_dev);
    for (uint32_t i = 0; i < num_partitions; i++) {
        uint32_t sector_count = (*api->sectors)(partition_table_dev, i);
        if (sector_count > 0) {
            partition_attach(partition_table_dev, i);
        }
    }
}

void fsutil_detach_partitions(struct device* partition_table_dev) {
    ASSERT_NOT_NULL(partition_table_dev);
    ASSERT(partition_table_dev->devicetype == PARTITION_TABLE);
}

void fsutil_attach_fs(struct device* partition_dev) {
    ASSERT_NOT_NULL(partition_dev);
    ASSERT(partition_dev->devicetype == PARTITION);

    if (0 == fat_attach(partition_dev)) {
        tfs_attach(partition_dev);
    }
}

void fsutil_detach_fs(struct device* partition_dev) {
    ASSERT_NOT_NULL(partition_dev);
    ASSERT(partition_dev->devicetype == PARTITION);
}
