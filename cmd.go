package main

import (
	"flag"
	"log"

	"github.com/hydrogenium2020-offical/wheat/device"
	"github.com/hydrogenium2020-offical/wheat/rcm"
)

func main() {
	log.Default().SetFlags(0)
	device_type := flag.String("d", "t124", "Device type such as fuze f1,jetson tk1 ...")
	var device_model device.Device
	flag.Parse()

	switch *device_type {
	case "fuzef1":
		device_model = device.FuzeF1()
		rcm.RunRcmPayload(&device_model)
	default:
		flag.Usage()
		return
	}
}
