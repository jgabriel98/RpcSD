#include "HostNode.h"

#include"stdafx.h"
#include"Headers.h"

using namespace std;

HostNode::HostNode(uint16_t port) : Node(port) {
    CreateServer(port);
}

void HostNode::CreateServer(uint16_t port){
    //faz os bindings padrões de um Node
    //Node::CreateServer(port);

    //return neighbors Nodes list that the caller must connect
    server_rpc.bind("getCloseNodes", [this](NodeAddr &newNode) { return calculateNodesToConnect(newNode); });

    server_rpc.bind("registerNodeToNet", [this](NodeAddr &newNode){ this->nodes_in_Network.push_back(newNode); cout <<"nodes in Network = "+to_string(nodes_in_Network.size())+"\n";});

    //remove o registro do NodeCaller da rede
    server_rpc.bind("exit", [this](NodeAddr callerAddr) {
        cout<<to_string(callerAddr.port)+" called exit() by rpc\n";
        rpc::this_session().post_exit(); //finaliza sessao/conexao com esse client
        nodes_in_Network.remove(callerAddr);
    });

    nodes_in_Network.push_back(this->serverAddr);
    //server_rpc.suppress_exceptions(false);
}

list<NodeAddr> HostNode::calculateNodesToConnect(NodeAddr newNodeAddr){
    //aqui deve fazer o calculo e determinar quais nós o novo Node vai se conectar
    //cout << "chamou calculateNodesToConnect-->size = "+to_string(nodes_in_Network.size())+"\n";
    return this->nodes_in_Network;
}