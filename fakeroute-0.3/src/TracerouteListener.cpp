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

#include "TracerouteListener.h"
#include <stdio.h>

TracerouteListener::TracerouteListener(void) : FD_COUNT(31), LOW_ORDER_PORT(32768 + 666) {
  int i = 0;
  
  for (i=0;i<31;i++) {
    fds[i].fd     = bindSocket(LOW_ORDER_PORT + i);
    fds[i].events = POLLIN|POLLPRI;
  }
}

void TracerouteListener::listen(class TracerouteEvents &tracerouteEvents) {
  int *activeFds;
  int eventCount, i, index = 0;

  if ((eventCount = poll(fds, (unsigned long)FD_COUNT, -1)) < 0) {
    fprintf(stderr, "Error During Poll.");
    exit(1);
  }

  activeFds = (int*)malloc(eventCount * sizeof(int));

  for (i=0;i<FD_COUNT;i++) {
    if ((fds[i].revents & POLLIN == POLLIN) ||
	(fds[i].revents & POLLPRI == POLLPRI)) {
      activeFds[index++] = fds[i].fd;
    }
  }

  tracerouteEvents.setActiveFds(activeFds);
  tracerouteEvents.setActiveFdCount(index);
}


int TracerouteListener::bindSocket(int port) {
  struct sockaddr_in localAddress;
  int sockfd;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    fprintf(stderr, "Error Creating Socket.\n");
    exit(1);
  }

  memset((void*)&localAddress, 0, sizeof(localAddress));

  localAddress.sin_family      = AF_INET;
  localAddress.sin_addr.s_addr = INADDR_ANY;
  localAddress.sin_port        = htons(port);

  if ((bind(sockfd, (struct sockaddr*)&localAddress, sizeof(localAddress))) < 0) {
    fprintf(stderr, "Error Binding Socket\n");
    exit(1);
  }

  return sockfd;
}
