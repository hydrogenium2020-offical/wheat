package device

import "github.com/hydrogenium2020-offical/wheat/soc"

type Device struct {
	Name string
	Vid  uint16  //vendor_id
	Pid  uint16  //product_id
	Soc  soc.Soc //Soc type,such as T124 or T132
}

func FuzeF1() Device {
	return Device{
		Name: "Fuze F1",
		Vid:  0x0955,
		Pid:  0x7f40,
		Soc:  soc.T124(),
	}
}
