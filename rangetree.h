#include<iostream>
#include<memory>
#include<vector>
#include<algorithm>
#include<fstream>
#include <filesystem>

class d3point{
    public:
    d3point(std::string filepath); // constructor using filepath
    d3point(std::string d1,std::string d2,std::string d3,std::string filepath); //constructor for testing purposes
    d3point(const d3point& dp); //copy constructor
    d3point();

    //member functions
    std::string get_filepath();
    void print_dimensionKeys();

    //private
    std::array<std::string,3> dimensionKeys;
    std::string filepath;

};

class Rnode{
    public:
    Rnode(d3point& d3,int dimension): value(d3.dimensionKeys[dimension]),right(nullptr),left(nullptr),next_dim(nullptr),data(nullptr){}
    Rnode(): value(""),right(nullptr),left(nullptr),next_dim(nullptr),data(nullptr){} // NULL constructor

    static std::shared_ptr<Rnode> buildNDRT(std::vector<d3point> vecp, int start, int end, int dimension);
    static void pre_order(std::shared_ptr<Rnode> const& node, int dimension, int level, std::string comment);
    static std::shared_ptr<Rnode> findSplitNode(std::shared_ptr<Rnode> const& node, std::string start , std::string end);
    static void queryNDRT(std::shared_ptr<Rnode> const& node,std::vector<std::string> &fpcontained,  d3point start , d3point end , int dimension);

    void print();
    std::string value; // The key
    std::shared_ptr<Rnode> right;
    std::shared_ptr<Rnode> left;
    std::shared_ptr<Rnode> next_dim;
    std::shared_ptr<d3point> data; // maybe can be easily conversed to unique pointer
};



