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

#include "TracerouteReader.h"

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

class TraceroutePacket* TracerouteReader::readPacket(int fd) {
  char packet[32*1024];

  struct sockaddr_in remoteAddress;
  struct sockaddr_in localAddress;

  unsigned int size = sizeof(remoteAddress);

  memset((void*)&remoteAddress, 0, size);
  memset((void*)&localAddress, 0, size);

  if ((recvfrom(fd, packet, 32*1024, 0, (struct sockaddr*)&remoteAddress, &size)) < 0) {
    fprintf(stderr, "Error receiving datagram.");
    exit(1);
  }

  if ((getsockname(fd, (struct sockaddr*)&localAddress, &size)) < 0) {
    fprintf(stderr, "Error getting local name.");
    exit(1);
  }

  return new TraceroutePacket(inet_ntoa(remoteAddress.sin_addr),
			      inet_ntoa(localAddress.sin_addr),
			      ntohs(remoteAddress.sin_port),
			      ntohs(localAddress.sin_port));
}
