/*-
 * Copyright (c) 2002, Matt Rosenfeld
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of this program nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "FakeroutePacket.h"
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "in_cksum.h"

FakeroutePacket::FakeroutePacket(u_int16_t localReceivedSourcePort,
				 u_int16_t localReceivedDestinationPort,
				 char* localSpoofedSourceAddress, 
				 char* localSpoofedDestinationAddress) : 
  ICMP_HDR_SIZE(sizeof(struct icmp)),
  IP_HDR_SIZE(sizeof(struct iphdr)),
  UDP_HDR_SIZE(sizeof(struct udphdr))
{
  receivedSourcePort        = localReceivedSourcePort;
  receivedDestinationPort   = localReceivedDestinationPort;
  spoofedSourceAddress      = localSpoofedSourceAddress;
  spoofedDestinationAddress = localSpoofedDestinationAddress;
}

void FakeroutePacket::sendOn(int fd) {

  int sockfd;
  int one = 1;

  struct sockaddr_in packetTo;
  unsigned char packet[IP_HDR_SIZE + ICMP_HDR_SIZE + UDP_HDR_SIZE];

  memset((void*)packet, 0, sizeof(packet));
  memset((void*)&packetTo, 0, sizeof(packetTo));

  if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
    fprintf(stderr, "Error Creating Raw Socket.");
    exit(1);
  }

  if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, (char*)&one, sizeof(one)) < 0) {
    fprintf(stderr, "Error Setting Raw Socket Socket Option.");
    exit(1);
  }
  
  packetTo.sin_family      = AF_INET;
  packetTo.sin_addr.s_addr = inet_addr(spoofedDestinationAddress);

  buildIpHeader(spoofedSourceAddress, spoofedDestinationAddress, (char*)packet);
  buildIcmpHeader((char*)(packet + IP_HDR_SIZE), IPPROTO_UDP);
  buildUdpHeader((char*)(packet + IP_HDR_SIZE + ICMP_HDR_SIZE), 
		 receivedSourcePort, receivedDestinationPort);

  if (sendto(sockfd, packet, sizeof(packet), 0x0, (struct sockaddr*)&packetTo, sizeof(packetTo)) < 0) {
    fprintf(stderr, "Packet Send Failed.");
    exit(1);
  }
}

void FakeroutePacket::buildUdpHeader(char* packet, u_int16_t sourcePort, 
				     u_int16_t destinationPort) 
{
  struct udphdr* udphdr = (struct udphdr*)packet;
  
  udphdr->source = htons(sourcePort);
  udphdr->dest   = htons(destinationPort);
}

void FakeroutePacket::buildIcmpHeader(char* packet, int protocol) {
  struct iphdr *fakeIpHdr;
  struct icmp  *icmpHeader = (struct icmp*)packet;

  char fakePacket[IP_HDR_SIZE];
  fakeIpHdr           = (struct iphdr*)fakePacket;
  fakeIpHdr->ihl      = 5;
  fakeIpHdr->version  = 4;
  fakeIpHdr->protocol = protocol;

  icmpHeader->icmp_type  = getIcmpType();
  icmpHeader->icmp_code  = getIcmpCode();
  icmpHeader->icmp_ip    = *(struct ip*)fakePacket;
  icmpHeader->icmp_cksum = (int)in_cksum((unsigned short*)icmpHeader, ICMP_HDR_SIZE);
}

void FakeroutePacket::buildIpHeader(char* source, char* destination, char* packet) {
  struct iphdr *ipHeader;

  ipHeader           = (struct iphdr*)packet;
  ipHeader->ihl      = 5;
  ipHeader->version  = 4;
  ipHeader->tot_len  = htons(IP_HDR_SIZE + ICMP_HDR_SIZE);
  ipHeader->id       = 31337;
  ipHeader->ttl      = 60;
  ipHeader->protocol = IPPROTO_ICMP;
  ipHeader->saddr    = inet_addr(source);
  ipHeader->daddr    = inet_addr(destination);
  ipHeader->check    = (unsigned short)in_cksum((unsigned short*)ipHeader, IP_HDR_SIZE);
}
