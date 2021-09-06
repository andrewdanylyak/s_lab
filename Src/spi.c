static SPI_DATA_SETUP_T XferSetup;
uint16_t m_rx_buffer[256+12];

void SetupSPI(LPC_SPI_T *pSPI){
  SPI_CFG_T spiCfg;
  SPI_DELAY_CONFIG_T spiDelayCfg;
  Chip_SPI_Init(pSPI);
  spiCfg.ClkDiv = Chip_SPI_CalClkRateDivider(pSPI, 180000);
  spiCfg.ClockMode = SPI_CLOCK_MODE1;//SPI_CLOCK_MODE1;
  spiCfg.DataOrder = SPI_DATA_MSB_FIRST;
  spiCfg.Mode = SPI_MODE_MASTER;
  spiCfg.SSELPol = SPI_CFG_SPOL0_LO;
  Chip_SPI_SetConfig(pSPI, &spiCfg);
  spiDelayCfg.PreDelay = 10;
  spiDelayCfg.PostDelay = 10;
  spiDelayCfg.FrameDelay = 10;
  spiDelayCfg.TransferDelay = 10;
  Chip_SPI_DelayConfig(pSPI, &spiDelayCfg);
  Chip_SPI_Enable(pSPI);
}

bool MasterSpiMessage(LPC_SPI_T *pSPI, uint16_t *tx_buffer, uint32_t tx_length, uint16_t *rx_buffer){
  XferSetup.DataSize = 8;
  XferSetup.Length = tx_length;
  XferSetup.pTx = tx_buffer;
  XferSetup.pRx = rx_buffer;
  XferSetup.TxCnt = 0;
  XferSetup.RxCnt = 0;
  XferSetup.ssel = SPI_TXCTL_ASSERT_SSEL0;
  if(Chip_SPI_RWFrames_Blocking(pSPI, &XferSetup) > 0)
    return true;
  return false;
} 