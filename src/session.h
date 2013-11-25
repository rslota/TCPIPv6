//
//  session.h
//  TCP-IP Stack
//
//  Created by Rafał Słota on 25.11.2013.
//  Copyright (c) 2013 Rafał Słota, Konrad Zemek. All rights reserved.
//

#ifndef TCP_IP_Stack_session_h
#define TCP_IP_Stack_session_h

#define DEFAULT_PROTOCOL    0
#define TCP_PROTOCOL        1
#define UDP_PROTOCOL        2

/// Initializes socket
/// Protocol shall be either TCP_PROTOCOL or UDP_PROTOCOL
int session(int protocol);

/// Closes the socket
int destroy(int session_id);


#endif
