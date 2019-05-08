/*============================================================================
 * Licencia:
 * Autor: Hernan D. Ferreyra
 * Fecha:
 *===========================================================================*/

#ifndef _W5500_H_
#define _W5500_H_

/*==================[inclusiones]============================================*/
#include "sapi.h"
/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif
/*==================[constantes]=================================================*/
#define MAPSPI	SPI0
#define MAX_SOCK_NUM 8  //8 sockets (n<8)
#define DATA_BUF_SIZE			2048

#define _W5500_IO_BASE_         0x00000000  ///Para Bloques de datos variables
//#define _W5500_IO_BASE_			0x00000001  //Para Bloques de 1 bytes
//#define _W5500_IO_BASE_			0x00000010  //Para Bloques de 2 bytes
//#define _W5500_IO_BASE_			0x00000011  //Para Bloques de 4 bytes


#define WIZCHIP_CREG_BLOCK          0x00				//< Common register block
#define WIZCHIP_SREG_BLOCK(N)       (1+(4*N))			//< Socket N register block
#define WIZCHIP_TXBUF_BLOCK(N)      (2+(4*N))			//< Socket N Tx buffer address block
#define WIZCHIP_RXBUF_BLOCK(N)      (3+(4*N))			//< Socket N Rx buffer address block
#define WIZCHIP_OFFSET_INC(ADDR, N)    (ADDR + (N<<8))  //< Increase offset address +N
#define _WIZCHIP_SOCK_NUM_   8   					///< The count of independant socket of @b WIZCHIP




/**
 * @ingroup Common_register_group
 * @brief Mode Register address(R/W)\n
 * @ref MR is used for S/W reset, ping block mode, PPPoE mode and etc.
 * @details Each bit of @ref MR defined as follows.
 * <table>
 * 		<tr>  <td>7</td> <td>6</td> <td>5</td> <td>4</td> <td>3</td> <td>2</td> <td>1</td> <td>0</td>   </tr>
 * 		<tr>  <td>RST</td> <td>Reserved</td> <td>WOL</td> <td>PB</td> <td>PPPoE</td> <td>Reserved</td> <td>FARP</td> <td>Reserved</td> </tr>
 * </table>
 * - \ref MR_RST		 	: Reset
 * - \ref MR_WOL       	: Wake on LAN
 * - \ref MR_PB         : Ping block
 * - \ref MR_PPPOE      : PPPoE mode
 * - \ref MR_FARP			: Force ARP mode
 */
#define MR                 (_W5500_IO_BASE_ + (0x0000 << 8) + (WIZCHIP_CREG_BLOCK << 3))

//Sn_TX_FSR indicates the free size of Socket n TX Buffer Block.
//It is initialized to the configured size by @ref Sn_TXBUF_SIZE.
#define Sn_TX_FSR(N)       (_W5500_IO_BASE_ + (0x0020 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

//Sn_RX_RSR indicates the data size received and saved in Socket n RX Buffer.
#define Sn_RX_RSR(N)       (_W5500_IO_BASE_ + (0x0026 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

//Transmit memory read pointer register address(R)
#define Sn_TX_RD(N)        (_W5500_IO_BASE_ + (0x0022 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

/**
 * @ingroup Socket_register_group
 * @brief Transmit memory write pointer register address(R/W)
 * @details @ref Sn_TX_WR is initialized by OPEN command. However, if Sn_MR(P[3:0]) is TCP mode(001, it is re-initialized while connecting with TCP.\n
 * It should be read or be updated like as follows.\n
 * 1. Read the starting address for saving the transmitting data.\n
 * 2. Save the transmitting data from the starting address of Socket n TX buffer.\n
 * 3. After saving the transmitting data, update @ref Sn_TX_WR to the increased value as many as transmitting data size.
 * If the increment value exceeds the maximum value 0xFFFF(greater than 0x10000 and the carry bit occurs),
 * then the carry bit is ignored and will automatically update with the lower 16bits value.\n
 * 4. Transmit the saved data in Socket n TX Buffer by using SEND/SEND command
 */
#define Sn_TX_WR(N)        (_W5500_IO_BASE_ + (0x0024 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

// Read point of Receive memory(R/W)
#define Sn_RX_RD(N)        (_W5500_IO_BASE_ + (0x0028 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

//Write point of Receive memory(R)
#define Sn_RX_WR(N)        (_W5500_IO_BASE_ + (0x002A << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

// @brief Socket status register(R)
// @details @ref Sn_SR indicates the status of Socket n.\n
// The status of Socket n is changed by @ref Sn_CR or some special control packet as SYN, FIN packet in TCP.
// - @par Normal status
// - @ref SOCK_CLOSED 		: Closed
// - @ref SOCK_INIT   		: Initiate state
// - @ref SOCK_LISTEN    	: Listen state
// - @ref SOCK_ESTABLISHED 	: Success to connect
// - @ref SOCK_CLOSE_WAIT   : Closing state
// - @ref SOCK_UDP   		: UDP socket
// - @ref SOCK_MACRAW  		: MAC raw mode socket
#define Sn_SR(N)           (_W5500_IO_BASE_ + (0x0003 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

//socket Mode register(R/W)
// 7			6		5			4				3			2			1			0
// MULTI/MFEN	BCASTB	ND/MC/MMB	UCASTB/MIP6B	Protocol[3]	Protocol[2]	Protocol[1]	Protocol[0]
#define Sn_MR(N)           (_W5500_IO_BASE_ + (0x0000 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

//UDP
#define Sn_MR_UDP                    0x02

//TCP
#define Sn_MR_TCP                    0x01


/* Sn_SR values */
//Closed-  This indicates that Socket n is released.
#define SOCK_CLOSED                  0x00


// Initiate state - This indicates Socket n is opened with TCP mode.
#define SOCK_INIT                    0x13

// Listen state -This indicates Socket n is operating as TCP server mode
//and waiting for connection-request (SYN packet) from a peer TCP client
#define SOCK_LISTEN                  0x14

// Connection state
#define SOCK_SYNSENT                 0x15

// Connection state
#define SOCK_SYNRECV                 0x16

// Success to connect
#define SOCK_ESTABLISHED             0x17

//Closing state - This indicates Socket n received the disconnect-request (FIN packet) from the connected peer
#define SOCK_CLOSE_WAIT              0x1C

// Socket interrupt register(R)
// @details @ref Sn_IR indicates the status of Socket Interrupt such as establishment,
// termination, receiving data, timeout)
// When an interrupt occurs and the corresponding bit of @ref Sn_IMR is  the corresponding bit of @ref Sn_IR becomes \n
// In order to clear the @ref Sn_IR bit, the host should write the bit to \n
// 7		6		 5		  4		  3		  2	   1	  0
// Reserved Reserved Reserved SEND_OK TIMEOUT RECV DISCON CON
// 	- \ref Sn_IR_SENDOK : <b>SEND_OK Interrupt</b>
//  - \ref Sn_IR_TIMEOUT : <b>TIMEOUT Interrupt</b>
//  - \ref Sn_IR_RECV : <b>RECV Interrupt</b>
//  - \ref Sn_IR_DISCON : <b>DISCON Interrupt</b>
//	- \ref Sn_IR_CON : <b>CON Interrupt</b>
#define Sn_IR(N)           (_W5500_IO_BASE_ + (0x0002 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

/* Sn_IR values */
// SEND_OK Interrupt -This is issued when SEND command is completed.
#define Sn_IR_SENDOK                 0x10

// * @brief TIMEOUT Interrupt -@details This is issued when ARPTO or TCPTO occurs.
#define Sn_IR_TIMEOUT                0x08

/**
 * @brief RECV Interrupt
 * @details This is issued whenever data is received from a peer.
 */
#define Sn_IR_RECV                   0x04

/**
 * @brief DISCON Interrupt
 * @details This is issued when FIN or FIN/ACK packet is received from a peer.
 */
#define Sn_IR_DISCON                 0x02

/**
 * @brief CON Interrupt
 * @details This is issued one time when the connection with peer is successful and then @ref Sn_SR is changed to @ref SOCK_ESTABLISHED.
 */
#define Sn_IR_CON                    0x01

// Update RX buffer pointer and receive data
// RECV completes the processing of the received data
//in Socket n RX Buffer by using a RX read pointer register
#define Sn_CR_RECV                   0x40

// Receive memory size register(R/W)
// Sn_RXBUF_SIZE configures the RX buffer block size of Socket n.
// Socket n RX Buffer Block size can be configured with 1,2,4,8, and 16 Kbytes.
// If a different size is configured, the data cannot be normally received from a peer.
#define Sn_RXBUF_SIZE(N)   (_W5500_IO_BASE_ + (0x001E << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

//Transmit memory size register(R/W)
//Sn_TXBUF_SIZE configures the TX buffer block size of Socket n.
//Socket n TX Buffer Block size can be configured with 1,2,4,8, and 16 Kbytes.
// If a different size is configured, the data can�t be normally transmitted to a peer.
#define Sn_TXBUF_SIZE(N)   (_W5500_IO_BASE_ + (0x001F << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

/**
 * @ingroup Socket_register_group
 * @brief Socket command register(R/W)
 * @details This is used to set the command for Socket n such as OPEN, CLOSE, CONNECT, LISTEN, SEND, and RECEIVE.\n
 * After W5500 accepts the command, the @ref Sn_CR register is automatically cleared to 0x00.
 * Even though @ref Sn_CR is cleared to 0x00, the command is still being processed.\n
 * To check whether the command is completed or not, please check the @ref Sn_IR or @ref Sn_SR.
 * - @ref Sn_CR_OPEN 		: Initialize or open socket.
 * - @ref Sn_CR_LISTEN 		: Wait connection request in TCP mode(<b>Server mode</b>)
 * - @ref Sn_CR_CONNECT 	: Send connection request in TCP mode(<b>Client mode</b>)
 * - @ref Sn_CR_DISCON 		: Send closing request in TCP mode.
 * - @ref Sn_CR_CLOSE   	: Close socket.
 * - @ref Sn_CR_SEND    	: Update TX buffer pointer and send data.
 * - @ref Sn_CR_SEND_MAC	: Send data with MAC address, so without ARP process.
 * - @ref Sn_CR_SEND_KEEP 	: Send keep alive message.
 * - @ref Sn_CR_RECV		: Update RX buffer pointer and receive data.
 */
#define Sn_CR(N)           (_W5500_IO_BASE_ + (0x0001 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

/**
 * @brief Close socket
 * @details Sn_SR is changed to @ref SOCK_CLOSED.
 */
#define Sn_CR_CLOSE                  0x10

/**
 * @brief Update TX buffer pointer and send data
 * @details SEND transmits all the data in the Socket n TX buffer.\n
 * For more details, please refer to Socket n TX Free Size Register (@ref Sn_TX_FSR), Socket n,
 * TX Write Pointer Register(@ref Sn_TX_WR), and Socket n TX Read Pointer Register(@ref Sn_TX_RD).
 */
#define Sn_CR_SEND                   0x20

/**
 * @brief Send closing request in TCP mode
 * @details Regardless of <b>TCP server</b>or <b>TCP client</b> the DISCON command processes the disconnect-process (b>Active close</b>or <b>Passive close</b>.\n
 * @par Active close
 * it transmits disconnect-request(FIN packet) to the connected peer\n
 * @par Passive close
 * When FIN packet is received from peer, a FIN packet is replied back to the peer.\n
 * @details When the disconnect-process is successful (that is, FIN/ACK packet is received successfully), @ref Sn_SR is changed to @ref SOCK_CLOSED.\n
 * Otherwise, TCPTO occurs (\ref Sn_IR(3)='1') and then @ref Sn_SR is changed to @ref SOCK_CLOSED.
 * @note Valid only in TCP mode.
 */
#define Sn_CR_DISCON                 0x08

/**
 * @ingroup Socket_register_group
 * @brief Peer IP register address(R/W)
 * @details @ref Sn_DIPR configures or indicates the destination IP address of Socket n. It is valid when Socket n is used in TCP/UDP mode.
 * In TCP client mode, it configures an IP address of TCP serverbefore CONNECT command.
 * In TCP server mode, it indicates an IP address of TCP clientafter successfully establishing connection.
 * In UDP mode, it configures an IP address of peer to be received the UDP packet by SEND or SEND_MAC command.
 */
#define Sn_DIPR(N)         (_W5500_IO_BASE_ + (0x000C << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

/**
 * @ingroup Socket_register_group
 * @brief Peer port register address(R/W)
 * @details @ref Sn_DPORT configures or indicates the destination port number of Socket n. It is valid when Socket n is used in TCP/UDP mode.
 * In TCP clientmode, it configures the listen port number of TCP serverbefore CONNECT command.
 * In TCP Servermode, it indicates the port number of TCP client after successfully establishing connection.
 * In UDP mode, it configures the port number of peer to be transmitted the UDP packet by SEND/SEND_MAC command.
 */
#define Sn_DPORT(N)        (_W5500_IO_BASE_ + (0x0010 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

/**
 * @brief Send connection request in TCP mode(Client mode)
 * @details  To connect, a connect-request (SYN packet) is sent to <b>TCP server</b>configured by @ref Sn_DIPR & Sn_DPORT(destination address & port).
 * If the connect-request is successful, the @ref Sn_SR is changed to @ref SOCK_ESTABLISHED and the Sn_IR(0) becomes \n\n
 * The connect-request fails in the following three cases.\n
 * 1. When a @b ARPTO occurs (@ref Sn_IR[3] =  ) because destination hardware address is not acquired through the ARP-process.\n
 * 2. When a @b SYN/ACK packet is not received and @b TCPTO (Sn_IR(3) =  )\n
 * 3. When a @b RST packet is received instead of a @b SYN/ACK packet. In these cases, @ref Sn_SR is changed to @ref SOCK_CLOSED.
 * @note This is valid only in TCP mode and operates when Socket n acts as <b>TCP client</b>
 */
#define Sn_CR_CONNECT                0x04

/**
 * @ingroup Common_register_group
 * @brief Source IP Register address(R/W)
 * @details @ref SIPR configures the source IP address.
 */
#define SIPR               (_W5500_IO_BASE_ + (0x000F << 8) + (WIZCHIP_CREG_BLOCK << 3))

/**
 * @brief MAC LAYER RAW SOCK
 * @details This configures the protocol mode of Socket n.
 * @note MACRAW mode should be only used in Socket 0.
 */
#define Sn_MR_MACRAW                 0x04

#define Sn_MR_IPRAW                  0x03     /**< IP LAYER RAW SOCK */

/* Sn_MR Default values */
/**
 * @brief Support UDP Multicasting
 * @details 0 : disable Multicasting\n
 * 1 : enable Multicasting\n
 * This bit is applied only during UDP mode(P[3:0] = 010.\n
 * To use multicasting, @ref Sn_DIPR & @ref Sn_DPORT should be respectively configured with the multicast group IP address & port number
 * before Socket n is opened by OPEN command of @ref Sn_CR.
 */
#define Sn_MR_MULTI                  0x80

/* Sn_MR value used with Sn_MR_UDP & Sn_MR_MULTI */
/**
 * @brief IGMP version used in UDP mulitcasting
 * @details 0 : disable Multicast Blocking\n
 * 1 : enable Multicast Blocking\n
 * This bit is applied only when MACRAW mode(P[3:0] = 100. It blocks to receive the packet with multicast MAC address.
 */
#define Sn_MR_MC                     Sn_MR_ND

/**
 * @brief No Delayed Ack(TCP), Multicast flag
 * @details 0 : Disable No Delayed ACK option\n
 * 1 : Enable No Delayed ACK option\n
 * This bit is applied only during TCP mode (P[3:0] = 001.\n
 * When this bit is  It sends the ACK packet without delay as soon as a Data packet is received from a peer.\n
 * When this bit is  It sends the ACK packet after waiting for the timeout time configured by @ref _RTR_.
 */
#define Sn_MR_ND                     0x20

/* Sn_CR values */
/**
 * @brief Initialize or open socket
 * @details Socket n is initialized and opened according to the protocol selected in Sn_MR(P3:P0).
 * The table below shows the value of @ref Sn_SR corresponding to @ref Sn_MR.\n
 * <table>
 *   <tr>  <td>\b Sn_MR (P[3:0])</td> <td>\b Sn_SR</td>            		 </tr>
 *   <tr>  <td>Sn_MR_CLOSE  (000)</td> <td></td>         	   		 </tr>
 *   <tr>  <td>Sn_MR_TCP  (001)</td> <td>SOCK_INIT (0x13)</td>  		 </tr>
 *   <tr>  <td>Sn_MR_UDP  (010)</td>  <td>SOCK_UDP (0x22)</td>  		 </tr>
 *   <tr>  <td>S0_MR_MACRAW  (100)</td>  <td>SOCK_MACRAW (0x02)</td>  </tr>
 * </table>
 */
#define Sn_CR_OPEN                   0x01

/**
 * @ingroup Socket_register_group
 * @brief source port register(R/W)
 * @details @ref Sn_PORT configures the source port number of Socket n.
 * It is valid when Socket n is used in TCP/UDP mode. It should be set before OPEN command is ordered.
 */
#define Sn_PORT(N)         (_W5500_IO_BASE_ + (0x0004 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

/**
 * @brief Unicast Block in UDP Multicasting
 * @details 0 : disable Unicast Blocking\n
 * 1 : enable Unicast Blocking\n
 * This bit blocks receiving the unicast packet during UDP mode(P[3:0] = 010 and MULTI =
 */
#define Sn_MR_UCASTB                 0x10


/**
 * @ingroup Common_register_group
 * @brief Gateway IP Register address(R/W)
 * @details @ref GAR configures the default gateway address.
 */
#define GAR                (_W5500_IO_BASE_ + (0x0001 << 8) + (WIZCHIP_CREG_BLOCK << 3))

/**
 * @ingroup Common_register_group
 * @brief Subnet mask Register address(R/W)
 * @details @ref SUBR configures the subnet mask address.
 */
#define SUBR               (_W5500_IO_BASE_ + (0x0005 << 8) + (WIZCHIP_CREG_BLOCK << 3))

/**
 * @ingroup Common_register_group
 * @brief Source MAC Register address(R/W)
 * @details @ref SHAR configures the source hardware address.
 */
#define SHAR               (_W5500_IO_BASE_ + (0x0009 << 8) + (WIZCHIP_CREG_BLOCK << 3))


/**
 * @ingroup Common_register_group
 * @brief PHY Status Register(R/W)
 * @details @ref PHYCFGR configures PHY operation mode and resets PHY. In addition, @ref PHYCFGR indicates the status of PHY such as duplex, Speed, Link.
 */
#define PHYCFGR            (_W5500_IO_BASE_ + (0x002E << 8) + (WIZCHIP_CREG_BLOCK << 3))

/**
 * @ingroup Socket_register_group
 * @brief Maximum Segment Size(Sn_MSSR0) register address(R/W)
 * @details @ref Sn_MSSR configures or indicates the MTU(Maximum Transfer Unit) of Socket n.
 */
#define Sn_MSSR(N)         (_W5500_IO_BASE_ + (0x0012 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

/*==================[macros]=================================================*/

//Get @ref Sn_TX_WR register
#define WgetSn_TX_WR(sn) \
		(((uint16_t)WREAD(Sn_TX_WR(sn)) << 8) + WREAD(WIZCHIP_OFFSET_INC(Sn_TX_WR(sn),1)))

//Set @ref Sn_TX_WR register
#define WsetSn_TX_WR(sn, txwr) { \
		WWRITE(Sn_TX_WR(sn),   (uint8_t)(txwr>>8)); \
		WWRITE(WIZCHIP_OFFSET_INC(Sn_TX_WR(sn),1), (uint8_t) txwr); \
		}

//Get @ref Sn_RX_RD register
#define WgetSn_RX_RD(sn) \
		(((uint16_t)WREAD(Sn_RX_RD(sn)) << 8) + WREAD(WIZCHIP_OFFSET_INC(Sn_RX_RD(sn),1)))

//Set @ref Sn_RX_RD register
#define WsetSn_RX_RD(sn, rxrd) { \
		WWRITE(Sn_RX_RD(sn),   (uint8_t)(rxrd>>8)); \
		WWRITE(WIZCHIP_OFFSET_INC(Sn_RX_RD(sn),1), (uint8_t) rxrd); \
	}

// Get @ref Sn_SR register
#define WgetSn_SR(sn) \
		WREAD(Sn_SR(sn))


// Set @ref Sn_IR register --@param (uint8_t)ir Value to set @ref Sn_IR
#define WgetSn_IR(sn) \
		(WREAD(Sn_IR(sn)) & 0x1F)

// Sn_IR register - @param (uint8_t)ir Value to set @ref Sn_IR
#define WsetSn_IR(sn, ir) \
		WWRITE(Sn_IR(sn), (ir & 0x1F))



// Get @ref Sn_MR register - return uint8_t. Value of @ref Sn_MR.
#define WgetSn_MR(sn) \
	WREAD(Sn_MR(sn))

//Socket_register_access_function
//Gets the max buffer size of socket sn passed as parameter.
//return uint16_t. Value of Socket n RX max buffer size.
#define WgetSn_RxMAX(sn) \
		(((uint16_t)WgetSn_RXBUF_SIZE(sn)) << 10)

// Set @ref Sn_RXBUF_SIZE register
#define WgetSn_RXBUF_SIZE(sn) \
		WREAD(Sn_RXBUF_SIZE(sn))

//Socket_register_access_function
//Get @ref Sn_SR register
//@param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
//@return uint8_t. Value of @ref Sn_SR.
#define WgetSn_SR(sn) \
		WREAD(Sn_SR(sn))

/**
 * @brief Socket_register_access_function
 * @brief Gets the max buffer size of socket sn passed as parameters.
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of Socket n TX max buffer size.
 */
#define WgetSn_TxMAX(sn) \
		(((uint16_t)WgetSn_TXBUF_SIZE(sn)) << 10)

// @brief Socket_register_access_function
// @brief Gets the max buffer size of socket sn passed as parameters.
// @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
// @return uint16_t. Value of Socket n TX max buffer size.
#define WgetSn_TxMAX(sn) \
		(((uint16_t)WgetSn_TXBUF_SIZE(sn)) << 10)

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref Sn_TXBUF_SIZE register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref Sn_TXBUF_SIZE.
 * @sa setSn_TXBUF_SIZE()
 */
#define WgetSn_TXBUF_SIZE(sn) \
		WREAD(Sn_TXBUF_SIZE(sn))

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref Sn_CR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t)cr Value to set @ref Sn_CR
 * @sa getSn_CR()
 */
#define WsetSn_CR(sn, cr) \
		WWRITE(Sn_CR(sn), cr)
/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref Sn_CR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref Sn_CR.
 * @sa setSn_CR()
 */
#define WgetSn_CR(sn) \
		WREAD(Sn_CR(sn))

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref Sn_DIPR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t*)dipr Pointer variable to get socket n destination IP address. It should be allocated 4 bytes.
 * @sa setSn_DIPR()
 */
#define WgetSn_DIPR(sn, dipr) \
		WIZCHIP_READ_BUF(Sn_DIPR(sn), dipr, 4)

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref Sn_DPORT register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint16_t)dport Value to set @ref Sn_DPORT
 * @sa getSn_DPORT()
 */
#define WsetSn_DPORT(sn, dport) { \
		WWRITE(Sn_DPORT(sn),   (uint8_t) (dport>>8)); \
		WWRITE(WIZCHIP_OFFSET_INC(Sn_DPORT(sn),1), (uint8_t)  dport); \
	}
/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref Sn_DPORT register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of @ref Sn_DPORT.
 * @sa setSn_DPORT()
 */
#define WgetSn_DPORT(sn) \
		(((uint16_t)WREAD(Sn_DPORT(sn)) << 8) + WREAD(WIZCHIP_OFFSET_INC(Sn_DPORT(sn),1)))





/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref Sn_DIPR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t*)dipr Pointer variable to set socket n destination IP address. It should be allocated 4 bytes.
 * @sa getSn_DIPR()
 */
#define WsetSn_DIPR(sn, dipr) \
		WWRITE_BUF(Sn_DIPR(sn), dipr, 4)

/**
 * @ingroup Common_register_access_function
 * @brief Get local IP address
 * @param (uint8_t*)sipr Pointer variable to get local IP address. It should be allocated 4 bytes.
 * @sa setSIPR()
 */
#define WgetSIPR(sipr) \
		WREAD_BUF(SIPR, sipr, 4)

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref Sn_MR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t)mr Value to set @ref Sn_MR
 * @sa getSn_MR()
 */
#define WsetSn_MR(sn, mr) \
		WWRITE(Sn_MR(sn),mr)

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref Sn_PORT register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint16_t)port Value to set @ref Sn_PORT.
 * @sa getSn_PORT()
 */
#define WsetSn_PORT(sn, port)  { \
		WWRITE(Sn_PORT(sn),   (uint8_t)(port >> 8)); \
		WWRITE(WIZCHIP_OFFSET_INC(Sn_PORT(sn),1), (uint8_t) port); \
	}

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref Sn_PORT register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of @ref Sn_PORT.
 * @sa setSn_PORT()
 */
#define WgetSn_PORT(sn) \
		(((uint16_t)WREAD(Sn_PORT(sn)) << 8) + WREAD(WIZCHIP_OFFSET_INC(Sn_PORT(sn),1)))

/**
 * @ingroup Common_register_access_function
 * @brief Get local MAC address
 * @param (uint8_t*)shar Pointer variable to get local MAC address. It should be allocated 6 bytes.
 * @sa setSHAR()
 */
#define WgetSHAR(shar) \
		WREAD_BUF(SHAR, shar, 6)


/**
 * @ingroup Common_register_access_function
 * @brief Set local MAC address
 * @param (uint8_t*)shar Pointer variable to set local MAC address. It should be allocated 6 bytes.
 * @sa getSHAR()
 */
#define WsetSHAR(shar) \
		WWRITE_BUF(SHAR, shar, 6)


/**
 * @ingroup Common_register_access_function
 * @brief Get Mode Register
 * @return uint8_t. The value of Mode register.
 * @sa setMR()
 */
#define WgetMR() \
		WREAD(MR)

/**
 * @ingroup Common_register_access_function
 * @brief Set local IP address
 * @param (uint8_t*)sipr Pointer variable to set local IP address. It should be allocated 4 bytes.
 * @sa getSIPR()
 */
#define WsetSIPR(sipr) \
		WWRITE_BUF(SIPR, sipr, 4)

/**
 * @ingroup Common_register_access_function
 * @brief Get @ref PHYCFGR register
 * @return uint8_t. Value of @ref PHYCFGR register.
 * @sa setPHYCFGR()
 */
#define WgetPHYCFGR() \
		WREAD(PHYCFGR)

/**
 * @ingroup Common_register_access_function
 * @brief Set gateway IP address
 * @param (uint8_t*)gar Pointer variable to set gateway IP address. It should be allocated 4 bytes.
 * @sa getGAR()
 */
#define WsetGAR(gar) \
		WWRITE_BUF(GAR,gar,4)

/**
 * @ingroup Common_register_access_function
 * @brief Get gateway IP address
 * @param (uint8_t*)gar Pointer variable to get gateway IP address. It should be allocated 4 bytes.
 * @sa setGAR()
 */
#define WgetGAR(gar) \
		WREAD_BUF(GAR,gar,4)


/**
 * @ingroup Common_register_access_function
 * @brief Set subnet mask address
 * @param (uint8_t*)subr Pointer variable to set subnet mask address. It should be allocated 4 bytes.
 * @sa getSUBR()
 */
#define WsetSUBR(subr) \
		WWRITE_BUF(SUBR, subr,4)


/**
 * @ingroup Common_register_access_function
 * @brief Get subnet mask address
 * @param (uint8_t*)subr Pointer variable to get subnet mask address. It should be allocated 4 bytes.
 * @sa setSUBR()
 */
#define WgetSUBR(subr) \
		WREAD_BUF(SUBR, subr, 4)

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref Sn_MSSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint16_t)mss Value to set @ref Sn_MSSR
 * @sa setSn_MSSR()
 */
#define WsetSn_MSSR(sn, mss) { \
		WWRITE(Sn_MSSR(sn),   (uint8_t)(mss>>8)); \
		WWRITE(WIZCHIP_OFFSET_INC(Sn_MSSR(sn),1), (uint8_t) mss); \
	}

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref Sn_MSSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of @ref Sn_MSSR.
 * @sa setSn_MSSR()
 */
//M20150401 : Type explict declaration
/*
#define getSn_MSSR(sn) \
		((WIZCHIP_READ(Sn_MSSR(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_MSSR(sn),1)))
*/
#define WgetSn_MSSR(sn) \
		(((uint16_t)WREAD(Sn_MSSR(sn)) << 8) + WREAD(WIZCHIP_OFFSET_INC(Sn_MSSR(sn),1)))
/*==================[tipos de datos declarados por el usuario]===============*/
typedef enum{
  UNKNOWN,
  LINK_ON,
  LINK_OFF
} W5500Linkstatus;




/*==================[declaraciones de datos externos]========================*/


/*==================[declaraciones de funciones externas]====================*/
uint8_t WREAD (uint32_t addr);
void WWRITE (uint32_t addr, uint8_t wb);
uint16_t WgetSn_TX_FSR(uint8_t sn);
uint16_t WgetSn_RX_RSR(uint8_t sn);
void Wsend_data(uint8_t sn, uint8_t *wizdata, uint16_t len);
void Wrecv_data(uint8_t sn, uint8_t *wizdata, uint16_t len);
void Wrecv_ignore(uint8_t sn, uint16_t len);

void     WWRITE_BUF (uint32_t AddrSel, uint8_t* pBuf, uint16_t len);
void     WREAD_BUF (uint32_t AddrSel, uint8_t* pBuf, uint16_t len);

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _W5500_H_ */
