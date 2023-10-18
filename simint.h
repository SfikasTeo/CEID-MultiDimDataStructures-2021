#pragma once

#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <set>
#include <string>
#include <fstream>
#include <random>
#include <array>
#include <cassert>
#include <memory>
#include <functional>
#include "Eigen/Dense"



#define sigr 100

class htable
{
    
    public:
    int prime_for_hash = 4999;//paizetai
	
	//an array of virtualbuckets
	std::array<std::vector<int>,4999> vb; //size is the number of docs	
    htable();
    ~htable();
    void update_bucket(int hres, int index);
    std::vector<int> get_sign_from_bucket(int hres);
    std::vector<int> bucket_empty();
    int hashfun(const Eigen::Matrix<uint32_t, 5, 1> m);
   
};

void SkipBOM(std::ifstream& in);

std::string conv_to_lower(std::string shingle);

uint32_t MurmurOAAT_32(const char* str, uint32_t h);

std::vector<int> findSpaces(std::string s);

std::set<std::uint32_t> Murmurshingling(const std::string filepath);

double JaccardSimilarity(std::set<uint32_t> s1, std::set<uint32_t> s2);

double JaccardSimilaritySig(Eigen::Matrix<uint32_t,sigr,Eigen::Dynamic> S);

Eigen::Matrix<uint32_t,sigr,Eigen::Dynamic>  makeSignatures(std::vector<std::set<uint32_t>> S);

std::vector<std::unique_ptr<htable>> hashbands(Eigen::Matrix<uint32_t, sigr, Eigen::Dynamic> M);

