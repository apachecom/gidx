//
// Created by alpachec on 16-09-18.
//

#include <iostream>
#include <fstream>
#include <string>
#include <benchmark/benchmark.h>





auto fn = []()->void{

};





int main (int argc, char *argv[] ){

    if(argc < 1){
        std::cout<<"bad parameters....";
        return 0;
    }



    std::string collection(argv[1]);
    std::cout<<"collection : "<<collection<<std::endl;



}