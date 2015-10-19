// Copyright (c) 2012, NetEase Inc. All rights reserved.
//
// Wang Rongtao <rtwang@corp.netease.com>
// 2012/3/30
//
// the proxy defination

#ifndef NET_BASE_PROXY_H_
#define NET_BASE_PROXY_H_

#include <string>
#include "base/base_types.h"
#include "net/base/net_export.h"
#include "third_party/curl/include/curl/curl.h"

namespace net
{

enum ProxyType
{
	ProxyNone = -1,
	ProxyHttp11 =  CURLPROXY_HTTP,
	ProxyHttp10 = CURLPROXY_HTTP_1_0,
	ProxySock4 = CURLPROXY_SOCKS4,
	ProxySock5 = CURLPROXY_SOCKS5,
	ProxySock4a = CURLPROXY_SOCKS4A,
	ProxySock5HostName = CURLPROXY_SOCKS5_HOSTNAME,
};

struct NET_EXPORT Proxy
{
	uint16_t port;
	ProxyType type;
	std::string host;
	std::string user;
	std::string pass;

	Proxy() : port(0), type(ProxyNone) {}
	bool IsEmpty() const { return type == ProxyNone; }
	void Clear()
	{
		port = 0;
		type = ProxyNone;
		host.clear();
		user.clear();
		pass.clear();
	}
};

} // namespace net

#endif // NET_BASE_PROXY_H_
