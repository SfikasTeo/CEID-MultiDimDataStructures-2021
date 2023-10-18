#include "quadtree.h"
#include "simint.h"

point::point(std::string fp)
{
    std::string tmp;
    std::string tmp2;
    std::ifstream input(fp);
    if (input.is_open())
    {
           SkipBOM(input);
      
            input >> tmp;
            input >> tmp2;
            x = conv_to_lower(tmp);
            y = conv_to_lower(tmp2);  
    }
    else
    {
        std::cerr << "Error at opening the document" << std::endl;
    }
    
}


void node::print()
{
    std::cout<<"["<<p.x<<","<<p.y<<"]"<<" text:"<<filepath<<std::endl;
}   

bool QuadTreeNode::bboxcontains(const node& data){
    
    bool c_x = bound.p1.x <= data.p.x && bound.p2.x >= data.p.x;
    bool c_y = bound.p1.y <= data.p.y && bound.p2.y >= data.p.y;
    
    return c_x && c_y;
}


bool QuadTreeNode::insert(node data)
{
    if(!bboxcontains(data)) // Ignores the date  that do not belong in this quad tree node
        return false;
    
    if(nodes.size()<4 && panoaristera == nullptr) // τεσταρω αν χωράει στο nodes.size() και τεστάρω αν ο κομβος έχει γίνει subdivited. Οποιοδήποτε pointer και αν ελεγξω μας κανει την δουλεια μας
    {
        nodes.push_back(data);
        return true;
    }
    if(panoaristera == nullptr)
        subdivide();

    // αν μπει σωστά σταμάτα
    if(panoaristera->insert(data))
        return true;
    if(panodexia->insert(data))
        return true;
    if(katoaristera->insert(data))
        return true;
    if(katodexia->insert(data))
        return true;
   //sh*t happened and the point wasn't inserted.
    return false;
}


void QuadTreeNode::subdivide()
{
    std::string xMid = middleofStrings(bound.p1.x, bound.p2.x);
    //std::cout<<"The xMId is : "<< xMid << std::endl;
    std::string yMid = middleofStrings(bound.p1.y , bound.p2.y);
    //std::cout<<"The yMId is : "<< yMid << std::endl;

    //This will definitely lead to problems with scew inputs or very large data sizes. The finite space of chars can only be divited a certain amount before everything reaches 0. BUT there is no other option.
    
    //to bbox που φτιάχνεται κάθε φορά δέχεται το κάτω αριστερά διαγώνιο σημείο του ορθ. και το πάνω διαγώνιο αριστερά.
    //  αριστερά διαγώνιο σημείο του ορθ = [x0,y0] πάνω διαγώνιο αριστερά = [xn,yn]
    Bbox paristquadrant(point(bound.p1.x,yMid),point(xMid,bound.p2.y)); 
    panoaristera = std::make_unique<QuadTreeNode>(paristquadrant);

    Bbox panodexquadrant(point(xMid,yMid),point(bound.p2.x,bound.p2.y)); 
    panodexia = std::make_unique<QuadTreeNode>(panodexquadrant);

    Bbox katoaristquadrant(point(bound.p1.x,bound.p1.y),point(xMid,yMid));
    katoaristera = std::make_unique<QuadTreeNode>(katoaristquadrant);

    Bbox katdexquadrant(point(xMid,bound.p1.y),point(bound.p2.x,yMid));
    katodexia = std::make_unique<QuadTreeNode>(katdexquadrant);
    
}


bool QuadTreeNode::containsPoint(const node& data, const Bbox bbox){
    bool c_x = bbox.p1.x <= data.p.x && bbox.p2.x >= data.p.x;
    bool c_y = bbox.p1.y <= data.p.y && bbox.p2.y >= data.p.y;
    
    return c_x && c_y;
}


void QuadTreeNode::QuadTreeFromDirectory(std::string path, QuadTreeNode *root){
    for (const auto & entry : std::filesystem::recursive_directory_iterator(path)){
        //std::cout<<entry.path().filename().string()<<"\n";
        root->insert(node(point(entry.path().string()),entry.path().string()));
        
    }
        //std::cout<<"Ftasame"<<"\n";
}

bool QuadTreeNode::intersectsWith(Bbox t_boundary){
   return bound.p1.x < t_boundary.p2.x && bound.p2.x > t_boundary.p1.x && bound.p1.y < t_boundary.p2.y && bound.p2.y > t_boundary.p1.y;

}
void QuadTreeNode::rangequery(Bbox test_boundary,std::vector<node>& outp)
{
    
    if(!intersectsWith(test_boundary))
        return ;
    
    for(int i =0; i<nodes.size(); i++) 
    {
        if(containsPoint(nodes[i],test_boundary))
            outp.push_back(nodes[i]);
    }

    if(panoaristera == nullptr) //base case
        return ;
    
    panoaristera->rangequery(test_boundary,outp);
    panodexia->rangequery(test_boundary,outp);
    katoaristera->rangequery(test_boundary,outp);
    katodexia->rangequery(test_boundary,outp);

    return ;

}
void QuadTreeNode::rangequery2(Bbox test_boundary,std::vector<std::string>& outp) // only returns filepaths
{
    
    if(!intersectsWith(test_boundary))
        return ;
    
    for(int i =0; i<nodes.size(); i++) 
    {
        if(containsPoint(nodes[i],test_boundary))
            outp.push_back(nodes[i].filepath);
    }

    if(panoaristera == nullptr) //base case
        return ;
    
    panoaristera->rangequery2(test_boundary,outp);
    panodexia->rangequery2(test_boundary,outp);
    katoaristera->rangequery2(test_boundary,outp);
    katodexia->rangequery2(test_boundary,outp);

    return ;

}


void QuadTreeNode::print_tree()
{
    
    for(int i=0; i<nodes.size();i++)
            nodes[i].print();
    
    if(panoaristera == nullptr) //base case
        return ;

    
    std::cout<<"===== Pano Aristera child ====="<<std::endl;
    panoaristera->print_tree();
    std::cout<<"===== Pano Dexia child ====="<<std::endl;
    panodexia->print_tree();
    std::cout<<"===== Kato Aristera child ====="<<std::endl;
    katoaristera->print_tree();
    std::cout<<"===== Kato Dexia child ====="<<std::endl;
    katodexia->print_tree();
    
}

std::string middleofStrings(const std::string& s1 , const std::string& s2){
    std::string output="";
    
    if(s1.length() > s2.length()){
        int i=0;
        while(i<s2.length()){
            output+=(s1[i] + s2[i])/2.0;
            i++;
        }
        while(i<s1.length()){
            output+=s1[i]/2.0;
            i++;
        }
    }
    else{
        int i=0;
        while(i<s1.length()){
            output+=(s1[i] + s2[i])/2.0;
            i++;
        }
        while(i<s2.length()){
            output+=s2[i]/2.0;
            i++;
        }
    }
    return output;
}

