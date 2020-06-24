#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <stack>
#include <algorithm>
#include <queue>
#include <cctype>
#include <unordered_map>
using namespace std;

#define MAIL_LINE 6
#define SPACE ' '
#define COLON ':'
#define TILDE '~'

enum LINE {FROM, DATE, MESSAGE_ID, SUBJECT, TO, CONTENT};

struct Mail {
	unordered_map<string, bool> has_word;
	string date;
};

struct Word {
	set<int> used_mails;
	int cursor;
};

class Database {
	private:
		unordered_map <int, Mail> M;
        	unordered_map <string, Word> W;
        	unordered_map <string, set<int>> from_id;
        	unordered_map <string, set<int>> to_id;
		
		priority_queue <pair<int, int>> heap; // only for longest()
		set <int> in;

		vector<pair<char, int>> record; // record every operation that is processed
		unordered_map <string, string> monthTable;
	public:
		Database();
        
		void add();
		void remove();
		void longest();
		void query();

		vector <int> getAllID(const string& from, const string& to, const string& start_date, const string& end_date);
		vector<int> calculator(vector <string>& postfix, const vector <int>& candidates);
        
		void setMonthTable();
};

#endif
