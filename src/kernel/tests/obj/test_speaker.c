//*****************************************************************
// This file is part of CosmOS                                    *
// Copyright (C) 2020-2021 Tom Everett                            *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

#include <sys/kprintf/kprintf.h>
#include <sys/objectinterface/objectinterface_speaker.h>
#include <sys/sleep/sleep.h>
#include <tests/obj/test_speaker.h>

/* void BeethovensFifth() {
    // get the speaker
    struct object* speaker = objectmgr_find_object("speaker0");
    if (0 != speaker) {
        struct objectinterface_speaker* speaker_api = (struct objectinterface_speaker*)speaker->api;
        speaker_beep_function beep_func = speaker_api->beep;
        //	(*beep_func)(speaker, 4000, 50);
        (*beep_func)(speaker, NOTE_G5, 200);
        sleep_wait(100);
        (*beep_func)(speaker, NOTE_G5, 200);
        sleep_wait(100);
        (*beep_func)(speaker, NOTE_G5, 200);
        sleep_wait(100);
        (*beep_func)(speaker, NOTE_DS5, 400);
        sleep_wait(400);

        (*beep_func)(speaker, NOTE_F5, 200);
        sleep_wait(100);
        (*beep_func)(speaker, NOTE_F5, 200);
        sleep_wait(100);
        (*beep_func)(speaker, NOTE_F5, 200);
        sleep_wait(100);
        (*beep_func)(speaker, NOTE_D5, 400);
        sleep_wait(100);
    } else {
        kprintf("Unable to find speaker0\n");
    }
} */

void chirp() {
    // get the speaker
    struct object* speaker = objectmgr_find_object("speaker0");
    if (0 != speaker) {
        struct objectinterface_speaker* speaker_api = (struct objectinterface_speaker*)speaker->api;
        speaker_beep_function beep_func = speaker_api->beep;
        (*beep_func)(speaker, 4000, 50);
    } else {
        kprintf("Unable to find speaker0\n");
    }
}
