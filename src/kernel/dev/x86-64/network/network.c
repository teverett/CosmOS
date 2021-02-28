// This file is part of CosmOS                                    *
// Copyright (C) 2020-2021 Tom Everett                            *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

#include <dev/x86-64/network/network.h>

void network_objectmgr_register_devices() {
    rtl8139_objectmgr_register_devices();
    // ne2000pci_objectmgr_register_devices();
    //   ne2000isa_objectmgr_register_devices();
    //  e1000_objectmgr_register_devices();
    //   vnic_objectmgr_register_devices();
}
