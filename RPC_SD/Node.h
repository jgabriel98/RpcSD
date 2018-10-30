#ifndef NODE_H
#define NODE_H

#include "Headers.h"

#include "rpc/client.h"
#include "rpc/server.h"
//#include "rpc/this_session.h"

using namespace std;

class Node{
	private:
	uint16_t serverPort;
	void CreateServer(uint16_t port);

	public:
	vector<rpc::client *> conexoes_client;
	rpc::server server_rpc;

	~Node();

	//construtor, informa o endereço do Host a qual vai se conectar e porta
	Node(uint16_t port);

	//conecta este nó com os outros Nós (Nodes) necessários (até um maximo de MAXNODES)
	void connectToNodes(uint16_t port);

	//envia uma mensagem para o "servidor"/Node correspondente do cliente 'conexoes_client[clientIdx]' .
	void repassMessage(string msg);
};

#endif
