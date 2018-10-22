#include"Headers.h"
#include"stdafx.h"

using namespace std;

enum {zero, client_op, host_op, exit_opt};

void menu();
void run_client(uint16_t port);
void run_host(uint16_t port);

int main(){
	int opt = -1;

	//Print menu interface
	menu();

}

void menu() {
	int opt = -1;
	string addr;
	uint16_t port;

	cout << "1 - Connect to Host\n 2-Host a chat\n3-Exit\n";

	cin >> opt;

	switch (opt) {
	case client_op:
			//client();
			cout<<"Inform host port:\n";
			cin >> port;

			run_client(port);
	break;

	case host_op:
		//host();

		cout << "Inform desired port:\n";
		cin >> port;
		
		run_host(port);
		break;

	case exit_opt:
		exit(0);
		break;
	}
}

void run_client(uint16_t port){
	Node node(port);
	thread host_server_run([&node] { node.run(); });
	string mss;
	node.connectToNode(port+1);
	node.conexoes_client[0]->call("ctn",port);
	while (cin >> mss && mss != "-1") {
		node.conexoes_client[0]->call("mss", mss);
	}
}

void run_host(uint16_t port) {
	Host_Node host(port);
	thread host_server_run([&host] { host.run(); });

	string mss;
	cin >> mss;
	while (cin >> mss && mss != "-1") {
		host.conexoes_client[0]->call("mss", mss);
	}
}
