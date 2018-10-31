#include"stdafx.h"
#include"Headers.h"
#include "Node.h"
#include "rpc/this_session.h"
#include "rpc/rpc_error.h"

using namespace std;

Node::~Node() {
	for(auto &clientPointer: conexoes_client){
		delete clientPointer.second;
	}
}

Node::Node(uint16_t port): server_rpc(port) {
	serverAddr.ip = IP;
	serverAddr.port= port;
	name = "Node"+to_string(port);
	CreateServer(port);
}

void Node::CreateServer(uint16_t port) {
	//Message
	server_rpc.bind("sendMessage", [this](std::string message) { this->repassMessage(message); });

	//Connect To caller Node
	server_rpc.bind("requestConnect", [this](NodeAddr callerAddr) {
		cout<<callerAddr.port<<" asked to conect to him\n";

		auto sucess2 = this->conexoes_client.insert({callerAddr, new rpc::client(callerAddr.ip, callerAddr.port)});
		if(sucess2.second == false){
			cout << "erro ao conectar-se ao Node (NodeAddr (ip e porta) já existe)" << callerAddr.ip<<" - "<< callerAddr.port << endl;
			return; //aqui devemos retornar um rpc::error, implementar depois
		}
	});

	//stop session with caller client
	server_rpc.bind("disconnect", [this](NodeAddr callerAddr) {
		cout<<to_string(callerAddr.port)+" called disconnect() by rpc\n";
		rpc::this_session().post_exit();	//finaliza sessao/conexao com esse client

		auto it = conexoes_client.find(callerAddr);
		if (it != conexoes_client.end()){
			delete it->second;	//destroi objeto rpc::client correspondente ao caller
			conexoes_client.erase(callerAddr); //remove essa conexao da lista de clients
		}
	});
}

//conecta este nó com os outros Nós (Nodes) necessários
void Node::connectToNodes(uint16_t hostPort){
	//conecta-se ao host primeiro, para saber a quem deverá se conectar
	rpc::client hostNode(IP, hostPort);

	try{
		list<NodeAddr> nodesVizinhos = hostNode.call("getCloseNodes", this->serverAddr).as<list<NodeAddr>>();

		for (auto &nodeToConnect : nodesVizinhos){
			rpc::client *newClient = new rpc::client(nodeToConnect.ip, nodeToConnect.port); //cria rpcClient e conecta-se ao objeto servidor do outro nó

			auto sucess = conexoes_client.insert({nodeToConnect, newClient});	//adiciona a lista de clients(Nodes) conectados
			if (sucess.second == false){
				cout << "erro ao conectar-se ao Node (já existe registro em conexoes_client)" << nodeToConnect.port << endl;
				//aqui deverá então tratar desse erro e conectar-se a algum outro, ou ficar sem mesmo?
			}

			//chama procedimento remoto para fazer o outro nó conectar-se a este nó
			newClient->call("requestConnect", this->serverAddr);
		}
		cout<<endl;

		//registra esse Node à lista de nodes, armazenada no Host
		hostNode.call("registerNodeToNet", serverAddr);

		//termina sessao (conexao) com o Host
		hostNode.send("disconnect", serverAddr);
	} catch (rpc::rpc_error &e) {
		std::cout << std::endl << e.what() << std::endl;
		std::cout << "in function " << e.get_function_name() << ": ";

		using err_t = std::tuple<int, std::string>;
		auto err = e.get_error().as<err_t>();
		std::cout << "[error " << std::get<0>(err) << "]: " << std::get<1>(err) << std::endl;
		exit(1);
	}
}


void Node::repassMessage(string msg){
	cout << CYN("") << msg << endl;

	/*
	for(auto &connection: conexoes_client){
		// if( esse cliente ainda nao recebeu essa mensagem / ainda nao foi enviado pra esse cliente ) 
		connection->async_call("sendMessage", msg);
	}
	*/
}

