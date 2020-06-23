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

class Database {
	private:
			unordered_map <int, string> id_date;
        	unordered_map <string, set<int>> wordset;
        	unordered_map <string, set<int>> from_id;
        	unordered_map <string, set<int>> to_id;

			priority_queue <pair<int, int>> heap; // only for longest()
			set <int> in;

        	unordered_map <string, string> monthTable;
	public:
		Database();
        
		void add();
		void remove();
		void longest();
		void query();

		set <int> getAllID(const string& from, const string& to, const string& start_date, const string& end_date);
		// set<int> calculator(const vector <string>& postfix, const set <int> candidates);
		set<int> calculator(const vector <string>& postfix, const set <int> candidates);
        
        void setMonthTable();
};

#endif
