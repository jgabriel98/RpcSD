#include "HostNode.h"

#include"stdafx.h"
#include"Headers.h"

using namespace std;


HostNode::HostNode(uint16_t port, string nickName) : Node(port, nickName){
	node_graph.clear();
	CreateServer(port);
	matrix_window = new thread(&HostNode::run_matrix_observer, this);
}

HostNode::~HostNode(){
	end_thread = true;
	matrix_print.notify_one();
	matrix_window->join();
}

void HostNode::CreateServer(uint16_t port) {
	//faz os bindings padrões de um Node
	//Node::CreateServer(port);

	//return neighbors Nodes list that the caller must connect
	server_rpc.bind("getNodesToConnect", [this](NodeAddr &newNode) {
		return calculateNodesToConnect(newNode);
	});

	server_rpc.bind("registerNodeToNet", [this](NodeAddr &newNode) {
		this->nodes_in_Network.push_back(newNode); cout << "nodes in Network = " + to_string(nodes_in_Network.size()) + "\n";
	});

	//remove o registro do NodeCaller da rede
	server_rpc.bind("exit", [this](NodeAddr callerAddr) {
		cout << to_string(callerAddr.port) + " called exit() by rpc\n";
		rpc::this_session().post_exit(); //finaliza sessao/conexao com esse client
		//nodes_in_Network.remove(callerAddr);
	});

	nodes_in_Network.push_back(this->serverAddr);
	//server_rpc.suppress_exceptions(false);

	vector <int> n0;
	n0.push_back(0);
	node_graph.push_back(n0);

	priority_list.push_back(make_pair(0, HOST_CONNECTIONS));
	qnode = 1;
}

void HostNode::run_matrix_observer(){
	FILE *pipe = fopen("graph_pipe.file", "w+");
	int pid = system("gnome-terminal -e \"bash -c 'tail -f graph_pipe.file'\" & exit $!");


	unique_lock<mutex> lock(matrix_mtx);
	while(end_thread == false){
		//fprintf(pipe, "\033[2J");
		system("echo $(clear) > graph_pipe.file");
		fprintf(pipe, "Current number of Nodes in net: " YEL("%lu\n"), nodes_in_Network.size());
		
		for (int i = 0; i < node_graph.size(); i++) {
			for (int j = 0; j < node_graph[i].size(); j++) {
				fprintf(pipe, BLU(" %d"), node_graph[i][j]);
			}
			fprintf(pipe, "\n");
		}
		fflush(pipe);
		matrix_print.wait(lock);
	}

	fclose(pipe);

	kill(pid, SIGINT);
}

list<NodeAddr> HostNode::calculateNodesToConnect(NodeAddr newNodeAddr) {
	//List of nodes to connect to

	list<NodeAddr> nodes_to_connect;
	vector<int> edges;

	int qnode = (int)nodes_in_Network.size();

	//adjust the node graph
	for (int i = 0; i < (int)node_graph.size(); i++) {
		node_graph[i].push_back(0);
		edges.push_back(0);
	}
	edges.push_back(0);
	node_graph.push_back(edges);


	//Gets first node from the priority list;
	nodes_to_connect.push_back(nodes_in_Network[priority_list.front().first]);
	priority_list.front().second--;
	node_graph[priority_list.front().first][qnode] = 1;
	node_graph[qnode][priority_list.front().first] = 1;


	//If the first node of the priority list does not have any more slots, remove it
	if (priority_list.front().second <= 0) {
		priority_list.erase(priority_list.begin());
	}

	if (qnode >= NODES_TO_TRIGER_DIJKSTRA) {
		//DIKSTRA

		for (int dijkstra_count = 0; dijkstra_count < NODES_TO_CONNECT_ON_DIJKSRTA; dijkstra_count ++) {

			vector<int> dist;     // The output array.  dist[i] will hold the shortest  distance from src to i 
			vector<bool> sptSet;		// sptSet[i] will be true if vertex i is included in shortest path tree or shortest distance from src to i is finalized 

			for (int i = 0; i < node_graph.size(); i++)
				dist.push_back(INT_MAX), sptSet.push_back(false);

			// Distance of source vertex from itself is always 0 
			dist.back() = 0;
			sptSet.back() = false;

			// Find shortest path for all vertices 
			for (int count = 0; count < qnode; count++)
			{
				// Pick the minimum distance vertex from the set of vertices not 
				// yet processed. u is always equal to src in the first iteration. 

				// Initialize min value 
				int min = INT_MAX, min_index;

				for (int v = 0; v < node_graph.size(); v++) {
					if (sptSet[v] == false && dist[v] <= min) {
						min = dist[v], min_index = v;
					}
				}

				int u = min_index;
				// Mark the picked vertex as processed 
				sptSet[u] = true;

				// Update dist value of the adjacent vertices of the picked vertex. 
				for (int v = 0; v < qnode; v++) {

					// Update dist[v] only if is not in sptSet, there is an edge from  
					// u to v, and total weight of path from src to  v through u is  
					// smaller than current value of dist[v] 
					if (!sptSet[v] &&
						node_graph[u][v] &&
						dist[u] != INT_MAX
						&& dist[u] + node_graph[u][v] < dist[v]) {
						dist[v] = dist[u] + node_graph[u][v];
					}
				}
			}


			//the two most distant nodes that will be added to nodes_to_connect
			int v1 = 0, b1 = 0, c1 = 0;

			//Checks the most distant nodes that are in the priority list
			for (int i = 0; i < priority_list.size(); i++) {
				if (v1 < dist[priority_list[i].first]) {
					b1 = priority_list[i].first;
					v1 = dist[priority_list[i].first];
					c1 = i;
				}
			}
			//Adds the two chosen nodes to the list and adjust the graph
			nodes_to_connect.push_back(nodes_in_Network[b1]);
			node_graph[b1][qnode] = 1;
			node_graph[qnode][b1] = 1;

			//adjust priority list
			priority_list[c1].second--;
			if (priority_list[c1].second <= 0) {
				priority_list.erase(priority_list.begin() + c1);
			}

		}

		//Add new node to priority list
		priority_list.push_back(make_pair(qnode, NODE_CONNECTIONS - NODES_TO_CONNECT_ON_DIJKSRTA - 1));
	}

	else {
		//Add new node to priority list
		priority_list.push_back(make_pair(qnode, NODE_CONNECTIONS - 1));
	}


	//// GABA AQUI
	matrix_print.notify_one();	//notifica a thread que imprime a matriz no outro terminal

	return(nodes_to_connect);

}