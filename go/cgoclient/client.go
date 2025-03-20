package cgoclient

// #cgo CXXFLAGS: -I${SRCDIR}
// #cgo LDFLAGS: -L${SRCDIR} -lextract
// #include "stdlib.h"
// #include "wrap_extract.h"
import "C"
import "errors"

type XpdfCgoClient struct{}

func (c *XpdfCgoClient) Extract(input []byte) ([]byte, error) {
	cData := C.CBytes(input)
	defer C.free(cData)
	inSize := C.size_t(len(input))

	out := make([]byte, len(input))
	outBuf := C.CBytes(out)
	defer C.free(outBuf)

	// input and output buffers of the same size, because the output size
	// is not known in advance, but it is guaranteed to be less than or equal
	// to the input size
	count := C.wrap_extract(cData, inSize, outBuf, inSize)
	if count == -1 {
		return nil, errors.New("cgo extract failed")
	}
	data := C.GoBytes(outBuf, count)
	return data, nil
}
