#include"stdafx.h"
#include"Headers.h"

using namespace std;

Node::~Node() {

}

Node::Node(uint16_t port):server(port){
	//Message
	this->bind("mss", [](std::string message) {cout << message << endl; });

	//Connect To Node
	this->bind("ctn", [this](string &addr, uint16_t port) {this->connectToNode(addr, port); });
}
	

void Node::connectToNode(string &addr, uint16_t port){
	conexoes_client.push_back(new rpc::client(addr, port));

}

void Node::sendMessage(string msg, size_t clientIdx){
	conexoes_client[clientIdx]->call("print", msg);
}

void Node::repassMessage(string msg){

}

int Node::foo(int a, int noboB) {
	return a;
}