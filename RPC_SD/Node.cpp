#include"stdafx.h"
#include"Headers.h"

using namespace std;

Node::~Node() {

}

Node::Node(uint16_t port):server(port){
	//Message
	this->bind("mss", [](std::string message) {cout << message; });

	//Connect To Node
	this->bind("ctn", [this](uint16_t port) {this->connectToNode( port); });
}
	

void Node::connectToNode(uint16_t port){
	conexoes_client.push_back(new rpc::client(IP, port));

}

void Node::sendMessage(string msg, size_t clientIdx){
	conexoes_client[clientIdx]->call("print", msg);
}

void Node::repassMessage(string msg){

}

int Node::foo(int a, int noboB) {
	return a;
}