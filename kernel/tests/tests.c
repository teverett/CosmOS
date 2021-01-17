//*****************************************************************
// This file is part of CosmOS                                    *
// Copyright (C) 2020 Tom Everett                                 *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

#include <tests/tests.h>

void tests_run() {
    // alway run malloc tests
    // TODO, check this, something might be weird in kfree?
    test_malloc();

    // alway run array tests
    test_array();

    // always run the list tests
    test_list();

    // always run the string tests
    //  test_string();
}
