#include "usb.h"
#include <stdio.h>

/* the device's vendor and product id */
#define MY_VID 0xFFFE
#define MY_PID 0x0001

/* the device's endpoints */
#define EP_IN 0x81
#define EP_OUT 0x02

#define BUF_SIZE 64

usb_dev_handle *open_dev(void);

usb_dev_handle *open_dev(void)
{
  struct usb_bus *bus;
  struct usb_device *dev;

  for(bus = usb_get_busses(); bus; bus = bus->next) 
    {
      for(dev = bus->devices; dev; dev = dev->next) 
        {
          if(dev->descriptor.idVendor == MY_VID
             && dev->descriptor.idProduct == MY_PID)
            {
              return usb_open(dev);
            }
        }
    }
  return NULL;
}

int32_t main(void)
{
  usb_dev_handle *dev = NULL; /* the device handle */
  char tmp[BUF_SIZE];
  int32_t size;
  int32_t i;
  int32_t count=0;

  usb_init(); /* initialize the library */
  usb_find_busses(); /* find all busses */
  usb_find_devices(); /* find all connected devices */


  if(!(dev = open_dev()))
    {
      printf("error: device not found!\n");
      return 0;
    }

  if(usb_set_configuration(dev, 1) < 0)
    {
      printf("error: setting config 1 failed\n");
      usb_close(dev);
      return 0;
    }

  if(usb_claim_interface(dev, 0) < 0)
    {
      printf("error: claiming interface 0 failed\n");
      usb_close(dev);
      return 0;
    }

        tmp[0] = 0x06;
        tmp[1] = 'P';
        tmp[2] = 'I';
        tmp[3] = 'N';
        tmp[4] = 'G';
        tmp[5] = '\r';
		tmp[6] = '\n';
      if(usb_bulk_write(dev, EP_OUT, tmp, sizeof(tmp), 5000) 
         != sizeof(tmp))
        {
          printf("error: bulk write failed\n");
        } else {
            // Send all characters we sent out to the screen
          size = tmp[0];
          for (i=1; i<=size; i++)
            printf(">%c", tmp[i]);
          printf("\n");
        }
    while (1) {
        tmp[0] = 0;
      if(usb_bulk_write(dev, EP_OUT, tmp, sizeof(tmp), 5000) 
         != sizeof(tmp))
        {
          printf("error: bulk write failed\n");
        }
      if((size = usb_bulk_read(dev, EP_IN, tmp, sizeof(tmp), 5000)) 
         != sizeof(tmp))
        {
          printf("error: bulk read failed (read: %d bytes)\n", size);
        } else {
            // Echo all received characters out to the screen
          size = tmp[0];
          for (i=1; i<=size; i++)
            printf("<%c", tmp[i]);
          if (size == 0) {
            printf("\r");
            count++;
          } else {
            //printf("\n count=%d", count);
			  printf("\n");
			  Sleep(10);
            count=0;
          }
        }
//        Sleep(100);
    }  
#if 0
  if(usb_bulk_write(dev, EP_OUT, tmp, sizeof(tmp), 5000) 
     != sizeof(tmp))
    {
      printf("error: bulk write failed\n");
    }

  if((size = usb_bulk_read(dev, EP_IN, tmp, sizeof(tmp), 5000)) 
     != sizeof(tmp))
    {
      printf("error: bulk read failed (read: %d bytes)\n", size);
    }
#endif

  usb_release_interface(dev, 0);
  usb_close(dev);

  return 0;
}
