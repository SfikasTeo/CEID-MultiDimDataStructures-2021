#include "simint.h"

double JaccardSimilarity(std::set<uint32_t> s1, std::set<uint32_t> s2)
{
    std::set<std::uint32_t> intersection;
    std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(),std::inserter(intersection,intersection.begin()));
    std::cout << "S1 cardinality: " << s1.size() << "\tS2 cardinality: " << s2.size() <<std::endl;
    std::cout<<"Cardinality of intersection: "<<intersection.size()<<"\tCardinality of union: "<<(s1.size()+s2.size()-intersection.size())<<std::endl;
    return static_cast<double>(intersection.size()) / static_cast<double>(s1.size()+s2.size()-intersection.size()); 
}

double JaccardSimilaritySig(Eigen::Matrix<uint32_t,sigr,Eigen::Dynamic> S)
{
   int cardintersection = 0;
   for(int i=0; i<2/*S.cols()-1*/; i++)
   {
       for(int j = 0; j<S.rows(); j++)
       {
           if(S(j,i) == S(j,i+1))
           {
             cardintersection++;
           }
       }
   }

   return static_cast<double>(cardintersection) / static_cast<double>(2*sigr - cardintersection);
}