#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <cctype>
#include <vector>
#include <string>
#include <stack>
#include <set>
#include <algorithm>
#include "mail.h"
using namespace std;

#define MAIL_LINE 6
#define SPACE ' '
#define COLON ':'
#define TILDE '~'

enum LINE {FROM, DATE, MESSAGE_ID, SUBJECT, TO, CONTENT};

class Database {
	private:
		map <int, Mail*>* info_ptr;
		map <int, map <int, Mail*> >* sortByLengthID_ptr;
		int size;
	public:
		Database();
		void add_operation(unordered_map <string, string> monthTable);
		void print_mail(int id);
		void remove_operation();
		void longest_operation();
		void query_operation();
		set <int> getAllID(string from, string to, string start_date, string end_date);
		bool calculator(int id, vector <string> postfix);
};

Database::Database(){
	info_ptr = new map <int, Mail*>;
	sortByLengthID_ptr = new map <int, map <int, Mail*> >;
	size = 0;
}

void replaceWithSpace(string &text){
	for(int i = 0; i < text.length(); i++) if(not isalnum(text[i])) text[i] = ' ';
}

string to_upper(string str){
	transform(str.begin(), str.end(),str.begin(), ::toupper);
	return str;
}

void Database::add_operation(unordered_map <string, string> monthTable){
	string filePath, line, title;
	cin >> filePath;
	ifstream fp(filePath);
	Mail* mail_ptr = new Mail;
	for(int line_count = 0; line_count < MAIL_LINE; line_count++){
		string text;
		getline(fp, line);
		stringstream tokenized_line(line);
		getline(tokenized_line, title, SPACE);
		switch(line_count){
			case LINE(FROM):{
				getline(tokenized_line, text, SPACE);
				mail_ptr -> from = to_upper(text);
				break;
			}
			case LINE(DATE):{
				getline(tokenized_line, text, SPACE);
				int num = stoi(text);
				if(num < 10) text = "0" + text;
				mail_ptr -> date = text + mail_ptr -> date;
				getline(tokenized_line, text, SPACE);
				mail_ptr -> date = monthTable[text] + mail_ptr -> date;
				getline(tokenized_line, text, SPACE);
				mail_ptr -> date = text + mail_ptr -> date;
				getline(tokenized_line, text, SPACE);
				getline(tokenized_line, text, SPACE);
				stringstream tokenized_text(text);
				getline(tokenized_text, text, COLON);
				mail_ptr -> date = mail_ptr -> date + text;
				getline(tokenized_text, text, COLON);
				mail_ptr -> date = mail_ptr -> date + text;
				break;
			}
			case LINE(MESSAGE_ID):{
				getline(tokenized_line, text, SPACE);
				if((*info_ptr)[stoi(text)] != NULL){
					cout << "-" << endl;
					fp.close();
					delete mail_ptr;
					return;
				}
				mail_ptr -> message_id = stoi(text);
				break;
			}
			case LINE(SUBJECT):{
				getline(tokenized_line, text);
				replaceWithSpace(text);
				stringstream tokenized_text(text);
				while(getline(tokenized_text, text, SPACE)){
					if(text == "") continue;
					mail_ptr -> content[to_upper(text)] = true;
				}
				break;
			}
			case LINE(TO):{
				getline(tokenized_line, text, SPACE);
				mail_ptr -> to = to_upper(text);
				break;
			}
			case LINE(CONTENT):{
				while(getline(fp, line)){
					replaceWithSpace(line);
					stringstream tokenized_line(line);
					while(getline(tokenized_line, text, SPACE)){
						if(text == "") continue;
						mail_ptr -> content[to_upper(text)] = true;
						mail_ptr -> length += text.length();
					}
				}
				break;
			}
		}
	}
	(*info_ptr)[mail_ptr -> message_id] = mail_ptr;
	(*sortByLengthID_ptr)[mail_ptr -> length][mail_ptr -> message_id] = mail_ptr;
	size += 1;
	cout << size << endl;
	fp.close();
}

void Database::remove_operation(){
	int id;
	cin >> id;
	if(info_ptr -> find(id) == info_ptr -> end()){
		cout << "-" << endl;
		return;
	}
	int len = (*info_ptr)[id] -> length;
	(*sortByLengthID_ptr)[len].erase(id);
	if((*sortByLengthID_ptr)[len].size() == 0) (*sortByLengthID_ptr).erase(len);
	(*info_ptr).erase(id);
	size -= 1;
	cout << size << endl;
}

void Database::longest_operation(){
	if(size == 0){
		cout << "-" << endl;
		return;
	}
	map <int, map <int, Mail*> >::reverse_iterator iter_len = sortByLengthID_ptr -> rbegin();
	map <int, Mail*>::iterator iter_id = iter_len -> second.begin();
	int len = iter_len -> first;
	int id = iter_id -> first;
	cout << id << " " << len << endl;
}

bool isWord(string input){
	return input != "(" and input != ")" and input != "|" and input != "&" and input != "!";
}

int priority(string operator_){
	if(operator_ == "!") return 3;
	else if(operator_ == "&") return 2;
	else if(operator_ == "|") return 1;
	else return 0;
}

vector <string> tokenize(string expression){
	vector <string> tokenized_expression;
	bool lastIsWord = false;
	string last = "", empty = "";
	for(int i = 0; i < expression.length(); i++){
		if(isalnum(expression[i])){
			if(lastIsWord) last += expression[i];
			else last = expression[i];
			lastIsWord = true;
		}
		else{
			if(lastIsWord){
				tokenized_expression.push_back(empty + last);
				last = "";
			}
			tokenized_expression.push_back(empty + expression[i]);
			lastIsWord = false;
		}
	}
	if(lastIsWord) tokenized_expression.push_back(empty + last);
	return tokenized_expression;
}

vector <string> infix2posfix(vector <string> tokenized_expression){
	vector <string> postfix;
	stack <string> S;
	for(int i = 0; i < tokenized_expression.size(); i++){
		if(isWord(tokenized_expression[i])) postfix.push_back(tokenized_expression[i]);
		else if(tokenized_expression[i] == "(") S.push(tokenized_expression[i]);
		else if(tokenized_expression[i] == ")"){
			while(S.top() != "("){
				postfix.push_back(S.top());
				S.pop();
			}
			S.pop();
		}
		else{
			while(!S.empty() and priority(S.top()) >= priority(tokenized_expression[i])){
				if(priority(tokenized_expression[i]) == 3) break;
				postfix.push_back(S.top());
				S.pop();
			}
			S.push(tokenized_expression[i]);
		}
	}
	while(!S.empty()){
		postfix.push_back(S.top());
		S.pop();
	}
	return postfix;
}

bool notOperator(string text){
	return text != "&" and text != "|" and text != "!";
}

set <int> Database::getAllID(string from, string to, string start_date, string end_date){
	set <int> candidates;
	map <int, Mail*>::iterator iter;
	for(iter = info_ptr -> begin(); iter != info_ptr -> end(); iter++){
		if(from != "" and iter -> second -> from != from) continue;
		else if(to != "" and iter -> second -> to != to) continue;
		else if(start_date > iter -> second -> date or iter -> second -> date > end_date) continue;
		else candidates.insert(iter -> first);
	}
	return candidates;
}

bool getElement(stack <bool>& S){
	bool element = S.top();
	S.pop();
	return element;
}

bool Database::calculator(int id, vector <string> postfix){
	stack <bool> S;
	for(int i = 0; i < postfix.size(); i++){
		if(notOperator(postfix[i])) S.push((*info_ptr)[id] -> content[to_upper(postfix[i])]);
		else{
			if(postfix[i] == "!") S.push(!getElement(S));
			else if(postfix[i] == "&") S.push(getElement(S) & getElement(S));
			else S.push(getElement(S) | getElement(S));
		}
	}
	return S.top();
}

void Database::query_operation(){
	string line, text;
	getline(cin, line);
	stringstream tokenized_line(line);
	getline(tokenized_line, text, SPACE);
	string from = "", to = "", start_date = "", end_date = "999999999999", expression = "";
	while(getline(tokenized_line, text, SPACE)){
		if(text[0] == '-'){
			if(text[1] == 'f') from = to_upper(text.substr(3, text.length() - 4));
			else if(text[1] == 't') to = to_upper(text.substr(3, text.length() - 4));
			else if(text[1] == 'd'){
				string date;
				text = text.substr(2, text.length() - 2);
				stringstream tokenized_text(text);
				getline(tokenized_text, date, TILDE);
				if(date != "") start_date = date;
				getline(tokenized_text, date, TILDE);
				if(date != "") end_date = date;
			}
		}
		else expression = text;
	}
	vector <string> tokenized_expression = tokenize(expression);
	vector <string> postfix = infix2posfix(tokenized_expression);
	set <int> candidates = getAllID(from, to, start_date, end_date);
	int count = 0;
	if(candidates.size() > 0){
		for(set <int>::iterator iter = candidates.begin(); iter != candidates.end(); iter++){
			if(calculator(*iter, postfix)){
				if(count > 0) cout << " ";
				cout << *iter;
				count += 1;
			}
		}
	}
	if(count == 0) cout << "-";
	cout << "\n";
}