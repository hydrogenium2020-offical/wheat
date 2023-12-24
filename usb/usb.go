package usb

//#include "mini_libusb.h"
import "C"
import (
	"fmt"
	"runtime"
	"unsafe"
)

type USB_device int32

// Control Mode (only was used in exploit)
const RCM_CTRL_BMREQUEST_TYPE_DEVICE_EP_TO_HOST = 0x82
const RCM_CTRL_BREQUEST_GET_STATUS = 0x00

func (d *USB_device) Close() {
	C.usb_close(C.int(*d))
}

// https://gongluck.github.io/go/cgo_types/
// int usb_send_control_txn( int usb, uint8_t bRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t len, uint8_t *data, int32_t timeout );
func (d *USB_device) Transfer_Ctrl_GET_STATUS(bRequestType uint, len uint16, data *[]byte) {
	sliceData := unsafe.SliceData(*data)

	// 将 slice 中的 data 指针指向的对象 Pin 住。 Pin the object of pointer
	var pinner runtime.Pinner
	defer pinner.Unpin()
	pinner.Pin(unsafe.Pointer(sliceData))

	C.usb_send_control_txn(C.int(*d), C.uchar(bRequestType), C.uchar(RCM_CTRL_BREQUEST_GET_STATUS), 0, 0, C.ushort(len), (*C.uchar)(sliceData), 500)
}
func (d *USB_device) Transfer_bulk_EP_IN(ep uint, length int, data *[]byte) error {
	//Go 1.21 feature
	//https://github.com/golang/go/issues/46787
	//https://uncledou.site/page/2/
	sliceData := unsafe.SliceData(*data)

	// 将 slice 中的 data 指针指向的对象 Pin 住。 Pin the object of pointer
	var pinner runtime.Pinner
	defer pinner.Unpin()
	pinner.Pin(unsafe.Pointer(sliceData))

	r := C.usb_send_bulk_txn(C.int(*d), C.uint(ep), C.uint(length), unsafe.Pointer(sliceData))

	if r < 0 {
		return fmt.Errorf("could not read EP1_IN")
	} else {

		return nil
	}
}

func OpenUSB_by_vid_pid(vid, pid uint16) (*USB_device, error) {
	d := USB_device(C.usb_open_by_vid_pid(C.ushort(vid), C.ushort(pid), 1))
	if d < 0 {
		return &d, fmt.Errorf("couldn't open usb device")
	}
	return &d, nil
}
