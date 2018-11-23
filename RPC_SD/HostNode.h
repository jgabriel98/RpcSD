#ifndef HOST_NODE_H
#define HOST_NODE_H

#include "Headers.h"
#include "Node.h"

#include "rpc/this_session.h"

using namespace std;

class HostNode : public Node {
  private:
    vector<NodeAddr> nodes_in_Network;
    //executa o algoritmo para calculo do(s) nó(s) que o novo node deve se conectar
    list<NodeAddr> calculateNodesToConnect(NodeAddr newNodeAddr);

  public:
    void CreateServer(uint16_t port);
    HostNode(uint16_t port, string nickName = "");

  public:
    vector<vector<int>> node_graph;

  public:
    vector<pair<int, int>> priority_list;

  public:
    int qnode;

};



#endif