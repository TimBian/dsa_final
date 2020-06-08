#include <vector>
using namespace std;

class Mail {
	public:
		string from;
		string date;
		int message_id;
		vector <string> subject;
		string to;
		vector <string> content;
		int length;
		Mail();
};

Mail::Mail(){
	date = "";
	length = 0;
}