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
#include "mail.h"
using namespace std;

#define MAIL_LINE 6
#define SPACE ' '
#define COLON ':'
#define TILDE '~'

enum LINE {FROM, DATE, MESSAGE_ID, SUBJECT, TO, CONTENT};

class Database {
	private:
		unordered_map <int, Mail> mail_info;
        	//set<int> mailset;
        	unordered_map <string, set<int>> word_mailset;
		priority_queue <pair<int, int>> heap; // only for longest()
		int size;

        	unordered_map <string, string> monthTable;
	public:
		Database();
        
		void add();
		void remove();
		void longest();
		void query();

		vector <int> getAllID(const string& from, const string& to, const string& start_date, const string& end_date);
		vector<int> calculator(const vector <string>& postfix, const vector<int>& universal);
        	void setMonthTable();
};

#endif
