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
double skip_factor = 5;
int phit_factor = 1;


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
void readPhit(char* PhitFile)
{
	phit = new double*[CHARS];
	for(int i=0;i<CHARS;i++)
	{
		phit[i] = new double[CHARS];
	}
	ifstream iff(PhitFile);
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
void readPhi0(char* Phi0File)
{
	phi0 = new double*[N_IMGS];
	for(int i=0;i<N_IMGS;i++)
	{
		phi0[i] = new double[CHARS];
	}
	ifstream iff(Phi0File);
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
void readImages(char* ImageFile)
{
	allImages.resize(0);
	ifstream iff(ImageFile);
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
	





double getScore(char* word)
{
	double score = 1;
	
	// We know Score(word/img) = mult(phi_0(img[i],w[i]))*mult(phi_t(w[i],w[i+1]))*mult(phi_s(w[i],w[j]))	-	Eqn. 1
	
	// Term 1 of eqn. 1 corresponding phi_0
	int n = strlen(word);
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
				if(images[i] == images[j])
				{
					if(word[i] == word[j])
						score *= 5;
					else score *= 0.5;
				}
				else
				{
					if(word[i] != word[j])
						score *= 2;
				}
			}
		}
	}


	return score;
}

// Calculated Z value with image given 
double calcZ(char* wordnow)
{
	int length = images.size();
	int len = strlen(wordnow);
	double totalScore = 0;
	if(len == length)
	{
		// Do both max-finding and Z calculation simultaneously
		// return Score only if wordnow is fully filled
		double tmpScore = getScore(wordnow);
		if(bestScore < tmpScore)
		{
			bestScore = tmpScore;	
			bestWord = wordnow;
		}
		return tmpScore;
	}
	else
	{
		// If wordnow is not fully filled , add one character and recurse
		for(int i = 0 ; i < N_LTRS ; i++)
		{
			char newword[len+2];
			strcpy(newword , wordnow);
			newword[len] = letters[i];
			newword[len+1] = '\0' ;
			totalScore += calcZ( newword );
		}
		return totalScore;
	}
}





double getLogProb(char* word)
{
	// // We know P(word/img) = (1/Z)*Score(word/img)
	// // So , Log(P) = log(Score(word)) - log(Z)	-	Eqn. 1
	return getScore(word)*(1/Z);
}



// Currently not used because its task is done inside calcZ() function itself
void getMaxProb(char* wordnow )
{
	int length = images.size();
	int len = strlen(wordnow);
	if(len == length)
	{
		// Replace if score greater
		double tmpScore = getLogProb(wordnow);
		if(bestScore < tmpScore)
		{
			bestScore = tmpScore;	
			bestWord = wordnow;
		}
	}
	else
	{	
		for(int i = 0 ; i < N_LTRS ; i++)
		{
			char newword[len+2];
			strcpy(newword , wordnow);
			newword[len] = letters[i];
			newword[len+1] = '\0' ;
			getMaxProb(newword);
		}
	}
}



// Default Mode 3
void loadModel(bool loadmod)
{
	if(loadmod)
	{
		readPhit("OCRDataset/potentials/trans.dat");
		readPhi0("OCRDataset/potentials/ocr.dat");
	}
	mode = 3;
}

void changeMode(int mod)
{
	mode = mod;
}

double getProb(vector<int>& image1 , string& word , int mod , bool loadmod)
{
	//Convert string to char* first
	char* chr = new char[word.length()+1];
	strcpy(chr , word.c_str());
	//Load model if required
	loadModel(loadmod);
	changeMode(mod);
	//Load image
	images = image1;
	double result = getLogProb(chr);

	//Delete dynamically allocated char array and return probability.
	delete[] chr;
	return result;
}



void getBestWord()
{

	// Calculate Z value
	bestScore = -1000000000;
	bestWord = "";
	Z = calcZ("");
	// printf("Z calculation over\n");
	// Run getting max probability (currently done in calcZ() itself)
	// getMaxProb("");
}

vector<string> getBestWords(char* inputImageFile , string& outFile , int mod)
{
	// Update global variables from file
	createHash();
	readImages(inputImageFile);

	// Load model into global variables
	loadModel(1);
	changeMode(mod);
	vector<string> words;
	double logs = 0;
	ofstream off(outFile.c_str());
	ofstream off1("logs.txt", ofstream::out | ofstream::app);
	for(int i = 0 ; i < allImages.size() ; i++)
	{

		images = allImages[i];
		getBestWord();
		words.push_back(bestWord);
		off << words[i];
		off <<  "\n";		
		// Calculate log(P)
		logs += log(getProb(allImages[i] , words[i], mode , 0));		
	}
	off.close();
	
	cout<< logs/double(allImages.size())<<endl;
	off1 << logs/double(allImages.size()) << endl;
	off1.close();
	return words;
}



int main()
{
	createHash();
	int start_s=clock();
	// getBestWords("OCRDataset/data/small/images.dat",3);
	// for(int i=1;i<10;i++)
	// {
	// 	skip_factor = 1 + float(i)/float(10);
		string s = "extra/out_extra_dec.txt";
		getBestWords("OCRDataset/data/small/images.dat", s , 3);
	// }
	int stop_s=clock();
	cout << "time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << endl;
}