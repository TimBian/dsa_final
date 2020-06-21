#include <iostream>
#include <unordered_map>
#include <thread>
#include <chrono>
#include "database.h"
using namespace std;

void setMonthTable(unordered_map <string, string> &monthTable){
	monthTable["January"] = "01";
	monthTable["February"] = "02";
	monthTable["March"] = "03";
	monthTable["April"] = "04";
	monthTable["May"] = "05";
	monthTable["June"] = "06";
	monthTable["July"] = "07";
	monthTable["August"] = "08";
	monthTable["September"] = "09";
	monthTable["October"] = "10";
	monthTable["November"] = "11";
	monthTable["December"] = "12";
}

int main(){
	ios_base::sync_with_stdio(false);
	cin.tie(0);
	unordered_map <string, string> monthTable;
	setMonthTable(monthTable);
	string operation;
	Database database;
	while(cin >> operation){
		if(operation == "add") database.add_operation(monthTable);
		else if(operation == "remove") database.remove_operation();
		else if(operation == "longest") database.longest_operation();
		else if(operation == "query") database.query_operation();
	}
}