//======================================================================
//
//	Morgan Stanley Control Server for HERMES Trading App Simulator
//	==============================================================
//
//  Provides supporting network calls for MSControl_Server.
//
//
//	Copyright (C) 2013	Morgan Stanley and Andrew Stokes
//                   All rights reserved
//
//  For full details of this copyright, please see MSControl_Server.cpp
//======================================================================


#include "stdafx.h"
#include "NetSupport_Server.hpp"
#include "Exception.hpp"
#include <netdb.h>

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

using namespace std;

NetSupport_Server::NetSupport_Server() {};

IPNumber NetSupport_Server::FindHostIP(const char *pServerName)
{
	struct hostent *pHostent;

	// Get hostent structure for hostname:
	if (!(pHostent = gethostbyname(pServerName)))
			throw Exception("could not resolve hostname.");
	
	// Extract primary IP address from hostent structure:
	if (pHostent->h_addr_list && pHostent->h_addr_list[0])
		return *reinterpret_cast<IPNumber*>(pHostent->h_addr_list[0]);
	
	return 0;
}

void NetSupport_Server::SetServerSockAddr(sockaddr_in *pSockAddr, int portNumber)
{
    // Set family, port and find IP
    pSockAddr->sin_family = AF_INET;
    pSockAddr->sin_port = htons(portNumber);
    pSockAddr->sin_addr.s_addr = INADDR_ANY;
//    pSockAddr->sin_addr.S_un.S_addr = INADDR_ANY;
//    pSockAddr->sin_addr.S_un.S_un_b.s_b1 = 192;
//    pSockAddr->sin_addr.S_un.S_un_b.s_b2 = 168;
//    pSockAddr->sin_addr.S_un.S_un_b.s_b3 = 1;
//    pSockAddr->sin_addr.S_un.S_un_b.s_b4 = 13;
}

void NetSupport_Server::GetClientDescription(const sockaddr_in &sockAddr)
{
    snprintf(ClientDesc, 128, "%s:%d", inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port));
    return;
}

void NetSupport_Server::NetworkInit() {
	/**
	WSADATA wsaData;

	if (!WSAStartup(MAKEWORD(REQ_WINSOCK_VER,0), &wsaData)) {               // Winsock startup
		if (LOBYTE(wsaData.wVersion) < REQ_WINSOCK_VER) {                    // Check it supports Winsock 2
			throw Exception("Winsock required version not supported");
		}
  } else {
		throw Exception("Winsock startup failed", WSAGetLastError());
  }
  return;
  */

}

void NetSupport_Server::NetworkBindListen(const char *pServername, const int HostPort) {

	sockaddr_in	sockAddr = {0};
  char tempbuf[128];

	// Lookup hostname and fill sockaddr_in structure:
	cout << "Looking up hostname " << pServername << "... ";
	SetServerSockAddr(&sockAddr, HostPort);
	cout << "found.\n";
	
	cout << "Creating socket... ";
	if ((hListenerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) throw Exception("Could not create socket");
	cout << "created.\n";
		
	snprintf(tempbuf, 128,"%s:%d", inet_ntoa(sockAddr.sin_addr),HostPort);
  cout << "Binding socket " << tempbuf <<  "... ";
	if (bind(hListenerSocket, reinterpret_cast<struct sockaddr*>(&sockAddr), sizeof(sockAddr))!=0) {
  	snprintf(tempbuf, 128,"Could not bind %s:%d", inet_ntoa(sockAddr.sin_addr),HostPort);
    throw Exception(tempbuf);
  }
	cout << "bound.\n";

  // Put socket in listening mode
  cout << "Putting socket in listening mode... ";
  if (listen(hListenerSocket, SOMAXCONN)!=0)
  {
    throw new Exception("Could not put socket in listening mode");
  }
  cout << "done.\n";


	return;
};


void NetSupport_Server::NetworkAccept() {

  cout << "Waiting for client connection... ";  
  clientSockSize = sizeof(clientSockAddr);

  hClientSocket = accept(hListenerSocket, reinterpret_cast<struct sockaddr*>(&clientSockAddr),
                 &clientSockSize);
  if (hClientSocket == INVALID_SOCKET) throw Exception("Accept connection failed");
  GetClientDescription(clientSockAddr);

  cout << "found " << ClientDesc << "\n"; 

  return;
}

void NetSupport_Server::NetworkCloseAndCleanUp() {
  if (hListenerSocket!=INVALID_SOCKET)
	{
		close(hListenerSocket);
	}
  //WSACleanup();
  return;
};
