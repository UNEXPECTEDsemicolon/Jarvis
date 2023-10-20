#pragma once

#include <filesystem>
#include <fstream>
#include <bitset>
#include <unordered_map>
#include <thread>
#include "fft.hpp"
#include "config.hpp"

auto ReadData(fs::path file) {
    size_t read_size = 4096;
    auto stream = std::ifstream(file);
    stream.exceptions(std::ios_base::badbit);

    if (!stream) {
        fmt::println("can't open file {}", file);
        exit(1);
    }

    auto result = std::string();
    auto buf = std::string(read_size, '\0');
    while (stream.read(&buf[0], read_size)) {
        result.append(buf, 0, stream.gcount());
    }
    result.append(buf, 0, stream.gcount());
    return result;
}

auto BytesToComplex(const std::string& data, uint8_t main_byte) {
    std::vector<ComplexField> res(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        res[i] = ComplexField(data[i] == main_byte);
    }
    return res;
}

auto CompareBytes(const std::vector<ComplexField>& data1,
                  const std::vector<ComplexField>& data2) {
    Polynom<ComplexField> poly1(data1.size() - 1, data1);
    Polynom<ComplexField> poly2(data2.size() - 1, data2);
    auto res_poly = poly1 * poly2;
    return std::move(res_poly.coefs);
}

auto Compare(fs::path file1, fs::path file2) {
    auto data1 = ReadData(file1);
    auto data2 = ReadData(file2);
    if (data1.size() == data2.size() && !std::memcmp(data1.data(), data2.data(), data1.size())) {
        return 1.0;
    }
    std::vector<double> coefs;
    #pragma omp parallel for
    for (uint8_t byte = 0; byte < UINT8_MAX; ++byte) {
        auto complex_bytes1 = BytesToComplex(data1, byte);
        auto complex_bytes2 = BytesToComplex(data2, byte);

        std::reverse(complex_bytes2.begin(), complex_bytes2.end());
        auto new_coefs = CompareBytes(complex_bytes1, complex_bytes2);
        #pragma omp critical
        {
            if (coefs.size() == 0) {
                coefs.assign(new_coefs.size(), 0.0);
            }
            for (size_t i = 0; i < coefs.size(); ++i) {
                coefs[i] += new_coefs[i].real();
            }
        }
    }
    auto max_match = *std::max_element(coefs.begin(), coefs.end());
    return max_match / std::max(data1.size(), data2.size());
}

auto Walk(fs::path dir1, fs::path dir2) {
    if (!fs::is_directory(dir1)) {
        fmt::println("path {} is not a directory", dir1.string());
        exit(1);
    }
    if (!fs::is_directory(dir2)) {
        fmt::println("path {} is not a directory", dir2.string());
        exit(1);
    }
    std::unordered_map<std::string, bool> files2_are_unique;
    for (const auto& file1 : fs::recursive_directory_iterator(dir1)) {
        if (!file1.is_regular_file() && !file1.is_symlink()) continue;
        bool file1_is_unique = true;
        for (const auto& file2 : fs::recursive_directory_iterator(dir2)) {
            if (!file2.is_regular_file() && !file2.is_symlink()) continue;
            files2_are_unique.emplace(file2.path().string(), true);
            fmt::println("Comparing \"{}\" and \"{}\"", file1.path().string(), file2.path().string());
            auto similarity = Compare(file1.path(), file2.path());
            fmt::println("Similarity = {}", similarity);
            if (similarity >= cfg.sim_percent_) {
                file1_is_unique = false;
                files2_are_unique.emplace(file2.path().string(), false);
                if (similarity == 1.0) {
                    fmt::println("The files are EQUAL");
                } else {
                    fmt::println("The files are SIMILAR");
                }
            }
        }
        if (file1_is_unique) {
            fmt::println("The file {} is unique", file1.path().string());
        }
    }
    for (auto& [file2, is_unique]: files2_are_unique) {
        if (is_unique) {
            fmt::println("The file {} is unique", file2);
        }
    }
}
