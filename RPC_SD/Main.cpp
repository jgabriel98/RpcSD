#include "Headers.h"
#include "stdafx.h"
#include "Node.h"
#include "HostNode.h"

/*** COMO FECHAR UM SOCKET MANUALMENTE PELO TERMINAL:  fuser -k -n tcp numero_Da_porta     *****/

using namespace std;

enum {zero, client_op, host_op, exit_opt=0};

void menu();
void run_simpleNode(uint16_t myPort, uint16_t hostPort);
void run_host(uint16_t port);
string getTimeLog();

int main(){
	int opt = -1;

	//Print menu interface
	menu();

}

void menu() {
	int opt = -1;
	string addr;
	uint16_t myPort, hostPort;

	cout << " 1 - Connect to Host\n 2 - Host a chat\n0-Exit\n";

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

	node.connectToNodes(hostPort); //realiza conexão entre os dois nós (em ambas direções)

	//cout<<GRN("Conected to Host")<<endl;
	cout<<"conection state: ";
	switch (node.conexoes_client.back()->get_connection_state()){
		case rpc::client::connection_state::initial:
		cout<<YEL("initial\n");
		break;

		case rpc::client::connection_state::connected:
		cout << GRN("connected\n");
		break;

		case rpc::client::connection_state::disconnected:
		cout << RED("disconnected\n");
		break;

		case rpc::client::connection_state::reset:
		cout << MAG("reset\n");
		break;
	}
	//limpando o buffer cin
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	do{
		getline(cin, mss);
		string msgLog = getTimeLog() +
						BOLD(WHT(" Node: ")); //nome do usuário, ou IP, o que quiser

		for(auto &connectedNode: node.conexoes_client){
			connectedNode->async_call("sendMessage", msgLog + mss);
		}
	}while(mss != "-1" && mss != "sair" && mss != "exit");
}

void run_host(uint16_t port) {
	HostNode host(port);
	string mss;
	//lança X nova(s) thread(s) (nesse caso 1) para rodar o servidor e atender chamadas das funções amarradas (bind)
	host.server_rpc.async_run(1);

	//limpando o buffer cin
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	do{
		getline(cin, mss);
		string msgLog = getTimeLog() +
						BOLD(WHT(" Host: ")); //nome do usuário, ou IP, o que quiser

		for(auto &connectedNode: host.conexoes_client){
			connectedNode->async_call("sendMessage", msgLog + mss);
		}
	}while(mss != "-1" && mss != "sair" && mss != "exit");
	
	host.server_rpc.stop();
}


string getTimeLog(){
	string timeLog;

	time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());
	tm local_tm = *localtime(&tt); //get local time, insteand of UTC time --> *gmtime(&tt);

	timeLog = BRIGHT(CYN(
		"[" + to_string(local_tm.tm_hour) + ":" + to_string(local_tm.tm_min) + ", " +									//hora
		to_string(local_tm.tm_mday) + "/" + to_string(local_tm.tm_mon) + "/" + to_string(local_tm.tm_year + 1900) + "]" //data
		));
	return timeLog;
}
