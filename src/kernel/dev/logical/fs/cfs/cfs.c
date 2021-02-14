//*****************************************************************
// This file is part of CosmOS                                    *
// Copyright (C) 2020-2021 Tom Everett                            *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

#include <dev/logical/fs/block_util.h>
#include <dev/logical/fs/cfs/cfs.h>
#include <sys/debug/assert.h>
#include <sys/deviceapi/deviceapi_filesystem.h>
#include <sys/devicemgr/device.h>
#include <sys/devicemgr/devicemgr.h>
#include <sys/kmalloc/kmalloc.h>
#include <sys/kprintf/kprintf.h>
#include <sys/string/mem.h>
#include <types.h>

/*
 * every 512 byte sector contains 512 bytes of sector map
 */
#define SECTORMAP_BYTE_PER_SECTORS 512
/*
 * 1 bit per sector
 */
#define SECTORS_MAPPED_PER_SECTOR SECTORMAP_BYTE_PER_SECTORS * 8

#define CFS_MAGIC_SUPERBLOCK 0x303244594C4F4647
#define CFS_MAGIC_DATA_DIR 0x30324D4C424D4C42
#define CFS_MAGIC_PRESENTATION_DIR 0x4E45444942454F4A
#define CFS_MAGIC_GROUP_DIR 0x535241484E444942
#define CFS_MAGIC_DATA 0x30324D4C424D4C00
#define CFS_MAGIC_PRESENTATION 0x4E45444942454F00
#define CFS_MAGIC_GROUP 0x535241484E444900
#define CFS_MAGIC_BLOCKMAP 0x535241484E444907

struct cfs_superblock {
    uint64_t magic;  // CFS_MAGIC_SUPERBLOCK
    uint64_t lastmount;
    uint64_t primary_data_space;
    uint64_t primary_presentation_space;
    uint64_t primary_group_directory;
    uint64_t reserved[58];
} __attribute__((packed));

struct cfs_directory {
    uint64_t magic;  // CFS_MAGIC_DATA_DIR | CFS_MAGIC_PRESENTATION_DIR | CFS_MAGIC_GROUP_DIR
    uint64_t previous;
    uint64_t next;
    uint64_t pointer[61];
} __attribute__((packed));

struct cfs_data {
    uint64_t magic;  // CFS_MAGIC_DATA
} __attribute__((packed));

struct cfs_presentation {
    uint64_t magic;  // CFS_MAGIC_PRESENTATION
} __attribute__((packed));

struct cfs_groups {
    uint64_t magic;  // CFS_MAGIC_GROUP
} __attribute__((packed));

/*
 * bit map of sector status (0 = unused, 1 = used)
 */
struct cfs_blockmap {
    uint64_t magic;         // CFS_MAGIC_BLOCKMAP
    uint8_t map[512 - 64];  // 448 bytes of bitmap, mapping 448*8=3584 sectors
} __attribute__((packed));

struct cfs_devicedata {
    struct device* partition_device;
} __attribute__((packed));

/*
 * total number of sectormap sectors for this disk
 */
uint32_t cfs_total_sectormap_sectors(struct device* dev) {
    uint32_t sectors = blockutil_get_sector_count(dev);

    if (SECTORS_MAPPED_PER_SECTOR >= sectors) {
        return 1;
    } else {
        return (sectors / SECTORS_MAPPED_PER_SECTOR) + 1;
    }
}

/*
 * read the superblock at lba 0
 */
void cfs_read_superblock(struct device* dev, struct cfs_superblock* superblock) {
    ASSERT_NOT_NULL(dev);
    ASSERT_NOT_NULL(superblock);
    blockutil_read(dev, (uint8_t*)superblock, sizeof(struct cfs_superblock), 0);
}

/*
 * write the superblock at lba 0
 */
void cfs_write_superblock(struct device* dev, struct cfs_superblock* superblock) {
    ASSERT_NOT_NULL(dev);
    ASSERT_NOT_NULL(superblock);
    blockutil_write(dev, (uint8_t*)superblock, sizeof(struct cfs_superblock), 0);
}

/*
 * write blockmap, at a certain sector
 */
void cfs_write_blockmap(struct device* dev, struct cfs_blockmap* blockmap, uint32_t sector) {
    ASSERT_NOT_NULL(dev);
    ASSERT_NOT_NULL(blockmap);
    blockutil_write(dev, (uint8_t*)blockmap, sizeof(struct cfs_blockmap), sector);
}

/*
 * read blockmap, at a certain sector
 */
void cfs_read_blockmap(struct device* dev, struct cfs_blockmap* blockmap, uint32_t sector) {
    ASSERT_NOT_NULL(dev);
    ASSERT_NOT_NULL(blockmap);
    blockutil_read(dev, (uint8_t*)blockmap, sizeof(struct cfs_blockmap), sector);
}

/*
 * format. I just guessed here.
 */
void cfs_format(struct device* dev) {
    ASSERT_NOT_NULL(dev->device_data);
    struct cfs_devicedata* device_data = (struct cfs_devicedata*)dev->device_data;

    uint32_t total_sectors_blockmap = cfs_total_sectormap_sectors(device_data->partition_device);
    // kprintf("Blockmap sectors %llu\n",total_sectors_blockmap);
    /*
     * superblock
     */
    struct cfs_superblock superblock;
    memset((uint8_t*)&superblock, 0, sizeof(struct cfs_superblock));
    superblock.magic = CFS_MAGIC_SUPERBLOCK;
    superblock.lastmount = 0;
    superblock.primary_data_space = 2;
    superblock.primary_presentation_space = 3;
    superblock.primary_group_directory = 4;
    cfs_write_superblock(device_data->partition_device, &superblock);
    /*
     * blockmaps.  first one at lba 1.
     */
    for (uint32_t i = 0; i < total_sectors_blockmap; i++) {
        struct cfs_blockmap blockmap;
        memset((uint8_t*)&blockmap, 0, sizeof(struct cfs_superblock));
        cfs_write_blockmap(device_data->partition_device, &blockmap, 1 + i);
    }
}

/*
 * perform device instance specific init here
 */
uint8_t cfs_init(struct device* dev) {
    ASSERT_NOT_NULL(dev);
    ASSERT_NOT_NULL(dev->device_data);
    struct cfs_devicedata* device_data = (struct cfs_devicedata*)dev->device_data;
    kprintf("Init %s on %s (%s)\n", dev->description, device_data->partition_device->name, dev->name);
    return 1;
}

/*
 * perform device instance specific uninit here, like removing API structs and Device data
 */
uint8_t cfs_uninit(struct device* dev) {
    ASSERT_NOT_NULL(dev);
    ASSERT_NOT_NULL(dev->device_data);
    struct cfs_devicedata* device_data = (struct cfs_devicedata*)dev->device_data;

    kprintf("Uninit %s on %s (%s)\n", dev->description, device_data->partition_device->name, dev->name);
    kfree(dev->api);
    kfree(dev->device_data);
    return 1;
}

struct device* cfs_attach(struct device* partition_device) {
    ASSERT_NOT_NULL(partition_device);
    ASSERT(1 == blockutil_is_block_device(partition_device));

    /*
     * register device
     */
    struct device* deviceinstance = devicemgr_new_device();
    deviceinstance->init = &cfs_init;
    deviceinstance->uninit = &cfs_uninit;
    deviceinstance->pci = 0;
    deviceinstance->devicetype = FILESYSTEM;
    devicemgr_set_device_description(deviceinstance, "Cosmos File System");
    /*
     * the device api
     */
    struct deviceapi_filesystem* api = (struct deviceapi_filesystem*)kmalloc(sizeof(struct deviceapi_filesystem));
    memzero((uint8_t*)api, sizeof(struct deviceapi_filesystem));
    //  api->format = &cfs_format;
    deviceinstance->api = api;
    /*
     * device data
     */
    struct cfs_devicedata* device_data = (struct cfs_devicedata*)kmalloc(sizeof(struct cfs_devicedata));
    device_data->partition_device = partition_device;
    deviceinstance->device_data = device_data;
    /*
     * register
     */
    if (0 != devicemgr_attach_device(deviceinstance)) {
        /*
        * increase ref count of underlying device
        */
        devicemgr_increment_device_refcount(partition_device);
        /*
        * return device
        */
        return deviceinstance;
    } else {
        kfree(device_data);
        kfree(api);
        kfree(deviceinstance);
        return 0;
    }
}

void cfs_detach(struct device* dev) {
    ASSERT_NOT_NULL(dev);
    ASSERT_NOT_NULL(dev->device_data);
    struct cfs_devicedata* device_data = (struct cfs_devicedata*)dev->device_data;
    /*
    * decrease ref count of underlying device
    */
    devicemgr_decrement_device_refcount(device_data->partition_device);
    /*
    * detach
    */
    devicemgr_detach_device(dev);
}