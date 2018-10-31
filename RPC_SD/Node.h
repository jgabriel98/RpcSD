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
	
	bool operator==(const NodeAddr &other) const{	//define operador ==, necessário para a função hash usada em unordered_map
		return (ip == other.ip && port == other.port);
	}
};


struct NodeAddr_hash{
	//template <class T1, class T2>
	std::size_t operator() (const NodeAddr &key)const{
		return hash<string>()(key.ip) ^ hash<uint16_t>()(key.port);
		//std::size_t seed = 0;
		//boost::hash_combine(seed, boost::hash_value(key.ip));
		//boost::hash_combine(seed, boost::hash_value(key.port));
		//return seed;
	}
};


class Node{
	protected:

	public:
	NodeAddr serverAddr;	//funciona como identificador do Node, seria mais apropriado chamar de myAddr?
	string name="?";
	
	//mapeia um endereço de nó para o objeto do cliente
	unordered_map<NodeAddr, rpc::client *, NodeAddr_hash> conexoes_client;
	/*recomenda-se usar enviar uma struct NodeAddr para identificar o caller, e dentro das funções Bind usar o NodeAddr resultante para  obter o objeto do cliente (rpc::client)
	  já o segundo (conexoes_client) fora das funções Bind, no código normal mesmo, para acessar diretamente um objeto do cliente através de seu endereço (NodeAddr)
	*/



	rpc::server server_rpc;

	~Node();

	//construtor, informa o endereço do Host a qual vai se conectar e porta
	Node(uint16_t port);

	//realiza os binds do servidor
	void CreateServer(uint16_t port);

	//conecta este nó com os outros Nós (Nodes) necessários (até um maximo de MAXNODES)
	void connectToNodes(uint16_t port);

	//envia uma mensagem para o "servidor"/Node correspondente do cliente 'conexoes_client[clientIdx]' .
	void repassMessage(string msg);
};

#endif
