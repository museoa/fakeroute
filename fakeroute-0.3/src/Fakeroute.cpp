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

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include "TracerouteListener.h"
#include "TracerouteEvents.h"
#include "TracerouteReader.h"
#include "TraceroutePacket.h"
#include "ConnectionManager.h"
#include "FakeroutePacket.h"

void start(char*);
void printUsage(char*);
static void daemonize();

int main(int argc, char** argv) {
  char* fileName = "route.conf";
  int op;

  if (getuid()) {
    fprintf(stderr, "Must be root to run fakeroute.\n");
    exit(1);
  }

  while ((op = getopt(argc, argv, "hf:")) != -1) {
    switch (op) {
    case 'h': 
      printUsage(argv[0]); 
      exit(1);
    case 'f': 
      fileName = optarg;   
      break;
    default:
      printUsage(argv[0]);  
      exit(1);
    }
  }

  daemonize();
  start(fileName);
}  

void printUsage(char* name) {
  printf("Usage: %s [options]\n", name);
  printf("  -h\t\tPrint Help (This)\n");
  printf("  -f <fileName>\tLoad fake route information from\n\t\t<fileName> rather than route.conf\n");
}

void start(char* routeFile) {
  int* fds;
  int fdCount, i;

  class TracerouteListener listener;
  class TracerouteReader reader;
  class ConnectionManager manager(routeFile);
  
  for (;;) {
    class TracerouteEvents events;

    listener.listen(events);

    fds     = events.getActiveFds();
    fdCount = events.getActiveFdCount();

    for (i=0;i<fdCount;i++) {
      class TraceroutePacket *traceroutePacket;
      class FakeroutePacket  *fakeroutePacket;

      traceroutePacket = reader.readPacket(fds[i]);      
      fakeroutePacket  = manager.getPacketFor((*traceroutePacket).getSourceAddress(),
					      (*traceroutePacket).getDestinationAddress(),
					      (*traceroutePacket).getSourcePort(),
					      (*traceroutePacket).getDestinationPort());

      (*fakeroutePacket).sendOn(fds[i]);

      delete fakeroutePacket;
      delete traceroutePacket;
    }
  }
}

static void daemonize() {
  switch (fork()) {
  case -1: 
    perror("fork failed");        
    exit(1);
  case 0:  
    if (setsid() == -1) exit(4);  
    break;
  default:  
    exit(0);
  }
}
