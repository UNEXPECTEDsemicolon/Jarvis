#pragma once

#include <getopt.h>
#include <thread>
#include <string>
#include <fmt/core.h>
#include <filesystem>

namespace fs = std::filesystem;

namespace config {

class Config {
public:
    void Init(int argc, char** argv) {
        ParseOpts(argc, argv);
    }

private:
    void ParseOpts(int argc, char** argv) {
        auto short_options = "abc";

        static const struct option long_options[] = {
            {"help", no_argument, nullptr, 'h'},
            {"simpercent", required_argument, nullptr, 's'},
            {"dir1", required_argument, nullptr, '1'},
            {"dir2", required_argument, nullptr, '2'},
        };

        int option_index = -1;
        int option = -1;
        while ((option = getopt_long(argc, argv, short_options,
                                     long_options, &option_index)) != -1) {
            switch (option) {
                case 'h': {
                    fmt::println(
                        "Jarvis usage:\n"
                        "    jarvis [options] <dir1> <dir2>\n"
                        "where options are:\n"
                        "  --help (-h) see this message\n"
                        "  --simpercent=<p> (-s <p>) files are similar when matching more than p percent"
                    );
                    break;
                };
                case 's': {
                    char* success;
                    sim_percent_ = std::strtof(optarg, &success);

                    if (!success) {
                        fmt::println(stderr, "error: simpercent must be non-negative float");
                        exit(1);
                    }
                    break;
                };
                case '1': {
                    dir1_ = fs::canonical(optarg);
                    break;
                };
                case '2': {
                    dir2_ = fs::canonical(optarg);
                    break;
                };
                default: {
                    fmt::println(stderr, "error: unknown option", option);
                    exit(1);
                    break;
                };
            };
        };
    }
public:
    float sim_percent_ = 0.5;
    fs::path dir1_, dir2_;    
};

}

static config::Config cfg;