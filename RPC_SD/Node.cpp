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

Node::Node(uint16_t port, string nickName): server_rpc(port) {
	serverAddr.ip = IP;
	serverAddr.port= port;
	if (nickName == "")
		_name = "guest " + serverAddr.ip + to_string(port);
	else
		_name = nickName;
	CreateServer(port);
}


void Node::CreateServer(uint16_t port) {
	//Message
	server_rpc.bind("sendMessage", [this](NodeAddr callerAddr, MSG_PACKET message) { this->repassMessage(callerAddr, message); });

	//Connect To caller Node
	server_rpc.bind("requestConnect", [this](NodeAddr callerAddr) {
		cout<<callerAddr.port<<" asked to conect to him\n";

		connections_mutex.lock();
		auto sucess2 = this->conexoes_client.insert({callerAddr, new rpc::client(callerAddr.ip, callerAddr.port)});
		connections_mutex.unlock();

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
		list<NodeAddr> nodesConnected = hostNode.call("getNodesToConnect", this->serverAddr).as<list<NodeAddr>>();
		//termina sessao (conexao) com o Host
		//hostNode.call("disconnect", serverAddr);

		for (auto &nodeToConnect : nodesConnected){
			rpc::client *newClient = new rpc::client(nodeToConnect.ip, nodeToConnect.port); //cria rpcClient e conecta-se ao objeto servidor do outro nó

			connections_mutex.lock();
			auto sucess = conexoes_client.insert({nodeToConnect, newClient});	//adiciona a lista de clients(Nodes) conectados
			connections_mutex.unlock();
			
			if (sucess.second == false){
				cout << "erro ao conectar-se ao Node (já existe registro em conexoes_client)" << nodeToConnect.port << endl;
				//aqui deverá então tratar desse erro e conectar-se a algum outro, ou ficar sem mesmo?
			}

			//chama procedimento remoto para fazer o outro nó conectar-se a este nó
			newClient->call("requestConnect", this->serverAddr);
		}
		cout<<endl;

		//rpc::client temp(IP, hostPort);
		//registra esse Node à lista de nodes, armazenada no Host
		hostNode.call("registerNodeToNet", serverAddr);

	} catch (rpc::rpc_error &e) {
		std::cout << std::endl << e.what() << std::endl;
		std::cout << "in function " << e.get_function_name() << ": ";

		using err_t = std::tuple<int, std::string>;
		auto err = e.get_error().as<err_t>();
		std::cout << "[error " << std::get<0>(err) << "]: " << std::get<1>(err) << std::endl;
		exit(1);
	}
}


void Node::repassMessage(NodeAddr callerAddr, MSG_PACKET packet){
	MSG_ID msg_code = packet.code;

	// check if there the key 'msg_id_senderAddr' exist in the received_msg_counter map
	counters_mutex.lock();
	bool is_firstMsg = ( received_msg_counter.find(msg_code.senderAddr) == received_msg_counter.end() ) ? true : false;
	counters_mutex.unlock();

	if (is_firstMsg){	//if there's still no counter registred for this sender Node
		cout << CYN("") << packet.msg << endl;

		received_msg_counter.insert({msg_code.senderAddr, msg_code.msgCounter}); //then create it and set the counter value

		connections_mutex.lock();
		for(auto &connection: conexoes_client)
			if(callerAddr != connection.first)	connection.second->async_call("sendMessage", serverAddr, packet);
			
		connections_mutex.unlock();

	}else{
		//se a mensagem não passou por esse Node ainda, a envia para os vizinhos
		if ( received_msg_counter.at(msg_code.senderAddr) < msg_code.msgCounter){
			cout << CYN("") << packet.msg << endl;

			connections_mutex.lock();
			for(auto &connection: conexoes_client)
				if (callerAddr != connection.first)	connection.second->async_call("sendMessage", serverAddr, packet);
				
			connections_mutex.unlock();

			received_msg_counter.at(msg_code.senderAddr) = msg_code.msgCounter;	//atualiza o contador
		}
	}

}