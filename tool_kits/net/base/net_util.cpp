// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Wang Rongtao <rtwang@corp.netease.com>
// 2013/9/3
//
// Network utilities

#include "net/base/net_util.h"

#include <assert.h>
#include "base/util/string_util.h"

namespace net
{

std::string NetAddressToString(const struct addrinfo* net_address)
{
	return NetAddressToString(net_address->ai_addr, net_address->ai_addrlen);
}

std::string NetAddressToString(
	const struct sockaddr* net_address, socklen_t address_len)
{
	// This buffer is large enough to fit the biggest IPv6 string.
	char buffer[INET6_ADDRSTRLEN];

	int result = getnameinfo(net_address, address_len, buffer, sizeof(buffer),
		NULL, 0, NI_NUMERICHOST);

	if (result != 0)
		buffer[0] = '\0';

	return std::string(buffer);
}

std::string NetAddressToStringWithPort(const struct addrinfo* net_address)
{
	return NetAddressToStringWithPort(
		net_address->ai_addr, net_address->ai_addrlen);
}
std::string NetAddressToStringWithPort(
	const struct sockaddr* net_address, socklen_t address_len)
{
	std::string ip_address_string = NetAddressToString(net_address, address_len);
	if (ip_address_string.empty())
		return std::string();  // Failed.

	int port = GetPortFromSockaddr(net_address, address_len);

	if (ip_address_string.find(':') != std::string::npos) {
		// Surround with square brackets to avoid ambiguity.
		return nbase::StringPrintf("[%s]:%d", ip_address_string.c_str(), port);
	}

	return nbase::StringPrintf("%s:%d", ip_address_string.c_str(), port);
}

std::string GetHostName()
{
	// Host names are limited to 255 bytes.
	char buffer[256];
	int result = gethostname(buffer, sizeof(buffer));
	if (result != 0)
		buffer[0] = '\0';
	return std::string(buffer);
}

int SetNonBlocking(int fd)
{
#if defined(OS_WIN)
	unsigned long no_block = 1;
	return ioctlsocket(fd, FIONBIO, &no_block);
#elif defined(OS_POSIX)
	int flags = fcntl(fd, F_GETFL, 0);
	if (-1 == flags)
		return flags;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#endif
}

addrinfo* CreateCopyOfAddrinfo(const struct addrinfo* info, bool recursive)
{
	assert(info);
	struct addrinfo* copy = new addrinfo;

	// Copy all the fields (some of these are pointers, we will fix that next).
	memcpy(copy, info, sizeof(addrinfo));

	// ai_canonname is a NULL-terminated string.
	if (info->ai_canonname) {
		copy->ai_canonname = _strdup(info->ai_canonname);
	}

	// ai_addr is a buffer of length ai_addrlen.
	if (info->ai_addr) {
		copy->ai_addr = reinterpret_cast<sockaddr *>(new char[info->ai_addrlen]);
		memcpy(copy->ai_addr, info->ai_addr, info->ai_addrlen);
	}

	// Recursive copy.
	if (recursive && info->ai_next)
		copy->ai_next = CreateCopyOfAddrinfo(info->ai_next, recursive);
	else
		copy->ai_next = NULL;

	return copy;
}

void FreeCopyOfAddrinfo(struct addrinfo* info)
{
	assert(info);
	if (info->ai_canonname)
		free(info->ai_canonname);  // Allocated by strdup.

	if (info->ai_addr)
		delete [] reinterpret_cast<char*>(info->ai_addr);

	struct addrinfo* next = info->ai_next;

	delete info;

	// Recursive free.
	if (next)
		FreeCopyOfAddrinfo(next);
}

// Returns the port field of the sockaddr in |info|.
uint16_t* GetPortFieldFromAddrinfo(struct addrinfo* info) {
	const struct addrinfo* const_info = info;
	const uint16_t* port_field = GetPortFieldFromAddrinfo(const_info);
	return const_cast<uint16_t*>(port_field);
}

const uint16_t* GetPortFieldFromAddrinfo(const struct addrinfo* info)
{
	assert(info);
	const struct sockaddr* address = info->ai_addr;
	assert(address);
	assert(info->ai_family == address->sa_family);
	return GetPortFieldFromSockaddr(address, info->ai_addrlen);
}

uint16_t GetPortFromAddrinfo(const struct addrinfo* info)
{
	const uint16_t* port_field = GetPortFieldFromAddrinfo(info);
	if (!port_field)
		return -1;
	return ntohs(*port_field);
}

const uint16_t* GetPortFieldFromSockaddr(
	const struct sockaddr* address, socklen_t address_len)
{
	if (address->sa_family == AF_INET) {
		assert(sizeof(sockaddr_in) <= static_cast<size_t>(address_len));
		const struct sockaddr_in* sockaddr =
			reinterpret_cast<const struct sockaddr_in*>(address);
		return &sockaddr->sin_port;
	} else if (address->sa_family == AF_INET6) {
		assert(sizeof(sockaddr_in6) <= static_cast<size_t>(address_len));
		const struct sockaddr_in6* sockaddr =
			reinterpret_cast<const struct sockaddr_in6*>(address);
		return &sockaddr->sin6_port;
	} else {
		assert(false);
		return NULL;
	}
}

int GetPortFromSockaddr(const struct sockaddr* address, socklen_t address_len)
{
	const uint16_t* port_field = GetPortFieldFromSockaddr(address, address_len);
	if (!port_field)
		return -1;
	return ntohs(*port_field);
}

// Assign |port| to each address in the linked list starting from |head|.
void SetPortForAllAddrinfos(struct addrinfo* head, uint16_t port)
{
	assert(head);
	for (struct addrinfo* ai = head; ai; ai = ai->ai_next) {
		uint16_t* port_field = GetPortFieldFromAddrinfo(ai);
		if (port_field)
			*port_field = htons(port);
	}
}

} // net
