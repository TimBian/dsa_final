#include <iostream>
#include "database.h"
using namespace std;

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(0);

	string operation;
	Database database;
	while(cin >> operation) {
		if(operation == "add") database.add();
		else if(operation == "remove") database.remove();
		else if(operation == "longest") database.longest();
		else if(operation == "query") database.query();
	}
}
