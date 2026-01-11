package main

// #cgo CFLAGS: -I./zig-out/include
// #cgo LDFLAGS: -L./zig-out/lib -lgbc
// #include "gbc.h"
import "C"
import "fmt"

func main() {
	res := C.run()
	fmt.Println(res)
}
