// Copyright (c) 2020 lordrafa. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither my name nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include <errno.h>
#include <ctype.h>

#include <sys/ioctl.h>
#include <linux/usbdevice_fs.h>

#define USB_BULK_TIMEOUT 500

#if DEBUG
    #define DEBUG_MSG(fmt, ...) do { fprintf( stderr, "%s:%d:%s(): " fmt, \
                                __FILE__, __LINE__, __func__, ##__VA_ARGS__ ); } while(0)
#else
    #define DEBUG_MSG(fmt, ...) 
#endif

int usb_open_by_vid_pid( uint16_t vid, uint16_t pid, uint8_t wait );
int usb_close( int usb );
int usb_send_bulk_txn( int usb, uint32_t ep, uint32_t len, void *data );
int usb_send_control_txn( int usb, uint8_t bRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t len, uint8_t *data, int32_t timeout );



