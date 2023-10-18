#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <algorithm>
#include <memory>
#include <filesystem>

struct point
{
    std::string x, y;     
    point():x("0"),y("0"){}
    point( std::string fp); // final way of constructing points from filepath
    point( std::string x_, std::string y_  ):x(x_),y(y_){} // copy constructor
};

//axis-aligned boundary box
struct Bbox
{
    point p1; //[x0,y0]
    point p2; //[xn,yn]
    Bbox(const point p1_,const point p2_):p1(p1_),p2(p2_){} // copy constuctor
};

struct node
{   
    std::string filepath;
    point p;
    node():p(),filepath(std::move("")){}
    node(point p, std::string filepath ):p(p),filepath(std::move(filepath)){}
    void print();
};

class QuadTreeNode
{
    public:
    Bbox bound;
    std::vector<node> nodes; // these are the leaf-nodes 
    // std::unique_ptr<node> data;

    std::unique_ptr<QuadTreeNode> panoaristera;
    std::unique_ptr<QuadTreeNode> panodexia;
    std::unique_ptr<QuadTreeNode> katoaristera;
    std::unique_ptr<QuadTreeNode> katodexia;
    
    QuadTreeNode(Bbox b):panoaristera(nullptr),panodexia(nullptr),katoaristera(nullptr),katodexia(nullptr),bound(b){
        nodes.reserve(4);
    }
    bool insert(node data); //done
    void subdivide(); //done
    void rangequery(Bbox boundary,std::vector<node>& outp); // pros to paron ua kanei cout
    void rangequery2(Bbox boundary, std::vector<std::string>& outp); //outputs only the found filepaths
    bool bboxcontains(const node& data); //checks if the searched point in contained within the bbox
    bool containsPoint(const node& data, const Bbox); //checks if the searched point in contained within the bbox
    bool intersectsWith(Bbox t_boundary);
    void print_tree();
    static void QuadTreeFromDirectory(std::string path, QuadTreeNode *root); // left to be completed
};

std::string middleofStrings(const std::string& s1 , const std::string& s2);
