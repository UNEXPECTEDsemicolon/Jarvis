#include <iostream>
#include <filesystem>
#include <fmt/core.h>
#include "algorithm.hpp"
#include "config.hpp"


int main(int argc, char** argv) {
    cfg.Init(argc, argv);
    Walk(cfg.dir1_, cfg.dir2_);
}