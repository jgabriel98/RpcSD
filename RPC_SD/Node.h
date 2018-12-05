#ifndef NODE_H
#define NODE_H

#include "Headers.h"

#include "rpc/client.h"
#include "rpc/server.h"
#include "rpc/msgpack.hpp"

//#include "rpc/this_session.h"

using namespace std;

struct NodeAddr{
	string ip;
	uint16_t port;
	
	MSGPACK_DEFINE_ARRAY(ip, port)	//adiciona infomação sobre esse tipo de dado ao rpc, para poder ser transmitido corretamente
	
	bool operator == (const NodeAddr &other) const{	//define operador ==, necessário para a função hash usada em unordered_map
		return (ip == other.ip && port == other.port);
	}
	bool operator != (const NodeAddr &other) const{	//define operador ==, necessário para a função hash usada em unordered_map
		return !(*this==other);
	}
	bool operator < (const NodeAddr &other) const{
		if (ip == other.ip)	return port < other.port;
		
		return ip < other.ip;
	}
};

struct MSG_ID{
	struct NodeAddr senderAddr;
	unsigned short msgCounter;		//act like a unique message code for the Node who sent it/make it

	MSGPACK_DEFINE_ARRAY(senderAddr, msgCounter) // adiciona/registra esse tipo de dado ao rpc.
};

struct MSG_PACKET{
	struct MSG_ID code;
	string msg;

	MSGPACK_DEFINE_ARRAY(code, msg) // adiciona/registra esse tipo de dado ao rpc.
};

//função hash para o tipo definido por usuário (strcut NodeAddr) funcionar como chave
struct NodeAddr_hash{
	std::size_t operator() (const NodeAddr &key)const{
		return hash<string>()(key.ip) ^ hash<uint16_t>()(key.port);
	}
};


class Node{
	protected:
	string _name;
	map<NodeAddr, unsigned short> received_msg_counter;		//counters for the received messages, there's a counter for each Node in network (excluding itself)
	std::mutex connections_mutex;
	std::mutex counters_mutex;

	public:
	rpc::server server_rpc;
	NodeAddr serverAddr;	//funciona como identificador do Node, seria mais apropriado chamar de myAddr?
	unsigned short msgCounter = 0;		//message counter, for this node itself.
	
	//mapeia um endereço de nó para o objeto do cliente
	unordered_map<NodeAddr, rpc::client *, NodeAddr_hash> conexoes_client;
	/*recomenda-se enviar uma struct NodeAddr para identificar o caller*/

//ps: sim, precisamos do unordered_map, pra pordermos acessar o cliente diretamente e também garantir que só existe um registro para cada NodeAddr



	~Node();

	//construtor, informa o endereço do Host a qual vai se conectar e porta
	Node(uint16_t port, string nickName="");

	const string& name(){return _name;}
	//realiza os binds do servidor
	void CreateServer(uint16_t port);

	//conecta este nó com os outros Nós (Nodes) necessários (até um maximo de MAXNODES)
	void connectToNodes(uint16_t port);

	//envia uma mensagem para o "servidor"/Node correspondente do cliente 'conexoes_client[clientIdx]' .
	void repassMessage(NodeAddr senderAddr, struct MSG_PACKET packet);
};

#endif
