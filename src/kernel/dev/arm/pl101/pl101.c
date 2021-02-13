//*****************************************************************
// This file is part of CosmOS                                    *
// Copyright (C) 2020-2021 Tom Everett                            *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

#include <dev/arm/pl101/pl101.h>
#include <sys/asm/asm.h>
#include <sys/collection/ringbuffer/ringbuffer.h>
#include <sys/debug/assert.h>
#include <sys/deviceapi/deviceapi_serial.h>
#include <sys/devicemgr/devicemgr.h>
#include <sys/interrupt_router/interrupt_router.h>
#include <sys/kprintf/kprintf.h>

// void serial_irq_handler(stack_frame *frame){
//	ASSERT_NOT_NULL(frame);
//    struct rs232_16550* comport = (struct rs232_16550*) COM1_ADDRESS;
//    uint8_t data = asm_in_b((uint64_t)&(comport->data));

// echo the data
//    serial_write_char(data);
//}

/*
 * perform device instance specific init here
 */
void serial_device_init(struct device* dev) {
    ASSERT_NOT_NULL(dev);
    //    struct serial_devicedata* device_data = (struct serial_devicedata*) dev->device_data;
    //    kprintf("Init %s at IRQ %llu Base %#hX (%s)\n",dev->description, device_data->irq, device_data->address, dev->name);
    //    interrupt_router_register_interrupt_handler(device_data->irq, &serial_irq_handler);
    //    serial_init_port(device_data->address);
}

void serial_register_device(uint8_t irq, uint64_t base) {
    /*
     * ISA serial port specific data
     */
    //   struct serial_devicedata* device_data = kmalloc(sizeof(struct serial_devicedata));
    //   device_data->irq=irq;
    //   device_data->address=base;
    //   device_data->buffer = ringbuffer_new();
    /*
     * the device instance
     */
    struct device* deviceinstance = devicemgr_new_device();
    deviceinstance->init = &serial_device_init;
    //    deviceinstance->device_data = device_data;
    deviceinstance->devicetype = SERIAL;
    devicemgr_set_device_description(deviceinstance, "PL101");
    /*
     * the device api
     */
    //    struct deviceapi_serial* api = (struct deviceapi_serial*) kmalloc(sizeof(struct deviceapi_serial));
    //  api->write = &serial_write;
    //  deviceinstance->api = api;
    /*
     * register
     */
    devicemgr_register_device(deviceinstance);
}

/**
 * find all RS232 devices and register them
 */
void serial_devicemgr_register_devices() {
    //   serial_register_device(SERIAL_IRQ2,COM1_ADDRESS);

    // TODO add code to check if these even exist
    //    registerRS232Device(SERIAL_IRQ1,COM2_ADDRESS);
    //    registerRS232Device(SERIAL_IRQ2,COM3_ADDRESS);
    //    registerRS232Device(SERIAL_IRQ1,COM4_ADDRESS);
}