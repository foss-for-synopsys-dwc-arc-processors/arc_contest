/*
	RW8711 programming define
*/

//Wi-Fi AT Command List

//SYSTEM
#define	AT_CMD	"AT\r\n"
//#define AT_CMD_SHOW_ALLCMD	"ATS?\r\n"
#define AT_CMD_RESTART_MODULE	"ATSR\r\n"
#define AT_CMD_SHOW_FW_VER	"ATSV\r\n"
#define AT_CMD_UART_ECHO_CTRL	"ATSE=%d\r\n"
//#define AT_CMD_UART_ECHO_CTRL	"ATSE="
#define AT_CMD_RESET_TO_FACTORY	"ATSY\r\n"

//UART Configureation
//ATSU=<baudrate>,<databits>,<stopbits>,<parity>,<flowcontrol>,<configmode>
#define AT_CMD_UART_CONFIG	"ATSU=%d,%d,%d,%d,%d,%d\r\n"
#define AT_CMD_SHOW_UART_CONFIG	"ATSU?\r\n"

//ATSO=<Download server IP>,<Download server port>
#define AT_CMD_OTA_UPGRADE	"ATSO=%s,%d\r\n"

//WLAN
#define AT_CMD_SET_WIFI_MODE	"ATPW=%d\r\n"
//#define AT_CMD_SET_WIFI_MODE	"ATPW="
#define AT_CMD_CON_TO_AP	"ATPN=%s,%s\r\n"
#define AT_CMD_CON_TO_AP	"ATPN=%s,%s\r\n"
#define AT_CMD_DIS_CON_FROM_AP	"ATPWD\r\n"
#define AT_CMD_SCAN_WIFI	"ATPWS\r\n"

//ATPA=<ssid>,<pwd>,<channel>,<hidden>
#define AT_CMD_SET_AP_MODE	"ATPA=\"%s\",%d,%d,%d\r\n"
//#define AT_CMD_SET_AP_MODE	"ATPA="
#define AT_CMD_SHOW_WIFI_INFO	"ATW?\r\n"

//ATPH=<mode>,<enable>
//<mode> : 0 for AP, 1 for Station
//<enable> : 1 for DHCP, 2 for Static IP
#define AT_CMD_SET_DHCP_MODE	"ATPH=%d,%d\r\n"

//ATPE=<ip_addr>,<gateway>,<mask>
#define AT_CMD_SET_STATIC_IP_STANTION	"ATPE=%s,%s,%s"

//ATPF=<start_ip>,<end_ip>,<gateway>
#define AT_CMD_SET_DHCP_RULE_AP	"ATPF=%s,%s,%s\r\n"

//TCPIP
//ATPS=0,<local port>
#define AT_CMD_OPEN_TCP_SERVER	"ATPS=0,%d\r\n"
//#define AT_CMD_OPEN_TCP_SERVER	"ATPS=0,"
//ATPC=0,<Remote IP>,<Remote Port>
#define AT_CMD_OPEN_TCP_CLIENT	"ATPC=0,%s,%d\r\n"
#define AT_CMD_CLOSE_SOCKET	"ATPD\r\n"

//ATPT=<data_size>,<id>:<data>
#define AT_CMD_SEND_DATA	"ATPT=%d,%d:%s\r\n"
//#define AT_CMD_SEND_DATA	"ATPT="
//ATPR=<Buffer Size>,<id>,<data>
#define AT_CMD_RECV_DATA	"ATPR=%d,%d,%s\r\n"

//ATPK=<enable>
#define AT_CMD_AUTO_RECV_DATA	"ATPK=%d\r\n"

#define AT_CMD_NETWORK_CON_STATE	"ATPI\r\n"

//ATPP=<IP>,<Count/Loop>
#define AT_CMDD_PINT_TEST	"ATPP=%s,%d"

#define WIFI_SSID	"8711_test"
#define WIFI_PASSWORD	12345678
#define SERVER_PORT	8000
#define TEST_STR	"test 8711 pass through\r\n"


typedef enum WIFI_AT_CMD_ECHO_CTRL_PARA
{
	WIFI_ECHO_DISABLE,
	WIFI_ECHO_ENABLE

}WIFI_AT_CMD_ECHO_CTRL;

typedef enum WIFI_AT_CMD_BAUDRATE_PARA
{
	WIFI_UART_BAUDRATE_9600 = 9600,
	WIFI_UART_BAUDRATE_115200 = 115200
	
}WIFI_AT_CMD_BAUDRATE;

typedef enum WIFI_AT_CMD_DATABITS_PARA
{
	WIFI_UART_DATABITS_5 = 5,
	WIFI_UART_DATABITS_6,
	WIFI_UART_DATABITS_7,
	WIFI_UART_DATABITS_8

}WIFI_AT_CMD_DATABITS;

typedef enum WIFI_AT_CMD_STOPBITS_PARA
{
	WIFI_UART_STOPBITS_1 = 1,
	WIFI_UART_STOPBITS_2

}WIFI_AT_CMD_STOPBITS;

typedef enum WIFI_AT_CMD_PARITY_PARA
{
	WIFI_UART_PARITY_NONE,
	WIFI_UART_PARITY_ODD,
	WIFI_UART_PARITY_Even

}WIFI_AT_CMD_PARITY;

typedef enum WIFI_AT_CMD_FLOW_CTRL_PARA
{
	WIFI_UART_FLOW_CTRL_DISABLE,
	WIFI_UART_FLOW_CTRL_ENABLE

}WIFI_AT_CMD_FLOW_CTRL;

/*
	Config mode
	mode 0 : set the current configuration and will not save to flash.
	mode 1 : save configuration to flash and take effect immediately.
	mode 2 : save configuration to flash and take effect after reboot.
*/
typedef enum WIFI_AT_CMD_CONFIG_MODE_PARA
{
	WIFI_UART_CONFIG_MODE_0,
	WIFI_UART_CONFIG_MODE_1,
	WIFI_UART_CONFIG_MODE_2
	

}WIFI_AT_CMD_CONFIG_MODE;


typedef enum WIFI_AT_CMD_CH_SET_PARA
{
	WIFI_CH_1=1,
	WIFI_CH_2,
	WIFI_CH_3,
	WIFI_CH_4,
	WIFI_CH_5,
	WIFI_CH_6,
	WIFI_CH_7,
	WIFI_CH_8, 
	WIFI_CH_9, 
	WIFI_CH_10, 
	WIFI_CH_11 

}WIFI_AT_CMD_CH_SET;

typedef enum WIFI_AT_CMD_WIFI_MODE_PARA
{
	WIFI_STATION_MODE=1,
	WIFI_AP_MODE

}WIFI_AT_CMD_WIFI_MODE;

typedef enum WIFI_AT_CMD_SET_HIDDEN_SSID_PARA
{
	WIFI_HIDDEN_SSID_DISABLE,
	WIFI_HIDDEN_SSID_ENABLE

}WIFI_AT_CMD_SET_HIDDEN_SSID;

