//Copyright (c) 2021 Picorims<picorims.contact@gmail.com>, France.

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

using namespace std;

//calculate the sum of a vector of integers
int sum(const vector<int> &vec) {
    int sum = 0;
    for (int i=0; i<vec.size(); i++) sum += vec[i];
    return sum;
}

//print command usage and exit
void logUsageAndExit() {
    cout << "usage: <program_name> -c | <program_name> <number_rows> <season_points>" << endl;
    cout << "program_name: the name of the program. (to call the file. It is ignored otherwise.)." << endl;
    cout << "-c: console input." << endl;
    cout << "number_rows: value between 1 and 999 999 999, numbers of rows generated." << endl; // max is 4 294 967 295 but set to 999 999 999 for security
    cout << "season_points: 8 relative integers all separated by '|'." << endl << endl;
    exit(1);
}









//main program
int main(int argc, char** argv) {
    //user input
    unsigned long int numRows;
    string progVal;
    string pointsSeason;
    //input verification
    regex progValRegex("^[0-9]{1,9}$"); // number with 1 to 9 digits
    regex pointsSeasonRegex("^(-?[0-9]+[|]){7}-?[0-9]+$");


    //command system
    if (argc < 2 || argc > 4) {
        //not the right number of arguments
        logUsageAndExit();
    } else if (argc == 2) {
        //is it -c ? otherwise invalid
        progVal = argv[1];
        if (progVal != "-c") {
            //invalid
            logUsageAndExit();
        }
    } else {
        progVal = argv[1];
        pointsSeason = argv[2];

        if (!regex_match(progVal, progValRegex)) {
            //2nd value is invalid (-c or number)
            cout << "invalid value: " << progVal << endl;
            logUsageAndExit();
        } else if (!regex_match(pointsSeason, pointsSeasonRegex)) {
            //3rd value is invalid (strategy)
            cout << "invalid point distribution: " << pointsSeason << endl;
            cout << "example 1: 1|1|1|1|1|1|2|2" << endl;
            cout << "example 2: -1|1|1|1|1|1|-1|1" << endl;
            logUsageAndExit();
        }
    }

    //console input
    if (progVal == "-c") {
        cout << "How much rows do you want ? (max is 999 999 999)" << endl; //max is 4 294 967 295 but 999 999 999 is a security
        cin >> numRows;

        cout << "What is the point distribution of the season ? (8 relative integers all separated by '|')." << endl;
        cout << "example 1: 1|1|1|1|1|1|2|2" << endl;
        cout << "example 2: -1|1|1|1|1|1|-1|1" << endl;
        cin >> pointsSeason;

        cout << "processing..." << endl;
    } else {
        numRows = stoi(progVal);
    }

    //check values
    if (numRows > 999999999 || numRows < 0) {
        cout << "invalid value: " << numRows << endl;
        logUsageAndExit();
    }
    if (!regex_match(pointsSeason, pointsSeasonRegex)) {
        cout << "invalid points distribution: " << pointsSeason << endl;
        logUsageAndExit();
    }









    //processing
    cout << "points distribution: " << pointsSeason << endl;
    cout << "Generation of " << numRows << " rows..." << endl;

    //open names
    string path = "assets/MOCK_DATA_MERGED_10000.csv";
    ifstream names(path);
    if (!names.is_open()) {
        cout << "Error: couldn't open names data." << endl;
        exit(1);
    }

    //store names (thanks to https://www.codespeedy.com/fetch-a-random-line-from-a-text-file-in-cpp/)
    vector<string> namesVec;
    string nameLine;
    while(getline(names, nameLine)) namesVec.push_back(nameLine);


    //output file
    string outputFilename = "generated_strategies/strategies"+to_string(numRows)+".csv";
    ofstream output(outputFilename);
    if (!output.is_open()) {
        cout << "Error: couldn't create output file." << endl;
        exit(1);
    }

    //setup random
    srand(time(NULL));

    //setup time tracking
    int PreviousTime = 0;

    //add season information
    output << "1,Random Strategies by Picorims," << pointsSeason << endl;





    //generating lines
    for (unsigned long int i=0; i<numRows; i++) {
        string line;

        //pick random name
        int random = (rand() % 10000); //0 to 9999 -> line 1 to 10000
        line = namesVec[random];

        //randomize username so it is not considered as the same player
        line += to_string(rand());

        //set to no image
        line += ",0";

        //generate strategy
        vector<int> strategy = {0,0,0,0,0,0,0,0};
        for (int j=0; j<strategy.size(); j++) {
            strategy[j] = rand() % 9; //between 0 and 8
        }

        //the sum of the strategy must be equal to 64. The loop below generated a sum of 64 or less.
        //We just need to keep adding values while it is not equal to 64.
        int sumStrategy = sum(strategy);
        while (sumStrategy < 64) {
            int offset = 64 - sumStrategy;
            //10 or more to add ? -> random between 5 and 10 (10-5+1=6) OR otherwise add the offset.
            int toAdd = (offset >= 10)? 5 + rand()%6 : offset;

            //assign the value to a random index
            strategy[rand()%strategy.size()] += toAdd;

            //recalculate sum
            sumStrategy = sum(strategy);
        }

        //add the strategy to the line
        line += "," + to_string(strategy[0]);
        for (int j=1; j<strategy.size(); j++) line += "|" + to_string(strategy[j]);

        //write the line
        output << line << endl;

        //log progress every second
        unsigned long int currentTime = time(NULL);
        if (currentTime != PreviousTime) {
            PreviousTime = currentTime;
            cout << i << " lines generated..." << endl;
        }
    }

    //end
    cout << outputFilename << " successfully generated!";

    return 0;
}
