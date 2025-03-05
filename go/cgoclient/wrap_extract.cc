#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cstring>

#include "extract.h"
#include "wrap_extract.h"

int wrap_extract(void *in, size_t len, void *out)
{
  char *pdf_data = (char *)in;
  std::string as_string(pdf_data, len);
  std::stringstream outstream;
  int outlen = extractPdfText(as_string, &outstream);
  if (outlen == -1)
  {
    return outlen;
  }

  std::string out_data = outstream.str();
  memcpy(out, out_data.c_str(), outlen);
  return outlen;
}