//*****************************************************************
// This file is part of CosmOS                                    *
// Copyright (C) 2020-2021 Tom Everett                            *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

/*
* userland console terminal
*/

#ifndef _TERM_HPP
#define _TERM_HPP

#include <cosmos.hpp>

class Term {
  private:
    SerialDevice* serialDevice;

  public:
    Term();
    void run();
};

#endif
