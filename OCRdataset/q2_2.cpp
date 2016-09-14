#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <map>
#include <limits>
using namespace std;

string index2char = "etaoinshrd";
map<char,int> char2index;
double ocr_factor[10][1000];
double trans_factor[10][10];
double normalizer = 0;
double log_5 = log(5);

string norm_filepath = "norm2.txt";

// conf = '1234' indexes as string for each function
// vector<string> image_ids is image_ids line tokenized already

vector<string> string2vec(string line, char delimiter){
	vector<string> res;
	string cur = "";
	for(int i = 0; i < line.length(); i++){
		if(line[i] == delimiter){
			res.push_back(cur);
			cur = "";
		}
		else{
			cur += line[i];
		}
	}
	res.push_back(cur);
	return res;
}

double getLogConfScore(string conf_index_list, vector<string> image_ids, bool include_transition, bool include_skip){
	double log_score = 0;
	for (int i  = 0; i < image_ids.size(); i++){
		int image_id = atoi(image_ids[i].c_str());
		log_score += ocr_factor[conf_index_list[i] - '0'][image_id];
	}
	if(include_transition){
		for(int i = 0; i < conf_index_list.length()-1; i++){
			log_score += trans_factor[conf_index_list[i] - '0'][conf_index_list[i+1] - '0'];
		}
	}
	if(include_skip){
		for(int i = 0; i < conf_index_list.length()-1; i++){
			for(int j = i+1; j < conf_index_list.length(); j++){
				if((image_ids[i] == image_ids[j]) && (conf_index_list[i] == conf_index_list[j])){
					log_score += log_5;
				}
			}
		}
	}
	return log_score;
}


string int2flstr(int i, int len){
	string res = to_string(i);
	int resid = len - res.length();
	for(int j = 0; j < resid; j++){
		res = "0" + res;
	}
	return res;
}

double getNormalizer(vector<string> image_ids, bool include_transition, bool include_skip){
	double norm = 0;
	int word_len = image_ids.size();
	for(int i = 0; i < pow(10,word_len); i++){
		string conf_str = int2flstr(i, word_len);
		norm += exp(getLogConfScore(conf_str, image_ids, include_transition, include_skip));
	}
	return norm;
}


void loadOCRFactors(string ocr_filename){
	ifstream ocr_file(ocr_filename);
	int image_id;
	char character;
	double factor;
	while(ocr_file >> image_id >> character >> factor){
		ocr_factor[char2index[character]][image_id] = log(factor);
	}
}


void loadTransFactor(string trans_filename){
	ifstream trans_file(trans_filename);
	char char1, char2;
	double factor;
	while(trans_file >> char1 >> char2 >> factor){
		trans_factor[char2index[char1]][char2index[char2]] = log(factor);
	}	
}

void runQuery(string in_filename, string out_filename, bool include_transition, bool include_skip){
	ifstream in_file (in_filename);
	ofstream out_file (out_filename);
	ofstream norm_file (norm_filepath);
	string line = "";
	while(getline(in_file,line)){
		string max_conf = "";
		double max_score = -1*numeric_limits<double>::max();
		vector<string> image_ids = string2vec(line, '\t');
		int word_len = image_ids.size();
		double norm = 0;
		for(int i = 0; i < pow(10,word_len); i++){
			string conf_str = int2flstr(i, word_len);
			double cur_score = getLogConfScore(conf_str, image_ids, include_transition, include_skip);
			norm += exp(cur_score);
			if(cur_score >= max_score){
				max_score = cur_score;
				max_conf = conf_str;
			}
		}
		for(int i = 0; i < word_len; i++){
			string s = "0";
			s[0] = index2char[max_conf[i] - '0'];
			out_file << s;
		}
		norm_file << norm << "\n";
		out_file << "\n";
	}
}
double getProb(string conf, vector<string> image_ids, bool include_transition, bool include_skip){
	int conf_index = atoi(conf.c_str());
	ifstream norm_file (norm_filepath);
	double norm;
	for (int i = 0; i <conf_index; i++){
		norm_file >> norm;
	}
	norm_file >> norm;
	return (exp(getLogConfScore(conf, image_ids, include_transition, include_skip) - log(norm)));
}

double getCharWiseAcc(string output_filename, string correct_output_filename){
	ifstream my_output (output_filename);
	ifstream corr_output (correct_output_filename);
	string my_line = "", corr_line = "";
	double correct_preds = 0, total_chars = 0;
	while(getline(corr_output, corr_line)){
		total_chars += corr_line.length();
		getline(my_output, my_line);
		for(int i = 0; i < corr_line.length(); i++){
			if(corr_line[i] == my_line[i]) correct_preds++;
		}
	}
	return (correct_preds/total_chars);
}

double getWordWiseAcc(string output_filename, string correct_output_filename){
	ifstream my_output (output_filename);
	ifstream corr_output (correct_output_filename);
	string my_line = "", corr_line = "";
	double correct_preds = 0, total_words = 0;
	while(getline(corr_output, corr_line)){
		total_words += 1.0;
		getline(my_output, my_line);
		if(corr_line == my_line) correct_preds++;
	}
	return (correct_preds/total_words);
}

double avgDataLogLike(string words_filename, string img_ids_filename,  bool include_transition, bool include_skip){
	ifstream words_file (words_filename);
	ifstream img_ids_file (img_ids_filename);
	ifstream norm_file (norm_filepath);
	string line;
	double total_log_like = 0;
	double total_words = 0.0;
	while(getline(words_file, line)){
		string img_ids_line;
		getline(img_ids_file, img_ids_line);
		double norm;
		norm_file >> norm;
		vector<string> image_ids = string2vec(img_ids_line, '\t');
		for(int i = 0; i < line.length(); i++){
			line[i] = char2index[line[i]] + '0';
		}
		double prob = exp(getLogConfScore(line, image_ids, include_transition, include_skip) - log(norm));
		total_log_like += log(prob);
		total_words += 1.0;
	}
	return (total_log_like/total_words);
}

int main(int argc, char** argv){
	char2index['e']	= 0;
	char2index['t']	= 1;
	char2index['a']	= 2;
	char2index['o']	= 3;
	char2index['i']	= 4;
	char2index['n']	= 5;
	char2index['s']	= 6;
	char2index['h']	= 7;
	char2index['r']	= 8;
	char2index['d']	= 9;

	ifstream input_file ("input2");
	string ocr_potentials_filepath;
	
	ofstream out_file ("result/results_2.txt");
	for(int i = 0; i< 5; i++){
		input_file >> ocr_potentials_filepath;

		loadOCRFactors(ocr_potentials_filepath);

		int include_transition;
		input_file >> include_transition;

		if(include_transition){
			string trans_potentials_filepath;
			input_file >> trans_potentials_filepath;
			loadTransFactor(trans_potentials_filepath);
		}
		
		int include_skip;
		input_file >> include_skip;
		
		string images_filepath;
		input_file >> images_filepath;

		string output_filepath;
		input_file >> output_filepath;
		
		string words_filepath;
		input_file >> words_filepath;


		runQuery(images_filepath,output_filepath, include_transition, include_skip);
		
		double char_acc = getCharWiseAcc(output_filepath, words_filepath);
		cout << "Chanracter wise accuracy :" << char_acc << endl;
		double word_acc = getWordWiseAcc(output_filepath, words_filepath);
		cout << "Words wise accuracy : "  << word_acc << endl;
		double loglike = avgDataLogLike(words_filepath, images_filepath, include_transition, include_skip);
		cout << "Avg. Dataset log-likelihood : " << loglike << endl;

		out_file << images_filepath << " , " << words_filepath << " , " << include_transition << " , " << include_skip  << "---------------" << endl;
		out_file << "Chanracter wise accuracy :" << char_acc << endl;
		out_file << "Words wise accuracy : "  << word_acc << endl;
		out_file << "Avg. Dataset log-likelihood : " << loglike << endl;
		out_file << endl;
	}
		out_file.close();
}	