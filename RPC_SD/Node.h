#pragma once

#include "rpc/this_session.h"
#include <vector>


using namespace std;

class Node: rpc::server{

  private:
	vector<rpc::client *> conexoes_client;


  public:
	//construtor, informa o endereço do Host a qual vai se conectar e porta
	Node(uint16_t port);

	void connectToNode(string const &addr, uint16_t port);

	//envia uma mensagem para o "servidor"/Node correspondente do cliente 'conexoes_client[clientIdx]' .
	void sendMessage(string msg, uint clientIdx);
	void repassMessage(string msg);

	int foo(int a, int b);
};
