#ifndef _GEN_BULK_STREAM_H_
#define _GEN_BULK_STREAM_H_

#include "usb.h"

class GenBulkStreamPrivate;

class GenBulkStream {
    public:
        GenBulkStreamPrivate *iPriv;
    public:
        GenBulkStream();
        ~GenBulkStream();
        int Open(void);
        void Close(void);
        int ReadChar(void);
        void WriteChar(char c);
        int Read(char *aBuffer, unsigned int aMaxBytes);
        int Write(char *aBuffer, unsigned int aNumBytes);
        void Flush(void);
		int GetErrorCode(void);
};

#endif // _GEN_BULK_STREAM_H_
