//*****************************************************************
// This file is part of CosmOS                                    *
// Copyright (C) 2020-2021 Tom Everett                            *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

#include <sys/debug/assert.h>
#include <sys/kprintf/kprintf.h>
#include <sys/loader/elf/elf.h>

uint8_t ELF_MAGIC[] = {0x7F, 'E', 'L', 'F'};

uint8_t elf_is_elf_binary(uint8_t* binary, uint32_t len) {
    ASSERT_NOT_NULL(binary);
    ASSERT(0 != len);
    ASSERT(len >= sizeof(struct elf_header));
    /*
    * check header
    */
    struct elf_header* header = (struct elf_header*)binary;
    for (uint8_t i = 0; i < 4; i++) {
        if (header->e_ident[i] != ELF_MAGIC[i]) {
            // it's not ELF
            return 0;
        }
    }

    /*
    * check version
    */
    if (0x01 != header->e_version) {
        // we expect elf v1
        return 0;
    }
    return 1;
}

void elf_dump(uint8_t* binary, uint32_t len) {
    ASSERT_NOT_NULL(binary);
    ASSERT(0 != len);
    ASSERT(len >= sizeof(struct elf_header));

    struct elf_header* header = (struct elf_header*)binary;
    kprintf("Program Header offset %llu entry size %llu count %llu\n", header->e_phoff, header->e_phentsize,
            header->e_phnum);
    kprintf("Section Header offset %llu entry size %llu count %llu\n", header->e_shoff, header->e_shentsize,
            header->e_shnum);
}

uint8_t elf_parse(uint8_t* binary, uint32_t len, struct elf_file* elf) {
    ASSERT_NOT_NULL(binary);
    ASSERT_NOT_NULL(elf);
    ASSERT(0 != len);
    ASSERT(len >= sizeof(struct elf_header));
    /*
    * make sure its elf
    */
    if (1 != elf_is_elf_binary(binary, len)) {
        // not elf
        return 0;
    }
    /*
    * set section count
    */
    struct elf_header* header = (struct elf_header*)binary;
    elf->section_count = header->e_shnum;
    /*
    * iterate sections
    */
    for (uint8_t i = 0; i < elf->section_count; i++) {
    }

    //  elf_dump(binary, len);
    return 1;
}