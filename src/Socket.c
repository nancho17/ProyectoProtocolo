/*============================================================================
 * Licencia:
 * Autor: Hernan D. Ferreyra
 * Fecha:
 *===========================================================================*/
/*==================[inclusions]=============================================*/
#include "Socket.h"

/*==================[macros and definitions]=================================*/
#define SOCK_ANY_PORT_NUM  0xC000

#define CHECK_SOCKNUM()   \
   do{                    \
      if(sn > _WIZCHIP_SOCK_NUM_) return SOCKERR_SOCKNUM;   \
   }while(0);             \

#define CHECK_SOCKMODE(mode)  \
   do{                     \
      if((WgetSn_MR(sn) & 0x0F) != mode) return SOCKERR_SOCKMODE;  \
   }while(0);              \

#define CHECK_SOCKINIT()   \
   do{                     \
      if((WgetSn_SR(sn) != SOCK_INIT)) return SOCKERR_SOCKINIT; \
   }while(0);              \

#define CHECK_SOCKDATA()   \
   do{                     \
      if(len == 0) return SOCKERR_DATALEN;   \
   }while(0);              \

/*==================[internal data declaration]==============================*/
static uint16_t sock_io_mode = 0;

static uint16_t sock_is_sending = 0;

static uint16_t sock_remained_size[_WIZCHIP_SOCK_NUM_] = {0,0,};


uint8_t  sock_pack_info[_WIZCHIP_SOCK_NUM_] = {0,};


static uint16_t sock_any_port = SOCK_ANY_PORT_NUM;
/*==================[internal functions declaration]=========================*/


/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[external functions definition]==========================*/

/*==================[internal functions definition]==========================*/

int32_t recv(uint8_t sn, uint8_t * buf, uint16_t len)
{
   uint8_t  tmp = 0;
   uint16_t recvsize = 0;

   CHECK_SOCKNUM();
   CHECK_SOCKMODE(Sn_MR_TCP);
   CHECK_SOCKDATA();

   recvsize = WgetSn_RxMAX(sn);
   if(recvsize < len) len = recvsize;



      while(1)
      {
         recvsize = WgetSn_RX_RSR(sn);
         tmp = WgetSn_SR(sn);
         if (tmp != SOCK_ESTABLISHED)
         {
            if(tmp == SOCK_CLOSE_WAIT)
            {
               if(recvsize != 0) break;
               else if(WgetSn_TX_FSR(sn) == WgetSn_TxMAX(sn))
               {
                  close(sn);
                  return SOCKERR_SOCKSTATUS;
               }
            }
            else
            {
               close(sn);
               return SOCKERR_SOCKSTATUS;
            }
         }
         if((sock_io_mode & (1<<sn)) && (recvsize == 0)) return SOCK_BUSY;
         if(recvsize != 0) break;
      };

   if(recvsize < len) len = recvsize;
   Wrecv_data(sn, buf, len);
   WsetSn_CR(sn,Sn_CR_RECV);
   while(WgetSn_CR(sn));


   //M20150409 : Explicit Type Casting
   //return len;
   return (int32_t)len;
}

int8_t close(uint8_t sn)
{
	CHECK_SOCKNUM();
//A20160426 : Applied the erratum 1 of W5300


	WsetSn_CR(sn,Sn_CR_CLOSE);
   /* wait to process the command... */
	while( WgetSn_CR(sn) );
	/* clear all interrupt of the socket. */
	WsetSn_IR(sn, 0xFF);
	//A20150401 : Release the sock_io_mode of socket n.
	sock_io_mode &= ~(1<<sn);
	//
	sock_is_sending &= ~(1<<sn);
	sock_remained_size[sn] = 0;
	sock_pack_info[sn] = 0;
	while(WgetSn_SR(sn) != SOCK_CLOSED);
	return SOCK_OK;
}


int32_t send(uint8_t sn, uint8_t * buf, uint16_t len)
{
   uint8_t tmp=0;
   uint16_t freesize=0;

   CHECK_SOCKNUM();
   CHECK_SOCKMODE(Sn_MR_TCP);
   CHECK_SOCKDATA();
   tmp = WgetSn_SR(sn);
   if(tmp != SOCK_ESTABLISHED && tmp != SOCK_CLOSE_WAIT) return SOCKERR_SOCKSTATUS;
   if( sock_is_sending & (1<<sn) )
   {
      tmp = WgetSn_IR(sn);
      if(tmp & Sn_IR_SENDOK)
      {
         WsetSn_IR(sn, Sn_IR_SENDOK);

         sock_is_sending &= ~(1<<sn);
      }
      else if(tmp & Sn_IR_TIMEOUT)
      {
         close(sn);
         return SOCKERR_TIMEOUT;
      }
      else return SOCK_BUSY;
   }
   freesize = WgetSn_TxMAX(sn);
   if (len > freesize) len = freesize; // check size not to exceed MAX size.
   while(1)
   {
      freesize = WgetSn_TX_FSR(sn);
      tmp = WgetSn_SR(sn);
      if ((tmp != SOCK_ESTABLISHED) && (tmp != SOCK_CLOSE_WAIT))
      {
         close(sn);
         return SOCKERR_SOCKSTATUS;
      }
      if( (sock_io_mode & (1<<sn)) && (len > freesize) ) return SOCK_BUSY;
      if(len <= freesize) break;
   }
   Wsend_data(sn, buf, len);




   WsetSn_CR(sn,Sn_CR_SEND);
   /* wait to process the command... */
   while(WgetSn_CR(sn));
   sock_is_sending |= (1 << sn);
   //M20150409 : Explicit Type Casting
   //return len;
   return (int32_t)len;
}

int8_t disconnect(uint8_t sn)
{
   CHECK_SOCKNUM();
   CHECK_SOCKMODE(Sn_MR_TCP);
	WsetSn_CR(sn,Sn_CR_DISCON);
	/* wait to process the command... */
	while(WgetSn_CR(sn));
	sock_is_sending &= ~(1<<sn);
   if(sock_io_mode & (1<<sn)) return SOCK_BUSY;
	while(WgetSn_SR(sn) != SOCK_CLOSED)
	{
	   if(WgetSn_IR(sn) & Sn_IR_TIMEOUT)
	   {
	      close(sn);
	      return SOCKERR_TIMEOUT;
	   }
	}
	return SOCK_OK;
}



int8_t connect(uint8_t sn, uint8_t * addr, uint16_t port)
{
   CHECK_SOCKNUM();
   CHECK_SOCKMODE(Sn_MR_TCP);
   CHECK_SOCKINIT();
   //M20140501 : For avoiding fatal error on memory align mismatched
   //if( *((uint32_t*)addr) == 0xFFFFFFFF || *((uint32_t*)addr) == 0) return SOCKERR_IPINVALID;
// 	 {
//     	uint32_t taddr;
//  	taddr = ((uint32_t)addr[0] & 0x000000FF);
//    	taddr = (taddr << 8) + ((uint32_t)addr[1] & 0x000000FF);
//     	taddr = (taddr << 8) + ((uint32_t)addr[2] & 0x000000FF);
//     	taddr = (taddr << 8) + ((uint32_t)addr[3] & 0x000000FF);
//    	if( taddr == 0xFFFFFFFF || taddr == 0) return SOCKERR_IPINVALID;
//   }
//

	if(port == 0) return SOCKERR_PORTZERO;
	WsetSn_DIPR(sn,addr);
	WsetSn_DPORT(sn,port);
	WsetSn_CR(sn,Sn_CR_CONNECT);
   while(WgetSn_CR(sn));
   if(sock_io_mode & (1<<sn)) return SOCK_BUSY;
   while(WgetSn_SR(sn) != SOCK_ESTABLISHED)
   {
		if (WgetSn_IR(sn) & Sn_IR_TIMEOUT)
		{
			WsetSn_IR(sn, Sn_IR_TIMEOUT);
            return SOCKERR_TIMEOUT;
		}

		if (WgetSn_SR(sn) == SOCK_CLOSED)
		{
			return SOCKERR_SOCKCLOSED;
		}
	}

   return SOCK_OK;
}




int8_t socket(uint8_t sn, uint8_t protocol, uint16_t port, uint8_t flag)
{
	CHECK_SOCKNUM();
	switch(protocol)
	{
      case Sn_MR_TCP :
         {
            //M20150601 : Fixed the warning - taddr will never be NULL
		    /*
            uint8_t taddr[4];
            getSIPR(taddr);
            */
            uint32_t taddr;
            WgetSIPR((uint8_t*)&taddr);
            if(taddr == 0) return SOCKERR_SOCKINIT;
         }
      case Sn_MR_UDP :
      case Sn_MR_MACRAW :
	  case Sn_MR_IPRAW :
         break;
      default :
         return SOCKERR_SOCKMODE;
	}
	//M20150601 : For SF_TCP_ALIGN & W5300
	//if((flag & 0x06) != 0) return SOCKERR_SOCKFLAG;
	if((flag & 0x04) != 0) return SOCKERR_SOCKFLAG;

	if(flag != 0)
	{
   	switch(protocol)
   	{
   	   case Sn_MR_TCP:

   		     if((flag & (SF_TCP_NODELAY|SF_IO_NONBLOCK))==0) return SOCKERR_SOCKFLAG;


   	      break;
   	   case Sn_MR_UDP:
   	      if(flag & SF_IGMP_VER2)
   	      {
   	         if((flag & SF_MULTI_ENABLE)==0) return SOCKERR_SOCKFLAG;
   	      }

      	      if(flag & SF_UNI_BLOCK)
      	      {
      	         if((flag & SF_MULTI_ENABLE) == 0) return SOCKERR_SOCKFLAG;
      	      }

   	      break;
   	   default:
   	      break;
   	}
   }
	close(sn);
	//M20150601

   WsetSn_MR(sn, (protocol | (flag & 0xF0)));

	if(!port)
	{
	   port = sock_any_port++;
	   if(sock_any_port == 0xFFF0) sock_any_port = SOCK_ANY_PORT_NUM;
	}
   WsetSn_PORT(sn,port);
   WsetSn_CR(sn,Sn_CR_OPEN);
   while(WgetSn_CR(sn));
   //A20150401 : For release the previous sock_io_mode
   sock_io_mode &= ~(1 <<sn);
   //
	sock_io_mode |= ((flag & SF_IO_NONBLOCK) << sn);
   sock_is_sending &= ~(1<<sn);
   sock_remained_size[sn] = 0;
   //M20150601 : repalce 0 with PACK_COMPLETED
   //sock_pack_info[sn] = 0;
   sock_pack_info[sn] = PACK_COMPLETED;
   //
   while(WgetSn_SR(sn) == SOCK_CLOSED);
   return (int8_t)sn;
}








