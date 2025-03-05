package cgoclient

// #cgo CXXFLAGS: -I${SRCDIR}/extract
// #cgo LDFLAGS: -L${SRCDIR}/extract -lextract
// #include "stdlib.h"
// #include "wrap_extract.h"
import "C"

type XpdfCgoClient struct{}

func (c *XpdfCgoClient) Extract(input []byte) ([]byte, error) {
	cData := C.CBytes(input)
	defer C.free(cData)
	inSize := C.size_t(len(input))

	out := make([]byte, len(input))
	outBuf := C.CBytes(out)
	defer C.free(outBuf)
	count := C.wrap_extract(cData, inSize, outBuf)
	data := C.GoBytes(outBuf, count)
	return data, nil
}
