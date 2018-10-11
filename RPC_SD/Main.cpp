#include"Headers.h"
#include "Node.h"
#include"stdafx.h"
using namespace std;

enum { client_op, host_op, exit_opt};

void menu();

int main(){
	int opt = -1;

	//Print menu interface
	menu();

	cin >> opt;

}

void menu() {
	int opt = -1;

	cout << "1 - Connect to Host\n 2-Host a chat\n3-Exit\n";

	cin >> opt;

	switch (opt) {
	case client_op:
		//client();
		printf("Inform host:\n");
		break;

	case host_op:
		//host();
		printf("host yai");
		break;

	case exit_opt:
		exit(0);
		break;
	}
}
