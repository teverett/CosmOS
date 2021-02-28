//*****************************************************************
// This file is part of CosmOS                                    *
// Copyright (C) 2020-2021 Tom Everett                            *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

#ifndef _CPM_H
#define _CPM_H

#include <types.h>

struct object* cpm_attach(struct object* partition_objice);
void cpm_detach(struct object* obj);

#endif