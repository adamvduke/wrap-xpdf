#ifndef WRAP_EXTRACT_H
#define WRAP_EXTRACT_H

// __cplusplus gets defined when a C++ compiler processes the file
#ifdef __cplusplus
// extern "C" is needed so the C++ compiler exports the symbols without name
// manging.
extern "C" {
#endif

/*
 * wrap_extract wraps extractPdfText in a C friendly interface for use in CGO.
*/
int wrap_extract(void *in, size_t in_size, void *out, size_t out_size);
#ifdef __cplusplus
}
#endif


#endif