//*****************************************************************
// This file is part of CosmOS                                    *
// Copyright (C) 2020-2021 Tom Everett                            *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

#include <sys/collection/arraylist/arraylist.h>
#include <sys/debug/assert.h>
#include <sys/kprintf/kprintf.h>
#include <sys/obj/objecttypes/objecttypes.h>

struct arraylist* types;

void objecttypes_init() {
    types = arraylist_new();
}

uint32_t objecttypes_count() {
    ASSERT_NOT_NULL(types);
    return arraylist_count(types);
}

struct object_type* objecttypes_find(enum object_type_id id) {
    ASSERT_NOT_NULL(types);
    ASSERT_NOT_NULL(id);
    for (uint32_t i = 0; i < arraylist_count(types); i++) {
        struct object_type* ot = (struct object_type*)arraylist_get(types, i);
        ASSERT_NOT_NULL(ot);
        if (ot->id == id) {
            return ot;
        }
    }
    return 0;
}

struct object_type* objecttypes_get(uint32_t i) {
    ASSERT_NOT_NULL(types);
    ASSERT(i < arraylist_count(types));
    return (struct object_type*)arraylist_get(types, i);
}
