package tui

import (
	"fmt"
	"strings"

	tea "charm.land/bubbletea/v2"
)

func renderButton(label string, focused bool) string {
	green := "\033[92m"
	red := "\033[91m"
	reset := "\033[0m"

	if focused {
		return fmt.Sprintf("%s[ %s ]%s", red, label, reset)
	}
	return fmt.Sprintf("%s[ %s ]%s", green, label, reset)
}

func (m model) View() tea.View {
	var v tea.View

	if m.width == 0 || m.height == 0 {
		v.SetContent("Loading...")
	} else if m.width < 50 || m.height < 15 {
		v.SetContent("Terminal too small. Please resize.")
	} else {
		offsetY := (m.height - m.borderHeight) / 2
		offsetX := (m.width - m.borderWidth) / 2

		mainBorderLines := make([]string, m.borderHeight)

		// Top border
		mainBorderLines[0] = "╭" + strings.Repeat("─", m.borderWidth-2) + "╮"

		// Middle rows
		for i := 1; i < m.borderHeight-1; i++ {
			contentWidth := m.borderWidth - 2

			// We won't need this when we are actually getting image data from the emulator
			var content string
			if m.screen != "" {
				content = m.screen
			} else if m.imageData == "" {
				content = "Press SPACE to load image"
			}

			if content != "" && i == m.borderHeight/2 {
				padding := contentWidth - len(content)
				if padding < 0 {
					content = content[:contentWidth]
					padding = 0
				}
				leftPad := padding / 2
				rightPad := padding - leftPad
				mainBorderLines[i] = "│" + strings.Repeat(" ", leftPad) + content + strings.Repeat(" ", rightPad) + "│"
			} else {
				mainBorderLines[i] = "│" + strings.Repeat(" ", contentWidth) + "│"
			}
		}

		// Bottom border
		mainBorderLines[m.borderHeight-1] = "╰" + strings.Repeat("─", m.borderWidth-2) + "╯"

		btnUp := renderButton("↑", m.upFocused)
		btnDown := renderButton("↓", m.downFocused)
		btnLeft := renderButton("←", m.leftFocused)
		btnRight := renderButton("→", m.rightFocused)
		btnB := renderButton("B", m.bFocused)
		btnA := renderButton("A", m.aFocused)
		btnSelect := renderButton("Select", m.selectFocused)
		btnStart := renderButton("Start", m.startFocused)

		var sb strings.Builder

		for range offsetY {
			sb.WriteString("\n")
		}

		allButtonsHeight := 9
		buttonsStartY := (m.borderHeight - allButtonsHeight) / 2

		for i := range m.borderHeight {
			sb.WriteString(strings.Repeat(" ", offsetX))
			sb.WriteString(mainBorderLines[i])

			relY := i - buttonsStartY
			sb.WriteString("    ")

			if relY == 0 {
				sb.WriteString("    ")
				sb.WriteString(btnUp)
			} else if relY == 1 {
				sb.WriteString(btnLeft)
				sb.WriteString("   ")
				sb.WriteString(btnRight)
			} else if relY == 2 {
				sb.WriteString("    ")
				sb.WriteString(btnDown)
			} else if relY == 4 {
				sb.WriteString("    ")
				sb.WriteString(btnB)
			} else if relY == 5 {
				sb.WriteString("    ")
				sb.WriteString(btnA)
			} else if relY == 7 {
				sb.WriteString("  ")
				sb.WriteString(btnStart)
			} else if relY == 8 {
				sb.WriteString("  ")
				sb.WriteString(btnSelect)
			}
			sb.WriteString("\n")
		}

		sb.WriteString(fmt.Sprintf("\nTerminal: %dx%d  Border: %dx%d (ratio: %.2f)",
			m.width, m.height, m.borderWidth, m.borderHeight,
			float64(m.borderWidth*2)/float64(m.borderHeight)))

		v.SetContent(sb.String())
	}

	v.AltScreen = true
	v.KeyboardEnhancements.ReportEventTypes = true
	return v
}
