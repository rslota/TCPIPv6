//
//  ip_session.h
//  TCP-IP Stack
//
//  Copyright (c) 2013 Rafał Słota, Konrad Zemek. All rights reserved.
//

#ifndef TCP_IP_Stack_ip_session_h
#define TCP_IP_Stack_ip_session_h


#define IP_PAYLOAD_TO_BIG           -1;
#define IP_FRAME_ASSEMBLY_ERROR     -2;
#define NETWORK_SEND_ERROR          -3;

/**
 * Sends IP frame with given data to specified host.
 * @param session_id Session identifier returned by session/1 function
 * @param dest_adder Destination IPv6 address
 * @param payload Data to be send
 * @param payload_size Size of payload array
 * @return 0 on success. Negative value for error.
 */
int ip_send(int session_id, const char dest_addr[], const char payload[], int payload_size);

/**
 * Receives IP frame from specified host.
 * @param session_id Session identifier returned by session/1 function
 * @param src_adder Source IPv6 address
 * @param buffer Memory block to which received data will be saved
 * @param buffer_max_size Size of buffer
 * @return 0 on success. Negative value for error.
 */
int ip_recv(int session_id, const char src_addr[], char buffer[], int buffer_max_size);


#endif
