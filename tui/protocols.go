package tui

import (
	"fmt"
	"strings"
)

const (
	// This isn't really necessary, but I think it looks better for now
	imagePadding = 4
)

func (m *model) renderKittyImage() string {
	if m.imageData == "" {
		return ""
	}

	// Image dimensions are just border dimensions minus padding
	imageCols := m.borderWidth - imagePadding
	imageRows := m.borderHeight - imagePadding

	var result strings.Builder

	// 4096 bytes is the maximum size for Kitty graphics protocol escape sequences
	const chunkSize = 4096
	chunks := []string{}

	for i := 0; i < len(m.imageData); i += chunkSize {
		end := min(i+chunkSize, len(m.imageData))
		chunks = append(chunks, m.imageData[i:end])
	}

	for i, chunk := range chunks {
		if i == 0 {
			if len(chunks) == 1 {
				result.WriteString(fmt.Sprintf("\033_Ga=T,f=100,c=%d,r=%d;%s\033\\",
					imageCols, imageRows, chunk))
			} else {
				result.WriteString(fmt.Sprintf("\033_Ga=T,f=100,c=%d,r=%d,m=1;%s\033\\",
					imageCols, imageRows, chunk))
			}
		} else if i == len(chunks)-1 {
			result.WriteString(fmt.Sprintf("\033_Gm=0;%s\033\\", chunk))
		} else {
			result.WriteString(fmt.Sprintf("\033_Gm=1;%s\033\\", chunk))
		}
	}

	return result.String()
}
