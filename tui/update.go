package tui

import (
	"encoding/base64"
	"fmt"
	"os"

	tea "charm.land/bubbletea/v2"
)

func (m model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	switch msg := msg.(type) {
	case tea.WindowSizeMsg:
		m.width = msg.Width
		m.height = msg.Height
		m.calculateBorderSize()

	case tea.KeyPressMsg:
		switch msg.String() {
		case "esc", "ctrl+c":
			return m, tea.Quit
		case " ", "space":
			m.screen = "Key down"

			if m.imageData == "" {
				imageBytes, err := os.ReadFile("./tui/test-gbc-fitted.png")
				if err != nil {
					m.screen = fmt.Sprintf("Error loading image: %v", err)
					return m, nil
				}
				m.imageData = base64.StdEncoding.EncodeToString(imageBytes)

				// Calculate cursor position for centering
				offsetY := (m.height - m.borderHeight) / 2
				offsetX := (m.width - m.borderWidth) / 2

				// Center the image horizontally
				imageCols := m.borderWidth - imagePadding
				borderInnerWidth := m.borderWidth - 2
				remainingWidth := borderInnerWidth - imageCols
				horizontalOffset := remainingWidth / 2

				imageRow := offsetY + 2
				imageCol := offsetX + 1 + horizontalOffset

				fmt.Fprintf(os.Stdout, "\033[%d;%dH", imageRow+1, imageCol+1)
				fmt.Fprint(os.Stdout, m.renderKittyImage())
				m.imageSent = true
			}
		case "a", "x":
			m.aFocused = true
		case "b", "z":
			m.bFocused = true
		case "left", "h":
			m.leftFocused = true
		case "right", "l":
			m.rightFocused = true
		case "up", "k":
			m.upFocused = true
		case "down", "j":
			m.downFocused = true
		case "n":
			m.startFocused = true
		case "m":
			m.selectFocused = true
		}

	case tea.KeyReleaseMsg:
		switch msg.String() {
		case " ", "space":
			m.screen = ""
		case "a", "x":
			m.aFocused = false
		case "b", "z":
			m.bFocused = false
		case "left", "h":
			m.leftFocused = false
		case "right", "l":
			m.rightFocused = false
		case "up", "k":
			m.upFocused = false
		case "down", "j":
			m.downFocused = false
		case "n":
			m.startFocused = false
		case "m":
			m.selectFocused = false
		}
	}

	return m, nil
}
