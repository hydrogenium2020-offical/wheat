package util

import (
	"encoding/hex"
	"fmt"
)

func Print_hex(label string, data []byte) {
	fmt.Print(label)
	fmt.Println(hex.Dump(data))
}
