package master

import (
	"bytes"
	"strings"

	"github.com/ikihiki/cremona/daemon/internal/log"
)

type logWriter struct {
	buffer strings.Builder
	logger log.Logger
	isLogError bool
}

func (writer *logWriter) Write(p []byte) (n int, err error){
	total := len(p)
	for ; len(p) > 0; {
		newLineIndex := bytes.IndexByte(p, '\n')
		if newLineIndex == -1 {
			writer.buffer.Write(p)
			break
		}

		writer.buffer.Write(p[0:newLineIndex+1])
		if writer.isLogError {
			writer.logger.Errorf("%s", writer.buffer.String())
		}else{
			writer.logger.Printf("%s", writer.buffer.String())
		}
		writer.buffer.Reset()
		p = p[newLineIndex+1:]
	}
	return total, nil
}