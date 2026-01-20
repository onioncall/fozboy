package main

// #cgo CFLAGS: -I./zig-out/include
// #cgo LDFLAGS: -L./zig-out/lib -lgbc
// #include "gbc.h"
import "C"

import (
	"fmt"
	"os"

	tea "charm.land/bubbletea/v2"
	"github.com/onioncall/fozboy/tui"
)

func main() {
	if len(os.Args) > 1 {
		if os.Args[1] == "tui" {
			p := tea.NewProgram(tui.InitialModel())

			if _, err := p.Run(); err != nil {
				fmt.Printf("Error: %v", err)
				os.Exit(1)
			}
		}
	}

	res := C.run()
	fmt.Println(res)
}
