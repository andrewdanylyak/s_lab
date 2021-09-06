#ifndef _spi_h
#define _spi_h

extern uint16_t m_rx_buffer[256+12];
extern void SetupSPI(LPC_SPI_T *pSPI);
extern bool MasterSpiMessage(LPC_SPI_T *pSPI, uint16_t *tx_buffer, uint32_t tx_length, uint16_t *rx_buffer);

#endif