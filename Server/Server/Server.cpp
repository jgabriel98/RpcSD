#include "stdafx.h"
#include<iostream>
#include <rpc/client.h>
int main() {
	rpc::client c("127.0.0.1", 8080);
	int x;
	std::cin >> x;
	return c.call("add", 0, 0).get().as<int>();
}