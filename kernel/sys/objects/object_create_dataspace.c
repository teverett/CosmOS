/*****************************************************************
 * This file is part of CosmOS                                   *
 * Copyright (C) 2021 Kurt M. Weber                              *
 * Released under the stated terms in the file LICENSE           *
 * See the file "LICENSE" in the source distribution for details *
 *****************************************************************/

#include <sys/devicemgr/devicemgr.h>
#include <sys/kmalloc/kmalloc.h>
#include <sys/objects/objects.h>
#include <sys/panic/panic.h>
#include <types.h>

object_handle_t object_create_dataspace(device_t* dev, uint8_t idx) {
    object_dataspace_t* obj_data;

    obj_data = (object_dataspace_t*)kmalloc(sizeof(object_dataspace_t));
    if (!obj_data) {
        panic("kmalloc failed!");
    }

    obj_data->dev = dev;
    obj_data->idx = idx;

    return 0;
}