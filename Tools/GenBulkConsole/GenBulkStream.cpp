#include "GenBulkStream.h"

/* the device's endpoints */
#define EP_IN 0x81
#define EP_OUT 0x02
#define BUF_SIZE 64

#define FDI_VID 0x2416
#define FDI_SERIAL_BULK_PRODUCT_ID 0x0001
#define MAX_FIFO_SIZE                   16384

class SimpleFIFO {
    public:
        char iFIFO[MAX_FIFO_SIZE];
        int iHead;
        int iTail;
    private:
        int Next(int pos) {
            return (pos+1)%MAX_FIFO_SIZE;
        }
    public:
        SimpleFIFO() {
            iHead = iTail = 0;
        }
        int Count(void) {
            if (iHead >= iTail)
                return iHead - iTail;
            return (MAX_FIFO_SIZE+iHead)-iTail;
        }
        bool IsEmpty() {
            return (iHead == iTail);
        }
        bool IsFull() {
            return (Next(iTail) == iHead);
        }
        void In(char c) {
            if (!IsFull()) {
                iFIFO[iTail] = c;
                iTail = Next(iTail);
            }
        }
        int Out(void) {
            if (IsEmpty())
                return -1;
            else {
                char c = iFIFO[iHead];
                iHead = Next(iHead);
                return c;
            }
        }
};
class GenBulkStreamPrivate {
    public:
        bool iStarted;
        bool iOpened;
		int iError;
        usb_dev_handle *iDev;
        SimpleFIFO iFIFOIn;
        SimpleFIFO iFIFOOut;
    public:
        GenBulkStreamPrivate();
        ~GenBulkStreamPrivate();
        void EnsureStarted(void);
        usb_dev_handle *OpenDevice(void);
        int ReadChar(void);
        void WriteChar(char c);
        void UpdateFIFOIn(void);
        void Flush(void);
};

GenBulkStreamPrivate::GenBulkStreamPrivate()
{
    iStarted = false;
	iOpened = false;
    iDev = 0;
	iError = 0;
}

GenBulkStreamPrivate::~GenBulkStreamPrivate()
{
    // close out anything open
    if (iDev) {
        usb_release_interface(iDev, 0);
        usb_close(iDev);
        iDev = 0;
    }
}

usb_dev_handle *GenBulkStreamPrivate::OpenDevice(void)
{
    struct usb_bus *bus;
    struct usb_device *dev;

    for (bus = usb_get_busses(); bus; bus = bus->next)  {
        for (dev = bus->devices; dev; dev = dev->next)  {
            if (dev->descriptor.idVendor == FDI_VID
               && dev->descriptor.idProduct == FDI_SERIAL_BULK_PRODUCT_ID) {
                  iDev = usb_open(dev);
                  return iDev;
            }
        }
    }
    return NULL;
}

static char *G_error;

void GenBulkStreamPrivate::UpdateFIFOIn(void)
{
    char buffer[BUF_SIZE];
    int size;
    int i;

    // Read in a some data and put it into the In FIFO
	Flush();
    size = usb_bulk_read(iDev, EP_IN, buffer, sizeof(buffer), 500000);
	if (size == -116) { //timeout
		size = 0;
	}
    if (size != BUF_SIZE) {
		if (size < 0)
			iError = size;
		G_error = usb_strerror();
        return;
	}
    size = buffer[0];

    // Feed the data into the FIFO
    for (i=1; i<=size; i++)
        iFIFOIn.In(buffer[i]);
}

int GenBulkStreamPrivate::ReadChar(void)
{
    // Are we empty? Fetch more characters and put in then
    if (iFIFOIn.IsEmpty())
        UpdateFIFOIn();
    // Pull out a character
    return iFIFOIn.Out();
}

void GenBulkStreamPrivate::WriteChar(char c)
{
    // Do we have room?  If not, flush out what we got
    if (iFIFOOut.Count() >= (BUF_SIZE-1))
        Flush();

    // Put data into the output buffer
    iFIFOOut.In(c);
}

void GenBulkStreamPrivate::Flush(void)
{
    int i;
    char buffer[BUF_SIZE];
    char c;
    bool error = 0;

    // Write out enough characters to make a part or a full block
    for (i=1; i<BUF_SIZE; i++) {
        c = iFIFOOut.Out();
        if (c == -1)
            break;
		buffer[i] = c;
    }
    buffer[0] = i-1;
    usb_bulk_write(iDev, EP_OUT, buffer, sizeof(buffer), 500000);
}

void GenBulkStreamPrivate::EnsureStarted(void)
{
    if (!iStarted) {
        usb_init(); /* initialize the library */
        usb_find_busses(); /* find all busses */
        usb_find_devices(); /* find all connected devices */
        iStarted = true;
    }
}

GenBulkStream::GenBulkStream()
{
    iPriv = new GenBulkStreamPrivate();
}
GenBulkStream::~GenBulkStream()
{
    Close();
    delete iPriv;
}

int GenBulkStream::Open(void)
{
    iPriv->EnsureStarted();
    if (!iPriv->iOpened) {
		iPriv->iError = 0;
        if (!(iPriv->iDev = iPriv->OpenDevice()))  {
            // Could not open it
            return false;
        }
        if (usb_set_configuration(iPriv->iDev, 1) < 0)  {
            usb_close(iPriv->iDev);
            return false;
        }
        if (usb_claim_interface(iPriv->iDev, 0) < 0)  {
            usb_close(iPriv->iDev);
            return false;
        }
        iPriv->iOpened = true;
    }
    return true;
}

void GenBulkStream::Close(void)
{
    if (iPriv->iOpened) {
        usb_close(iPriv->iDev);
        iPriv->iOpened = false;
        iPriv->iDev = 0;
    }
}

int GenBulkStream::ReadChar(void)
{
    return iPriv->ReadChar();
}

void GenBulkStream::WriteChar(char c)
{
    iPriv->WriteChar(c);
}

int GenBulkStream::Read(char *aBuffer, unsigned int aMaxBytes)
{
    char c;
    unsigned int i;

    for (i=0; i<aMaxBytes; i++) {
        c = ReadChar();
        if (c == -1)
            break;
        *(aBuffer++) = c;
    }
    return i;
}

int GenBulkStream::Write(char *aBuffer, unsigned int aNumBytes)
{
    unsigned int i;

    for (i=0; i<aNumBytes; i++)
        WriteChar(*(aBuffer++));

    return i;
}

void GenBulkStream::Flush(void)
{
    iPriv->Flush();
}

int GenBulkStream::GetErrorCode(void)
{
	return iPriv->iError;
}
