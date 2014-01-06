#define BUILD_NUMBER                        2
#define BUILD_DATE                          "12/02/2013"
#define COMPILER_TYPE       				RenesasRX
#define UEZ_PROCESSOR       				RENESAS_RX63N
#define PROCESSOR_OSCILLATOR_FREQUENCY      96000000
#define EXTAL_FREQUENCY        				12000000
#define ICLK_MUL            				(8)
#define PCLK_MUL            				(4)
#define BCLK_MUL            				(4)
#define ICLK_FREQUENCY    					EXTAL_FREQUENCY * ICLK_MUL
#define PCLK_FREQUENCY    					EXTAL_FREQUENCY * PCLK_MUL
#define BCLK_FREQUENCY    					EXTAL_FREQUENCY * BCLK_MUL
#define CARRIER_R2      					0 
#define CARRIER_R4      					1 
// Use up 48K of the heap
#define configTOTAL_HEAP_SIZE        		( ( size_t ) (( 48 * 1024 ) - 64))
#define configMINIMAL_STACK_SIZE    		( 140 )
#define FDI_PLATFORM        				CARRIER_R4 // set for CARRIER board R4 or R5
//#define FDI_PLATFORM        				CARRIER_R2 // set for CARRIER board R2
// select which LCD is used here
//#define UEZ_DEFAULT_LCD_CONFIG            LCD_CONFIG_TIANMA_TM043NBH02
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_TIANMA_TM047NBH01

#define UEZ_ENABLE_WATCHDOG                 0 // Turn on watchdog for testing
#define UEZ_USE_EXTERNAL_EEPROM   			0 
#define GUI_SUPPORT_CURSOR 					0

#if (FDI_PLATFORM == CARRIER_R4)
#define UEZ_ENABLE_WIRELESS_NETWORK     	0
#define UEZ_WIRELESS_PROGRAM_MODE           0
#define UEZ_ENABLE_AUDIO_CODEC          	1
#define UEZ_ENABLE_AUDIO_AMP            	1
#define UEZ_ENABLE_I2S_AUDIO            	0
#else
#define UEZ_ENABLE_WIRELESS_NETWORK     	0   //not avalible
#define UEZ_ENABLE_AUDIO_CODEC          	0   //not avalible
#define UEZ_ENABLE_AUDIO_AMP            	0   //not avalible
#define UEZ_ENABLE_I2S_AUDIO            	0   //not avalible
#endif

#define UEZ_ENABLE_USB_HOST_STACK   		0
#define UEZ_ENABLE_USB_DEVICE_STACK         0
#define UEZ_ENABLE_TCPIP_STACK      		1
#define UEZ_BASIC_WEB_SERVER        		UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_WIRED_NETWORK    		UEZ_ENABLE_TCPIP_STACK
#define UEZ_HTTP_SERVER             		0
#define INTERRUPT_BASED_EMAC        		0

//#define UEZ_ICONS_SET       				ICONS_SET_UEZ_OPEN_SOURCE
#define UEZ_ICONS_SET       				ICONS_SET_PROFESSIONAL_ICONS
//#define UEZ_ICONS_SET                     ICONS_SET_PROFESSIONAL_ICONS_LARGE
#define SIMPLEUI_DOUBLE_SIZED_ICONS         0 // 1 to 1 icons

// Modify the default accelerometer demo settings
#if 0 // Set to 1 for Sekio displays
#define ACCEL_DEMO_SWAP_XY        			0
#define ACCEL_DEMO_FLIP_X         			0
#define ACCEL_DEMO_G_TO_PIXELS    			32
#define ACCEL_DEMO_ALLOW_ROTATE   			0
#else
#define ACCEL_DEMO_SWAP_XY        			1
#define ACCEL_DEMO_FLIP_X         			1
#define ACCEL_DEMO_G_TO_PIXELS    			32
#define ACCEL_DEMO_ALLOW_ROTATE   			0
#endif

#define APP_DEMO_DRAW               		1
#define APP_DEMO_APPS               		1
#define APP_DEMO_COM                        1
#define APP_SLIDESHOW                		1

#define INCLUDE_EMWIN               		0
#define EMWIN_LOAD_ONCE             		1
#define APP_DEMO_EMWIN              		INCLUDE_EMWIN
#define SHOW_GUIDEMO_AUTOMOTIVE             0
#define SHOW_GUIDEMO_GRAPH                  1
#define SHOW_GUIDEMO_LISTVIE                1
#define SHOW_GUIDEMO_SPEED                  1
#define SHOW_GUIDEMO_TREEVIEW               0

#define UEZ_SLIDESHOW_NAME        			"DK-RX63N"
