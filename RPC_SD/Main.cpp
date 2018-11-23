#include "Headers.h"
#include "stdafx.h"
#include "Node.h"
#include "HostNode.h"

/*** COMO FECHAR UM SOCKET MANUALMENTE PELO TERMINAL:  fuser -k -n tcp numero_Da_porta     *****/

using namespace std;

enum {zero, client_op, host_op, exit_opt=0};

void menu();
void run_simpleNode(string nickName, uint16_t myPort, uint16_t hostPort);
void run_host(string nickName, uint16_t port);

void printConnectionStates(Node &node);
string getTimeLog();

int main(){
	//Print menu interface
	menu();

}

void menu() {
	int opt = -1;
	string addr, name;
	uint16_t myPort, hostPort;

	cout << "enter your nickName(leave blank to default): ";
	cin >> name;
	

	cout << " 1 - Connect to Host\n 2 - Host a chat\n0-Exit\n";
	cin >> opt;

	switch (opt) {
	case client_op:
		cout<<"Inform your port:\n";
		cin >> myPort;
		cout << "Inform host port:\n";
		cin >> hostPort;

		run_simpleNode(name, myPort, hostPort);
	break;

	case host_op:
		cout << "Inform desired port:\n";
		cin >> myPort;
		
		run_host(name, myPort);
		break;

	case exit_opt:
		exit(0);
		break;
	}
}

/*--roda processo do Node (nao Host);
 * myPort: porta deste node, serve como identificador deste Node
 * hostPort: porta do Node host, serve como identificador da "sala de chat" 
 */
void run_simpleNode(string name, uint16_t myPort, uint16_t hostPort){
	Node node(myPort, name);
	string mss;
	//lança X nova(s) thread(s) (nesse caso 1) para rodar o servidor e atender chamadas das funções amarradas (bind)
	node.server_rpc.async_run(1);

	node.connectToNodes(hostPort); // descobre quais e conecta aos Nodes(em ambas direções, isto é, os nodes também se conectam a vc)

	printConnectionStates(node);
	//limpando o buffer cin
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	cout << endl;
	bool leave = false;
	while(!leave){
		getline(cin, mss);
		string msgLog = getTimeLog() +
						BOLD(WHT( " "+node.name() + ": " )); //nome do usuário, ou IP, o que quiser
		
		if (mss == "-1" || mss == "sair" || mss == "exit"){
			mss = YEL(""+node.name() + " is leaving!");
			leave = true;
		}

		MSG_ID msg_code = {node.serverAddr, node.msgCounter};
		MSG_PACKET packet = {msg_code, msgLog + mss};
		for (auto &connectedNode : node.conexoes_client){
			connectedNode.second->async_call("sendMessage", node.serverAddr, packet);
		}
		node.msgCounter++;
	}


/*********SE DESCONECTANDO DOS NODES E DA REDE, SEMPRE TEM Q FAZER ISSO PRA N DEIXAR VESTIGIOS****************/
	for (auto &connectedNode : node.conexoes_client)
		connectedNode.second->send("disconnect", node.serverAddr);		//faz os nodes conectados se desconectarem de vc

	rpc::client tempConection(IP, hostPort);
	tempConection.call("exit", node.serverAddr);		//envia sinal ao host informando que está saindo da rede;

}

/*--roda processo do Node Host, que vai hostear a sala;
 * port: porta deste node, serve como identificador deste Node e da sala de chat (na qual outros Nodes irão se conectar)
 */
void run_host(string name, uint16_t port) {
	HostNode host(port, name);
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
						BOLD(WHT( " "+host.name()+"(Host): ")); //nome do usuário, ou IP, o que quiser

		if (mss == "-1" || mss == "sair" || mss == "exit"){
			mss = host.name()+"(host) is leaving!";
			leave = true;
		}

		MSG_ID msg_code = {host.serverAddr, host.msgCounter};
		MSG_PACKET packet = {msg_code, msgLog + mss};
		for(auto &connectedNode: host.conexoes_client){
			connectedNode.second->async_call("sendMessage", host.serverAddr, packet);
		}
		host.msgCounter++;
	};

	/**
	 * seria legal depois implementarmos uma maneira de mudar o host da sala em runtime (claro q isso n seria tao simples)
	 */
	
	host.server_rpc.stop();
}


/*--Imprime no terminal os estados atuais de conexões do Node
nota: utiliza apenas o objeto conexoes_client para isso, pois não tem como descobrir quais clientes se conectaram num rpc::server.
*/
void printConnectionStates(Node &node){
	cout << "connected to " << node.conexoes_client.size() << " Node(s)\n";
	cout << "conection(s) state: ";
	for (auto &clientConection : node.conexoes_client){
		cout << clientConection.first.port << "-";
		switch (clientConection.second->get_connection_state())
		{
		case rpc::client::connection_state::initial:
			cout << YEL("initial\t");
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
}


/*--formata string para log de data e hora
*/
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
		"["+
		day.str() + "/" + month.str() + "/" + to_string(local_tm.tm_year + 1900) + //data
		", " +
		hours.str() + ":" + minutes.str() +									   //hora
		"]"));
	return timeLog;
}
