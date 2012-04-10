#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <string>

#include <algorithm>
#include <numeric>
#include <iterator>

#include <pugixml.hpp> 

#include <Eigen/Dense>

#include "xmsParse.h"

using namespace std;

namespace {
struct weight_matrix_walker: pugi::xml_tree_walker
{
    vector<pugi::xml_node> weightMatrices;
    virtual bool for_each(pugi::xml_node& node)
    {
        if (node.name() == string("weightmatrix"))
        {
            cerr << "found weight matrix node...\n";
            weightMatrices.push_back(node);
        }
        return true; // continue traversal
    }
};

struct column_walker : pugi::xml_tree_walker
{
    Eigen::MatrixXf wm;
    unsigned column;
    column_walker(unsigned numCols) : column(0)
    { 
        wm.resize(4, numCols);
    }
    virtual bool for_each(pugi::xml_node& node)
    {
        if (node.name() == string("column"))
        {
            column = node.attribute("pos").as_int();
        }        
        if (node.name() == string("weight"))
        {      
            if (string("thymine") == node.attribute("symbol").value())
            {
                wm(3, column) = ::atof(node.child_value());
            }
            else if (string("guanine") == node.attribute("symbol").value())
            {
                wm(2, column) = ::atof(node.child_value());
            }
            else if (string("cytosine") == node.attribute("symbol").value())
            {
                wm(1, column) = ::atof(node.child_value());
            }
            else if (string("adenine") == node.attribute("symbol").value())
            {
                wm(0, column) = ::atof(node.child_value());
            }
        }
        return true; // continue traversal
    }
};
}

std::vector<Eigen::MatrixXf> xmsToMatrices(std::string fileName)
{
    std::vector<Eigen::MatrixXf> matrices;

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(fileName.c_str());        
    cout << "Load result: " << result.description() << ", motifset: "
         << doc.child("motifset").attribute("xmlns").value() << '\n';
    pugi::xml_node motifset = doc.child("motifset");        

    weight_matrix_walker walker;
    doc.traverse(walker);

    for (auto& node : walker.weightMatrices)
    {
        cout << node.name() << ' ' << node.attribute("columns").value() << " columns\n";
        unsigned columns = node.attribute("columns").as_int();
        column_walker cwalker(columns);
        node.traverse(cwalker);
        matrices.push_back(cwalker.wm);
    }
    return matrices;
}




