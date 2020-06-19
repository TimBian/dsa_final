#include <vector>
#include <unordered_map>
using namespace std;

class Mail {
	public:
		string from;
		string date;
		int message_id;
		string to;
		unordered_map <string, bool> content;
		int length;
		Mail();
};

Mail::Mail(){
	date = "";
	length = 0;
}