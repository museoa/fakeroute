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

#ifndef FAKEROUTE_PACKET_H
#define FAKEROUTE_PACKET_H

#include <sys/types.h>

class FakeroutePacket {

 private:
  const int ICMP_HDR_SIZE;
  const int IP_HDR_SIZE;
  const int UDP_HDR_SIZE;

  u_int16_t receivedSourcePort;
  u_int16_t receivedDestinationPort;
  char*     spoofedSourceAddress;
  char*     spoofedDestinationAddress;

  virtual void buildUdpHeader(char* packet, u_int16_t sourcePort, u_int16_t destinationPort);
  virtual void buildIcmpHeader(char* packet, int protocol);
  virtual void buildIpHeader(char* source, char* destination, char* packet);

 protected:
  virtual u_int16_t getIcmpType(void) = 0;
  virtual u_int16_t getIcmpCode(void) = 0;  

 public:
  FakeroutePacket(u_int16_t receivedSourcePort, u_int16_t receivedDestinationPort,
		  char* spoofedSourceAddress, char* spoofedDestinationAddress);
  virtual void sendOn(int fd);
};

#endif
