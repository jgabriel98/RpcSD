#pragma once


#include <vector>


using namespace std;

class Node{

  private:
	//vector<rpc::client *> conexoes_client;


  public:
	//construtor, informa o endereço do Host a qual vai se conectar e porta
		Node(uint16_t port, std::shared_ptr<grpc::Channel> channel) : stub_(Greeter::NewStub(grpc::channel));

		void connectToNode(string const &addr, uint16_t port);

		//envia uma mensagem para o "servidor"/Node correspondente do cliente 'conexoes_client[clientIdx]' .
		void sendMessage(string msg, size_t clientIdx);
		void repassMessage(string msg);

		int foo(int a, int b);
};
