#include "Headers.h"

#include "rpc/client.h"
#include "rpc/server.h"
//#include "rpc/this_session.h"

using namespace std;

class Node{

public:
	vector<rpc::client *> conexoes_client;

	rpc::server* server_rpc;

	~Node();

	void CreateServer(uint16_t port);

	//construtor, informa o endereço do Host a qual vai se conectar e porta
	Node(uint16_t port);

	void connectToNode(uint16_t port);

	//envia uma mensagem para o "servidor"/Node correspondente do cliente 'conexoes_client[clientIdx]' .
	void sendMessage(string msg, size_t clientIdx);

	void repassMessage(string msg);

	int foo(int a, int b);
};
