/*****************************************************
* Program name: uniclient
*
* Module ID: UFCFQ4-30-2
* Module Name: Computer Networks and Operating Systems
*
* First written on 02/03/2015
*
* Authors: Allister Quinn - 13000014
*          Ben Slatter    - 13007555
*
* Program Description:
* Unicast SNTP client. Sends an SNTP packet request
* to a user specified server. Receives a packet back,
* interprets this packet and displays the information.
*
*****************************************************/
/*  Includes */
 
#include <stdio.h>      // printf etc
#include <stdlib.h>   	// exit()
#include <unistd.h>   	// close()
#include <sys/time.h>   // time
#include <errno.h>      // error reporting
#include <string.h>  	// memset
#include <sys/socket.h> // network
#include <netinet/in.h> // network
#include <arpa/inet.h>  // network
#include <netdb.h>      // network
#include <math.h>       // pow
#include <stdint.h>     // uint32_t type
#include <time.h>		// rand
 
/****************************************************/
/*  Defines */
 
// Size of SNTP packet
#ifndef PACKETSIZE
#define PACKETSIZE 48
#endif
 
// Secs since 1/1/1970 required in unix_to_ntp_time
#ifndef STARTOFTIME
#define STARTOFTIME 2208988800UL
#endif
 
// Used to shift usecs in unix_to_ntp_time
#ifndef USECSHIFT
#define USECSHIFT (1LL << 32) * 1.0e-6
#endif
 
// Used in calculating offset and delay.
#ifndef TOFRACTION
#define TOFRACTION 10000000000
#endif
 
// Set packet LI to no leap.
#ifndef NOLEAP
#define NOLEAP 0x0
#endif
 
// Set packet mode to client.
#ifndef CLIENTMODE
#define CLIENTMODE 0x3
#endif
 
// Set packet VN to SNTPv4.
#ifndef SNTPV4
#define SNTPV4 0x20
#endif
 
/****************************************************/
/*  Structures */
 
// Struct to hold SNTP packet in logical format
struct NTPPacket {
    int leap;
    int version;
    int mode;
    int stratum;
    double poll_int;
    double precision;
    int root_delay_sec;
    int root_delay_frac;
    int root_disp_sec;
    int root_disp_frac;
    unsigned long ref_id;
 
    unsigned long ref_time[2];
    unsigned long orig_time[2];
    unsigned long recv_time[2];
    unsigned long trans_time[2];
    double offset;
    double delay;
};
 
/****************************************************/
/*  Functions */
 
/*********************************************************************************************
* Name: build_packet(char udp_packet[], struct timeval* transmit_time)
* Returns               : void
* Parameters            :
*    udp_packet[]       : UDP Packet to be built
*    transmit_time      : Struct to hold transmit timestamp, used in conversion from
*                         Unix to NTP time.
* Created by            : Allister Quinn / Ben Slatter
* Date created          : 04/03/2015
* Description           : Sets the LI, VN, Mode, and transmit time of the packet.
* Notes:                : None
*********************************************************************************************/
void build_packet(unsigned char sntp_packet[], struct timeval* transmit_time);
 
/*********************************************************************************************
* Name: void packet_to_struct(struct NTPPacket* recv_packet, unsigned char buf[])
* Returns:              : void
* Parameters            :
*    recv_packet        : Empty NTPPacket struct to be populated
*    buf[]              : Packet received from the server
* Created by            : Allister Quinn / Ben Slatter
* Date created          : 27/03/2015
* Description           : Converts original packet received from server to NTPPacket struct
* Notes                 : None
*********************************************************************************************/
void packet_to_struct(struct NTPPacket* recv_packet, unsigned char buf[]);
 
/*********************************************************************************************
* Name: void verify_packet(struct NTPPacket* recv_packet, struct timeval* transmit_time))
* Returns:              : int value 0 or 1.
* Parameters            :
*    recv_packet        : Received packet from server
*    transmit_time      : Original timestamp of sent packet
* Created by            : Allister Quinn / Ben Slatter
* Date created          : 28/03/2015
* Description           : Checks validity of packet. Rejects packet if criteria met (LI not 3,
*                         mode not 4, stratum is 0, originate timestamp mismatch).
*                         Returns 0 if packet successfully verified, 1 if verification failed.
* Notes                 : None
*********************************************************************************************/
int verify_packet(struct NTPPacket* recv_packet, struct timeval* transmit_time);
 
/*********************************************************************************************
* Name: void print_packet(struct NTPPacket* recv_packet)
* Returns:              : void
* Parameters            :
*    recv_packet        : Packet recieved from the server
* Created by            : Allister Quinn / Ben Slatter
* Date created          : 05/03/2015
* Description           : Prints contents of packet to console.
* Notes                 : None
*********************************************************************************************/
void print_packet(struct NTPPacket* recv_packet);
 
/*********************************************************************************************
* Name: void resolve_ref_id(long identifier)
* Returns:              : void
* Parameters            :
*    identifier         : Reference identifier
* Created by            : Allister Quinn / Ben Slatter
* Date created          : 05/03/2015
* Description           : Resolves reference identifier and prints info.
* Notes                 : None
*********************************************************************************************/
void resolve_ref_id(long identifier);
 
/*********************************************************************************************
* Name: void unix_to_ntp_time(struct timeval *time)
* Returns:              : void
* Parameters            :
*    *time              : UNIX timestamp
* Created by            : Allister Quinn / Ben Slatter
* Date created          : 07/03/2015
* Description           : Converts a UNIX timestamp into a NTP compatible version.
* Notes                 : None
*********************************************************************************************/
void unix_to_ntp_time(struct timeval *time);