#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <string.h>
#include <vector>
#include <sstream>
#include <math.h>
#include <stdlib.h>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>

using namespace std;

vector<string> input_trainfiles;
vector<int> list_train_classes;
vector<string> input_testfiles;
vector<int> list_test_classes;
vector< vector<string> > list_of_docs;
map<string,int>  my_vocab_list;
int *return_vec;
vector< vector<int> > train_mat;

vector<float> p_lib_vect;
vector<float> p_con_vect;
float p_lib;
float p_con;

void create_vocab_list();
void set_of_words_to_vec(int idx);
void get_train_matrix();
void train_NB();


int main(int argc, char *argv[])
{
    //input data
    ifstream trainFile(argv[1]),testFile(argv[2]);
    //ifstream trainFile("split.train"),testFile("split.test");
    string line;
    while (getline(trainFile, line))
    {
        input_trainfiles.push_back(line);
        if (line.find("lib") != string::npos)
            list_train_classes.push_back(0);
        else
            list_train_classes.push_back(1);
    }
    while (getline(testFile, line))
    {
        input_testfiles.push_back(line);
        if (line.find("lib") != string::npos)
            list_test_classes.push_back(0);
        else
            list_test_classes.push_back(1);
    }
    trainFile.close();
    testFile.close();

    // create list_of_docs
    for(unsigned int i=0; i<input_trainfiles.size();i++)
    {
        vector<string> vec;
        char filename[10];
        strcpy(filename, input_trainfiles[i].c_str());
        ifstream in(filename);
        while(getline(in,line))
        {
            transform(line.begin(),line.end(),line.begin(),::tolower);
            vec.push_back(line);
        }
        list_of_docs.push_back( vec );
        in.close();
    }

    create_vocab_list();
    get_train_matrix();
    train_NB();  //get the probability


    /*     TEST     */
    float wrongrate = 0;
    int vocabulary_size = my_vocab_list.size();
    for(unsigned int i=0; i<input_testfiles.size();i++)
    {
        return_vec = new int[ vocabulary_size ]();

        vector<string> vec;
        char filename[10];
        strcpy(filename, input_testfiles[i].c_str());
        ifstream in(filename);
        while(getline(in,line))
        {
            transform(line.begin(),line.end(),line.begin(),::tolower);
            int pos = my_vocab_list[ line ];
            if( pos!=0 )
				return_vec[ pos ] += 1;
        }

        float plib = log(p_lib), pcon = log(p_con);
        for(int j=0; j<vocabulary_size; j++)
        {
            plib += p_lib_vect[j]*return_vec[j];
            pcon += p_con_vect[j]*return_vec[j];
        }

        if( plib > pcon )
        {
            cout << "L" <<endl;
            if (list_test_classes[i]== 1)
                wrongrate++;
        }
        else
        {
			cout << "C" <<endl;
			if (list_test_classes[i]== 0)
                wrongrate++;
        }

        in.close();
    }
    wrongrate = wrongrate/list_test_classes.size();

    cout.setf(ios::fixed);
    cout << "Accuracy: " << fixed << setprecision(4) << (1-wrongrate) <<endl;

    return 0;
}



void create_vocab_list()
{
    vector< vector<string> > :: iterator it = list_of_docs.begin();
	int index = 1;
	while( it!=list_of_docs.end() )
    {
		vector<string> vec = *it;
		vector<string> :: iterator tmp_it = vec.begin();
		while(tmp_it != vec.end())
		{
            if( my_vocab_list[*tmp_it] == 0 )
                my_vocab_list[*tmp_it] = index++; //index is the location of the vocabulary
            tmp_it++;
        }
        it++;
    }
}

//calculate the times of words appears in doc[i]
void set_of_words_to_vec(int idx)
{
    int len = my_vocab_list.size();
    return_vec = new int[len]();
    fill(return_vec,return_vec+len,0);

	vector< vector<string> >:: iterator it = list_of_docs.begin() + idx;
	vector<string> vec  = *it;
	vector<string> :: iterator itt = vec.begin();
	int pos = 0 ;
	while( itt!=vec.end() )
	{
		pos = my_vocab_list[ *itt ];
		if(pos!=0)
            return_vec[pos] += 1;
		itt++;
	}
}

void get_train_matrix()
{
    train_mat.clear();
    for(unsigned int i=0;i<list_of_docs.size();i++)
    {
        set_of_words_to_vec(i);
        vector<int> vec( return_vec , return_vec + my_vocab_list.size() );
        train_mat.push_back(vec);
        delete []return_vec;
    }
}

void train_NB()
{
    int num_train_docs = train_mat.size();  //the total number of train docs
    int num_words = my_vocab_list.size();

    int sum = accumulate(list_train_classes.begin(),list_train_classes.end(),0); //calculate the sum of the con classes
    p_con =  (float)sum/(float)num_train_docs;
    p_lib = 1 - p_con;

    p_lib_vect.resize(my_vocab_list.size(),1);
    p_con_vect.resize(my_vocab_list.size(),1);

    float plibDenom = num_words; //the total number of words in lib docs
    float pconDenom = num_words; //the total number of words in con docs

    //calculate the probability
    for(int i=0;i<num_train_docs;i++)
    {
        if(list_train_classes[i] == 1)  //con
        {
            for(unsigned int j=0; j<my_vocab_list.size();j++)
                p_con_vect[j] += train_mat[i][j];
            pconDenom += list_of_docs[i].size();
        }
        else   //lib
        {
            for(unsigned int j=0;j<my_vocab_list.size();j++)
                p_lib_vect[j] += train_mat[i][j];
            plibDenom += list_of_docs[i].size();
        }
    }

    for(unsigned int i=0;i<my_vocab_list.size();i++)
    {
        p_lib_vect[i] = log(p_lib_vect[i]/plibDenom);
        p_con_vect[i] = log(p_con_vect[i]/pconDenom);
    }
}
