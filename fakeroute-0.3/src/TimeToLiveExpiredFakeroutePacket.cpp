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

#include "TimeToLiveExpiredFakeroutePacket.h"

#include <netinet/ip_icmp.h>

TimeToLiveExpiredFakeroutePacket::TimeToLiveExpiredFakeroutePacket(u_int16_t localReceivedSourcePort,
								   u_int16_t localReceivedDestinationPort,
								   char* localSpoofedSourceAddress, 
								   char* localSpoofedDestinationAddress) : 
  FakeroutePacket(localReceivedSourcePort, localReceivedDestinationPort,
		  localSpoofedSourceAddress, localSpoofedDestinationAddress)
{}

u_int16_t TimeToLiveExpiredFakeroutePacket::getIcmpType(void) {
  return ICMP_TIME_EXCEEDED;
}

u_int16_t TimeToLiveExpiredFakeroutePacket::getIcmpCode(void) {
  return ICMP_EXC_TTL;
}
