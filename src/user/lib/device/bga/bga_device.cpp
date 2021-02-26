
//*****************************************************************
// This file is part of CosmOS                                    *
// Copyright (C) 2020-2021 Tom Everett                            *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

#include <device/bga/bga_device.hpp>

void BGADevice::get_resolution(struct device* dev, struct deviceapi_resolution* resolution) {}
void BGADevice::set_resolution(struct device* dev, struct deviceapi_resolution* resolution) {}
uint32_t BGADevice::get_buffersize(struct device* dev) {
    return 0;
}
void BGADevice::blt(struct device* dev, uint8_t* buffer, uint32_t buffer_size) {}
