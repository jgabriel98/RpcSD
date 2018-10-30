#include "Headers.h"
#include "stdafx.h"
#include "Node.h"
#include "HostNode.h"

using namespace std;

enum {zero, client_op, host_op, exit_opt=0};

void menu();
void run_simpleNode(uint16_t myPort, uint16_t hostPort);
void run_host(uint16_t port);

int main(){
	int opt = -1;

	//Print menu interface
	menu();

}

void menu() {
	int opt = -1;
	string addr;
	uint16_t myPort, hostPort;

	cout << " 1 - Connect to Host\n 2-Host a chat\n0-Exit\n";

	cin >> opt;

	switch (opt) {
	case client_op:
		//client();
		cout<<"Inform your port:\n";
		cin >> myPort;
		cout << "Inform host port:\n";
		cin >> hostPort;

		run_simpleNode(myPort, hostPort);
	break;

	case host_op:
		//host();

		cout << "Inform desired port:\n";
		cin >> myPort;
		
		run_host(myPort);
		break;

	case exit_opt:
		exit(0);
		break;
	}
}

/* roda processo do client.
 * port: porta deste nó, onde a 
 */
void run_simpleNode(uint16_t myPort, uint16_t hostPort){
	Node node(myPort);
	string mss;

	//lança X nova(s) thread(s) (nesse caso 1) para rodar o servidor e atender chamadas das funções amarradas (bind)
	node.server_rpc.async_run(1);

	node.connectNodes(hostPort); //realiza conexão entre os dois nós (em ambas direções)
	/*while (cin >> mss && mss != "-1") {			//Antigo código seu - (comentário por: João Gabriel)
		node.conexoes_client[0]->call("sendMsg", mss);
	}*/
	cout<<GRN("Conected to Host");
	do{
		getline(cin, mss);
		//node.conexoes_client[0]->call("sendMsg", mss); 		//Antigo código seu - (comentário por: João Gabriel)
		for(auto &connectedNode: node.conexoes_client){
			connectedNode->call("sendMsg", mss);
		}
	}while(mss != "-1" && mss != "exit");
}

void run_host(uint16_t port) {
	HostNode host(port);

	//lança X nova(s) thread(s) (nesse caso 1) para rodar o servidor e atender chamadas das funções amarradas (bind)
	host.server_rpc.async_run(1);

	string mss;
	/*cin >> mss;							//Antigo código seu - (comentário por: João Gabriel)
	while (cin >> mss && mss != "-1") {
		host.conexoes_client[0]->call("sendMsg", mss);
	}*/
	do{
		getline(cin, mss);
		//host.conexoes_client[0]->call("sendMsg", mss); 		//Antigo código seu - (comentário por: João Gabriel)
		for(auto &connectedNode: host.conexoes_client){
			connectedNode->call("sendMsg", mss);
		}
	}while(mss != "-1" && mss != "sair");
}
