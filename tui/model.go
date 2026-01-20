package tui

import tea "charm.land/bubbletea/v2"

type model struct {
	screen        string
	width         int
	height        int
	borderWidth   int
	borderHeight  int
	aFocused      bool
	bFocused      bool
	leftFocused   bool
	rightFocused  bool
	upFocused     bool
	downFocused   bool
	startFocused  bool
	selectFocused bool
	imageData     string
	imageSent     bool
	loadImage     bool
}

const (
	// We need to calculate dimensions that maintain 10:9 ratio (gbc screen)
	// and account for terminal cells being roughly 2:1 height to width. All
	// this to say we are working with a 20:9 ratio
	aspectRatioNumerator   = 20
	aspectRatioDenominator = 9

	// Minimum image dimensions in terminal cells
	minImageCols = 10
	minImageRows = 5
)

func InitialModel() model {
	m := model{
		screen: "",
	}

	return m
}

func (m model) Init() tea.Cmd {
	return nil
}

func (m *model) calculateBorderSize() {
	availWidth := m.width - 20
	availHeight := m.height - 4

	if availWidth < 10 {
		availWidth = 10
	}
	if availHeight < 5 {
		availHeight = 5
	}

	heightBasedWidth := (availHeight * 20) / 9
	if heightBasedWidth <= availWidth {
		m.borderHeight = availHeight
		m.borderWidth = heightBasedWidth
	} else {
		m.borderWidth = availWidth
		m.borderHeight = (availWidth * 9) / 20
	}

	if m.borderWidth < 10 {
		m.borderWidth = 10
	}
	if m.borderHeight < 5 {
		m.borderHeight = 5
	}
}
