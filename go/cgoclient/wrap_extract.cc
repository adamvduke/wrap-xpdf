#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cstring>

#include "extract.h"
#include "wrap_extract.h"

int wrap_extract(void *in, size_t in_size, void *out, size_t out_size)
{
  if (in == nullptr || out == nullptr)
  {
    // can't read/write to/from null
    return -1;
  }

  if (in_size == 0 || out_size == 0)
  {
    // Invalid sizes
    return -1;
  }

  char *pdf_data = (char *)in;
  std::string as_string(pdf_data, in_size);

  std::stringstream outstream;
  int outlen = extractPdfText(as_string, &outstream);
  if (outlen == -1)
  {
    return outlen; // Extraction failed
  }

  std::string out_data = outstream.str();
  if ((size_t)outlen > out_size)
  {
    return -1; // Output buffer too small
  }

  memcpy(out, out_data.c_str(), outlen);
  return outlen;
}