#define CATCH_CONFIG_MAIN 
#include "catch.hpp"

#include "uv3decoder.h"
#include <fstream>
#include <iostream>

using namespace freeUV3;

TEST_CASE( "open", "[factorial]" ) {

    UV3Decoder kk("test.UVData");
    if (kk.good()) {
        
        //uv3 t("");
        auto v = kk.get();
        for (const auto &n : v) {
            std::ofstream ofs(n->filename_, std::ios::binary);
            ofs << n->buffer_.rdbuf();
            ofs.close();
        }
        kk.close();

        for (auto n : v) {
            bool b = n->buffer_.good();
            std::cout << b;
        }
        std::cout << "end";
    }
}


TEST_CASE( "open and close", "[factorial]" ) {

    UV3Decoder kk("test.UVData");

    UV3Decoder kk2("test.UVData");


    if (kk2.good()) {
        
        //uv3 t("");
        auto v = kk.get();
        for (const auto &n : v) {
            std::ofstream ofs(n->filename_, std::ios::binary);
            ofs << n->buffer_.rdbuf();
            ofs.close();
        }
        kk.close();

        for (auto n : v) {
            bool b = n->buffer_.good();
            std::cout << b;
        }
        std::cout << "end";
    }
}