#include "rangetree.h"
#include "simint.h"

//Open file functions
/*
std::string conv_to_lower(std::string shingle) // converstion of string to lower case letters ( include algorithm needed)
{
    std::transform(shingle.begin(), shingle.end(), shingle.begin(), [](unsigned char ch)
                   { return std::tolower(ch); });
    return shingle;
}

void SkipBOM(std::ifstream &in) // skips the first 3 letters in txt due to UTF-8
{
    char test[3] = {0};
    in.read(test, 3);
    if ((unsigned char)test[0] == 0xEF &&
        (unsigned char)test[1] == 0xBB &&
        (unsigned char)test[2] == 0xBF)
    {
        return;
    }
    in.seekg(0);
}
*/
//d3point member functions
std::string d3point::get_filepath() { return filepath; } //getter method for filepath

void d3point::print_dimensionKeys()
{
    std::cout <<"("<< dimensionKeys[0] << "," << dimensionKeys[1] << "," << dimensionKeys[2] << ")"<< std::endl;
}

d3point::d3point()
{ // Null cosntructor for d3points
    dimensionKeys[0] = "";
    dimensionKeys[1] = "";
    dimensionKeys[2] = "";
    filepath = "";
}

d3point::d3point(std::string fp) // d3point constructor using filepath
{
    int i = 0;
    std::string tmp;
    std::ifstream input(fp);
    filepath = fp;
    if (input.is_open())
    {
        SkipBOM(input);
        while (i < 3) // i is the number of keys == dimensions
        {
            input >> tmp;
            dimensionKeys[i] = conv_to_lower(tmp);
            i++;
        }
    }
    else
    {
        std::cerr << "Error at opening the document" << std::endl;
    }
}

d3point::d3point(std::string d1, std::string d2, std::string d3, std::string fp)
{ //helper constructor for testing
    dimensionKeys[0] = d1;
    dimensionKeys[1] = d2;
    dimensionKeys[2] = d3;
    filepath = fp;
}

d3point::d3point(const d3point &dp)
{ // copy constructor of d3point
    dimensionKeys[0] = dp.dimensionKeys[0];
    dimensionKeys[1] = dp.dimensionKeys[1];
    dimensionKeys[2] = dp.dimensionKeys[2];
    filepath = dp.filepath;
    ;
}

void Rnode::print()
{
    if (data != nullptr)
    {
        std::cout << " |--> ";
        data->print_dimensionKeys();
    }
    else
    {
        std::cout << " -> " << value << std::endl;
    }
}

void Rnode::pre_order(const std::shared_ptr<Rnode> &Rnode, int dimension, int level, std::string comment)
{
    for (int j = 0; j < dimension; j++) // puts tabs
    {
        for (int k = 0; k <= level; k++)
        {
            std::cout << '\t';
        }
    }
    for (int j = 0; j < level; j++) // puts tabs
    {
        std::cout << " ";
    }

    if (Rnode == nullptr)
    {
        std::cout << " - Null - " << std::endl;
        return;
    }
    std::cout << comment;
    Rnode->print();

    if (Rnode->next_dim != nullptr)
    {
        pre_order(Rnode->next_dim, dimension + 1, level, " Next Dimension :  ");
    }

    if (Rnode->data != nullptr)
        return;

    pre_order(Rnode->left, dimension, level + 1, " Left Child : ");
    pre_order(Rnode->right, dimension, level + 1, " Right Child :  ");
}

std::shared_ptr<Rnode> Rnode::buildNDRT(std::vector<d3point> vecp, int start, int end, int dimension)
{

    if (start == end) //base case
    {
        std::shared_ptr<Rnode> leaf = std::make_shared<Rnode>();
        leaf->data = std::make_shared<d3point>(vecp[start]);
        return leaf;
    }

    int mid = (start + end) / 2;
    std::shared_ptr<Rnode> thisRnode = std::make_shared<Rnode>(vecp[mid], dimension); //0 is the dimension from which the value is taken

    if (dimension < 2)
    {
        std::vector<d3point> vecp2(vecp.begin() + start, vecp.begin() + end + 1);
        //sorted by Dimension +1.
        sort(vecp2.begin(), vecp2.end(), [dimension](const d3point &d1, const d3point &d2)
             { return d1.dimensionKeys[dimension + 1] < d2.dimensionKeys[dimension + 1]; });

        std::shared_ptr<Rnode> Tassoc = buildNDRT(vecp2, 0, vecp2.size() - 1, dimension + 1);
        thisRnode->next_dim = Tassoc;
    }

    thisRnode->left = buildNDRT(vecp, start, mid, dimension);
    thisRnode->right = buildNDRT(vecp, mid + 1, end, dimension);

    return thisRnode;
}

std::shared_ptr<Rnode> Rnode::findSplitNode(std::shared_ptr<Rnode> const &Rnode, std::string start, std::string end)
{
    // gets root and returns the split Rnode of the search query
    //std::cout << "We will now find the split Rnode from the given root :" << Rnode->value << std::endl;    
    if (Rnode->data != nullptr){
        //std::cout << "Found during FindSplitRnode Leaf : ";
        Rnode->data->print_dimensionKeys();
        return Rnode;
    } 
    if (start <= Rnode->value && end > Rnode->value)
        return Rnode;
    if (start > Rnode->value)
        return findSplitNode(Rnode->right, start, end);
    if (end <= Rnode->value)
        return findSplitNode(Rnode->left, start, end);

    std::cout << "Something went wrong here  \n";
    return nullptr; // something went wrong here
}

void Rnode::queryNDRT(std::shared_ptr<Rnode> const &thisRnode, std::vector<std::string> &fpcontained, d3point start , d3point end, int dimension ) // root , output , query limits, dimension
{
    //std::cout<<"Start of queryNDRT for dimension : "<< dimension << "\n";
    if (thisRnode->data != nullptr){
        //std::cout << "At the start of queryNDRT found leaf : ";
        thisRnode->data->print_dimensionKeys();
    } 

    std::shared_ptr<Rnode> vsplit = findSplitNode(thisRnode,start.dimensionKeys[dimension],end.dimensionKeys[dimension]);

    //if( vsplit == nullptr) std::cout<< " vsplit is nullptr \n";

    //std::cout << " The root Rnode is : " << thisRnode->value <<  "\n";
    //std::cout << " The split Rnode is : " << vsplit->value << "   The Dimension is : " << dimension << "\n";
    if( vsplit->data != nullptr){
        //std::cout << "Vsplit->data != NULLPTR so we Found  : ";
        //vsplit->data->print_dimensionKeys();
        //std::cout << std::endl;
        //Check if the point vsplit  satisfies the querie's boundary
        // could be improved - if you have jumped to the next dimension from a canonical Rnode
        if ( vsplit->data->dimensionKeys[0] >= start.dimensionKeys[0] && vsplit->data->dimensionKeys[1] >= start.dimensionKeys[1] && vsplit->data->dimensionKeys[2] >= start.dimensionKeys[2] && vsplit->data->dimensionKeys[0] <= end.dimensionKeys[0] && vsplit->data->dimensionKeys[1] <= end.dimensionKeys[1] && vsplit->data->dimensionKeys[2] <= end.dimensionKeys[2])
        fpcontained.push_back(vsplit->data->filepath);
        return ;
    }

    std::shared_ptr<Rnode> tmp = vsplit->left;
    //std::cout << "The tmp = vsplit->left value is : " << tmp->value << "   The Dimension is : " << dimension << std::endl;

    while ( tmp->data == nullptr ){
        if(start.dimensionKeys[dimension] <= tmp->value)
        {
            //std::cout<<"HERE 1"<< "   The Dimension is : " << dimension <<std::endl;
            if( dimension < 2)
            { // could be defined by a parameter as the total number of dimensions 
                //std::cout << "tmp->right value is : " << tmp->right->value << std::endl;
                if (tmp->right->data != nullptr)
                {
                    //std::cout << "HERE 2 found leaf : ";
                    //tmp->right->data->print_dimensionKeys();
                    if ( tmp->right->data->dimensionKeys[0] >= start.dimensionKeys[0] && tmp->right->data->dimensionKeys[1] >= start.dimensionKeys[1] && tmp->right->data->dimensionKeys[2] >= start.dimensionKeys[2] && tmp->right->data->dimensionKeys[0] <= end.dimensionKeys[0] && tmp->right->data->dimensionKeys[1] <= end.dimensionKeys[1] && tmp->right->data->dimensionKeys[2] <= end.dimensionKeys[2]){
                        fpcontained.push_back(tmp->right->data->filepath);
                        //std::cout << "Inserted into fpcontained : ";
                        //tmp->right->data->print_dimensionKeys();
                    }
                }
                else
                {
                    //std::cout << "HERE 3 new queryNDRT called for next dimension : " << dimension+1 << std::endl;
                    queryNDRT(tmp->right->next_dim, fpcontained , start, end, dimension+1);
                } 
            }
            tmp = tmp->left;
            //std::cout << "The tmp = tmp->left value is : " << tmp->value << "   The Dimension is : " << dimension << std::endl;
        }
        else 
        {
            tmp = tmp->right;
            //std::cout<<"HERE 4 \n";
            //if (tmp->data != nullptr) tmp->data->print_dimensionKeys();
        }
    }
    //std::cout<<"HERE 5 \n";
    if ( tmp->data->dimensionKeys[0] >= start.dimensionKeys[0] && tmp->data->dimensionKeys[1] >= start.dimensionKeys[1] && tmp->data->dimensionKeys[2] >= start.dimensionKeys[2] && tmp->data->dimensionKeys[0] <= end.dimensionKeys[0] && tmp->data->dimensionKeys[1] <= end.dimensionKeys[1] && tmp->data->dimensionKeys[2] <= end.dimensionKeys[2]){
        fpcontained.push_back(tmp->data->filepath);
        //std::cout << "HERE 6 "; 
        //std::cout << "Inserted into fpcontained : ";
        //tmp->data->print_dimensionKeys();
    }

    tmp = vsplit->right;
    //std::cout << "The tmp = vsplit->right value is : " << tmp->value << "   The Dimension is : " << dimension<< std::endl;
    //std::cout<<"HERE 6 \n";
    while ( tmp->data == nullptr ){
        //std::cout<<"HERE 7 \n";
        if(end.dimensionKeys[dimension] >= tmp->value){
            if( dimension < 2){
                if (tmp->left->data != nullptr)
                {
                     //std::cout << "HERE 8 found leaf : ";
                     //tmp->left->data->print_dimensionKeys();

                    if ( tmp->left->data->dimensionKeys[0] >= start.dimensionKeys[0] && tmp->left->data->dimensionKeys[1] >= start.dimensionKeys[1] && tmp->left->data->dimensionKeys[2] >= start.dimensionKeys[2] && tmp->left->data->dimensionKeys[0] <= end.dimensionKeys[0] && tmp->left->data->dimensionKeys[1] <= end.dimensionKeys[1] && tmp->left->data->dimensionKeys[2] <= end.dimensionKeys[2]){
                        fpcontained.push_back(tmp->left->data->filepath);
                        //std::cout << "Inserted into fpcontained : ";
                        //tmp->left->data->print_dimensionKeys();
                    }
                } 
                else
                {
                    //std::cout << "HERE 9 new queryNDRT called for next dimension : " << dimension+1 << std::endl;
                    queryNDRT(tmp->left->next_dim, fpcontained , start, end, dimension+1);
                } 
            }
            tmp = tmp->right;
            //std::cout << "The tmp = tmp->right value is : " << tmp->value << "   The Dimension is : " << dimension << std::endl;

        }
        else{
            tmp = tmp->left;
            //std::cout<<"HERE 10 \n";
            //if (tmp->data != nullptr) tmp->data->print_dimensionKeys();
        }
    }
    if ( tmp->data->dimensionKeys[0] >= start.dimensionKeys[0] && tmp->data->dimensionKeys[1] >= start.dimensionKeys[1] && tmp->data->dimensionKeys[2] >= start.dimensionKeys[2] && tmp->data->dimensionKeys[0] <= end.dimensionKeys[0] && tmp->data->dimensionKeys[1] <= end.dimensionKeys[1] && tmp->data->dimensionKeys[2] <= end.dimensionKeys[2]){
        fpcontained.push_back(tmp->data->filepath);
        //std::cout<<"HERE 11 ";
        //std::cout << "Inserted into fpcontained : ";
        //tmp->data->print_dimensionKeys();
    }

    return;
}