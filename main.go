package main

// #cgo CFLAGS: -I./zig-out/include
// #cgo LDFLAGS: -L./zig-out/lib -lgbc
// #include "gbc.h"
import "C"

import (
	"fmt"
	"os"

	tea "github.com/charmbracelet/bubbletea"
	"github.com/onioncall/fozboy/tui"
)

func main() {
	if len(os.Args) > 1 {
		if os.Args[1] == "tui" {
			p := tea.NewProgram(tui.InitialModel(), tea.WithAltScreen())
			if _, err := p.Run(); err != nil {
				fmt.Printf("Error: %v", err)
				return
			}
		}
	}

	res := C.run()
	fmt.Println(res)
}
