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

#include "mini_libusb.h"

#define SYSFS_MOUNT_PATH	"/sys"
#define SYSFS_DEVICE_PATH	SYSFS_MOUNT_PATH "/bus/usb/devices"

struct setup_data {
	uint8_t bRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
	uint8_t data[];
};

int usb_get_att(uint16_t *val, char *d_name, char *attr, int base) {

    int ret = -1;
    char filepath[256];
    char attr_buf[20];
    int attr_fd;

    snprintf(filepath, sizeof(filepath), SYSFS_DEVICE_PATH "/%s/%s", d_name, attr);
    attr_fd = open(filepath, O_RDONLY);
    if (attr_fd < 0) {
        DEBUG_MSG( "Usb %s: %s attribute not found.\n", d_name, attr );
        goto exit;
    }

    if ( !read(attr_fd, attr_buf, sizeof(attr_buf)) ) {
        DEBUG_MSG( "Usb %s: couldn't read %s.\n", d_name, attr );
        goto exit;
    }

    *val = (uint16_t) strtol(attr_buf, NULL, base);

    ret = 0;

exit:
    close(attr_fd);
    return ret;

}

int usb_find_path_by_vid_pid( char *path, uint16_t vid, uint16_t pid ) {

    int ret = -1;
    uint16_t d_vid, d_pid, d_bus, d_num;
    DIR *devices = opendir( SYSFS_DEVICE_PATH );
	struct dirent *entry;

	if ( !devices ) {
		fprintf( stderr, "Critical Error: Sysfs not avaiable." );
		return 1;
	}

	while ( ( entry = readdir( devices ) ) ) {

        if ( !isdigit( entry->d_name[0] ) || strchr( entry->d_name, ':' ) )
			continue;
        
        if ( !usb_get_att( &d_vid, entry->d_name, "idVendor", 16 ) && ( vid == d_vid ) &&
             !usb_get_att( &d_pid, entry->d_name, "idProduct", 16 ) && ( pid == d_pid ) &&
             !usb_get_att( &d_bus, entry->d_name, "busnum", 10 ) &&
             !usb_get_att( &d_num, entry->d_name, "devnum", 10 ) ) {

            sprintf( path, "/dev/bus/usb/%03d/%03d", d_bus, d_num );
            ret = 0;
            break;

        }

    }

    closedir( devices );
    return ret;

}

int usb_open_by_vid_pid( uint16_t vid, uint16_t pid, uint8_t wait ) {

    int ret;
    char path[256];

    do {
        ret = usb_find_path_by_vid_pid( path, vid, pid );
        usleep( 500 );
    } while ( wait && ( ret < 0 ) );
    DEBUG_MSG( "USB Path: %s\n", path );

    if (ret) {
        return -1;
    }

    return open( path, O_RDWR );

}

int usb_close( int usb ) {
    return usb;
}

int usb_send_control_txn( int usb, uint8_t bRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t len, uint8_t *data, int32_t timeout ) {

    int ret = -1;
    size_t setup_len = sizeof ( struct setup_data ) + len;
    struct setup_data *setup_data_buf;
    setup_data_buf = malloc( setup_len );
    memset( setup_data_buf, 0, setup_len );

    setup_data_buf->bRequestType = bRequestType;
    setup_data_buf->bRequest = bRequest;
    setup_data_buf->wValue = wValue;
    setup_data_buf->wIndex = wIndex;
    setup_data_buf->wLength = len;

    struct usbdevfs_urb usb_control_urb;
    memset(&usb_control_urb, 0, sizeof (usb_control_urb));

    usb_control_urb.type          = USBDEVFS_URB_TYPE_CONTROL;
    usb_control_urb.endpoint      = 0x0;
    usb_control_urb.buffer        = setup_data_buf;
    usb_control_urb.buffer_length = setup_len;

    ioctl( usb, USBDEVFS_SUBMITURB, &usb_control_urb );

    struct usbdevfs_urb *urb_reaped = NULL;

    if ( timeout ) { 

        while ( timeout > 0 ) {

            int _ret = ioctl( usb, USBDEVFS_REAPURBNDELAY, &urb_reaped );
            if ( _ret == 0 ) {
                break;
            }
            usleep(200000);
            timeout -= 200;

        }
 
    } else {

       ioctl( usb, USBDEVFS_REAPURB, &urb_reaped );

    }

    if ( urb_reaped && urb_reaped->status == 0 ) {
        ret = 0;
        memcpy( data, setup_data_buf->data, len );
    }

    if ( setup_data_buf ) free( setup_data_buf );
    return ret;

}

int usb_send_bulk_txn( int usb, uint32_t ep, uint32_t len, void *data ) {

    struct usbdevfs_bulktransfer bulk_txn;
    memset( &bulk_txn, 0, sizeof ( bulk_txn ) );

    bulk_txn.ep = ep;
    bulk_txn.len = len;
    bulk_txn.timeout = USB_BULK_TIMEOUT;
    bulk_txn.data = data;

    if ( ioctl( usb, USBDEVFS_BULK, &bulk_txn ) == len )
        return 0;

    return -1;

}

