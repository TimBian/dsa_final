#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cassert>
using namespace std;

#define MAIL_LINE 6
#define SPACE ' '
#define COLON ':'
#define TILDE '~'
#define LIMIT 5

void setMonthTable(unordered_map <string, string>& monthTable){
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

enum LINE {FROM, DATE, MESSAGE_ID, SUBJECT, TO, CONTENT};

random_device rd;
default_random_engine gen = default_random_engine(rd());

string OP[] = { "add", "remove", "longest", "query" };
vector<string> files;
vector<string> words;
vector<string> from;
vector<int> id;
vector<string> to;
vector<string> date;
uniform_int_distribution<int> yesOrNo(0, 1);

string to_upper(string str){
	transform(str.begin(), str.end(),str.begin(), ::toupper); // why "::"?
	return str;
}

string generate_expression(uniform_int_distribution<int> dis_word) {
	string str = "";
	for(int i = 0; i < LIMIT; i++){
		if(str != ""){
			if(yesOrNo(gen)) str = "!" + str;
			if(yesOrNo(gen)) str = "(" + str + ")";
			if(yesOrNo(gen)) str += "&";
			else str += "|";
		}
		str += words[dis_word(gen)];
	}
	return str;
}

int main(int argc, char* argv[]) {
	unordered_map <string, string> monthTable;
	setMonthTable(monthTable);
	uniform_int_distribution<int> dis_op(0, 3);

	int test_size = stoi(argv[1]);
	string list_file = argv[2];
	ifstream list(list_file);

	string dir;
	string file;
	
	getline(list, dir);
	files.push_back(dir);
	while(getline(list, file)) {
		files.push_back(dir+file);
	}
	assert(files.size() == 10001);

	for(int i = 1; i <= 10000; i++) {
		string line, title;
		ifstream mail(files[i]);
		for(int line_count = 0; line_count < MAIL_LINE; line_count++){
			string text;
			getline(mail, line);
			stringstream tokenized_line(line);
			getline(tokenized_line, title, SPACE);
			switch(line_count){
				case LINE(FROM):{
					getline(tokenized_line, text, SPACE);
					from.push_back(text);
					break;
				}
				case LINE(DATE):{
					string date_str = "";
					getline(tokenized_line, text, SPACE);
					int num = stoi(text);
					if(num < 10) text = "0" + text;
					date_str = text + date_str;
					getline(tokenized_line, text, SPACE);
					date_str = monthTable[text] + date_str;
					getline(tokenized_line, text, SPACE);
					date_str = text + date_str;
					getline(tokenized_line, text, SPACE);
					getline(tokenized_line, text, SPACE);
					stringstream tokenized_text(text);
					getline(tokenized_text, text, COLON);
					date_str = date_str + text;
					getline(tokenized_text, text, COLON);
					date_str = date_str + text;
					date.push_back(date_str);
					break;
				}
				case LINE(MESSAGE_ID):{
					getline(tokenized_line, text, SPACE);
					id.push_back(stoi(text));
					break;
				}
				case LINE(SUBJECT):{
					getline(tokenized_line, text);
					string str = "";
					for(int i = 0; i < text.size(); i++){
						if(isalnum(text[i])) str += text[i];
						else{
							if(str != "") words.push_back(str);
							str = "";
						}
					}
					if(str != ""){
						words.push_back(str);
					}
					break;
				}
				case LINE(TO):{
					getline(tokenized_line, text, SPACE);
					to.push_back(text);
					break;
				}
				case LINE(CONTENT):{
					while(getline(mail, line)){
						string str = "";
						for(int i = 0; i < line.size(); i++){
							if(isalnum(line[i])) str += line[i];
							else{
								if(str != ""){
									words.push_back(str);
								}
								str = "";
							}
						}
						if(str != ""){
							words.push_back(str);
						}
					}
					break;
				}
			}
		}
	}
	sort( date.begin(), date.end() );
	sort( words.begin(), words.end() );
	cerr << "Total: " << words.size() << " words" << endl;
	words.erase( unique( words.begin(), words.end() ), words.end() );
	cerr << "After removing duplicates: " << words.size() << " words" << endl;
	cerr << "generating " << test_size << " operations randomly..." << endl;
	for(int t = 1; t <= test_size; t++) {
		string op = OP[dis_op(gen)];
		if(op == "add") {
			cout << op;
			uniform_int_distribution<int> dis_file(1, 10000);
			cout << " " << files[dis_file(gen)] << endl;
		}
		else if(op == "remove") {
			cout << op;
			uniform_int_distribution<int> dis_id(1, 10000);
			cout << " " << dis_id(gen) << endl;
		}
		else if(op == "longest") {
			cout << op << endl;
		}
		else if(op == "query") {
			cout << op;
			uniform_int_distribution<int> dis_from(0, from.size() - 1);
			uniform_int_distribution<int> dis_to(0, to.size() - 1);
			uniform_int_distribution<int> dis_date(0, date.size() - 1);
			uniform_int_distribution<int> dis_word(0, words.size()-1);

			if(yesOrNo(gen)) cout << " -f\"" + from[dis_from(gen)] + "\"";
			if(yesOrNo(gen)) cout << " -t\"" + to[dis_to(gen)] + "\"";
			if(yesOrNo(gen) and yesOrNo(gen)) cout << " -d" + date[dis_date(gen)] + "~" + date[dis_date(gen)];
			else if(yesOrNo(gen)) cout << " -d" + date[dis_date(gen)] + "~";
			else if(yesOrNo(gen)) cout << " -d~" + date[dis_date(gen)] << "";
			cout << " " + generate_expression(dis_word) << endl;
		}
	}	
}
