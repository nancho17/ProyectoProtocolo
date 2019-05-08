/*============================================================================
 * Licencia:
 * Autor:
 * Fecha:
 *===========================================================================*/

#ifndef _SOCKET_H_
#define _SOCKET_H_

/*==================[inclusiones]============================================*/
#include "W5500.h"
/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[Definiciones]=================================================*/
//#define SOCKET                uint8_t  ///< SOCKET type define for legacy driver

#define SOCK_OK               1        ///< Result is OK about socket process.
#define SOCK_BUSY             0        ///< Socket is busy on processing the operation. Valid only Non-block IO Mode.
#define SOCK_FATAL            -1000    ///< Result is fatal error about socket process.

#define SOCK_ERROR            0
#define SOCKERR_SOCKNUM       (SOCK_ERROR - 1)     ///< Invalid socket number
#define SOCKERR_SOCKOPT       (SOCK_ERROR - 2)     ///< Invalid socket option
#define SOCKERR_SOCKINIT      (SOCK_ERROR - 3)     ///< Socket is not initialized or SIPR is Zero IP address when Sn_MR_TCP
#define SOCKERR_SOCKCLOSED    (SOCK_ERROR - 4)     ///< Socket unexpectedly closed.
#define SOCKERR_SOCKMODE      (SOCK_ERROR - 5)     ///< Invalid socket mode for socket operation.
#define SOCKERR_SOCKFLAG      (SOCK_ERROR - 6)     ///< Invalid socket flag
#define SOCKERR_SOCKSTATUS    (SOCK_ERROR - 7)     ///< Invalid socket status for socket operation.
#define SOCKERR_ARG           (SOCK_ERROR - 10)    ///< Invalid argument.
#define SOCKERR_PORTZERO      (SOCK_ERROR - 11)    ///< Port number is zero
#define SOCKERR_IPINVALID     (SOCK_ERROR - 12)    ///< Invalid IP address
#define SOCKERR_TIMEOUT       (SOCK_ERROR - 13)    ///< Timeout occurred
#define SOCKERR_DATALEN       (SOCK_ERROR - 14)    ///< Data length is zero or greater than buffer max size.
#define SOCKERR_BUFFER        (SOCK_ERROR - 15)    ///< Socket buffer is not enough for data communication.

#define SOCKFATAL_PACKLEN     (SOCK_FATAL - 1)     ///< Invalid packet length. Fatal Error.

//////////////
#define SF_TCP_NODELAY         (Sn_MR_ND)          ///< In @ref Sn_MR_TCP, Use to nodelayed ack.

#define SF_IGMP_VER2           (Sn_MR_MC)          ///< In @ref Sn_MR_UDP with \ref SF_MULTI_ENABLE, Select IGMP version 2.

#define SF_IO_NONBLOCK           0x01              ///< Socket nonblock io mode. It used parameter in \ref socket().

#define SF_MULTI_ENABLE        (Sn_MR_MULTI)       ///< In @ref Sn_MR_UDP, Enable multicast mode.

#define PACK_COMPLETED           0x00              ///< In Non-TCP packet, It indicates to complete to receive a packet. (When W5300, This flag can be applied)

#define SF_UNI_BLOCK           (Sn_MR_UCASTB)   ///< In @ref Sn_MR_UDP with \ref SF_MULTI_ENABLE. Valid only in W5500

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones]====================*/

int32_t recv(uint8_t sn, uint8_t * buf, uint16_t len);

int8_t close(uint8_t sn);

int32_t send(uint8_t sn, uint8_t * buf, uint16_t len);

int8_t disconnect(uint8_t sn);

int8_t connect(uint8_t sn, uint8_t * addr, uint16_t port);

int8_t socket(uint8_t sn, uint8_t protocol, uint16_t port, uint8_t flag);

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _SOCKET_H_ */
