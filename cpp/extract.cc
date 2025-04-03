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

static void outputToStream(void *stream, const char *text, int len)
{
    std::stringstream *out = (std::stringstream *)stream;
    out->write(text, len);
    out->flush();
}

int extractPdfText(std::string input, std::stringstream *outstream)
{
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