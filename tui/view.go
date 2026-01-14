package tui

import (
	"strings"

	"github.com/charmbracelet/lipgloss"
)

func (m Model) View() string {
	if m.width == 0 || m.height == 0 {
		return ""
	}

	boxHeight := m.height - 2
	boxWidth := m.height

	contentHeight := (boxHeight - 3) / 2

	activeButtonStyle := lipgloss.NewStyle().
		Foreground(lipgloss.Color("208")) // Orange color

	boxStyle := lipgloss.NewStyle().
		Border(lipgloss.RoundedBorder()).
		Width(boxWidth).
		Height(boxHeight).
		Align(lipgloss.Center, lipgloss.Center)

	topSection := lipgloss.NewStyle().
		Height(contentHeight).
		Width(boxWidth-2).
		Align(lipgloss.Center, lipgloss.Center).
		Render("This will be a screen")

	dividerLine := strings.Repeat("─", boxWidth-2)
	divider := lipgloss.NewStyle().
		Width(boxWidth - 2).
		Render(dividerLine)

	selectButton := lipgloss.NewStyle().
		Border(lipgloss.RoundedBorder()).
		Padding(0, 1).
		Render("select")

	if m.selectFocused {
		selectButton = lipgloss.NewStyle().
			Border(lipgloss.RoundedBorder()).
			BorderForeground(lipgloss.Color("208")).
			Padding(0, 1).
			Foreground(lipgloss.Color("208")). // Orange color
			Render("select")
	}

	startButton := lipgloss.NewStyle().
		Border(lipgloss.RoundedBorder()).
		Padding(0, 1).
		Render("start")

	if m.startFocused {
		startButton = lipgloss.NewStyle().
			Border(lipgloss.RoundedBorder()).
			BorderForeground(lipgloss.Color("208")).
			Padding(0, 1).
			Foreground(lipgloss.Color("208")). // Orange color
			Render("start")
	}

	bButton := "[ B ]"
	if m.bFocused {
		bButton = activeButtonStyle.Render(bButton)
	}
	aButton := "[ A ]"
	if m.aFocused {
		aButton = activeButtonStyle.Render(aButton)
	}

	leftArrowButton := "[ ← │"
	if m.leftFocused {
		leftArrowButton = activeButtonStyle.Render(leftArrowButton)
	}
	rightArrowButton := "│ → ]"
	if m.rightFocused {
		rightArrowButton = activeButtonStyle.Render(rightArrowButton)
	}
	upArrowButton := "│ ↑ │"
	if m.upFocused {
		upArrowButton = activeButtonStyle.Render(upArrowButton)
	}
	downArrowButton := "│ ↓ │"
	if m.downFocused {
		downArrowButton = activeButtonStyle.Render(downArrowButton)
	}

	bottomButtons := lipgloss.JoinHorizontal(
		lipgloss.Center,
		selectButton,
		"  ",
		startButton,
	)

	rightButtons := lipgloss.JoinHorizontal(
		lipgloss.Center,
		bButton,
		"  ",
		aButton,
	)

	leftArrowButtons := lipgloss.JoinHorizontal(
		lipgloss.Center,
		leftArrowButton,
		"   ",
		rightArrowButton,
	)

	upArrowCentered := lipgloss.NewStyle().
		Width(lipgloss.Width(leftArrowButtons)).
		Align(lipgloss.Center, lipgloss.Center).
		Render(upArrowButton)

	downArrowCentered := lipgloss.NewStyle().
		Width(lipgloss.Width(leftArrowButtons)).
		Align(lipgloss.Center, lipgloss.Center).
		Render(downArrowButton)

	leftButtons := lipgloss.JoinVertical(
		lipgloss.Center,
		upArrowCentered,
		leftArrowButtons,
		downArrowCentered,
	)

	middleSection := lipgloss.NewStyle().
		Width(boxWidth - 2).
		Render(
			lipgloss.JoinHorizontal(
				lipgloss.Left,
				leftButtons,
				strings.Repeat(" ", (boxWidth-2)-lipgloss.Width(leftButtons)-lipgloss.Width(rightButtons)),
				rightButtons,
			),
		)

	bottomSection := lipgloss.NewStyle().
		Width(boxWidth-2).
		Align(lipgloss.Center, lipgloss.Center).
		Render(bottomButtons)

	middleHeight := contentHeight / 2
	remainingHeight := contentHeight - middleHeight - lipgloss.Height(bottomSection)

	bottomContent := lipgloss.JoinVertical(
		lipgloss.Left,
		strings.Repeat("\n", middleHeight-3),
		middleSection,
		strings.Repeat("\n", remainingHeight),
		bottomSection,
	)

	content := lipgloss.JoinVertical(
		lipgloss.Left,
		topSection,
		divider,
		bottomContent,
	)

	box := boxStyle.Render(content)

	return "\n\n" + lipgloss.Place(
		m.width,
		m.height-1,
		lipgloss.Center,
		lipgloss.Top,
		box,
		lipgloss.WithWhitespaceChars(" "),
	)
}
