#define BUILD_NUMBER                        2
#define BUILD_DATE                          "12/02/2013"
#define COMPILER_TYPE       				RenesasRX
#define UEZ_PROCESSOR       				RENESAS_RX62N
#define UEZ_DEFAULT_LCD_CONFIG    			LCD_CONFIG_TIANMA_QVGA_LCDC_3_5

// Use up 50K of the heap
#define configTOTAL_HEAP_SIZE        		( ( size_t ) (( 50 * 1024 ) - 64))
#define configMINIMAL_STACK_SIZE    		( 140 )

#define PROCESSOR_OSCILLATOR_FREQUENCY      96000000
#define EXTAL_FREQUENCY        				12000000
#define ICLK_MUL            				(8)
#define PCLK_MUL            				(4)
#define BCLK_MUL            				(4)

#define ICLK_FREQUENCY    					EXTAL_FREQUENCY * ICLK_MUL
#define PCLK_FREQUENCY    					EXTAL_FREQUENCY * PCLK_MUL
#define BCLK_FREQUENCY    					EXTAL_FREQUENCY * BCLK_MUL
#define UEZ_ENABLE_WATCHDOG                 0 // Turn on watchdog for testing
#define UEZ_USE_EXTERNAL_EEPROM   			0 // no External EEPROM on uEZGUI-RX62N-35QT Rev 2.1
#define GUI_SUPPORT_MEMDEV        			(1)

// Expansion Options
#define UEZGUI_EXP_BOARD                    UEZGUI_EXP_NONE
#define ENABLE_UEZ_BUTTON                   0

//#define UEZ_ICONS_SET       				ICONS_SET_UEZ_OPEN_SOURCE
#define UEZ_ICONS_SET       				ICONS_SET_PROFESSIONAL_ICONS
//#define UEZ_ICONS_SET 					ICONS_SET_PROFESSIONAL_ICONS_LARGE
#define SIMPLEUI_DOUBLE_SIZED_ICONS 		0 // 1 to 1 icons

#define INCLUDE_EMWIN               		1
#define EMWIN_LOAD_ONCE             		1
#define APP_DEMO_EMWIN              		INCLUDE_EMWIN 
#define APP_SLIDESHOW               		1
#define APP_DEMO_DRAW               		1
#define UEZ_ENABLE_TCPIP_STACK      		1
#define APP_DEMO_APPS               		0
#define APP_DEMO_COM                		0
#define UEZ_ENABLE_USB_HOST_STACK   		0
#define UEZ_ENABLE_USB_DEVICE_STACK 		0
#define INTERRUPT_BASED_EMAC        		0
#define UEZ_BASIC_WEB_SERVER        		0
#define UEZ_HTTP_SERVER             		UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_WIRED_NETWORK    		UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_WIRELESS_NETWORK 		0
#define UEZ_WIRELESS_PROGRAM_MODE   		0

#define UEZ_SLIDESHOW_NAME        "uEZGUI-RX62N-35QT"
