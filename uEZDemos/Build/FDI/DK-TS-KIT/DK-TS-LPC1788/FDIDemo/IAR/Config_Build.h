#define BUILD_NUMBER                            2
#define BUILD_DATE                              "12/02/2013"
#define UEZ_PROCESSOR       			NXP_LPC1788
#define PROCESSOR_OSCILLATOR_FREQUENCY          120000000 //96000000 //80000000
#define UEZ_ENABLE_WATCHDOG       	        0 // Turn on watchdog for testing
#define CARRIER_R2      					0 
#define CARRIER_R4      					1 
#define configTOTAL_HEAP_SIZE        		( ( size_t ) (( 48 * 1024 ) - 64)) // Use up 48K of the heap
#define FDI_PLATFORM        			CARRIER_R4 // set for CARRIER board R4 or R5
//#define FDI_PLATFORM        			CARRIER_R2 // set for CARRIER board R2

// select which LCD is used here
#define UEZ_DEFAULT_LCD_CONFIG 		LCD_CONFIG_OKAYA_VGA_LCDC_3_5          //DK-57VTS with LCD Carrier 3-5
//#define UEZ_DEFAULT_LCD_CONFIG 		LCD_CONFIG_TOSHIBA_QVGA_LCDC_REV2    //DK-57VTS with LCD Carrier 5.7
//#define UEZ_DEFAULT_LCD_CONFIG 		LCD_CONFIG_TOSHIBA_QVGA_LCDC_3_5     //DK-57TS with LCD Carrier 3-5
//#define UEZ_DEFAULT_LCD_CONFIG 		LCD_CONFIG_HITACHI_TX11D06VM2APA     //DK-43WQH with LCD Carrier 4WQ
//#define UEZ_DEFAULT_LCD_CONFIG 		LCD_CONFIG_SEIKO_43WQW1T
//#define UEZ_DEFAULT_LCD_CONFIG 		LCD_CONFIG_OKAYA_QVGA_LCDC_3_5       //DK-35TS with LCD Carrier 3-5 5.7"
//#define UEZ_DEFAULT_LCD_CONFIG 		LCD_CONFIG_OKAYA_QVGA_3x5_LCDC_3_5       //DK-35TS with LCD Carrier 3-5 Okaya RH320240T

#define USE_RESISTIVE_TOUCH                 1 // Set to 1 for resistive touch screen, 0 for PCAP. Affects some calibration/FCT code and startup routines.

#ifdef NDEBUG
#define UEZ_REGISTER              		0
#else
#define UEZ_REGISTER              		1  //Used for registering Queues and Semaphores in the RTOS
#endif

#if (FDI_PLATFORM == CARRIER_R4)
#define UEZ_ENABLE_WIRELESS_NETWORK     	0
#define UEZ_WIRELESS_PROGRAM_MODE               0
#define UEZ_ENABLE_AUDIO_CODEC          	1
#define UEZ_ENABLE_AUDIO_AMP            	1
#define UEZ_ENABLE_I2S_AUDIO            	0
#else
#define UEZ_ENABLE_WIRELESS_NETWORK     	0   //not avalible
#define UEZ_ENABLE_AUDIO_CODEC          	0   //not avalible
#define UEZ_ENABLE_AUDIO_AMP            	0   //not avalible
#define UEZ_ENABLE_I2S_AUDIO            	0   //not avalible
#endif

#define UEZ_ENABLE_USB_HOST_STACK   		1
#define UEZ_ENABLE_USB_DEVICE_STACK         1
#define UEZ_ENABLE_TCPIP_STACK      		0
#define UEZ_BASIC_WEB_SERVER        		UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_WIRED_NETWORK    		UEZ_ENABLE_TCPIP_STACK
#define UEZ_HTTP_SERVER             		0
#define INTERRUPT_BASED_EMAC        		1

#define DKTS_BUTTON_SLIDE_SHOW_DEMO        	0 // set to 1 to be able to load emwin or slideshows using buttons
//#define UEZ_ICONS_SET       			ICONS_SET_UEZ_OPEN_SOURCE
#define UEZ_ICONS_SET       			ICONS_SET_PROFESSIONAL_ICONS
//#define UEZ_ICONS_SET                         ICONS_SET_PROFESSIONAL_ICONS_LARGE
#define SIMPLEUI_DOUBLE_SIZED_ICONS         1 // 2 to 1 icons

// Modify the default accelerometer demo settings
#if 0 // Set to 1 for Sekio displays
#define ACCEL_DEMO_SWAP_XY        		0
#define ACCEL_DEMO_FLIP_X         		0
#define ACCEL_DEMO_G_TO_PIXELS    		32
#define ACCEL_DEMO_ALLOW_ROTATE   		0
#else
#define ACCEL_DEMO_SWAP_XY        		1
#define ACCEL_DEMO_FLIP_X         		1
#define ACCEL_DEMO_G_TO_PIXELS    		32
#define ACCEL_DEMO_ALLOW_ROTATE   		0
#endif

#define APP_DEMO_DRAW               		1
#define APP_DEMO_APPS               		1
#define APP_DEMO_SLIDESHOW             		1
#ifndef FREERTOS_PLUS_TRACE
#define APP_DEMO_COM                        1
#endif

#define INCLUDE_EMWIN               		1
#define APP_DEMO_EMWIN              		INCLUDE_EMWIN

// smaller demos
#define SHOW_GUIDEMO_SPEED                  0
#define SHOW_GUIDEMO_AATEXT                 1
#define SHOW_GUIDEMO_BARGRAPH               1
#define SHOW_GUIDEMO_COLORBAR               1
#define SHOW_GUIDEMO_CURSOR                 0

// large demos
#define SHOW_GUIDEMO_AUTOMOTIVE             0
#define SHOW_GUIDEMO_GRAPH                  0
#define SHOW_GUIDEMO_LISTVIEW               0
#define SHOW_GUIDEMO_TREEVIEW               0
#define SHOW_GUIDEMO_ICONVIEW               0
#define SHOW_GUIDEMO_TRANSPARENTDIALOG      0

#define UEZ_SLIDESHOW_NAME        		"DK-LPC1788"
