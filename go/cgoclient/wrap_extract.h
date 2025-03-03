#ifndef WRAP_EXTRACT_H
#define WRAP_EXTRACT_H

// __cplusplus gets defined when a C++ compiler processes the file
#ifdef __cplusplus
// extern "C" is needed so the C++ compiler exports the symbols without name
// manging.
extern "C" {
#endif

int wrap_extract(void *in, size_t len, void *out);

#ifdef __cplusplus
}
#endif


#endif