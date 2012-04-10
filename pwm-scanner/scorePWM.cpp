#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_map>
#include "dpj-utils/Utils.h"
#include "dpj-utils/Utils.hpp"

#include <Eigen/Dense>

namespace E = Eigen;
using namespace std;

typedef unordered_map<string, float> StrFloatMap;

string readFasta(string fileName);
vector<float> scoreFastaPWM(const E::MatrixXf& pwm, const string& fasta);


const string wigHeader("fixedStep\tchrom=chr22\tstart=1\tstep=1\n");

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        cout << "Arguments required!\n1) Fasta sequence\n2)Model file\n";
        exit(EXIT_FAILURE);
    }
    const string tf(argv[2]);
    const string pwmFlatFile(tf+"0_pwm");
    const string lassoModel("HK/"+tf+"_70");

    //string fasta = readFasta("Mus_small.fa");
    string fasta = readFasta(argv[1]);
    const unsigned numAs = count(fasta.begin(), fasta.end(), 'A');
    const unsigned numCs = count(fasta.begin(), fasta.end(), 'C');
    const unsigned numGs = count(fasta.begin(), fasta.end(), 'G');
    const unsigned numTs = count(fasta.begin(), fasta.end(), 'T');
    cout << "A : " << numAs << '\n';
    cout << "C : " << numCs << '\n';
    cout << "G : " << numGs << '\n';
    cout << "T : " << numTs << '\n';

    const unsigned histBins = 200;
    std::vector<float> floats;
    ofstream oFile;
    ifstream iFile;
    
    //
    // The PWM model.
    //
    iFile.clear();
    iFile.open(pwmFlatFile.c_str());
    if (! (iFile.good())) 
    {
        cerr << "couldn't open PWM file.\n";
        exit(EXIT_FAILURE);
    }
    vector<float> pwmTmp;
    while (iFile >> fTok)
        pwmTmp.push_back(fTok);
    iFile.close();

    typedef E::Matrix<float, E::Dynamic, E::Dynamic, E::RowMajor> MatrixXfRM;
    E::Map<MatrixXfRM> pwm(pwmTmp.data(), 4, pwmTmp.size()/4);
    cout << pwm << '\n';

    floats = scoreFastaPWM(pwm, fasta);

    oFile.open((tf+"_pwm_scores").c_str());
    oFile << wigHeader;
    for_each(floats.begin(), floats.end(), [&oFile](float f) {oFile << f << '\n';});
    oFile.close();
    dpj_utils::Histogram<float> pwmHist(floats.begin(), floats.end());
    pwmHist.numBins(histBins);
    pwmHist.binData();
    pwmHist.summary();
}


float pwmScoreFromPosition(const E::MatrixXf& pwm, string::const_iterator b)
{
    float score = 0;
    unsigned motifLength = pwm.size()/4;
    
    for (unsigned colIndex = 0; colIndex < motifLength; ++colIndex, ++b)
    {
        unsigned rowIndex;
        if (*b == 'N')
            return pwm.minCoeff() * motifLength;
        else if (*b == 'A') rowIndex = 0;
        else if (*b == 'C') rowIndex = 1;
        else if (*b == 'G') rowIndex = 2;
        else if (*b == 'T') rowIndex = 3;
        else 
        {
            cerr << "error: unknown symbol: " << *b << '\n';
            exit(EXIT_FAILURE);
        }
        score += pwm(rowIndex, colIndex);
    }

    return score;    
}

vector<float> scoreFastaPWM(const E::MatrixXf& pwm, const string& fasta)
{
    vector<float> scores;
    scores.reserve(fasta.size());
    unsigned motifLength = pwm.size()/4;
    auto segBegin = fasta.begin();
    auto end = fasta.end() - motifLength + 1;
    for (; segBegin != end; ++segBegin)
    {
        scores.push_back(pwmScoreFromPosition(pwm, segBegin));
    }
    return scores;
}


string readFasta(string fileName)
{
    string fasta, line;
    ifstream iFile(fileName.c_str());
    getline(iFile, line); // header
    unsigned lineCounter = 0;
    while (iFile >> line)
    {
        lineCounter++;
        fasta += line;
    }
    cerr << "read " << lineCounter << " lines of fasta.\n";

    return fasta;
}

