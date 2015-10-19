// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Wang Rongtao <rtwang@corp.netease.com>
// 2013/9/3
//
// Network utilities

#ifndef NET_BASE_NET_UTIL_H_
#define NET_BASE_NET_UTIL_H_

#include <vector>
#include "base/base_types.h"
#include "net/base/net_export.h"
#if defined(OS_WIN)
#include <ws2tcpip.h>
#elif defined(OS_POSIX)
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

namespace net
{

// Returns the string representation of an address, like "192.168.0.1".
// Returns empty string on failure.
NET_EXPORT std::string NetAddressToString(const struct addrinfo* net_address);
NET_EXPORT std::string NetAddressToString(const struct sockaddr* net_address,
	socklen_t address_len);

// Same as NetAddressToString, but additionally includes the port number. For
// example: "192.168.0.1:99" or "[::1]:80".
NET_EXPORT std::string NetAddressToStringWithPort(
	const struct addrinfo* net_address);
NET_EXPORT std::string NetAddressToStringWithPort(
	const struct sockaddr* net_address,
	socklen_t address_len);

// Returns the hostname of the current system. Returns empty string on failure.
NET_EXPORT std::string GetHostName();

// Set socket to non-blocking mode
NET_EXPORT int SetNonBlocking(int fd);

// IPAddressNumber is used to represent an IP address's numeric value as an
// array of bytes, from most significant to least significant. This is the
// network byte ordering.
//
// IPv4 addresses will have length 4, whereas IPv6 address will have length 16.
typedef std::vector<unsigned char> IPAddressNumber;
typedef std::vector<IPAddressNumber> IPAddressList;

static const size_t kIPv4AddressSize = 4;
static const size_t kIPv6AddressSize = 16;

// Makes a copy of |info|. The dynamically-allocated parts are copied as well.
// If |recursive| is true, chained entries via ai_next are copied too.
// The copy returned by this function should be freed using
// FreeCopyOfAddrinfo(), and NOT freeaddrinfo().
struct addrinfo* CreateCopyOfAddrinfo(const struct addrinfo* info,
	bool recursive);

// Frees an addrinfo that was created by CreateCopyOfAddrinfo().
void FreeCopyOfAddrinfo(struct addrinfo* info);

// Returns the port field of the sockaddr in |info|.
const uint16_t* GetPortFieldFromAddrinfo(const struct addrinfo* info);
uint16_t* GetPortFieldFromAddrinfo(struct addrinfo* info);

// Returns the value of |info's| port (in host byte ordering).
uint16_t GetPortFromAddrinfo(const struct addrinfo* info);

// Same except for struct sockaddr.
const uint16_t* GetPortFieldFromSockaddr(const struct sockaddr* address,
	socklen_t address_len);
NET_EXPORT int GetPortFromSockaddr(const struct sockaddr* address,
	socklen_t address_len);

// Sets every addrinfo in the linked list |head| as having a port field of
// |port|.
NET_EXPORT void SetPortForAllAddrinfos(struct addrinfo* head,
	uint16_t port);

}

#endif // NET_BASE_NET_UTIL_H_