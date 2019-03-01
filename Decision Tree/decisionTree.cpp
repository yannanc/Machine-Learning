#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <math.h>

using namespace std;

vector< vector<string> > trainData;
vector< vector<string> > testData;
vector< vector<string> > map_attribute_values;
unsigned int colNum, dataNum, i, j;
int tree[3][3]={0,0,0,0,0,0,0,0,0};

string A = "A\r";
bool isPos(const string& s)
{
	if (s.find("democrat") != string::npos)
		return true;
	else if (s == A)
		return true;
	else if (s.find("yes") != string::npos)
		return true;
	else 
		return false;
}

void formMap();
float calculateEntropy (vector< vector<string> > subset, int attribute, string value, bool ifroot);
float calculateGain (vector< vector<string> > subset, int attribute);
int findNode(vector< vector<string> > subset);
int majorityVote(vector< vector<string> > subset);

int main(int argc, char *argv[])
{
    //input data
    ifstream trainDataFile(argv[1]),testDataFile(argv[2]);
    string line;
    while (getline(trainDataFile, line))
    {
        stringstream ss(line);
        string str;
        vector<string> lineArray;
        while (getline(ss, str, ','))
            lineArray.push_back(str);
        trainData.push_back(lineArray);
    }
    while (getline(testDataFile, line))
    {
        stringstream ss(line);
        string str;
        vector<string> lineArray;
        while (getline(ss, str, ','))
            lineArray.push_back(str);
        testData.push_back(lineArray);
    }
    trainDataFile.close();
    testDataFile.close();

    colNum = trainData[0].size();
    dataNum = trainData.size();
    formMap();

    int rootNode = findNode(trainData);
    tree[0][0] = rootNode;

    //find subset1(rootNode value[0])
    vector< vector<string> > subset1;
    subset1.push_back(trainData[0]);
    for (i=1;i<trainData.size();i++)
    {
        if (trainData[i][rootNode] == map_attribute_values[rootNode][0])
            subset1.push_back(trainData[i]);
    }
    int childNode1 = findNode(subset1);
    tree[1][0] = childNode1;
    if (childNode1 != 100)
    {
        vector< vector<string> > subset11;
        vector< vector<string> > subset12;
        subset11.push_back(subset1[0]);
        subset12.push_back(subset1[0]);
        for (i=1;i<subset1.size();i++)
        {
            if (subset1[i][childNode1] == map_attribute_values[childNode1][0])
                subset11.push_back(subset1[i]);
            else
                subset12.push_back(subset1[i]);
        }
        tree[1][1] = majorityVote(subset11);
        tree[1][2] = majorityVote(subset12);
        subset11.clear();
        subset12.clear();
    }
    else
        tree[0][1]= majorityVote(subset1);

    //find subset2(rootNode value[1])
    vector< vector<string> > subset2;
    subset2.push_back(trainData[0]);
    for (i=1;i<trainData.size();i++)
    {
        if (trainData[i][rootNode] == map_attribute_values[rootNode][1])
            subset2.push_back(trainData[i]);
    }
    int childNode2 = findNode(subset2);
    tree[2][0] = childNode2;
    if (childNode2 != 100)
    {
        vector< vector<string> > subset21;
        vector< vector<string> > subset22;
        subset21.push_back(subset2[0]);
        subset22.push_back(subset2[0]);
        for (i=1;i<subset2.size();i++)
        {
            if (subset2[i][childNode2] == map_attribute_values[childNode2][0])
                subset21.push_back(subset2[i]);
            else
                subset22.push_back(subset2[i]);
        }
        tree[2][1] = majorityVote(subset21);
        tree[2][2] = majorityVote(subset22);
        subset21.clear();
        subset22.clear();
    }
    else
        tree[0][2]= majorityVote(subset2);

    //cout <<"tree"<<endl;
    //cout << tree[0][0] << tree[0][1] <<tree[0][2]<<endl;
    //cout << tree[1][0] << tree[1][1] <<tree[1][2]<<endl;
    //cout << tree[2][0] << tree[2][1] <<tree[2][2]<<endl;
////////////////Tree has been built////////////////////////

    int trueResult[6][2]={0};
    int trueTestResult[6][2]={0};
    float trainerror = 0, testerror = 0;

    //test traindata
    if (tree[0][1] != 0)
    {
        for (i=1;i<trainData.size();i++)
        {
            if (trainData[i][rootNode] == map_attribute_values[rootNode][0])
            {
                //if ((trainData[i][trainData[0].size()-1] == democrat)||(trainData[i][trainData[0].size()-1] == A)||(trainData[i][trainData[0].size()-1] == yes))
                if (isPos(trainData[i][trainData[0].size()-1]))
                    trueResult[0][0]++;
                else
                    trueResult[0][1]++;
            }
        }
    }
    if (tree[0][2] != 0)
    {
        for (i=1;i<trainData.size();i++)
        {
            if (trainData[i][rootNode] == map_attribute_values[rootNode][1])
            {
                //if ((trainData[i][trainData[0].size()-1] == democrat)||(trainData[i][trainData[0].size()-1] == A)||(trainData[i][trainData[0].size()-1] == yes))
                if (isPos(trainData[i][trainData[0].size()-1]))
                    trueResult[3][0]++;
                else
                    trueResult[3][1]++;
            }
        }
    }
    if (tree[0][1] == 1) trainerror += trueResult[0][1];
    if (tree[0][1] == 2) trainerror += trueResult[0][0];
    if (tree[0][2] == 1) trainerror += trueResult[3][1];
    if (tree[0][2] == 2) trainerror += trueResult[3][0];



    if (tree[0][1] == 0)
    {
        for (i=1;i<trainData.size();i++)
        {
            if ((trainData[i][rootNode] == map_attribute_values[rootNode][0])&&(trainData[i][childNode1] == map_attribute_values[childNode1][0]))
            {
                //f ((trainData[i][trainData[0].size()-1] == democrat)||(trainData[i][trainData[0].size()-1] == A)||(trainData[i][trainData[0].size()-1] == yes))
                if (isPos(trainData[i][trainData[0].size()-1]))
                    trueResult[1][0]++;
                else
                    trueResult[1][1]++;
            }
            else if ((trainData[i][rootNode] == map_attribute_values[rootNode][0])&&(trainData[i][childNode1] == map_attribute_values[childNode1][1]))
            {
                //if ((trainData[i][trainData[0].size()-1] == democrat)||(trainData[i][trainData[0].size()-1] == A)||(trainData[i][trainData[0].size()-1] == yes))
                if (isPos(trainData[i][trainData[0].size()-1]))
                    trueResult[2][0]++;
                else
                    trueResult[2][1]++;
            }
        }
    }
    if (tree[1][1] == 1) trainerror += trueResult[1][1];
    if (tree[1][1] == 2) trainerror += trueResult[1][0];
    if (tree[1][2] == 1) trainerror += trueResult[2][1];
    if (tree[1][2] == 2) trainerror += trueResult[2][0];


    if (tree[0][2] == 0)
    {
        for (i=1;i<trainData.size();i++)
        {
            if ((trainData[i][rootNode] == map_attribute_values[rootNode][1])&&(trainData[i][childNode2] == map_attribute_values[childNode2][0]))
            {
                //if ((trainData[i][trainData[0].size()-1] == democrat)||(trainData[i][trainData[0].size()-1] == A)||(trainData[i][trainData[0].size()-1] == yes))
                if (isPos(trainData[i][trainData[0].size()-1]))
                    trueResult[4][0]++;
                else
                    trueResult[4][1]++;
            }
            else if ((trainData[i][rootNode] == map_attribute_values[rootNode][1])&&(trainData[i][childNode2] == map_attribute_values[childNode2][1]))
            {
                //if ((trainData[i][trainData[0].size()-1] == democrat)||(trainData[i][trainData[0].size()-1] == A)||(trainData[i][trainData[0].size()-1] == yes))
                if (isPos(trainData[i][trainData[0].size()-1]))
                    trueResult[5][0]++;
                else
                    trueResult[5][1]++;
            }
        }
    }
    if (tree[2][1] == 1) trainerror += trueResult[4][1];
    if (tree[2][1] == 2) trainerror += trueResult[4][0];
    if (tree[2][2] == 1) trainerror += trueResult[5][1];
    if (tree[2][2] == 2) trainerror += trueResult[5][0];


    //test testdata
    if (tree[0][1] != 0)  //
    {
        for (i=1;i<testData.size();i++)
        {
            if (testData[i][rootNode] == map_attribute_values[rootNode][0])
            {
                //if ((testData[i][testData[0].size()-1] == democrat)||(testData[i][testData[0].size()-1] == A)||(testData[i][testData[0].size()-1] == yes))
                if (isPos(testData[i][testData[0].size()-1]))
                    trueTestResult[0][0]++;
                else
                    trueTestResult[0][1]++;
            }
        }
    }
    if (tree[0][2] != 0)
    {
        for (i=1;i<testData.size();i++)
        {
            if (testData[i][rootNode] == map_attribute_values[rootNode][1])
            {
                //if ((testData[i][testData[0].size()-1] == democrat)||(testData[i][testData[0].size()-1] == A)||(testData[i][testData[0].size()-1] == yes))
                if (isPos(testData[i][testData[0].size()-1]))
                    trueTestResult[3][0]++;
                else
                    trueTestResult[3][1]++;
            }
        }
    }
    if (tree[0][1] == 1) testerror += trueTestResult[0][1];
    if (tree[0][1] == 2) testerror += trueTestResult[0][0];
    if (tree[0][2] == 1) testerror += trueTestResult[3][1];
    if (tree[0][2] == 2) testerror += trueTestResult[3][0];

    if (tree[0][1] == 0)
    {
        for (i=1;i<testData.size();i++)
        {
            if ((testData[i][rootNode] == map_attribute_values[rootNode][0])&&(testData[i][childNode1] == map_attribute_values[childNode1][0]))
            {
                //if ((testData[i][testData[0].size()-1] == democrat)||(testData[i][testData[0].size()-1] == A)||(testData[i][testData[0].size()-1] == yes))
                if (isPos(testData[i][testData[0].size()-1]))
                    trueTestResult[1][0]++;
                else
                    trueTestResult[1][1]++;
            }
            else if ((testData[i][rootNode] == map_attribute_values[rootNode][0])&&(testData[i][childNode1] == map_attribute_values[childNode1][1]))
            {
                //if ((testData[i][testData[0].size()-1] == democrat)||(testData[i][testData[0].size()-1] == A)||(testData[i][testData[0].size()-1] == yes))
                if (isPos(testData[i][testData[0].size()-1]))
                    trueTestResult[2][0]++;
                else
                    trueTestResult[2][1]++;
            }
        }
    }
    if (tree[1][1] == 1) testerror += trueTestResult[1][1];
    if (tree[1][1] == 2) testerror += trueTestResult[1][0];
    if (tree[1][2] == 1) testerror += trueTestResult[2][1];
    if (tree[1][2] == 2) testerror += trueTestResult[2][0];

    if (tree[0][2] == 0)
    {
        for (i=1;i<testData.size();i++)
        {
            if ((testData[i][rootNode] == map_attribute_values[rootNode][1])&&(testData[i][childNode2] == map_attribute_values[childNode2][0]))
            {
                //if ((testData[i][testData[0].size()-1] == democrat)||(testData[i][testData[0].size()-1] == A)||(testData[i][testData[0].size()-1] == yes))
                if (isPos(testData[i][testData[0].size()-1]))
                    trueTestResult[4][0]++;
                else
                    trueTestResult[4][1]++;
            }
            else if ((testData[i][rootNode] == map_attribute_values[rootNode][1])&&(testData[i][childNode2] == map_attribute_values[childNode2][1]))
            {
                //if ((testData[i][testData[0].size()-1] == democrat)||(testData[i][testData[0].size()-1] == A)||(testData[i][testData[0].size()-1] == yes))
                if (isPos(testData[i][testData[0].size()-1]))
                    trueTestResult[5][0]++;
                else
                    trueTestResult[5][1]++;
            }
        }
    }
    if (tree[2][1] == 1) testerror += trueTestResult[4][1];
    if (tree[2][1] == 2) testerror += trueTestResult[4][0];
    if (tree[2][2] == 1) testerror += trueTestResult[5][1];
    if (tree[2][2] == 2) testerror += trueTestResult[5][0];


/*
    cout << "testtest"<<endl;
    for (i=0; i<6;i++)
    {
    	cout << trueResult[i][0] << " " << trueResult[i][1]<<endl;
    }
    cout << endl;

*/
    
    //output
    int posSum0 = 0, posSum1 = 0, negSum0 = 0,negSum1 = 0;
    for (i=0;i<6;i++)
    {
        if(i<3) posSum0 += trueResult[i][0];
        if(i<3) negSum0 += trueResult[i][1];
        if(i>2) posSum1 += trueResult[i][0];
        if(i>2) negSum1 += trueResult[i][1];
    }
    cout << "[" << posSum0+posSum1 << "+/" << negSum0+negSum1 << "-]" <<endl;
    cout << trainData[0][rootNode] << " = y: [" << posSum0 << "+/" << negSum0 << "-]" <<endl;
    if (tree[0][1] == 0)
    {
        cout <<"| " <<trainData[0][childNode1] << " = y: [" << trueResult[1][0] << "+/" << trueResult[1][1] << "-]" <<endl;
        cout <<"| " <<trainData[0][childNode1] << " = n: [" << trueResult[2][0] << "+/" << trueResult[2][1] << "-]" <<endl;
    }
    cout << trainData[0][rootNode] << " = n: [" << posSum1 << "+/" << negSum1 << "-]" <<endl;
    if (tree[0][2] == 0)
    {
        cout <<"| " <<trainData[0][childNode2] << " = y: [" << trueResult[4][0] << "+/" << trueResult[4][1] << "-]" <<endl;
        cout <<"| " <<trainData[0][childNode2] << " = n: [" << trueResult[5][0] << "+/" << trueResult[5][1] << "-]" <<endl;
    }

    cout << "error(train): " <<(float)trainerror/(trainData.size()-1)<<endl;
    cout << "error(test): " <<(float)testerror/(testData.size()-1)<<endl;

    return 0;
}

int majorityVote(vector< vector<string> > subset)
{
    int classifi[2]={0,0};
    for (i=1;i<subset.size();i++)  //check each row of data
    {
        //if ((subset[i][subset[0].size()-1] == democrat)||(subset[i][subset[0].size()-1] == A)||(subset[i][subset[0].size()-1] == yes))
        if (isPos(subset[i][subset[0].size()-1]))
            classifi[0]++;
        else
            classifi[1]++;
    }

    if (classifi[0] > classifi[1])
        return 1;//+
    else
        return 2;//-
}


void formMap()
{
    vector<string> values;
    for (j=0;j<(colNum-1);j++)
    {
        values.push_back(trainData[1][j]);
        for (i=1;i<dataNum;i++)
        {
            if (trainData[i][j] != trainData[1][j])
            {
                values.push_back(trainData[i][j]);
                break;
            }
        }
        map_attribute_values.push_back(values);
        values.clear();
    }
}

int findNode(vector< vector<string> > subset)
{
    float max_gain = 0, tmp = 0;
    int nodeNum = 0;
    for (j=0;j<(subset[0].size()-1);j++)
    {
        tmp = calculateGain(subset,j);
        if ( max_gain < tmp)
        {
            max_gain = tmp;
            nodeNum = j;
        }
    }
    if (max_gain >= 0.1)
        return nodeNum;
    else
        return 100;
}

float calculateEntropy (vector< vector<string> > subset, int attribute, string value, bool ifroot)
{
    int classifi[2]={0,0};
    for (i=1;i<subset.size();i++)
    {
        if (ifroot || (subset[i][attribute] == value))
        {
            //if ((subset[i][subset[0].size()-1] == democrat)||(subset[i][subset[0].size()-1] == A)||(subset[i][subset[0].size()-1] == yes))
            if (isPos(subset[i][subset[0].size()-1]))
                classifi[0]++;
            else
                classifi[1]++;
        }
    }
    if (classifi[0] == 0)    return 0;
    if (classifi[1] == 0)    return 0;
    float sum = classifi[0] + classifi[1];
	float entropy = classifi[0]/(sum)*log(sum/classifi[0])/log(2) + classifi[1]/(sum)*log(sum/classifi[1])/log(2);

    return entropy;
}

float calculateGain (vector< vector<string> > subset, int attribute)
{
    float father_entropy = calculateEntropy(subset, attribute, "", true);
    float child_entropy = 0;
    float value_count[2]={0,0};

    for (i=1;i<subset.size();i++)
    {
        if (subset[i][attribute] == map_attribute_values[attribute][0])
            value_count[0]++;
        else
            value_count[1]++;
    }

    child_entropy = value_count[0]/(value_count[0]+value_count[1])*calculateEntropy(subset, attribute,map_attribute_values[attribute][0], false)
                  + value_count[1]/(value_count[0]+value_count[1])*calculateEntropy(subset, attribute,map_attribute_values[attribute][1], false);
              
    return ( father_entropy - child_entropy );
}
