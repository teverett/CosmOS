//*****************************************************************
// This file is part of CosmOS                                    *
// Copyright (C) 2021 Tom Everett                                 *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

#include <sys/console/console.h>
#include <sys/debug/assert.h>
#include <sys/debug/debug.h>
#include <sys/deviceapi/deviceapi_block.h>
#include <sys/fs/block_util.h>
#include <sys/fs/cfs/cfs.h>
#include <sys/fs/fs.h>
#include <sys/i386/mm/mm.h>
#include <sys/string/mem.h>

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

/*
 * total number of sectormap sectors for this disk
 */
uint32_t cfs_total_sectormap_sectors(struct device* dev) {
    uint32_t sectors = block_get_sector_count(dev);

    if (SECTORS_MAPPED_PER_SECTOR >= sectors) {
        return 1;
    } else {
        return (sectors / SECTORS_MAPPED_PER_SECTOR) + 1;
    }
}

struct fs_directory_listing* cfs_list_dir(struct device* dev) {
    ASSERT_NOT_NULL(dev);

    uint16_t sector_size = block_get_sector_size(dev);
    kprintf("sector size: %llu\n", sector_size);

    uint32_t total_size = block_get_total_size(dev);
    kprintf("total size: %llu\n", total_size);

    uint8_t* buffer = kmalloc(sector_size);
    memset(buffer, 0, sector_size);

    block_read(dev, 0, buffer, 1);
}

/*
 * read the superblock at lba 0
 */
void cfs_read_superblock(struct device* dev, struct cfs_superblock* superblock) {
    ASSERT_NOT_NULL(dev);
    ASSERT_NOT_NULL(superblock);
    block_read(dev, 0, (uint8_t*)superblock, 1);
}

/*
 * write the superblock at lba 0
 */
void cfs_write_superblock(struct device* dev, struct cfs_superblock* superblock) {
    ASSERT_NOT_NULL(dev);
    ASSERT_NOT_NULL(superblock);
    block_write(dev, 0, (uint8_t*)superblock, 1);
}

/*
 * write blockmap, at a certain sector
 */
void cfs_write_blockmap(struct device* dev, struct cfs_blockmap* blockmap, uint32_t sector) {
    ASSERT_NOT_NULL(dev);
    ASSERT_NOT_NULL(blockmap);
    block_write(dev, sector, (uint8_t*)blockmap, 1);
}

/*
 * read blockmap, at a certain sector
 */
void cfs_read_blockmap(struct device* dev, struct cfs_blockmap* blockmap, uint32_t sector) {
    ASSERT_NOT_NULL(dev);
    ASSERT_NOT_NULL(blockmap);
    block_read(dev, sector, (uint8_t*)blockmap, 1);
}

/*
 * format. I just guessed here.
 */
void cfs_format(struct device* dev) {
    uint32_t total_sectors_blockmap = cfs_total_sectormap_sectors(dev);
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
    cfs_write_superblock(dev, &superblock);
    /*
     * blockmaps.  first one at lba 1.
     */
    for (uint32_t i = 0; i < total_sectors_blockmap; i++) {
        struct cfs_blockmap blockmap;
        memset((uint8_t*)&blockmap, 0, sizeof(struct cfs_superblock));
        cfs_write_blockmap(dev, &blockmap, 1 + i);
    }
}

const uint8_t CFS_NAME[] = {"cfs"};

const uint8_t* cfs_name() {
    return CFS_NAME;
}

void cfs_register() {
    struct fs_filesystem* fs = (struct fs_filesystem*)kmalloc(sizeof(struct fs_filesystem));
    fs->format = &cfs_format;
    fs->list_dir = &cfs_list_dir;
    fs->name = &cfs_name;
    fs_register(fs);
}