#include <string>
#include <sstream>
#include <iostream>
#include <mutex>

#include "GlobalParams.h"
#include "Object.h"
#include "PDFDoc.h"
#include "TextOutputDev.h"
#include "Stream.h"
#include "gtypes.h"

// Mutex to protect globalParams
static std::mutex globalParamsMutex;

// Define a max output size for safety
constexpr size_t MAX_OUTPUT_SIZE = 10 * 1024 * 1024; // 10 MB

static void outputToStream(void *stream, const char *text, int len)
{
    std::stringstream *out = (std::stringstream *)stream;
    if (len > MAX_OUTPUT_SIZE)
    {
        std::cerr << "Error: Output size exceeds maximum allowed limit." << std::endl;
        return;
    }

    out->write(text, len);
    out->flush();
}

int extractPdfText(std::string input, std::stringstream *outstream)
{
    if (input.empty() || outstream == nullptr)
    {
        std::cerr << "Error: Failed to extract text. Input is empty or null." << std::endl;
        return -1;
    }

    {
        std::lock_guard<std::mutex> lock(globalParamsMutex);
        if (globalParams == NULL)
        {
            globalParams = new GlobalParams(NULL);
            globalParams->setTextPageBreaks(gFalse);
        }
    }

    Object obj;
    obj.initNull();

    // PDFDoc takes over ownership and takes care of releasing the stream at destruction.
    std::unique_ptr<MemStream> dataStr = std::make_unique<MemStream>(input.data(), Guint(0), Guint(input.length()), &obj);
    PDFDoc doc(dataStr.release());

    /* probably not a pdf */
    if (!doc.isOk())
    {
        std::cerr << "Error: Failed to initialize PDFDoc. Input may not be a valid PDF." << std::endl;
        return -1;
    }

    int firstPage = 1;
    int lastPage = doc.getNumPages();
    double fixedPitch = 0;
    GBool clipText = gFalse;
    GBool discardDiag = gFalse;
    GBool insertBOM = gFalse;
    double marginLeft = 0;
    double marginRight = 0;
    double marginTop = 0;
    double marginBottom = 0;

    TextOutputControl textOutControl;
    textOutControl.mode = textOutTableLayout;
    textOutControl.fixedPitch = fixedPitch;
    textOutControl.clipText = clipText;
    textOutControl.discardDiagonalText = discardDiag;
    textOutControl.insertBOM = insertBOM;
    textOutControl.marginLeft = marginLeft;
    textOutControl.marginRight = marginRight;
    textOutControl.marginTop = marginTop;
    textOutControl.marginBottom = marginBottom;
    TextOutputDev textOut(&outputToStream, outstream, &textOutControl);
    if (textOut.isOk())
    {
        doc.displayPages(&textOut, firstPage, lastPage, 72, 72, 0, gFalse, gTrue, gFalse);
    }

    return outstream->str().length();
}