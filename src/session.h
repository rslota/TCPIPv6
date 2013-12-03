//
//  session.h
//  TCP-IP Stack
//
//  Copyright (c) 2013 Rafał Słota, Konrad Zemek. All rights reserved.
//

#ifndef TCP_IP_Stack_session_h
#define TCP_IP_Stack_session_h


#define TCP_PROTOCOL        0x06
#define UDP_PROTOCOL        0x11
#define DEFAULT_PROTOCOL    UDP_PROTOCOL

extern int ifindex;

/// Initializes socket
/// Protocol shall be either TCP_PROTOCOL or UDP_PROTOCOL
int session(int protocol, const char *iface);

/// Closes the socket
int destroy(int session_id);


#endif
