#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <stack>
#include <algorithm>
#include <queue>
#include "mail.h"
using namespace std;

#define MAIL_LINE 6
#define SPACE ' '
#define COLON ':'
#define TILDE '~'

enum LINE {FROM, DATE, MESSAGE_ID, SUBJECT, TO, CONTENT};

class Database {
	private:
		unordered_map <int, Mail> info;
		priority_queue <pair<int, int>> heap;
		int size;
	public:
		Database();
		void add_operation(unordered_map <string, string> monthTable);
		void remove_operation();
		void longest_operation();
		void query_operation();
		vector <int> getAllID(const string& from, const string& to, const string& start_date, const string& end_date);
		bool calculator(int id, vector <string> postfix);
};

Database::Database(){
	size = 0;
}

string to_upper(string str){
	transform(str.begin(), str.end(),str.begin(), ::toupper);
	return str;
}

void Database::add_operation(unordered_map <string, string> monthTable){
	string filePath, line, title;
	cin >> filePath;
	ifstream fp(filePath);
	Mail mail;
	for(int line_count = 0; line_count < MAIL_LINE; line_count++){
		string text;
		getline(fp, line);
		stringstream tokenized_line(line);
		getline(tokenized_line, title, SPACE);
		switch(line_count){
			case LINE(FROM):{
				getline(tokenized_line, text, SPACE);
				mail.from = to_upper(text);
				break;
			}
			case LINE(DATE):{
				getline(tokenized_line, text, SPACE);
				int num = stoi(text);
				if(num < 10) text = "0" + text;
				mail.date = text + mail.date;
				getline(tokenized_line, text, SPACE);
				mail.date = monthTable[text] + mail.date;
				getline(tokenized_line, text, SPACE);
				mail.date = text + mail.date;
				getline(tokenized_line, text, SPACE);
				getline(tokenized_line, text, SPACE);
				stringstream tokenized_text(text);
				getline(tokenized_text, text, COLON);
				mail.date = mail.date + text;
				getline(tokenized_text, text, COLON);
				mail.date = mail.date + text;
				break;
			}
			case LINE(MESSAGE_ID):{
				getline(tokenized_line, text, SPACE);
				if(info.find(stoi(text)) != info.end()){ // use find()?
					cout << "-" << endl;
					fp.close();
					return;
				}
				mail.message_id = stoi(text);
				break;
			}
			case LINE(SUBJECT):{
				getline(tokenized_line, text);
				string str = "";
				for(int i = 0; i < text.size(); i++){
					if(isalnum(text[i])) str += text[i];
					else{
						if(str != "") mail.content[to_upper(str)] = true;
						str = "";
					}
				}
				if(str != "") mail.content[to_upper(str)] = true;
				break;
			}
			case LINE(TO):{
				getline(tokenized_line, text, SPACE);
				mail.to = to_upper(text);
				break;
			}
			case LINE(CONTENT):{
				while(getline(fp, line)){
					string str = "";
					for(int i = 0; i < line.size(); i++){
						if(isalnum(line[i])) str += line[i];
						else{
							if(str != ""){
								mail.content[to_upper(str)] = true;
								mail.length += str.length();
							}
							str = "";
						}
					}
					if(str != ""){
						mail.content[to_upper(str)] = true;
						mail.length += str.length();
					}
				}
				break;
			}
		}
	}
	info[mail.message_id] = mail;
	heap.push(make_pair(mail.length, -mail.message_id)); // -id for convenience
	size += 1;
	cout << size << endl;
	fp.close();
}

void Database::remove_operation(){
	int id;
	cin >> id;
	if(info.find(id) == info.end()){
		cout << "-" << endl;
		return;
	}
	info.erase(id);
	size -= 1;
	cout << size << endl;
}

void Database::longest_operation(){
	if(size == 0){
		cout << "-" << endl;
		return;
	}
	while(info.find(-heap.top().second) == info.end()) { // use find()
		heap.pop();
	}
	int len = heap.top().first;	
	int id = -heap.top().second; // note -
	
	cout << id << " " << len << endl;
}

bool isWord(const string& input){
	return !(input == "(" or input == ")" or input == "|" or input == "&" or input == "!");
}

int priority(const string& operator_){
	if(operator_ == "!") return 3;
	else if(operator_ == "&") return 2;
	else if(operator_ == "|") return 1;
	else return 0;
}

vector <string> tokenize(const string& expression){
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

bool notOperator(const string& text){
	return !(text == "&" or text == "|" or text == "!");
}

vector <int> Database::getAllID(const string& from, const string& to, const string& start_date, const string& end_date){
	vector <int> candidates;
	unordered_map <int, Mail>::iterator iter;
	for(iter = info.begin(); iter != info.end(); iter++){
		if(!(from == "" or iter -> second.from == from)) continue;
		else if(!(to == "" or iter -> second.to == to)) continue;
		else if(start_date > iter -> second.date or iter -> second.date > end_date) continue;
		else candidates.push_back(iter -> first);
	}
	sort(candidates.begin(), candidates.end());
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
		if(notOperator(postfix[i])) S.push(info[id].content[to_upper(postfix[i])]);
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
	vector <int> candidates = getAllID(from, to, start_date, end_date);
	int count = 0;
	if(candidates.size() > 0){
		string output = "";		
		for(vector <int>::iterator iter = candidates.begin(); iter != candidates.end(); iter++){
			if(calculator(*iter, postfix)){
				output += to_string(*iter) + " ";
				count += 1;
			}
		}
		cout << output;
	}
	if(count == 0) cout << "-";
	cout << "\n";
}
