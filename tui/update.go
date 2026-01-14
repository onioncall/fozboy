package tui

import tea "github.com/charmbracelet/bubbletea"

func (m Model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	switch msg := msg.(type) {
	case tea.KeyMsg:
		switch msg.String() {
		case "ctrl+c", "q", "esc":
			return m, tea.Quit
		case "left", "a", "h":
			m.leftFocused = true
			m.ticksSinceKeyPress = 0
			if !m.tickRunning {
				m.tickRunning = true
				return m, tickCmd()
			}
		case "right", "d", "l":
			m.rightFocused = true
			m.ticksSinceKeyPress = 0
			if !m.tickRunning {
				m.tickRunning = true
				return m, tickCmd()
			}
		case "up", "w", "k":
			m.upFocused = true
			m.ticksSinceKeyPress = 0
			if !m.tickRunning {
				m.tickRunning = true
				return m, tickCmd()
			}
		case "down", "s", "j":
			m.downFocused = true
			m.ticksSinceKeyPress = 0
			if !m.tickRunning {
				m.tickRunning = true
				return m, tickCmd()
			}
		case "n":
			m.bFocused = true
			m.ticksSinceKeyPress = 0
			if !m.tickRunning {
				m.tickRunning = true
				return m, tickCmd()
			}
		case "m":
			m.aFocused = true
			m.ticksSinceKeyPress = 0
			if !m.tickRunning {
				m.tickRunning = true
				return m, tickCmd()
			}
		case "x":
			m.selectFocused = true
			m.ticksSinceKeyPress = 0
			if !m.tickRunning {
				m.tickRunning = true
				return m, tickCmd()
			}
		case "c":
			m.startFocused = true
			m.ticksSinceKeyPress = 0
			if !m.tickRunning {
				m.tickRunning = true
				return m, tickCmd()
			}
			return m, nil
		}

	case tickMsg:
		if m.tickRunning {
			m.ticksSinceKeyPress++

			// If no key press in last 3 ticks, assume released
			if m.ticksSinceKeyPress > 3 {
				m.leftFocused = false
				m.rightFocused = false
				m.upFocused = false
				m.downFocused = false
				m.aFocused = false
				m.bFocused = false
				m.startFocused = false
				m.selectFocused = false
				m.tickRunning = false
				return m, nil
			}

			return m, tickCmd()
		}

	case tea.WindowSizeMsg:
		m.width = msg.Width
		m.height = msg.Height
	}
	return m, nil
}
