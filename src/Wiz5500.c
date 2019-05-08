/* Copyright 2016, Alejandro Permingeat.
 * Copyright 2016, Eric Pernia.
 * Copyright 2018, Sergio Renato De Jesus Melean <sergiordj@gmail.com>.
 *
 * All rights reserved.
 *
 * This file is part sAPI library for microcontrollers.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* Date: 2018-07-06 */

/*==================[inclusions]=============================================*/
#include "W5500.h" // <= W5500 lib
#include "Socket.h" // <= W5500 lib
#include "sapi.h" // <= W5500 lib


#define loca(N)       (1+(4*N))


/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
void diskTickHook( void *ptr );
int32_t loopback_tcpc(uint8_t sn, uint8_t* buf, uint8_t* destip, uint16_t destport);

/* FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE RESET. */
int main(void){
   /* ------------- INICIALIZACIONES ------------- */

	spiMap_t sPI=SPI0;// MSBFIRST

	boardConfig();
	spiConfig( sPI );//inicia el spi
	tickConfig( 1 );

	 delay_t showman;
	 delayInit( &showman, 200);

   // Inicializar
   printf("Inicializando...\r\n" );


   bool_t status = 1;

   if( status < 0 ){
      printf( "Coso no inicializado, chequee las conexiones:\r\n\r\n" );
      printf( "Al While Papu.\r\n" );
      while(1);
   }
   printf("Comm inicializada correctamente.\r\n\r\n" );

   /* ------------- INITS ------------- */
 	//	Direccion del registro + Bloque de control(blok select 7:3 +R/w 2+ OPmode 1:0)
	//	Contruimos el bloque
	//	WIZCHIP_CREG_BLOCK << 3 hayt que añadirle el read write R/w+ tipo de data
	//	0read /1write
	//	00 variable 01 1byte 10 2byte 11 4byte
			uint32_t addr=0;
			addr= (0x00000000    +  (0x002E<< 8)  + (0x00000 << 3)); //ASI CONTRUIMOS LA ADDRESS
	//(_W5500_IO_BASE_ + (0x0015 << 8) + (WIZCHIP_CREG_BLOCK << 3))


   W5500Linkstatus alfa=UNKNOWN;
   Chip_GPIO_SetPinState( LPC_GPIO_PORT, 3, 0, 1); ///GPIO0
   Chip_GPIO_SetPinState( LPC_GPIO_PORT, 3, 0, 1); ///GPIO0

   // WWRITE_BUF(SHAR, leMAC, 6);
   // WWRITE_BUF(SHAR, leIP, 6);


   // Source
   //seteamos mac   E1-91-0C-6C-54-D1
   //uint8_t leMAC[6]={0xE1,0x91,0x0C,0x6C,0x54,0xD1};	//Random
   //uint8_t leMAC[6]={0x18,0x31,0xbf,0x51,0x68,0x71};	//18:31:BF	ASUSTek COMPUTER INC
   uint8_t leMAC[6]={0x78,0xD6,0xB2,0x73,0x55,0x01}; 	//78:D6:B2	Toshiba

   WsetSHAR(leMAC);

   uint8_t leIP[4]={192, 168, 1, 207};
   WsetSIPR(leIP);

   uint8_t leSUB[4]={255,255,255,0};
   WsetSUBR(leSUB);

   uint8_t leGETA[4]={192,168,1,1};
   WsetGAR(leGETA);



   // Destination (TCP Server) IP info (will be connected)
   		// >> loopback_tcpc() function parameter
   		// >> Ex)
   		//	uint8_t destip[4] = 	{192, 168, 0, 214};
   		//	uint16_t destport = 	5000;
   		// Port number for TCP client (will be increased)
   		//static uint16_t any_port = 	50000;
   	   	 uint8_t ipdestino[4] = 	{192, 168, 1, 15};
   	   	 uint16_t portdestino = 	5831;
   	   	 uint8_t socketnumber=3;
   	   	 //uint8_t buffer[2048]={'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k'};
   	   	 uint8_t buffer[11]={'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k'};

   		 uint16_t moderegister=44;
   		 uint16_t leMTU=1444;//1460 es el default

		 uint8_t laMAC[6];//{0x00,0x00,0x00,0x00,0x00,0x00};
		 uint8_t laIP[4];//{0, 0, 0, 0};
		 uint8_t laSUB[4];//{0, 0, 0, 0};
		 uint8_t laGETA[4];//{0, 0, 0, 0};


		   uint16_t lectura[4];

		   int32_t RESPUESTA=123;

	   //WWRITE (MR,0x00);

		   	   WsetSn_MSSR(socketnumber, leMTU);




		   	   	   	   WgetSHAR(laMAC);
		 	   	   	   WgetSIPR(laIP);
		 	   	   	   WgetSUBR(laSUB);
		 	   	   	   WgetGAR(laGETA);

		 	   		   printf( "Modereg de socket N:\r\n %x\r\n",moderegister);
		 	   		   printf( "MAC de la BOARD:\r\n %x:%x:%x:%x:%x:%x   \r\n",laMAC[0],laMAC[1],laMAC[2],laMAC[3],laMAC[4],laMAC[5]);
		 	   		   printf( "IP de la BOARD: %d.%d.%d.%d   \r\n",laIP[0],laIP[1],laIP[2],laIP[3]);
		 	   		   printf( "SUBRED          %d.%d.%d.%d   \r\n",laSUB[0],laSUB[1],laSUB[2],laSUB[3]);
		 	   		   printf( "GETAWAY         %d.%d.%d.%d   \r\n",laGETA[0],laGETA[1],laGETA[2],laGETA[3]);
		  ///



	/* ------------- REPETIR POR SIEMPRE ------------- */
   while(TRUE){
	   Chip_GPIO_SetPinState( LPC_GPIO_PORT, 3, 0, 1);
	   delay(1000);



//	   moderegister=WgetMR();
//	   lectura[0]= WREAD (addr);
	   lectura[0]= WgetPHYCFGR();

	  // moderegister=WgetSn_MR(socketnumber);
	 //  printf( "Modereg de socket N:     %x\r\n",moderegister);
	   moderegister=WgetSn_PORT(socketnumber);
	   printf( "Puerto Origen:     %d\r\n",moderegister);
	   moderegister=WgetSn_DPORT(socketnumber);
	   printf( "Puerto Destino:     %d\r\n",moderegister);
	 //  moderegister= WgetSn_MSSR(socketnumber);
	//   printf( "MTU:     %d\r\n",moderegister);



	   if(lectura[0]&0x01){
		   printf( "LINK_ON\r\n");



		   RESPUESTA=loopback_tcpc(socketnumber, buffer, ipdestino, portdestino);
//		   disconnect(socketnumber);
		   printf( "Bandera de ERRROR : %d  \r\n",RESPUESTA);
		   printf( "Buffer :   %c,  %c,  %c,  %c    \r\n \r\n \r\n", buffer[0], buffer[1],buffer[2],buffer[3]);



	   }
	   else{
		   printf( "LINK_OFF/UNKNOWN \r\n");

	   };


//	   kha[0]=  WgetSn_TX_FSR(0);
//	   kha[1]=  WgetSn_TX_FSR(1);
//	   kha[2]=  WgetSn_TX_FSR(2);
//	   kha[3]=  WgetSn_TX_FSR(3);

//		   printf( "REcibido:     Lectura0 (    %x) ; Lectura1 (    %x)  %x  %x   esto sirve?\r\n",
	//			   lectura[1],
		//		   lectura[2],
			//	   lectura[3],
				//   kha


		   		  //    	  	  );
//		   	   	   cmd[0]= 	lectura[0]&0x01;
//				   cmd[1]=	(lectura[0]>> 1)&0x01;
//				   cmd[2]=	(lectura[0]>> 2)&0x01;
//				   cmd[3]=	(lectura[0]>> 3)&0x01;
//				   cmd[4]=	(lectura[0]>> 4)&0x01;
//				   cmd[5]=	(lectura[0]>> 5)&0x01;
//				   cmd[6]=	(lectura[0]>> 6)&0x01;
//				   cmd[7]=	(lectura[0]>> 7)&0x01;


//		   printf( "      (%x, %x,         %x,%x,%x,      %x, %x,%x, )  \r\n",
//				   cmd[7],
//				   cmd[6],
//				   cmd[5],
//				   cmd[4],
//				   cmd[3],
//				   cmd[2],
//				   cmd[1],
//				   cmd[0]
//
//		   		      	  	  );

   }




   /* NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa no es llamado
      por ningun S.O. */
   return 0 ;
}






////////CLIENT APP////////////
   //////////////////////////////////////////////////////////////////////////

   int32_t loopback_tcpc(uint8_t sn, uint8_t* buf, uint8_t* destip, uint16_t destport)
   {
      int32_t ret; // return value for SOCK_ERRORs
      uint16_t size = 4, sentsize=0;//0,0
      bool_t Fstenv=1;

      // Destination (TCP Server) IP info (will be connected)
      // >> loopback_tcpc() function parameter
      // >> Ex)
      //	uint8_t destip[4] = 	{192, 168, 0, 214};
      //	uint16_t destport = 	5000;

      // Port number for TCP client (will be increased)
      static uint16_t any_port = 	49152;

      // Socket Status Transitions
      // Check the W5500 Socket n status register (Sn_SR, The 'Sn_SR' controlled by Sn_CR command or Packet send/recv status)
      switch(WgetSn_SR(sn))
      {
         case SOCK_ESTABLISHED :
        	 printf( "SOCK_ESTABLISHED\r\n");
        	 printf("%d:Connected to - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
        	 if(Fstenv){
        		 send( sn, buf, size);
        		 Fstenv=0;
        	 }////

            if(WgetSn_IR(sn) & Sn_IR_CON)	// Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
            {

   			WsetSn_IR(sn, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
            }

            //////////////////////////////////////////////////////////////////////////////////////////////
            // Data Transaction Parts; Handle the [data receive and send] process
            //////////////////////////////////////////////////////////////////////////////////////////////
   		 if((size = WgetSn_RX_RSR(sn)) > 0) // Sn_RX_RSR: Socket n Received Size Register, Receiving data length
            {
   			if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE; // DATA_BUF_SIZE means user defined buffer size (array)
   			ret = recv(sn, buf, size); // Data Receive process (H/W Rx socket buffer -> User's buffer)

   			if(ret <= 0) return ret; // If the received data length <= 0, receive failed and process end
   			size = (uint16_t) ret;
   			sentsize = 0;

   			// Data sentsize control
   			while(size != sentsize)
   			{
   				ret = send(sn, buf+sentsize, size-sentsize); // Data send process (User's buffer -> Destination through H/W Tx socket buffer)
   				if(ret < 0) // Send Error occurred (sent data length < 0)
   				{
   					close(sn); // socket close
   					return ret;
   				}
   				sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
   			}
            }
   		 //////////////////////////////////////////////////////////////////////////////////////////////
            break;

         case SOCK_CLOSE_WAIT :
        	 printf( "SOCK_CLOSE_WAIT\r\n");
            if((ret=disconnect(sn)) != SOCK_OK) return ret;

            break;

         case SOCK_INIT :
        	 printf( "SOCK_INIT\r\n");
       	 if( (ret = connect(sn, destip, destport)) != SOCK_OK) return ret;	//	Try to TCP connect to the TCP server (destination)
            break;

         case SOCK_CLOSED:
        	 printf( "SOCK_CLOSED\r\n");
       	  close(sn);
 //      	  if((ret=socket(sn, Sn_MR_TCP, any_port++, 0x00)) != sn){
      	  if((ret=socket(sn, Sn_MR_TCP, any_port++, 0x00)) != sn){

            if(any_port == 0xffff) any_port = 50000;
            return ret; // TCP socket open with 'any_port' port number
           }

            break;
         default:printf( "DEFAULT\r\n");
            break;
      }
      return 1;
   }

   //////////////////////////////////////////////////////////////////////////

/*==================[end of file]============================================*/
