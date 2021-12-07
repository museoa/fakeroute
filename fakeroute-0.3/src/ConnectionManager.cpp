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

#include "ConnectionManager.h"
#include "DestHostUnreachableFakeroutePacket.h"
#include "TimeToLiveExpiredFakeroutePacket.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

ConnectionManager::ConnectionManager(char* routeFile) : ENTRY_LIMIT(2) {
  head = NULL;
  tail = NULL;

  routeCount = 0;
  entryCount = 0;

  loadRoutePath(routeFile);
}

class FakeroutePacket* ConnectionManager::getPacketFor(char* sourceAddress, char* destinationAddress,
						       u_int16_t sourcePort, u_int16_t destinationPort) 
{
  class SourceIpEntry *entry    = findEntryFor(sourceAddress);
  char                *sourceIp = getIpForHop((*entry).getTtl()/3);

  class FakeroutePacket *spoofedPacket;

  if (isTransitHop((*entry).getTtl()/3)) {
    spoofedPacket = new TimeToLiveExpiredFakeroutePacket(sourcePort, destinationPort,
							 sourceIp, destinationAddress);
  } else {
    spoofedPacket = new DestHostUnreachableFakeroutePacket(sourcePort, destinationPort,
							   sourceIp, destinationAddress);
  }

  (*entry).incrementTtl(routeCount * 3);

  return spoofedPacket;
}

class SourceIpEntry* ConnectionManager::findEntryFor(char* sourceAddress) {
  class SourceIpEntry *current;

  for (current = head; current != NULL; current = (*current).getNext()) {
    if (strcmp((*current).getSourceIp(), sourceAddress) == 0) {
      return current;
    }
  }

  if (tail == NULL) {
    head = tail = new SourceIpEntry(sourceAddress);    
  } else {
    (*tail).setNext(new SourceIpEntry(sourceAddress));
    tail = (*tail).getNext();
  }

  if (++entryCount > ENTRY_LIMIT) {
    releaseHeadNode();
  }

  return tail;
}

void ConnectionManager::releaseHeadNode(void) {
  class SourceIpEntry *tmp = (*head).getNext();
  delete head;

  head = tmp;
  entryCount--;
}

int ConnectionManager::isTransitHop(u_int16_t hop) {
  return (hop < routeCount -1);
}

char* ConnectionManager::getIpForHop(u_int16_t hop) {
  if (hop < routeCount-1) {
    return routePath[hop];
  } else {
    return routePath[routeCount-1];
  }
}

void ConnectionManager::loadRoutePath(const char* routeFile) {
  FILE *configFile;
  char line[1024];

  routePath  = (char**)malloc(30 * sizeof(char*));
  configFile = fopen(routeFile, "r");

  if (configFile == NULL) {
    fprintf(stderr, "ERROR: Specified route file does not exist, or no route file\nwas specified and default route.conf does not exist.\n");
    exit(1);
  }

  while ((fgets((char*)line, 1024, configFile)) != NULL) {
    routePath[routeCount] = strdup((char*)line);
    routeCount++;
  }
}
