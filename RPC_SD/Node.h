#include"Headers.h"

using namespace std;

class Node: public rpc::server{

  public:
	vector<rpc::client *> conexoes_client;

  public:
	  ~Node();

  public:
	//construtor, informa o endereço do Host a qual vai se conectar e porta
	  Node(uint16_t port);

	void connectToNode(uint16_t port);

	//envia uma mensagem para o "servidor"/Node correspondente do cliente 'conexoes_client[clientIdx]' .
	void sendMessage(string msg, size_t clientIdx);

	void repassMessage(string msg);

	int foo(int a, int b);
};
