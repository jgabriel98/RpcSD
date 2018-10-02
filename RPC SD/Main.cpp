#include"Headers.h"
#include "Client.h"
int main(){
	enum { client_op, host_op };
	int opt = -1;

	//Print menu interface
	menu();

	cin >> opt;

	switch (opt){
	case client:
		client()
		printf("Inform host:\n");



