#pragma once

#include <iostream>

#include "../data_reading_sandbox/data_reading_sandbox.hpp"


namespace calculations{

class Calculations{
public: 
    static void data_test(data_reading_sandbox::Data data){
        int i = 0;
        std::cout<<"Calculations Static Method"<<std::endl;
    }
};

}
