//*****************************************************************
// This file is part of CosmOS                                    *
// Copyright (C) 2020-2021 Tom Everett                            *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

#include <obj/logical/telnet/telnet_commandloop.h>
#include <sys/debug/assert.h>
#include <sys/fs/fs_facade.h>
#include <sys/kprintf/kprintf.h>
#include <sys/obj/object/object.h>
#include <sys/obj/objectinterface/objectinterface_serial.h>
#include <sys/obj/objecttypes/objecttypes.h>
#include <sys/sleep/sleep.h>
#include <sys/string/mem.h>
#include <sys/string/string.h>
#include <types.h>

#define TELNET_LINE_LEN 255
#define TELNET_COSMOS_PROMPT "CosmOS> "

void telnet_read_line(struct object* serial_object, uint8_t* line, uint16_t size) {
    ASSERT_NOT_NULL(serial_object);
    ASSERT_NOT_NULL(line);
    ASSERT_NOT_NULL(size);
    struct objectinterface_serial* serial_api = (struct objectinterface_serial*)serial_object->api;
    ASSERT_NOT_NULL(serial_api);

    uint8_t read_more = 1;
    uint8_t count = 0;
    while (1 == read_more) {
        while (0 == (*serial_api->avail)(serial_object)) {
            sleep_wait(100);
        }
        uint8_t c = (*serial_api->readchar)(serial_object);
        if (c == 13) {
            line[count] = 0;
            return;
        } else {
            line[count] = c;
            (*serial_api->writechar)(serial_object, c);
        }
        /*
        * exit..
        */
        if (count == (size - 1)) {
            line[size] = 0;
            return;
        }
        count += 1;
    }
}

uint8_t telnet_process_line(uint8_t* line) {
    ASSERT_NOT_NULL(line);
    if (0 != line) {
        if (strcmp(line, "exit") == 0) {
            return 0;
        } else if (strcmp(line, "show_voh") == 0) {
            dump_VOH();
        } else if (strcmp(line, "show_object_types") == 0) {
            objecttypes_dump();
        } else if (strcmp(line, "show_objects") == 0) {
            objectmgr_dump_objects();
        } else {
            kprintf("Type 'show_voh' to show VOH\n");
            kprintf("Type 'show_object_types' to show object types\n");
            kprintf("Type 'show_objects' to show objects\n");
            kprintf("Type 'exit' to exit");
            return 1;
        }
    }
    return 1;
}

uint8_t telnet_command_loop(struct object* serial_object) {
    ASSERT_NOT_NULL(serial_object);

    struct objectinterface_serial* serial_api = (struct objectinterface_serial*)serial_object->api;
    ASSERT_NOT_NULL(serial_api);

    uint8_t go = 1;
    while (1 == go) {
        (serial_api->write)(serial_object, TELNET_COSMOS_PROMPT);
        uint8_t line[TELNET_LINE_LEN];
        memzero(line, TELNET_LINE_LEN);
        telnet_read_line(serial_object, line, TELNET_LINE_LEN);
        kprintf("\n");
        go = telnet_process_line(line);
        kprintf("\n");
    }
    return 0;
}