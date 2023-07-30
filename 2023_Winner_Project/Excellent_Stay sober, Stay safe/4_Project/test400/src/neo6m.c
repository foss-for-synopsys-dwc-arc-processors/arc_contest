#include "neo6m.h"
#include "SC16IS750_Bluepacket.h" // Include the SC16IS750 library, replace with your UART driver library

uint8_t gps_data_buf[NEO6M_BUF_SIZE];  // Buffer to store data from GPS module

void neo6m_init(void) {

}

void neo6m_read(uint8_t *buf, uint16_t *len) {
    // Clear the buffer
    memset(gps_data_buf, '\0', NEO6M_BUF_SIZE);

    // Read the data from UART
    UART_ReadBytes(SC16IS750_PROTOCOL_SPI);

    printf("GPS data buffer after reading from UART: %s\n", gps_data_buf);

    // Copy the data to the provided buffer
    memcpy(buf, gps_data_buf, NEO6M_BUF_SIZE);

    // Return the length of the data
    *len = strlen((char *)gps_data_buf);

    printf("Length of GPS data: %d\n", *len);
}

void print_gps_data(void) {
    uint16_t len;
    uint8_t buf[NEO6M_BUF_SIZE];

    neo6m_read(buf, &len);

    printf("GPS Data:\n%s\n", buf);
}
