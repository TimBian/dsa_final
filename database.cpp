#include "database.h"
using namespace std;

Database::Database(){
	size = 0;
}

string to_upper(string str){
	transform(str.begin(), str.end(),str.begin(), ::toupper); // why "::"?
	return str;
}

void Database::add() {
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
				if(mail_info.find(stoi(text)) != mail_info.end()){
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
						if(str != "") word_mailset[to_upper(str)].insert(mail.message_id);
						str = "";
					}
				}
				if(str != ""){
					word_mailset[to_upper(str)].insert(mail.message_id);
				}
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
								word_mailset[to_upper(str)].insert(mail.message_id);
								mail.length += str.length();
							}
							str = "";
						}
					}
					if(str != ""){
						word_mailset[to_upper(str)].insert(mail.message_id);
						mail.length += str.length();
					}
				}
				break;
			}
		}
	}
	mail_info[mail.message_id] = mail;
	//mailset.insert(mail.message_id);
	heap.push(make_pair(mail.length, -mail.message_id)); // take '-' on id for convenience
	size += 1;
	cout << size << endl;
	fp.close();
}

void Database::remove() {
	int id;
	cin >> id;
	if(mail_info.find(id) == mail_info.end()){
		cout << "-" << endl;
		return;
	}
	mail_info.erase(id);
	//mailset.erase(id);
	size -= 1;
	cout << size << endl;
}

void Database::longest() {
	if(size == 0){
		cout << "-" << endl;
		return;
	}
	while(mail_info.find(-heap.top().second) == mail_info.end()) { // note '-'
		heap.pop();
	}
	int len = heap.top().first;	
	int id = -heap.top().second; // note '-'
	
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

void Database::query() {
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
	vector<int> result = calculator(postfix, candidates);
    
	if(result.size() == 0) cout << "-" << endl;
    	else {
		for(int n : result)
			cout << n << " ";
		cout << endl;
	}
}

void Database::setMonthTable(){
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

vector <int> Database::getAllID(string from, string to, string start_date, string end_date) {
	vector <int> candidates;
	unordered_map <int, Mail>::iterator iter;
	for(iter = mail_info.begin(); iter != mail_info.end(); iter++){
		if(from != "" and iter -> second.from != from) continue;
		else if(to != "" and iter -> second.to != to) continue;
		else if(start_date > iter -> second.date or iter -> second.date > end_date) continue;
		else candidates.push_back(iter -> first);
	}
	sort(candidates.begin(), candidates.end());
	return candidates;
}

vector<int> Database::calculator(vector <string> postfix, vector<int> universal){
	stack <vector<int>> S;
	for(int i = 0; i < postfix.size(); i++){
		if(notOperator(postfix[i])) {
			const string& keyword = to_upper(postfix[i]);
			//cerr << keyword << endl;
			vector<int> filted;
			for(auto id : word_mailset[keyword]) {
				if(mail_info.find(id) != mail_info.end()) {
					//cerr << id << endl;
					filted.push_back(id);
				}
			}
			S.push(filted); // copy?
		}
		else{
			if(postfix[i] == "!") {
				const vector<int>& operand = S.top();
				vector<int> result;
				set_difference(universal.begin(), universal.end(), operand.begin(), operand.end(), back_inserter(result)); // why inserter here?
				/*
				cerr << "! [";
				for(auto id : operand) cerr << id << ", ";
				cerr << "]" << endl;
				*/
				S.pop();
				S.push(result);
			}
			else if(postfix[i] == "&") {
				vector<int> operand1 = S.top(); // copy!
				S.pop();
				vector<int> operand2 = S.top(); // copy!
				S.pop();
				
				vector<int> result;
				set_intersection(operand1.begin(), operand1.end(), operand2.begin(), operand2.end(), back_inserter(result));
				/*
				cerr << "[";
				for(auto id : operand1) cerr << id << ", ";
				cerr << "] & [";
				for(auto id : operand2) cerr << id << ", ";
				cerr << "]" << endl;
				*/
				S.push(result);
			}
			else { // postfix[i] == "|"
				vector<int> operand1 = S.top(); // copy!
				S.pop();
				vector<int> operand2 = S.top(); // copy!
				S.pop();
				
				vector<int> result;
				set_union(operand1.begin(), operand1.end(), operand2.begin(), operand2.end(), back_inserter(result));
				/*
				cerr << "[";
				for(auto id : operand1) cerr << id << ", ";
				cerr << "] & [";
				for(auto id : operand2) cerr << id << ", ";
				cerr << "]" << endl;
				*/
				S.push(result);
			}
		}
	}

	const vector<int>& operand = S.top();
	vector<int> result;
	set_intersection(universal.begin(), universal.end(), operand.begin(), operand.end(), back_inserter(result));
	S.pop();
	return result; // Does returning vector cause copy?
}
