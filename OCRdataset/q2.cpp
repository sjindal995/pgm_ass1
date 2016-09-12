#include <iostream>
#include <string>
#include <fstream>

using namespace std;

// vector<char> chars = {'e','t','a','o','i','n','s','h','r','d'}
// double ocr_factor[10][1000];
// double transition_factor[10];

double getConfProb(string line, bool include_transition, bool include_skip){
	
}

int main(){
	string str = "0x0002,A5651QPR87GBZ094RTF52,D,A,000001,ABC ,10000.00 , EOT";
    string word;
    stringstream stream(str);
    while( getline(stream, word, ',') )
        cout << word << "\n";
}