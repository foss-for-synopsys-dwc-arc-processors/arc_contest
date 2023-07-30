#ifndef NEO6M_H
#define NEO6M_H

#include <stdint.h>

#define NEO6M_BUF_SIZE 128

void neo6m_init(void);
void neo6m_read(uint8_t *buf, uint16_t *len);

#endif // NEO6M_H
