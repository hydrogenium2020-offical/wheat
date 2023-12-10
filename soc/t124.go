package soc

type Soc struct {
	IRAM_START  uint32
	IRAM_END    uint32
	IRAM_LEN    uint32
	RCM_PAYLOAD uint32
	USB_BUF_1   uint32
	USB_BUF_2   uint32
}

// t124 or t132 using rcm v40,the header is 0x284 bytes
func T124() Soc {
	return Soc{
		IRAM_START: 0x40000000, //4000:0000
		IRAM_END:   0x4003ffff, //4003:ffff
		IRAM_LEN:   0x00040000, //iRAM-A,B,C,D 64KB*4

		//rcm payload part address.payload size:>=0x400 && <= 0x31000 (IRAM_END-RCM_PAYLOAD.iRam limitation).
		//the size ends in 8. Because the payload needs to be aligned to 16 bytes.
		RCM_PAYLOAD: 0x4000e000,

		//RCM_PAYLOAD for t132/t124
		USB_BUF_1: 0x40004000,
		USB_BUF_2: 0x40008000,
	}
}
