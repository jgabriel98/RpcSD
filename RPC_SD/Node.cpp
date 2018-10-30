#include"stdafx.h"
#include"Headers.h"
#include "Node.h"

using namespace std;

Node::~Node() {
	for(auto &clientPointer: conexoes_client){
		delete clientPointer;
	}
}

Node::Node(uint16_t port): server_rpc(port) {
	serverPort = port;
	CreateServer(serverPort);
}

void Node::CreateServer(uint16_t port) {
	//Message
	server_rpc.bind("sendMessage", [this](std::string message) { this->repassMessage(message); });

	//Connect To Node
	server_rpc.bind("requestConnect", [this](uint16_t port) { this->conexoes_client.push_back(new rpc::client(IP, port)); });
}

void Node::connectNodes(uint16_t hostPort){
	//cria rpcClient e conecta-se ao objeto servidor do outro nó
	conexoes_client.push_back(new rpc::client(IP, hostPort));
	
	//chama procedimento remoto para fazer o outro nó conectar-se a este nó
	conexoes_client.back()->call("requestConnect", this->serverPort);
}


void Node::repassMessage(string msg){
	cout << msg << endl;
	//conexoes_client[clientIdx]->call("sendMessage", msg);

	for(auto &connection: conexoes_client){
		/* if( esse cliente ainda nao recebeu essa mensagem / ainda nao foi enviado pra esse cliente ) */
		connection->call("sendMessage", msg);
	}
}

