#include "quadtree.h"
#include  "simint.h"
#include "rangetree.h"
#include "chrono"

/*
                                                QUAD-TRE + LSH      RangeTree + LSH

Input: vector of strings of the filepaths
Output: 2-way. 1)Every candidate pair is printed on the console of each hash table 
               2)There is an output .txt file (candidatepairs.txt) in which every calculated pair is mentioned with these two methods.
*/
/*
Factors Worth Considering During compilation :
Please consider changing the filepath substring for nicer results

1) " -std=c++17", "-lstdc++fs" linker for correct compilation ( std::filesystem may cause some problems otherwise )
2) A typical compilation with terminal inside the source code folder
 using the command line would look like : g++ *.cpp -std=c++17 -lstdc++fs -o main  

ATTENTION: You must download and include the EIGEN/Dense library made for matrices/lin. alg.
           To do that you download the library from the official website and in simint.h you must include the path to Eigen/dense
           E.g. #include "/home/dimet/Documents/Dimitris/programming/cpp/projectπδμ/Eigen/Dense"
           App was built mainly Linux so handle the filepaths with care.

After setting up the dataset directory for the construction of the tree with method QuadTreeFromDirectory
you must change the search boundaries. A typical setting for searching in the [hpc,a]x[z,q] for the QuadTree would be 
"Bbox searchboundary(point("hpc", "a"), point("z", "q"))";
For the RangeTree case we would need something like 
d3point startpoint("hpc","a","/","NULL");
d3point endpoint("z","q","{","NULL");
*/

int main(int argc,char **argv)
{
    
    
    //===============================================================
    //Quad Tree part
    //===============================================================
    
    //set up root and boundbox.
    Bbox bounds(point("/", "/"), point("{", "{")); //finite space boundary
    QuadTreeNode *root = new QuadTreeNode(bounds);
    //The docs used to create the QuadTree and index the input are stored in a specified folder.
    auto startquadconstr = std::chrono::steady_clock::now();
    QuadTreeNode::QuadTreeFromDirectory("C:/Users/Sfikas/Desktop/SimilarDocs",root);
    auto endquadconstr = std::chrono::steady_clock::now();

    auto startquadsearch = std::chrono::steady_clock::now();
    std::vector<std::string> fpcontained;
    Bbox searchboundary(point("/", "/"), point("{", "{")); //must give 5 docs !! ALWAYS small case letters

    root->rangequery2(searchboundary, fpcontained); // only returns filepaths
    auto endquadsearch = std::chrono::steady_clock::now();

    std::vector<std::set<uint32_t>> listofdocs; // input of minhash
    for(auto i: fpcontained)
    {
        listofdocs.push_back(Murmurshingling(i));
    }

    for(auto i:fpcontained)
    {
        std::cout<<i<<"\n";
    }
    std::cout<< "The list of docs size is : " << listofdocs.size() << std::endl;



    //===============================================================
    //Range Tree part
    //===============================================================

    std::vector<d3point> vec; // vector of d3points declaration
    
    const std::filesystem::path testpath = "C:/Users/Sfikas/Desktop/SimilarDocs";

    //constuct d3points from directory into vec
    for (const auto & entry : std::filesystem::directory_iterator(testpath))
    {
        vec.push_back(d3point(entry.path().string()));
    }
    //std::cout << " Sort by x " << std::endl;
    sort(vec.begin(), vec.end(),[](const d3point& d1,const d3point& d2) 
    {
        return d1.dimensionKeys[0]< d2.dimensionKeys[0];
    });
    auto startrangeconstruction = std::chrono::steady_clock::now();
    // sort by x-coord
    

    //construct tree
    std::shared_ptr<Rnode> rootrange = Rnode::buildNDRT(vec,0,vec.size()-1,0);
    auto endrangeconstruction = std::chrono::steady_clock::now();
    //node::pre_order(root,0,0,"");

    std::vector<std::string> output;
    //the area we want to search
    d3point startpoint("/","/","/","NULL");
    d3point endpoint("{","{","{","NULL");
    
    std::cout<<std::endl;
    //std::shared_ptr<node> testsplit = node::findSplitNode(root,"0","9");
    //testsplit->print();
    auto startrangequery = std::chrono::steady_clock::now();
    Rnode::queryNDRT(rootrange,output,startpoint,endpoint,0);
    auto endrangequery = std::chrono::steady_clock::now();
    
    //std::cout<<"The result of the range query is : "<<std::endl;
    //for( auto i: output)
        //std::cout<< i << std::endl;

    std::vector<std::set<uint32_t>> listofdocs2; // input of minhash

     for(auto i: output)
    {
        listofdocs2.push_back(Murmurshingling(i));
    }



    // ====================================================================================
    //LSH part
    // ====================================================================================

    //must be made with dynamic columns
    Eigen::Matrix<uint32_t,100,Eigen::Dynamic> signatures = makeSignatures(listofdocs); // For Quad
    Eigen::Matrix<uint32_t,100,Eigen::Dynamic> signatures2 = makeSignatures(listofdocs2); // For Range 

    //Candidatepairs.txt has every doc with similarity
    std::ofstream candidatepairs("candidatepairs.txt");                    
   
    std::vector<std::unique_ptr<htable>> vhf; // For Quad
    std::vector<std::unique_ptr<htable>> vhf2; // For Range 
    vhf = hashbands(signatures); // For Quad
    vhf2 = hashbands(signatures2); // For Range 
  
    candidatepairs<<"THESE ARE DOCUMENTS THAT NEED TO BE CHECKED FOR PLAGIARISM !! \n\n\t\t\t\t\tQuadTree+LSH\n";
    //for each hashtable 
    int hnum = 0;
    for(auto&& htable: vhf)
    {
        std::cout<<"["<<hnum++<<"]:";
        candidatepairs<<"["<<hnum<<"]:";
	    //skanaroume ton vb poy einai oi kouvades toy hashmap oysiastika
	    for(int i = 0; i < htable->vb.size();i++)
	    {    
            //an se kapoion kouva exoyn hasharistei >1 stiles dhladh docs typose ta
            if(htable->vb[i].size()>1)
            {
            
			    std::cout<<"{"<<i<<"}:";
                candidatepairs<<"{"<<i<<"}:";
                for(int j =0; j < htable->vb[i].size(); j++){
                    std::cout<< fpcontained[htable->vb[i][j]]<<" "; 
                    //28 may need to be changed based on the last / of the path of the dataset.
                    //For example here path is /home/dimet/Desktop/dataset/ so the last / corresponds to the 28th character
                    candidatepairs<<fpcontained[htable->vb[i][j]].substr(36)<<" ";   
                 }
                candidatepairs<<"]";
                candidatepairs<<'\t';
                std::cout<<"]";
                std::cout<<'\t';
             }
	    }
	    std::cout<<"\n";
        candidatepairs<<"\n";
    }

    candidatepairs<<"\n\n\t\t\t\t\tRangeTree+LSH\n";
    int hnum2 = 0;
    for(auto&& htable2: vhf2)
    {
        std::cout<<"["<<hnum2++<<"]:";
        candidatepairs<<"["<<hnum2<<"]:";
	    //skanaroume ton vb poy einai oi kouvades toy hashmap oysiastika
	    for(int i = 0; i < htable2->vb.size();i++)
	    {    
            //an se kapoion kouva exoyn hasharistei >1 stiles dhladh docs typose ta
            if(htable2->vb[i].size()>1)
            {
            
			    std::cout<<"{"<<i<<"}:";
                candidatepairs<<"{"<<i<<"}:";
                for(int j =0; j < htable2->vb[i].size(); j++){
                    std::cout<< output[htable2->vb[i][j]]<<" "; 
                    //28 may need to be changed based on the last / of the path of the dataset.
                    //For example here path is /home/dimet/Desktop/dataset/ so the last / corresponds to the 28th character
                    candidatepairs<<output[htable2->vb[i][j]].substr(36)<<" ";   
                 }
                candidatepairs<<"]";
                candidatepairs<<'\t';
                std::cout<<"]";
                std::cout<<'\t';
             }
	    }
	    std::cout<<"\n";
        candidatepairs<<"\n";
    }

    std::cout<<"\n";

    

        std::cout << "Elapsed time in microseconds for building QuadTree:"<< std::chrono::duration_cast<std::chrono::microseconds>(endquadconstr - startquadconstr).count()<<
            "μs\n"<<"Elapsed time in microseconds for searching QuadTree:"<< std::chrono::duration_cast<std::chrono::microseconds>(endquadsearch - startquadsearch).count()<<
        "μs\n"<<"Elapsed time in microseconds for building RangeTree:"<<std::chrono::duration_cast<std::chrono::microseconds>(endrangeconstruction - startrangeconstruction).count()
        <<"μs\n"<<"Elapsed time in microseconds for searching RangeTree:"<<std::chrono::duration_cast<std::chrono::microseconds>(endrangequery - startrangequery).count()<<" μs\n";
}
