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
	cout<<"connected to "<<node.conexoes_client.size()<<" Node(s)\n";
	cout<<"conection(s) state: ";
	for (auto &clientConection : node.conexoes_client){
		cout<<clientConection.first.port<<"-";
		switch (clientConection.second->get_connection_state()) {
			case rpc::client::connection_state::initial:
			cout<<YEL("initial\t");
			break;

			case rpc::client::connection_state::connected:
			cout << GRN("connected\t");
			break;

			case rpc::client::connection_state::disconnected:
			cout << RED("disconnected\t");
			break;

			case rpc::client::connection_state::reset:
			cout << MAG("reset\t");
			break;
		}
	}
	//limpando o buffer cin
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	cout << endl;
	bool leave = false;
	while(!leave){
		getline(cin, mss);
		string msgLog = getTimeLog() +
						BOLD(WHT( " "+node.name + ": " )); //nome do usuário, ou IP, o que quiser
		
		if (mss == "-1" || mss == "sair" || mss == "exit"){
			mss = YEL(""+node.name + " is leaving!");
			leave = true;
		}

		for (auto &connectedNode : node.conexoes_client){
			connectedNode.second->async_call("sendMessage", msgLog + mss);
		}
	}

	for (auto &connectedNode : node.conexoes_client)
		connectedNode.second->send("disconnect", node.serverAddr);		//faz os nodes conectados se desconectarem de vc

	rpc::client tempConection(IP, hostPort);
	tempConection.call("exit", node.serverAddr);		//envia sinal ao host informando que está saindo da rede;
}

void run_host(uint16_t port) {
	HostNode host(port);
	string mss;
	//lança X nova(s) thread(s) (nesse caso 1) para rodar o servidor e atender chamadas das funções amarradas (bind)
	host.server_rpc.async_run(1);

	//limpando o buffer cin
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	cout<<endl;
	bool leave = false;
	while(!leave){
		getline(cin, mss);
		string msgLog = getTimeLog() +
						BOLD(WHT( " "+host.name+"(Host): ")); //nome do usuário, ou IP, o que quiser

		if (mss == "-1" || mss == "sair" || mss == "exit"){
			mss = host.name+"(host) is leaving!";
			leave = true;
		}

		for(auto &connectedNode: host.conexoes_client){
			connectedNode.second->async_call("sendMessage", msgLog + mss);
		}
	};
	
	host.server_rpc.stop();
}




string getTimeLog(){
	string timeLog;

	time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());
	tm local_tm = *localtime(&tt); //get local time, insteand of UTC time --> *gmtime(&tt);

	//formata as strings
	stringstream minutes, hours, day, month;
	hours << setw(2) << setfill('0') << to_string(local_tm.tm_hour);
	minutes << setw(2)<<setfill('0')<<to_string(local_tm.tm_min);
	day << setw(2) << setfill('0') << to_string(local_tm.tm_mday);
	month << setw(2) << setfill('0') << to_string(local_tm.tm_mon);

	timeLog = BRIGHT(CYN(
		"[" + hours.str() + ":" + minutes.str() + ", " +							   //hora
		day.str() + "/" + month.str() + "/" + to_string(local_tm.tm_year + 1900) + "]" //data
	));
	return timeLog;
}
