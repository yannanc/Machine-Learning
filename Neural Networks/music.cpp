#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <math.h>
#include <stdlib.h>

using namespace std;

vector< vector<double> > trainData;
vector<double> trainKey;
vector< vector<double> > devData;
vector< vector<double> > weight1;
vector<double> weight2;

double result = 0;  
double hdOutput[3]={0};
double inhd_weight[5][3] = {0};
double hdou_weight[4] = {0};
double delta_weight2 = 0;
double delta_weight1[3]={0};
unsigned int trainNum=0;    
unsigned int devNum=0;      
unsigned int i=0, j=0;
double LR = 0.4;      //learning rate

double Sigmoid(const double x)
{
    return 1/(1+exp(-x));
}

int main(int argc, char *argv[])
{
    //input data
    ifstream trainFile(argv[1]),trainKeyFile(argv[2]),devFile(argv[3]),weight1File(argv[4]),weight2File(argv[5]);

    string line;
    //get normalized train data
    getline(trainFile, line);
    while (getline(trainFile, line))    //ignore first line
    {
        stringstream ss(line);
        string str;
        vector<double> lineArray;
        while (getline(ss, str, ','))
        {
            if (str.find("yes") != string::npos) str = "1";
            lineArray.push_back(atof(str.c_str()));
        }
        trainData.push_back(lineArray);
    }
    trainNum = trainData.size();
    for (i = 0; i < trainNum; i++)
    {
        trainData[i][0] = (trainData[i][0]-1900)/100;
        trainData[i][1] = trainData[i][1]/7;
    }
    while (getline(trainKeyFile, line))
    {
        stringstream ss(line);
        string str;
        while (getline(ss, str, ','))
        {
            if (str.find("yes") != string::npos) str = "1";
            trainKey.push_back(atof(str.c_str()));
        }
    }
    //get normalized dev data
    getline(devFile, line);
    while (getline(devFile, line))    //ignore first line
    {
        stringstream ss(line);
        string str;
        vector<double> lineArray;
        while (getline(ss, str, ','))
        {
            if (str.find("yes") != string::npos) str = "1";
            lineArray.push_back(atof(str.c_str()));
        }
        devData.push_back(lineArray);
    }
    devNum = devData.size();
    for (i = 0; i < devNum; i++)
    {
        devData[i][0] = (devData[i][0]-1900)/100;
        devData[i][1] = devData[i][1]/7;
    }

    //get initialized weights
    while (getline(weight1File, line))
    {
        stringstream ss(line);
        string str;
        vector<double> lineArray;
        while (getline(ss, str, ','))
            lineArray.push_back(atof(str.c_str()));
        weight1.push_back(lineArray);
    }
    while (getline(weight2File, line))
    {
        stringstream ss(line);
        string str;
        while (getline(ss, str, ','))
            weight2.push_back(atof(str.c_str()));
    }

    trainFile.close();
    trainKeyFile.close();
    devFile.close();
    weight1File.close();
    weight2File.close();

    double loss = 0;            //loss of every epoch
    //begin training(gradient descent)
    for (i=0;i<5;i++)
        for (j=0;j<3;j++)
            inhd_weight[i][j] = weight1[i][j];
    for (i=0;i<4;i++)
        hdou_weight[i] = weight2[i];
    LR = 0.1;
    double inhd_weight_update[5][3] = {0};
    double hdou_weight_update[4] = {0};
    for(i = 0; i < 100; i++)
    {
        //initialize changes of weights to 0
        loss = 0;
        for (int k = 0;k<5;k++)
        {   inhd_weight_update[k][0] = 0;  inhd_weight_update[k][1] = 0;  inhd_weight_update[k][2] = 0;}
        hdou_weight_update[0]=0;
        hdou_weight_update[1]=0;
        hdou_weight_update[2]=0;
        hdou_weight_update[3]=0;

        //for every sample
        for(j = 0; j < trainNum; j++)
        {
            //ForwardTransfer
            hdOutput[0] = inhd_weight[0][0];
            hdOutput[1] = inhd_weight[0][1];
            hdOutput[2] = inhd_weight[0][2];
            for (int k=0;k<4;k++)
            {
                hdOutput[0] += inhd_weight[k+1][0]*trainData[j][k];
                hdOutput[1] += inhd_weight[k+1][1]*trainData[j][k];
                hdOutput[2] += inhd_weight[k+1][2]*trainData[j][k];
            }
            hdOutput[0] = Sigmoid(hdOutput[0]);
            hdOutput[1] = Sigmoid(hdOutput[1]);
            hdOutput[2] = Sigmoid(hdOutput[2]);
            result = Sigmoid( hdou_weight[0] + hdOutput[0]*hdou_weight[1] + hdOutput[1]*hdou_weight[2] + hdOutput[2]*hdou_weight[3]);

            loss += 0.5*(trainKey[j]-result)*(trainKey[j]-result);

            //calculate delta
            delta_weight2 = result*(1-result)*(trainKey[j]-result);
            delta_weight1[0] = hdOutput[0]*(1-hdOutput[0])*hdou_weight[1]*delta_weight2;
            delta_weight1[1] = hdOutput[1]*(1-hdOutput[1])*hdou_weight[2]*delta_weight2;
            delta_weight1[2] = hdOutput[2]*(1-hdOutput[2])*hdou_weight[3]*delta_weight2;

            //cumulative changes of weights
            hdou_weight_update[0] += LR*delta_weight2;
            hdou_weight_update[1] += LR*delta_weight2*hdOutput[0];
            hdou_weight_update[2] += LR*delta_weight2*hdOutput[1];
            hdou_weight_update[3] += LR*delta_weight2*hdOutput[2];
            for (int k=0;k<3;k++)
            {
                inhd_weight_update[0][k] += LR*delta_weight1[k];
                inhd_weight_update[1][k] += LR*delta_weight1[k]*trainData[j][0];
                inhd_weight_update[2][k] += LR*delta_weight1[k]*trainData[j][1];
                inhd_weight_update[3][k] += LR*delta_weight1[k]*trainData[j][2];
                inhd_weight_update[4][k] += LR*delta_weight1[k]*trainData[j][3];
            }
        }

        //update network
        hdou_weight[0] += hdou_weight_update[0];
        hdou_weight[1] += hdou_weight_update[1];
        hdou_weight[2] += hdou_weight_update[2];
        hdou_weight[3] += hdou_weight_update[3];
        for (int k=0;k<3;k++)
        {
            inhd_weight[0][k] += inhd_weight_update[0][k];
            inhd_weight[1][k] += inhd_weight_update[1][k];
            inhd_weight[2][k] += inhd_weight_update[2][k];
            inhd_weight[3][k] += inhd_weight_update[3][k];
            inhd_weight[4][k] += inhd_weight_update[4][k];
        }

        cout << loss << endl; //Print the loss after each epoch
    }
    cout << "GRADIENT DESCENT TRAINING COMPLETED!" << endl;

    //begin training(stochastic gradient descent)
    for (i=0;i<5;i++)
        for (j=0;j<3;j++)
            inhd_weight[i][j] = weight1[i][j];
    for (i=0;i<4;i++)
        hdou_weight[i] = weight2[i];
    LR = 0.4;
    for(i = 0; i < 50; i++)   //25-40:0.346154    50-100:0.269231
    {
        loss = 0;
        for(j = 0; j < trainNum; j++)
        {
            //ForwardTransfer
            hdOutput[0] = inhd_weight[0][0];
            hdOutput[1] = inhd_weight[0][1];
            hdOutput[2] = inhd_weight[0][2];
            for (int k=0;k<4;k++)
            {
                hdOutput[0] += inhd_weight[k+1][0]*trainData[j][k];
                hdOutput[1] += inhd_weight[k+1][1]*trainData[j][k];
                hdOutput[2] += inhd_weight[k+1][2]*trainData[j][k];
            }
            hdOutput[0] = Sigmoid(hdOutput[0]);
            hdOutput[1] = Sigmoid(hdOutput[1]);
            hdOutput[2] = Sigmoid(hdOutput[2]);
            result = Sigmoid( hdou_weight[0] + hdOutput[0]*hdou_weight[1] + hdOutput[1]*hdou_weight[2] + hdOutput[2]*hdou_weight[3]);

            //calculate delta
            delta_weight2 = result*(1-result)*(trainKey[j]-result);
            delta_weight1[0] = hdOutput[0]*(1-hdOutput[0])*hdou_weight[1]*delta_weight2;
            delta_weight1[1] = hdOutput[1]*(1-hdOutput[1])*hdou_weight[2]*delta_weight2;
            delta_weight1[2] = hdOutput[2]*(1-hdOutput[2])*hdou_weight[3]*delta_weight2;

            //update network
            hdou_weight[0] += LR*delta_weight2;
            hdou_weight[1] += LR*delta_weight2*hdOutput[0];
            hdou_weight[2] += LR*delta_weight2*hdOutput[1];
            hdou_weight[3] += LR*delta_weight2*hdOutput[2];
            for (int k=0;k<3;k++)
            {
                inhd_weight[0][k] += LR*delta_weight1[k];
                inhd_weight[1][k] += LR*delta_weight1[k]*trainData[j][0];
                inhd_weight[2][k] += LR*delta_weight1[k]*trainData[j][1];
                inhd_weight[3][k] += LR*delta_weight1[k]*trainData[j][2];
                inhd_weight[4][k] += LR*delta_weight1[k]*trainData[j][3];
            }
            loss += 0.5*(trainKey[j]-result)*(trainKey[j]-result);
        }
        if (i < 15) cout << loss << endl; //Print the loss after each epoch
    }
    cout << "STOCHASTIC GRADIENT DESCENT TRAINING COMPLETED! NOW PREDICTING." << endl;

    //test data
    for(j = 0; j < devNum; j++)
    {
        //ForwardTransfer
        hdOutput[0] = inhd_weight[0][0];
        hdOutput[1] = inhd_weight[0][1];
        hdOutput[2] = inhd_weight[0][2];
        for (int k=0;k<4;k++)
        {
            hdOutput[0] += inhd_weight[k+1][0]*devData[j][k];
            hdOutput[1] += inhd_weight[k+1][1]*devData[j][k];
            hdOutput[2] += inhd_weight[k+1][2]*devData[j][k];
        }
            hdOutput[0] = Sigmoid(hdOutput[0]);
            hdOutput[1] = Sigmoid(hdOutput[1]);
            hdOutput[2] = Sigmoid(hdOutput[2]);
            result = Sigmoid( hdou_weight[0] + hdOutput[0]*hdou_weight[1] + hdOutput[1]*hdou_weight[2] + hdOutput[2]*hdou_weight[3]);

        if (result <= 0.5)
            cout << "no" << endl;
        else
            cout << "yes" << endl;
    }

    return 0;
}