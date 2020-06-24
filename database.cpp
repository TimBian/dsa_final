#include <cassert>
#include "database.h"
using namespace std;

Database::Database(){
	setMonthTable();
}

void to_upper(string& str){
	transform(str.begin(), str.end(), str.begin(), ::toupper); // why "::"?
}

void Database::add() {
	string filePath, line, title;
	cin >> filePath;
	ifstream fp(filePath);
	string from_temp, to_temp, date_temp = "";
	int length_temp = 0, message_id_temp;
	Mail mail;
	for(int line_count = 0; line_count < MAIL_LINE; line_count++){
		string text;
		getline(fp, line);
		stringstream tokenized_line(line);
		getline(tokenized_line, title, SPACE);
		switch(line_count){
			case LINE(FROM):{
				getline(tokenized_line, text, SPACE);
				to_upper(text);
				from_temp = text;
				break;
			}
			case LINE(DATE):{
				getline(tokenized_line, text, SPACE);
				int num = stoi(text);
				if(num < 10) text = "0" + text;
				date_temp = text + date_temp;
				getline(tokenized_line, text, SPACE);
				date_temp = monthTable[text] + date_temp;
				getline(tokenized_line, text, SPACE);
				date_temp = text + date_temp;
				getline(tokenized_line, text, SPACE);
				getline(tokenized_line, text, SPACE);
				stringstream tokenized_text(text);
				getline(tokenized_text, text, COLON);
				date_temp = date_temp + text;
				getline(tokenized_text, text, COLON);
				date_temp = date_temp + text;
				break;
			}
			case LINE(MESSAGE_ID):{
				getline(tokenized_line, text, SPACE);
				message_id_temp = stoi(text);
				if(in.find(message_id_temp) != in.end()){
					cout << "-" << endl;
					record.push_back(make_pair('-', message_id_temp));
					return;
				}
				break;
			}
			case LINE(SUBJECT):{
				getline(tokenized_line, text);
				string str = "";
				for(int i = 0; i < text.size(); i++){
					if(isalnum(text[i])) str += text[i];
					else{
						if(str != "") {
							to_upper(str);
							mail.has_word[str] = true;
						}
						str = "";
					}
				}
				if(str != ""){
					to_upper(str);
					mail.has_word[str] = true;
				}
				break;
			}
			case LINE(TO):{
				getline(tokenized_line, text, SPACE);
				to_upper(text);
				to_temp = text;
				break;
			}
			case LINE(CONTENT):{
				while(getline(fp, line)){
					string str = "";
					for(int i = 0; i < line.size(); i++){
						if(isalnum(line[i])) str += line[i];
						else{
							if(str != ""){
								to_upper(str);
								mail.has_word[str] = true;
								length_temp += str.length();
							}
							str = "";
						}
					}
					if(str != ""){
						to_upper(str);
						mail.has_word[str] = true;
						length_temp += str.length();
					}
				}
				break;
			}
		}
	}
	mail.date = date_temp;
	M[message_id_temp] = mail;
	from_id[from_temp].insert(message_id_temp);
	to_id[to_temp].insert(message_id_temp);
	heap.push(make_pair(length_temp, -message_id_temp)); // take '-' on id for convenience
	in.insert(message_id_temp);
	cout << in.size() << endl;
	fp.close();
	record.push_back(make_pair('a', message_id_temp));
}

void Database::remove() {
	int id;
	cin >> id;
	if(in.find(id) == in.end()){
		cout << "-" << endl;
		record.push_back(make_pair('-', id));
		return;
	}
	in.erase(id);
	cout << in.size() << endl;
	record.push_back(make_pair('r', id));
}

void Database::longest() {
	if(in.size() == 0){
		cout << "-" << endl;
		record.push_back(make_pair('-', 0));
		return;
	}
	while(in.find(-heap.top().second) == in.end()) { // note '-'
		heap.pop();
	}
	int len = heap.top().first;	
	int id = -heap.top().second; // note '-'
	
	cout << id << " " << len << endl;
	record.push_back(make_pair('l', 0));
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

vector <string> infix2posfix(const vector <string>& tokenized_expression){
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

vector <int> Database::getAllID(const string& from, const string& to, const string& start_date, const string& end_date) {
	vector <int> candidates(in.begin(), in.end());
	vector <int> date_cand;
	vector <int> result;
	if(from != ""){
		if(from_id.find(from) != from_id.end()){
			set_intersection(candidates.begin(), candidates.end(), from_id[from].begin(), from_id[from].end(), back_inserter(result));
			candidates = result;
			result.clear();
		}
		else return result;
	}
	if(to != ""){
		if(to_id.find(to) != to_id.end()){
			set_intersection(candidates.begin(), candidates.end(), to_id[to].begin(), to_id[to].end(), back_inserter(result));
			candidates = result;
			result.clear();
		}
		else return result;
	}
	for(vector <int>::iterator iter = candidates.begin(); iter != candidates.end(); iter++) if(start_date <= M[*iter].date and M[*iter].date <= end_date) date_cand.push_back(*iter);
	return date_cand;
}

// set<int> getElement(stack <set<int>>& S){	
// 	set<int> element = S.top();
// 	S.pop();	
// 	return element;	
// }

vector<int> Database::calculator(vector <string>& postfix, const vector <int>& candidates){
	vector <vector<int>> S;

	//cerr << " ----------------- " << endl; 
	
	for(int i = 0; i < postfix.size(); i++){
		if(notOperator(postfix[i])) {
			string& keyword = postfix[i]; // copy
			to_upper(keyword);
			
			//cerr << keyword << endl;
			if(W.find(keyword) == W.end()) {
				if(W[keyword].cursor != 0) {
					cerr << " cursor != 0 " << endl;
				}
			}
			Word& word = W[keyword];
			while(word.cursor < record.size()) {
				const auto& oper = record[word.cursor];
				if(oper.first == 'a') {
					if(in.find(oper.second) != in.end() && M[oper.second].has_word[keyword]) {		
						word.used_mails.insert(oper.second);
					}
				}
				word.cursor++;
			}

			vector <int> filted;
			set<int>& s = W[keyword].used_mails;
			set_intersection(s.begin(), s.end(), candidates.begin(), candidates.end(), back_inserter(filted));
			S.push_back(filted); // copy?
		}
		else{
			if(postfix[i] == "!") {
				vector<int>& operand = S.back();
				vector<int> result;
				set_difference(candidates.begin(), candidates.end(), operand.begin(), operand.end(), back_inserter(result)); // or use inserter() here?
                                /*
				cerr << "! [";
                                for(auto id : operand) cerr << id << ", ";
                                cerr << "]" << endl;
                                */
				S.pop_back();
				S.push_back(result);
			}
			else if(postfix[i] == "&") {
				vector<int>& operand1 = S.back(); // copy!
				vector<int>& operand2 = S[S.size() - 2]; // copy!
				vector<int> result;
				set_intersection(operand1.begin(), operand1.end(), operand2.begin(), operand2.end(), back_inserter(result));        
                                /*
				cerr << "[";
                                for(auto id : operand1) cerr << id << ", ";
                                cerr << "] & [";
                                for(auto id : operand2) cerr << id << ", ";
                                cerr << "]" << endl;
				*/
				S.pop_back();
				S.pop_back();
				S.push_back(result);
			}
			else { // postfix[i] == "|"
				vector<int>& operand1 = S.back(); // copy!
				vector<int>& operand2 = S[S.size() - 2]; // copy!
				vector<int> result;
				set_union(operand1.begin(), operand1.end(), operand2.begin(), operand2.end(), back_inserter(result));        
                                /*
				cerr << "[";
                                for(auto id : operand1) cerr << id << ", ";
                                cerr << "] & [";
                                for(auto id : operand2) cerr << id << ", ";
                                cerr << "]" << endl;
				*/
				S.pop_back();
				S.pop_back();
				S.push_back(result);
			}
		}
	}
	
	//cerr << " ----------------- " << endl; 
	
	return S.back(); // Does returning vector cause copy?
}

void Database::query() {
	string line, text;
	getline(cin, line);
	stringstream tokenized_line(line);
	getline(tokenized_line, text, SPACE);
	string from = "", to = "", start_date = "", end_date = "999999999999", expression = "";
	while(getline(tokenized_line, text, SPACE)){
		if(text[0] == '-'){
			if(text[1] == 'f') {
				from = text.substr(3, text.length() - 4);
				to_upper(from);
			}
			else if(text[1] == 't') {
				to = text.substr(3, text.length() - 4);
				to_upper(to);
			}
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
    
	if(result.size() == 0) {
		cout << "-" << endl;
		record.push_back(make_pair('-', 0));
	}
    	else {
		for(int n : result)
			cout << n << " ";
		cout << endl;
		record.push_back(make_pair('q', 0));
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

