package rcm

import (
	"encoding/binary"
	"fmt"
	"log"
	"os"

	"github.com/hydrogenium2020-offical/wheat/device"
	"github.com/hydrogenium2020-offical/wheat/usb"
	"github.com/hydrogenium2020-offical/wheat/util"
)

// Bulk Mode (used in Read ChipID,Memory dump)
const RCM_BULK_EP_OUT = 0x01
const RCM_BULK_EP_IN = 0x81

const RCM_CTRL_EP_OUT = 0x00 //DEVICE_EP_TO_HOST
const RCM_CTRL_EP_IN = 0x80

func RunRcmPayload(model *device.Device) {
	//Connect the device
	fmt.Printf("-> %s VID:PID=0x%x:0x%x detected\n", model.Name, model.Vid, model.Pid)
	fmt.Println("-> Waiting for connecting")
	d, err := usb.OpenUSB_by_vid_pid(model.Vid, model.Pid)
	if err != nil {
		log.Fatal(err)
	}
	//Read ChipID
	read_chipid(d)
	stack_addr := read_stack(d)
	get_overwritten_len(stack_addr, model.Soc.USB_BUF_2)
	//over_written_len := get_overwritten_len(stack_addr, model.Soc.USB_BUF_2)
	//TODO:Send RCM command

}

func read_stack(d *usb.USB_device) uint32 {
	data := make([]byte, 2)
	fmt.Println("->", "GET_STATUS (normal) bRequestType=0x82(DEVICE_TO_HOST_EP) len=0x2 bytes...")
	d.Transfer_Ctrl_GET_STATUS(0x82, 0x2, &data)
	util.Print_hex("(USB Ctrl mode) Response->", data)

	data = make([]byte, 0x10)
	d.Transfer_Ctrl_GET_STATUS(0x82, 0x10, &data)
	fmt.Println("->", "GET_STATUS (stack overflow) bRequestType=0x82(DEVICE_TO_HOST_EP) len=0x10 bytes...")
	util.Print_hex("(USB Ctrl mode) Response->", data)

	// data = make([]byte, 0x40)
	// d.Transfer_Ctrl_GET_STATUS(0x82, 0x40, &data)
	// fmt.Println("Reading Stack len=0x10 bytes(stack overflow)...")
	// fmt.Println(hex.Dump(data))
	addr := binary.LittleEndian.Uint32(data[0xC : 0xC+4])
	fmt.Printf("-> Stack addr->0x%x\n", addr)
	return addr
}

func get_overwritten_len(stack_addr, USB_BUF_2 uint32) uint32 {
	fmt.Println("Hello,I got the address info for you!")
	//Get the SP addr
	sp_addr := stack_addr - 0xC
	fmt.Printf("\t-> SP addr->0x%x\n", sp_addr)
	memcpy_lp_addr := sp_addr - (4*2)*2
	fmt.Printf("\t-> memcpy addr->0x%x\n", memcpy_lp_addr)

	over_written_len := memcpy_lp_addr - USB_BUF_2
	fmt.Printf("\t-> overwritten_len->0x%x\n", over_written_len)
	return over_written_len
}

func read_chipid(d *usb.USB_device) {
	fmt.Println("-> Connected! Let's read ChipID")
	buf := make([]byte, 0x10)
	err := d.Transfer_bulk_EP_IN(RCM_BULK_EP_IN, 0x10, &buf)
	if err != nil {
		fmt.Println("Error -> Couldn't read ChipID. Please reset your device into RCM Mode")
		os.Exit(-1)
	} else {
		util.Print_hex("\tChip ID:", buf)
	}
}

func send_rcm_cmd(d *usb.USB_device, payload_path string, payload_thumb_mode bool) {
	//TODO

}

func resize_payload(f *[]byte) {

}
