#ifndef UDP_SERVER_LINUX
#define UDP_SERVER_LINUX

#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

namespace GLVM::core
{
	struct UDP_ServerLinux {
		unsigned long port;
		static const unsigned int  maxBufferSize = 1024;
		int           socketFileDescriptor;
		sockaddr_in   serverAddress;
		sockaddr_in   clientAddress;
		socklen_t     clientAddressLength = sizeof(clientAddress);
		char          buffer[maxBufferSize];

		UDP_ServerLinux( unsigned long port = 8080 );
		char* receive();
		void response();
		~UDP_ServerLinux();
	};
}; // namespace GLVM::core

#endif
