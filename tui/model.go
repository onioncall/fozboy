package tui

import (
	"time"

	tea "github.com/charmbracelet/bubbletea"
)

type tickMsg time.Time

type Model struct {
	width  int
	height int

	aFocused bool
	bFocused bool

	upFocused    bool
	downFocused  bool
	leftFocused  bool
	rightFocused bool

	selectFocused bool
	startFocused  bool

	ticksSinceKeyPress int
	tickRunning        bool
}

func InitialModel() Model {
	return Model{}
}

func (m Model) Init() tea.Cmd {
	return nil
}

func tickCmd() tea.Cmd {
	return tea.Tick(time.Second/60, func(t time.Time) tea.Msg {
		return tickMsg(t)
	})
}
