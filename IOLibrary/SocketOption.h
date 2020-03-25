#pragma once

enum SocketOption
{
	TCP_NoDelay, //true - Disable Nagle's algorithm
	IPV6_Only // true - Only ipv6 can connect. false = ipv4 and ipv6 can both connect.
};
