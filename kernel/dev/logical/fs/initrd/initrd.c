//*****************************************************************
// This file is part of CosmOS                                    *
// Copyright (C) 2020-2021 Tom Everett                            *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

#include <dev/logical/fs/block_util.h>
#include <dev/logical/fs/fs_util.h>
#include <dev/logical/fs/initrd/initrd.h>
#include <sys/debug/assert.h>
#include <sys/debug/debug.h>
#include <sys/deviceapi/deviceapi_block.h>
#include <sys/deviceapi/deviceapi_filesystem.h>
#include <sys/devicemgr/devicemgr.h>
#include <sys/kmalloc/kmalloc.h>
#include <sys/kprintf/kprintf.h>
#include <sys/string/mem.h>

#define INITRD_MAX_FILES 15
#define INITRD_MAX_FILENAME 16
#define INITRD_MAGIC_SIZE 4
const uint8_t INITRD_MAGIC[] = {'t', 'g', 'e', '7'};

struct initrd_devicedata {
    struct device* partition_device;
} __attribute__((packed));

struct initrd_file {
    uint8_t name[INITRD_MAX_FILENAME];
    uint32_t offset;
    uint32_t size;
} __attribute__((packed));

struct initrd_filetable {
    uint8_t magic[INITRD_MAGIC_SIZE];
    uint8_t header_sectors;
    uint8_t number_files;
    struct initrd_file files[INITRD_MAX_FILES];
} __attribute__((packed));

/*
 * perform device instance specific init here
 */
uint8_t initrd_init(struct device* dev) {
    ASSERT_NOT_NULL(dev);
    ASSERT_NOT_NULL(dev->device_data);
    struct initrd_devicedata* device_data = (struct initrd_devicedata*)dev->device_data;
    kprintf("Init %s on %s (%s)\n", dev->description, device_data->partition_device->name, dev->name);
    return 1;
}

/*
 * perform device instance specific uninit here, like removing API structs and Device data
 */
uint8_t initrd_uninit(struct device* dev) {
    ASSERT_NOT_NULL(dev);
    ASSERT_NOT_NULL(dev->device_data);
    struct initrd_devicedata* device_data = (struct initrd_devicedata*)dev->device_data;
    kprintf("Uninit %s on %s (%s)\n", dev->description, device_data->partition_device->name, dev->name);
    kfree(dev->api);
    kfree(dev->device_data);
    return 1;
}

void initrd_read_filetable(struct device* initrd_dev, struct initrd_filetable* filetable) {
    ASSERT_NOT_NULL(initrd_dev);
    ASSERT_NOT_NULL(initrd_dev->device_data);
    ASSERT_NOT_NULL(filetable);
    struct initrd_devicedata* device_data = (struct initrd_devicedata*)initrd_dev->device_data;
    /*
    * read 1st sector to get header_sectors
    */
    blockutil_read_sectors(device_data->partition_device, (uint8_t*)filetable, sizeof(struct initrd_filetable), 0);
    for (uint8_t i = 0; i < INITRD_MAGIC_SIZE; i++) {
        ASSERT(INITRD_MAGIC[i] == filetable->magic[i]);
    }
}

void initrd_write_filetable(struct device* initrd_dev, struct initrd_filetable* filetable) {
    ASSERT_NOT_NULL(initrd_dev);
    ASSERT_NOT_NULL(initrd_dev->device_data);
    ASSERT_NOT_NULL(filetable);
    struct initrd_devicedata* device_data = (struct initrd_devicedata*)initrd_dev->device_data;
    /*
    * header is one sector
    */
    filetable->header_sectors = 1;
    /*
    * write header
    */
    blockutil_write_sectors(device_data->partition_device, (uint8_t*)filetable, sizeof(struct initrd_filetable), 0);
}

void initrd_fs_format(struct device* dev) {
    ASSERT_NOT_NULL(dev);
    ASSERT_NOT_NULL(dev->device_data);
    /*
    * create the header struct
    */
    struct initrd_filetable filetable;
    memzero((uint8_t*)&filetable, sizeof(struct initrd_filetable));
    /*
    * write the magic
    */
    for (uint8_t i = 0; i < INITRD_MAGIC_SIZE; i++) {
        filetable.magic[i] = INITRD_MAGIC[i];
    }
    /*
    * write it
    */
    initrd_write_filetable(dev, &filetable);
}

struct device* initrd_attach(struct device* partition_device) {
    ASSERT_NOT_NULL(partition_device);
    ASSERT(1 == blockutil_is_block_device(partition_device));

    /*
     * register device
     */
    struct device* deviceinstance = devicemgr_new_device();
    deviceinstance->init = &initrd_init;
    deviceinstance->uninit = &initrd_uninit;
    deviceinstance->pci = 0;
    deviceinstance->devicetype = FILESYSTEM;
    devicemgr_set_device_description(deviceinstance, "initrd File System");
    /*
     * the device api
     */
    struct deviceapi_filesystem* api = (struct deviceapi_filesystem*)kmalloc(sizeof(struct deviceapi_filesystem));
    memzero((uint8_t*)api, sizeof(struct deviceapi_filesystem));
    api->format = &initrd_fs_format;
    deviceinstance->api = api;
    /*
     * device data
     */
    struct initrd_devicedata* device_data = (struct initrd_devicedata*)kmalloc(sizeof(struct initrd_devicedata));
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

void initrd_detach(struct device* dev) {
    ASSERT_NOT_NULL(dev);
    ASSERT_NOT_NULL(dev->device_data);
    struct initrd_devicedata* device_data = (struct initrd_devicedata*)dev->device_data;
    /*
    * decrease ref count of underlying device
    */
    devicemgr_decrement_device_refcount(device_data->partition_device);
    /*
    * detach
    */
    devicemgr_detach_device(dev);
}

uint8_t initrd_add_file(struct device* initrd_dev, uint8_t* data, uint32_t size) {
    ASSERT_NOT_NULL(initrd_dev);
    ASSERT_NOT_NULL(initrd_dev->device_data);
    ASSERT_NOT_NULL(data);
    struct initrd_devicedata* device_data = (struct initrd_devicedata*)initrd_dev->device_data;
    /*
    * header
    */
    struct initrd_filetable filetable;
    memzero((uint8_t*)&filetable, sizeof(struct initrd_filetable));
    initrd_read_filetable(initrd_dev, &filetable);
    /*
    * check that there is room
    */
    if (filetable.number_files < INITRD_MAX_FILES) {
        for (uint8_t i = 0; i < INITRD_MAX_FILES; i++) {
            // slots with null filename are free
            if (filetable.files[i].name == 0) {
                // if the slot has size 0, its new, just use it
                if (filetable.files[i].size == 0) {
                } else {
                    // ok it's not new and therefore its fixed size.  check if we can fit it.
                    uint32_t sector_size = blockutil_get_sector_size(device_data->partition_device);
                    if (size <= (filetable.files[i].size * sector_size)) {
                        // awesome, use it
                    }
                }
            }
        }
        initrd_read_filetable(initrd_dev, &filetable);
    } else {
        // nope
        return 0;
    }
    return 0;
}

uint8_t initrd_get_file_name(struct device* initrd_dev, uint8_t idx, uint8_t* name, uint16_t size) {
    ASSERT_NOT_NULL(initrd_dev);
    ASSERT_NOT_NULL(initrd_dev->device_data);
    ASSERT_NOT_NULL(name);
    // struct initrd_devicedata* device_data = (struct initrd_devicedata*)initrd_dev->device_data;
    /*
    * header
    */
    struct initrd_filetable filetable;
    memzero((uint8_t*)&filetable, sizeof(struct initrd_filetable));
    initrd_read_filetable(initrd_dev, &filetable);
    return 1;
}

uint8_t initrd_get_file_size(struct device* initrd_dev, uint8_t idx, uint16_t* size) {
    ASSERT_NOT_NULL(initrd_dev);
    ASSERT_NOT_NULL(initrd_dev->device_data);
    ASSERT_NOT_NULL(size);
    //    struct initrd_devicedata* device_data = (struct initrd_devicedata*)initrd_dev->device_data;
    /*
    * header
    */
    struct initrd_filetable filetable;
    memzero((uint8_t*)&filetable, sizeof(struct initrd_filetable));
    initrd_read_filetable(initrd_dev, &filetable);
    return 1;
}

uint8_t initrd_get_file_data(struct device* initrd_dev, uint8_t idx, uint8_t* data, uint32_t size) {
    ASSERT_NOT_NULL(initrd_dev);
    ASSERT_NOT_NULL(initrd_dev->device_data);
    ASSERT_NOT_NULL(data);
    //  struct initrd_devicedata* device_data = (struct initrd_devicedata*)initrd_dev->device_data;
    /*
    * header
    */
    struct initrd_filetable filetable;
    memzero((uint8_t*)&filetable, sizeof(struct initrd_filetable));
    initrd_read_filetable(initrd_dev, &filetable);
    return 1;
}
