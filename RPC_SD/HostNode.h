#ifndef HOST_NODE_H
#define HOST_NODE_H

#include "Headers.h"
#include "Node.h"

#include "rpc/this_session.h"

using namespace std;

class HostNode : public Node {
private:
	thread *matrix_window;
	mutex matrix_mtx;
	condition_variable matrix_print;
	bool end_thread = false;

	vector<NodeAddr> nodes_in_Network;
	//executa o algoritmo para calculo do(s) nó(s) que o novo node deve se conectar
	list<NodeAddr> calculateNodesToConnect(NodeAddr newNodeAddr);

	void run_matrix_observer();

public:
	void CreateServer(uint16_t port);
	HostNode(uint16_t port, string nickName = "");
	~HostNode();

	vector<vector<int>> node_graph;

	vector<pair<int, int>> priority_list;

	int qnode;

};



#endif