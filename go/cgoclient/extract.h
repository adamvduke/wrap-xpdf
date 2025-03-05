#include <string>

/*
 * extractPdfText reads the input parameter as bytes of a pdf file and attempts
 * to extract any text contained in the pdf. The resulting text bytes are
 * written to the outstream. The return value indicates the number of bytes in
 * the output. If the return value equals -1, the extraction was not able to
 * successfully read the input data as a pdf.
*/
int extractPdfText(std::string input, std::stringstream *outstream);