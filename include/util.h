#pragma once

#include <fstream>
#include <string>
#include <vector>

template <typename T>
std::vector<T> readFile(const std::string& filename)
{
    std::ifstream fstr(filename, std::ios::binary);
    fstr.unsetf(std::ios::skipws);

    std::streampos fileSize;

    fstr.seekg(0, std::ios::end);
    fileSize = fstr.tellg();
    fstr.seekg(0, std::ios::beg);

    std::vector<T> data;
    data.resize(fileSize);
    fstr.read((char*)data.data(), fileSize);

    return data;
}
