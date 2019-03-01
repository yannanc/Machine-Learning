#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void inputTrainData(string trainData[][10])
{
    //test data input
    ifstream infile("4Cat-Train.labeled");

    int i=0,j=0;
    for (i=0;i<11;i++)
        for (j=0;j<10;j++)
        {
            infile>>trainData[i][j];
        }

    infile.close();
}

int main(int argc, char *argv[])
{
//B1
    cout << 16 << endl;    //input space: 2^4 = 16
//B2
    cout << 65536 << endl;    //concept space: lg(2^16) = 65536

//B3
    //1. test data input
    int i=0,j=0,k=0;
	string trainData[11][10];
    inputTrainData(trainData);

    //list all possible input
    string all_input[16][4]={{"Young","3","Southampton","Male"},{"Young","3","Southampton","Female"},{"Young","3","Queenstown","Male"},{"Young","3","Queenstown","Female"},
                             {"Young","1","Southampton","Male"},{"Young","1","Southampton","Female"},{"Young","1","Queenstown","Male"},{"Young","1","Queenstown","Female"},
                             {"Old","3","Southampton","Male"},{"Old","3","Southampton","Female"},{"Old","3","Queenstown","Male"},{"Old","3","Queenstown","Female"},
                             {"Old","1","Southampton","Male"},{"Old","1","Southampton","Female"},{"Old","1","Queenstown","Male"},{"Old","1","Queenstown","Female"}};
    int traintype[11] = {0};
    for (i=0;i<11;i++)  //11 train data
    {
        for (k=0;k<16;k++) //16 input space
        {
            if (!all_input[k][0].compare(trainData[i][1]))
                if (!all_input[k][1].compare(trainData[i][3]))
                    if (!all_input[k][2].compare(trainData[i][5]))
                        if (!all_input[k][3].compare(trainData[i][7]))
                            traintype[i] = k;
        }
    }

    cout << 32 << endl; //2^(2^4-number of train data) = 2^(16-11) = 32

//list concept space
    int cp[32][16];
        //initialization to 3
    for (j=0;j<32;j++)
        for (k=0;k<16;k++)
            cp[j][k] = 3;
        //set the determined number
    for (i=0;i<11;i++)
    {
        if (!trainData[i][9].compare("Yes"))
            for (j=0;j<32;j++)
                cp[j][traintype[i]] = 1;
        else
            for (j=0;j<32;j++)
                cp[j][traintype[i]] = 0;
    }
        //set the 1 undetermined number
    for (k=0;k<16;k++)
    {
        if (cp[0][k]==3)
        {
            for (j=0;j<32;j++)
            {
                if (j%2 == 0)
                    cp[j][k] = 0;
                else
                    cp[j][k] = 1;
            }
            break;
        }
    }
        //set the 2 undetermined number
    for (k=0;k<16;k++)
    {
        if (cp[0][k]==3)
        {
            for (j=0;j<32;j++)
            {
                if (j%4 == 0){ cp[j][k] = 0; cp[j+1][k] = 0;}
                if (j%4 == 2){ cp[j][k] = 1; cp[j+1][k] = 1;}
            }
            break;
        }
    }
        //set the 3 undetermined number
    for (k=0;k<16;k++)
    {
        if (cp[0][k]==3)
        {
            for (j=0;j<32;j++)
            {
                if (j%8 == 0){ cp[j][k] = 0; cp[j+1][k] = 0; cp[j+2][k] = 0; cp[j+3][k] = 0;}
                if (j%8 == 4){ cp[j][k] = 1; cp[j+1][k] = 1; cp[j+2][k] = 1; cp[j+3][k] = 1;}
            }
            break;
        }
    }
        //set the 4 undetermined number
    for (k=0;k<16;k++)
    {
        if (cp[0][k]==3)
        {
            for (j=0;j<32;j++)
            {
                if (j%16 == 0){ cp[j][k] = 0; cp[j+1][k] = 0; cp[j+2][k] = 0; cp[j+3][k] = 0;
                                cp[j][k+4] = 0; cp[j+5][k] = 0; cp[j+6][k] = 0; cp[j+7][k] = 0;}
                if (j%16 == 8){ cp[j][k] = 1; cp[j+1][k] = 1; cp[j+2][k] = 1; cp[j+3][k] = 1;
                                cp[j][k+4] = 1; cp[j+5][k] = 1; cp[j+6][k] = 1; cp[j+7][k] = 1;}
            }
            break;
        }
    }
        //set the 5 undetermined number
    for (k=0;k<16;k++)
    {
        if (cp[0][k]==3)
        {
            for (j=0;j<32;j++)
            {
                if (j<16)
                    cp[j][k] = 0;
                else
                    cp[j][k] = 1;
            }
            break;
        }
    }

//B4 test data
    ifstream infile(argv[1]);
    //ifstream infile("4Cat-Dev.labeled");
	string testData[10][10];
    for (i=0;i<10;i++)
        for (j=0;j<10;j++)
        {
            infile>>testData[i][j];
        }
    infile.close();

    int testtype[10] = {17,17,17,17,17,17,17,17,17,17};
    for (i=0;i<10;i++)  //10 test data
    {
        for (k=0;k<16;k++) //16 input space
        {
            if (!all_input[k][0].compare(testData[i][1]))
                if (!all_input[k][1].compare(testData[i][3]))
                    if (!all_input[k][2].compare(testData[i][5]))
                        if (!all_input[k][3].compare(testData[i][7]))
                            testtype[i] = k;
        }
    }

    int noteYes = 0;//0:No 1:Yes
    for (i=0;i<10;i++)
    {
        if (testtype[i] == 17)
            break;
        noteYes = 0;
        for (j=0;j<32;j++)
        {
            if (cp[j][testtype[i]] == 1)
                noteYes++;
        }
        cout << noteYes << " " << (32-noteYes) <<endl;
    }

    return 0;
}