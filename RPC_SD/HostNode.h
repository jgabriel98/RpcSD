#ifndef HOST_NODE_H
#define HOST_NODE_H

#include "Headers.h"
#include "Node.h"

using namespace std;

class HostNode : public Node {
   public:
	HostNode(uint16_t port);
};

#endif