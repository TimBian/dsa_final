#ifndef MAIL_H
#define MAIL_H

#include <string>
using namespace std;

class Mail {
	public:
		string from;
		string date;
		int message_id;
		string to;
		int length;

		Mail() {	
			date = "";
			length = 0;
		}
};

#endif
