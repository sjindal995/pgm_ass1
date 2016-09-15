#include <bits/stdc++.h>
#include <fstream>
// #include <unordered_map>
#include <ctime>

using namespace std;

#define N_LTRS 10
#define N_IMGS 1000
#define CHARS 256


double** phit;
double** phi0;
vector<vector<int> > allImages;
vector<int> images;


// Mode 1	->	Only OCR
// Mode 2 	->	Both OCR and transition
// Mode 3	->	All combined
int mode;
char* letters;
string bestWord;
double bestScore;
double Z;
// unordered_map<char,int> diction;


// Create hash of all letters available
void createHash()
{
	letters = new char[N_LTRS];
	// letters = {'e','t','a','o','i','n','s','h','r','d'};
	letters[0] = 'e';
	letters[1] = 't';
	letters[2] = 'a';
	letters[3] = 'o';
	letters[4] = 'i';
	letters[5] = 'n';
	letters[6] = 's';
	letters[7] = 'h';
	letters[8] = 'r';
	letters[9] = 'd';

	// for(int i=0 ; i<N_LTRS ; i++)
	// {
	// 	diction[letters[i]] = i;
	// }
}


// Read phi_t(transition values) from a file and store in 2D array
void readPhit(string& PhitFile)
{
	phit = new double*[CHARS];
	for(int i=0;i<CHARS;i++)
	{
		phit[i] = new double[CHARS];
	}
	ifstream iff(PhitFile.c_str());
	string x;
	while(iff >> x)
	{
		char x1 = x[0];
		iff >> x;
		char x2 = x[0];
		double m;
		iff >> m;
		phit[x1][x2] = m;
	}
	iff.close();
}

// Read phi_0(ocr values) from file and store in array 
void readPhi0(string& Phi0File)
{
	phi0 = new double*[N_IMGS];
	for(int i=0;i<N_IMGS;i++)
	{
		phi0[i] = new double[CHARS];
	}
	ifstream iff(Phi0File.c_str());
	int n;
	string x;
	while(iff >> n)
	{
		int img = n;
		iff >> x;
		char x1 = x[0];
		double m;
		iff >> m;
		phi0[img][x1] = m;
	}
	iff.close();
}



// Read images
void readImages(string& ImageFile)
{
	ifstream iff(ImageFile.c_str());
	string s;
	while(getline(iff,s,'\n'))
	{
		vector<int> tempvect;
		string tmp = "";
		for(int i=0;i<s.size();i++)
		{
			if(s[i] == '\t')
			{
				tempvect.push_back(atoi(tmp.c_str()));
				tmp = "";
			}
			else
			{
				tmp += s[i];
			}
		}
		tempvect.push_back(atoi(tmp.c_str()));
		allImages.push_back(tempvect);
	}
	// for(int i=0;i<allImages.size();i++)
	// {
	// 	for(int j=0;j<allImages[i].size();j++)
	// 	{
	// 		cout<<allImages[i][j]<<"\t";
	// 	}
	// 	cout<<endl;
	// }
	iff.close();
}
	





double getScore(string& word)
{
	double score = 1;
	
	// We know Score(word/img) = mult(phi_0(img[i],w[i]))*mult(phi_t(w[i],w[i+1]))*mult(phi_s(w[i],w[j]))	-	Eqn. 1
	
	// Term 1 of eqn. 1 corresponding phi_0
	int n = word.size();
	if(mode > 0)
	{
		for(int i = 0 ; i < n ; i++)
			score *= phi0[images[i]][word[i]];
	}

	// Term 2 of eqn. 1 corresponding phi_t
	if(mode > 1)
	{
		for(int i = 0 ; i < n - 1 ; i++)
			score *= phit[word[i]][word[i+1]];
	}

	// For term 3 , mult(phi_s(w[i],w[j])) = 5^n where n = count when w[i],w[j] and img[i],img[j] are same
	if(mode > 2)
	{
		for(int i = 0 ; i < n ; i++)
		{
			for(int j = i + 1 ; j < n ; j++)
			{
				if(images[i] == images[j] && word[i] == word[j])
					score *= 5;
				
			}
		}
	}


	return score;
}


// string newword;

// Calculated Z value with image given 
double calcZ(string& wordnow)
{
	int length = images.size();
	int len = wordnow.size();
	double totalScore = 0;
	if(len == length)
	{
		// cout<< "YO" << endl;
		// Do both max-finding and Z calculation simultaneously
		double tmpScore = getScore(wordnow);
		// cout<< "YO1" << endl;
		if(bestScore < tmpScore)
		{
			bestScore = tmpScore;	
			bestWord = wordnow;

			// cout<< wordnow << endl;
		}
		return tmpScore;
	}
	else
	{
		for(int i = 0 ; i < N_LTRS ; i++)
		{
			string newword = wordnow + letters[i];
			// cout << newword << endl << endl << endl;
			totalScore += calcZ( newword );
		}
		return totalScore;
	}
}





double getLogProb(string& word)
{
	// // We know P(word/img) = (1/Z)*Score(word/img)
	// // So , Log(P) = log(Score(word)) - log(Z)	-	Eqn. 1
	return getScore(word)*(1/Z);
}


// void getMaxProb(string& wordnow )
// {
// 	int length = images.size();
// 	int len = wordnow.size();
// 	if(len == length)
// 	{
// 		// Replace if score greater
// 		double tmpScore = getLogProb(wordnow);
// 		if(bestScore < tmpScore)
// 		{
// 			bestScore = tmpScore;	
// 			bestWord = wordnow;

// 			// cout<< wordnow << endl;
// 		}
// 	}
// 	else
// 	{	
// 		for(int i = 0 ; i < N_LTRS ; i++)
// 		{
// 			newword = wordnow + letters[i];
// 			getMaxProb(newword);
// 		}
// 	}
// }


void getBestWord()
{

	// Calculate Z value
	bestScore = -1000000000;
	bestWord = "";
	string x = "";
	Z = calcZ(x);
	// printf("Z calculation over\n");
	// Run getting max probability
	// getMaxProb("");
}



// double getWordProb(char* word , int mod)
// {
// 	createHash();
// 	// readImages("OCRDataset/data/small/images.dat");
// 	// images = allImages[0];
// 	// mode = mod;
// 	// readPhit("OCRDataset/potentials/trans.dat");
// 	// readPhi0("OCRDataset/potentials/ocr.dat");
// 	// Z = calcZ("");
// 	cout<< getLogProb(word) << endl;
// }

vector<string> getBestWords(string& inputImageFile , int mod)
{
	createHash();
	readImages(inputImageFile);

	mode = mod;
	string phitFile = "OCRDataset/potentials/trans.dat";
	string phi0File = "OCRDataset/potentials/ocr.dat";
	readPhit(phitFile);
	readPhi0(phi0File);
	vector<string> words;
	double logs = 0;
	ofstream off("out.txt");
	for(int i = 0 ; i < allImages.size() ; i++)
	{
		printf("%d\n",i);
		images = allImages[i];
		getBestWord();
		words.push_back(bestWord);
		cout << words[i] << endl;

		off << words[i];
		// cout<<"YO"<<endl;
		off <<  "\n";

		
		// for(int j=0;j<words[i].size();j++) tmp[j] = words[i][j];
		// cout << "YOYO" << endl;
		logs += log(getLogProb(words[i]));
		cout<<logs<<endl;
	}
	cout<< logs/double(allImages.size())<<endl;
	return words;
}

int main()
{
	createHash();
	// readPhit("OCRDataset/potentials/trans.dat");
	// readPhi0("OCRDataset/potentials/ocr.dat");
	// readImages("OCRDataset/data/small/images.dat");
	int start_s=clock();
	string imgFile = "OCRDataset/data/small/images.dat";
	getBestWords(imgFile,3);
	// getWordProb("adad",3);
	int stop_s=clock();
	cout << "time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << endl;
}