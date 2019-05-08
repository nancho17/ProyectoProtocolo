/*============================================================================
 * Licencia:
 * Autor: Hernan D. Ferreyra
 * Fecha:
 *===========================================================================*/
/*==================[inclusions]=============================================*/
#include "W5500.h"
#include "Socket.h"


/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/


/*==================[internal functions declaration]=========================*/


/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[external functions definition]==========================*/

/*==================[internal functions definition]==========================*/


uint8_t WREAD (uint32_t addr){

	 	  uint8_t ret;
	  	  uint8_t spi_data[3];
	  	  spi_data[0] = (addr & 0x00FF0000) >> 16;
	  	  spi_data[1] = (addr & 0x0000FF00) >> 8;
	  	  spi_data[2] = (addr & 0x000000FF) >> 0;

	  	  Chip_GPIO_SetPinState( LPC_GPIO_PORT, 3, 0, 0); ///GPIO0 Comienza
	  	  spiWrite( MAPSPI, spi_data, 3);
	  	  spiRead( MAPSPI, &ret, 1);
	  	  Chip_GPIO_SetPinState( LPC_GPIO_PORT, 3, 0, 1); ///GPIO0 Termina

	  	  return ret;


}

void WWRITE (uint32_t addr, uint8_t wb){

		  addr=addr|(0x0001 << 2);  //0000 0100  WRITE
	 	  uint8_t ret;
	  	  uint8_t spi_data[3];
	  	  spi_data[0] = (addr & 0x00FF0000) >> 16;
	  	  spi_data[1] = (addr & 0x0000FF00) >> 8;
	  	  spi_data[2] = (addr & 0x000000FF) >> 0;
	  	  spi_data[3] = wb;
	  	  Chip_GPIO_SetPinState( LPC_GPIO_PORT, 3, 0, 0); ///GPIO0 Comienza
	  	  spiWrite( MAPSPI, spi_data, 4);
	  	  Chip_GPIO_SetPinState( LPC_GPIO_PORT, 3, 0, 1); ///GPIO0 Termina


	  	  return;


}
void     WREAD_BUF (uint32_t AddrSel, uint8_t* pBuf, uint16_t len)
{
   uint8_t spi_data[3];
   uint16_t i;



		spi_data[0] = (AddrSel & 0x00FF0000) >> 16;
		spi_data[1] = (AddrSel & 0x0000FF00) >> 8;
		spi_data[2] = (AddrSel & 0x000000FF) >> 0;

		Chip_GPIO_SetPinState( LPC_GPIO_PORT, 3, 0, 0); ///GPIO0 Comienza
		spiWrite( MAPSPI, spi_data, 3);
		spiRead( MAPSPI, pBuf, len);
		Chip_GPIO_SetPinState( LPC_GPIO_PORT, 3, 0, 1); ///GPIO0 Termina
		return;
}
void     WWRITE_BUF (uint32_t AddrSel, uint8_t* pBuf, uint16_t len)
{
   uint8_t spi_data[3];
   uint16_t i;

   	   AddrSel=AddrSel|(0x0001 << 2);  //0000 0100  WRITE


		spi_data[0] = (AddrSel & 0x00FF0000) >> 16;
		spi_data[1] = (AddrSel & 0x0000FF00) >> 8;
		spi_data[2] = (AddrSel & 0x000000FF) >> 0;

		Chip_GPIO_SetPinState( LPC_GPIO_PORT, 3, 0, 0); ///GPIO0 Comienza
		spiWrite( MAPSPI, spi_data, 3);
		spiWrite( MAPSPI, pBuf, len);
		Chip_GPIO_SetPinState( LPC_GPIO_PORT, 3, 0, 1); ///GPIO0 Termina
		return;
}




uint16_t WgetSn_TX_FSR(uint8_t sn)
{
   uint16_t val=0,val1=0;

   do
   {
      val1 = WREAD(Sn_TX_FSR(sn));
      val1 = (val1 << 8) + WREAD(WIZCHIP_OFFSET_INC(Sn_TX_FSR(sn),1));
      if (val1 != 0)
      {
        val = WREAD(Sn_TX_FSR(sn));
        val = (val << 8) + WREAD(WIZCHIP_OFFSET_INC(Sn_TX_FSR(sn),1));
      }
   }while (val != val1);
   return val;
}


uint16_t WgetSn_RX_RSR(uint8_t sn)
{
   uint16_t val=0,val1=0;

   do
   {
      val1 = WREAD(Sn_RX_RSR(sn));
      val1 = (val1 << 8) + WREAD(WIZCHIP_OFFSET_INC(Sn_RX_RSR(sn),1));
      if (val1 != 0)
      {
        val = WREAD(Sn_RX_RSR(sn));
        val = (val << 8) + WREAD(WIZCHIP_OFFSET_INC(Sn_RX_RSR(sn),1));
      }
   }while (val != val1);
   return val;
}



void Wsend_data(uint8_t sn, uint8_t *wizdata, uint16_t len)
{
   uint16_t ptr = 0;
   uint32_t addrsel = 0;

   if(len == 0)  return;
   ptr = WgetSn_TX_WR(sn);
   //M20140501 : implict type casting -> explict type casting
   //addrsel = (ptr << 8) + (WIZCHIP_TXBUF_BLOCK(sn) << 3);
   addrsel = ((uint32_t)ptr << 8) + (WIZCHIP_TXBUF_BLOCK(sn) << 3);
   //
   WWRITE_BUF(addrsel,wizdata, len);

   ptr += len;
   WsetSn_TX_WR(sn,ptr);
}



void Wrecv_data(uint8_t sn, uint8_t *wizdata, uint16_t len)
{
   uint16_t ptr = 0;
   uint32_t addrsel = 0;

   if(len == 0) return;
   ptr = WgetSn_RX_RD(sn);
   //M20140501 : implict type casting -> explict type casting
   //addrsel = ((ptr << 8) + (WIZCHIP_RXBUF_BLOCK(sn) << 3);
   addrsel = ((uint32_t)ptr << 8) + (WIZCHIP_RXBUF_BLOCK(sn) << 3);
   //
   WREAD_BUF(addrsel, wizdata, len);
   ptr += len;

   WsetSn_RX_RD(sn,ptr);
}


void Wrecv_ignore(uint8_t sn, uint16_t len)
{
   uint16_t ptr = 0;

   ptr = WgetSn_RX_RD(sn);
   ptr += len;
   WsetSn_RX_RD(sn,ptr);
}


