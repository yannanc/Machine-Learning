#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void inputTrainData(string trainData[][20])
{
    //test data input
    ifstream infile("9Cat-Train.labeled");

    int i=0,j=0;
    for (i=0;i<300;i++)
        for (j=0;j<20;j++)
        {
            infile>>trainData[i][j];
        }

    infile.close();
}

int main(int argc, char *argv[])
{
//A1-A5
    cout << 512 << endl;    //input space: 2^9 = 512
    cout << 155 << endl;    //concept space: lg(2^512) = 155
    cout << 19684 << endl;    //hypothesis space: 1+3^9 = 19684
    cout << 59050 << endl;    //hypothesis space: 1+3^10 = 59050
    cout << 26245 << endl;    //hypothesis space: 1+4*3^8 = 26245

//A6
    //1. test data input
	string trainData[300][20];
    int i=0,j=0;
    inputTrainData(trainData);


    //2. output partA6.txt, gain the hypothesis
    ofstream outfile("partA6.txt",ios::out);

    string h[20]={"Age","Young","Cabin","B","Class","3","CrewMember?","Yes","Embarked","Southampton",
    "Sex","Female","TravelingwithSibling?","No","TravelingWithSpouseOrParent?","Yes","NearRescueBoat?","No","Survived","Yes"};

	for(i=1;i<275;i++)
	{
		if ((i % 20) == 0)
		{
            outfile<<h[1]<<'\t'<<h[3]<<'\t'<<h[5]<<'\t'<<h[7]<<'\t'<<h[9]<<'\t'<<h[11]<<'\t'<<h[13]<<'\t'<<h[15]<<'\t'<<h[17]<<'\n';
		}
		if(!trainData[i][19].compare("Yes"))
		{
			for(j=0;j<18;j++)
			{
				if(h[j].compare(trainData[i][j]))
				{
					h[j]="?";
				}
			}
		}
	}
    outfile.close();

//gain the hypothesis:     Young	?	?	?	?	Female	?	?	?

//A7: calculate the misclassified data points
/*
    ifstream infile("9Cat-Dev.labeled");
	string devData[100][20];
    for (i=0;i<100;i++)
        for (j=0;j<20;j++)
        {
            infile>>devData[i][j];
        }
    infile.close();

    string c[100];
    for (i=0;i<100;i++)
    {
        c[i]="Yes";
        for (j=0;j<18;j++)
        {
            if (j % 2 ==0)
                continue;
            if ((h[j].compare("?"))&&(h[j].compare(devData[i][j])))
                c[i]="No";
        }
    }

    int misclassiRate = 0;
    for (i=0;i<100;i++)
    {
        if (c[i]!=devData[i][19])
        {
            misclassiRate++;
            cout << i << endl;
        }
    }
    cout << misclassiRate;    // output16
*/
    cout << 0.16 << endl;  // 16/100 = 0.16

//A8 test data
    ifstream infile(argv[1]);
	string testData[100][20];
    for (i=0;i<100;i++)
        for (j=0;j<20;j++)
        {
            infile>>testData[i][j];
        }
    infile.close();

    bool flag = 1;//0:No 1:Yes
    for (i=0;i<100;i++)
    {
        flag = 1;
        for (j=0;j<18;j++)
        {
            if (j % 2 ==0)
                continue;
            if ((h[j].compare("?"))&&(h[j].compare(testData[i][j])))
                flag = 0;
        }
        if (flag)
            cout << "Yes" << endl;
        else
            cout << "No" << endl;
    }

    return 0;
}