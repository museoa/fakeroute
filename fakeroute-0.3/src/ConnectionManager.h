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

#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include "SourceIpEntry.h"
#include "FakeroutePacket.h"

class ConnectionManager {

private:
  const u_int16_t ENTRY_LIMIT;

  class SourceIpEntry *head;
  class SourceIpEntry *tail;

  char** routePath;

  u_int16_t routeCount;
  u_int16_t entryCount;

  class SourceIpEntry* findEntryFor(char* sourceAddress);
  void  loadRoutePath(const char* routePath);
  void  releaseHeadNode(void);
  char* getIpForHop(u_int16_t hop);
  int   isTransitHop(u_int16_t hop);

public:
  ConnectionManager(char* routeFile);
  class FakeroutePacket* getPacketFor(char* sourceAddress, char* destinationAddress, 
				      u_int16_t sourcePort, u_int16_t destinatonPort);
  
};

#endif
