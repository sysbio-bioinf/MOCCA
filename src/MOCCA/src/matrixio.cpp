#include <cctype>

using namespace std;

#include "matrixio.h"

/*
	help functions (IO)
*/
void skip_line(istream &f) {
	int c;
	do {
		c = f.get();
	} while (c!=EOF && c!='\n');
}

void skip_empty_lines(istream &f) {
	int c;

	do {
		c = f.get();
		if(c=='#'){
			// f.ignore(0x7fff,'\n');
			do {
				c = f.get();
			} while(c!=EOF && c!='\n');
			
			continue;
		}
	} while(c!=EOF && isspace(c));
	if (!isspace(c) && c!=EOF)
		f.unget();
//		f.seekg(f.tellg()-1);
		/*
	else
		if ((c == '\n') || (c == EOF))
			;
			*/

}

/*
	count_words
	counts the number of words in this line
*/
int count_words(istream &f) {

	int num, c;
	bool in_word;
	
	num = 0;
	in_word = false;
	while(true) {
		c = f.get();
		if(c == '\n' || c == EOF)
			break;
		
		if( isspace(c) ) {
			if(in_word) {
				// word finished
				in_word = false;
			}
		} else {
			if(!in_word) {
				in_word = true;
				num++;
			}
		}
	}
	return num;
}

int get_words(istream &f, list<string> &words) {
	
	string buf;
	
	int c, num;
	
	num = 0;
	while(true) {
		c = f.get();
		if(c == '\n' || c == EOF)
			break;
		
		if( !isspace(c) ) {
			f.putback(c);
			
			f >> buf;
			words.push_back(buf);
			++num;
		} 
	}
	return num;
}

