#include "oead/aamp.h"
#include "oead/util/hash.h"
#include "util.h"
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <functional>
#include <string>

std::vector<std::string> readTextFileLines(const std::string& fileName)
{
    std::vector<std::string> lines;
    std::string line;

    std::ifstream inFile(fileName);
    while (std::getline(inFile, line))
        lines.push_back(line);

    return lines;
}

void writeStringToFile(const std::string& data, const std::string& fileName)
{
    std::ofstream outfile(fileName);
    outfile << data;
    outfile.close();
}

int main(int argc, char** args)
{
    if (argc < 3 || argc > 4) {
        printf("Usage: %s <aampfile> <stringfile> [outfile]\n", argc > 0 ? args[0] : "aamptoyaml");
        printf("  aampfile: Path to AAMP file\n");
        printf("  stringfile: Path to file with key strings seperated by newlines\n");
        printf("  outfile: Path to output YAML file (optional)\n");
        return 1;
    }

    const std::string_view aampFilePath { args[1] };
    const std::string_view stringFilePath { args[2] };

    if (!std::filesystem::exists(aampFilePath)) {
        printf("AAMP file '%s' does not exist\n", aampFilePath.data());
        return 1;
    }

    if (!std::filesystem::exists(stringFilePath)) {
        printf("String file '%s' does not exist\n", stringFilePath.data());
        return 1;
    }

    // load aamp file
    std::vector<u8> aampFileData = readFile<u8>(args[1]);
    oead::aamp::ParameterIO aampFile = oead::aamp::ParameterIO::FromBinary(aampFileData);

    std::vector<u32> hashes;

    // add all names hashes from aamp file to vector
    for (const auto& object : aampFile.objects) {
        hashes.push_back(object.first);
        for (const auto& param : object.second.params)
            hashes.push_back(param.first);
    }
    for (const auto& parameterListEntry : aampFile.lists) {

        const std::function<void(const std::pair<oead::aamp::Name, oead::aamp::ParameterList>&)> addHashFromParameterList = [&hashes, &addHashFromParameterList](const auto& entry) {
            hashes.push_back(entry.first.hash);
            for (const auto& object : entry.second.objects) {
                hashes.push_back(object.first);
                for (const auto& param : object.second.params)
                    hashes.push_back(param.first);
            }
            for (const std::pair<oead::aamp::Name, oead::aamp::ParameterList>& listEntry : entry.second.lists)
                addHashFromParameterList(listEntry);
        };

        addHashFromParameterList(parameterListEntry);
    }

    // remove duplicate hashes
    std::sort(hashes.begin(), hashes.end());
    auto it = std::unique(hashes.begin(), hashes.end());
    hashes.erase(it, hashes.end());

    // read string file
    std::vector<std::string> stringRes = readTextFileLines(stringFilePath.data());

    // search for matching strings and add them
    for (const std::string& str : stringRes)
        for (u32 hash : hashes)
            if (oead::util::crc32(str.data(), str.size()) == hash) {
                oead::aamp::GetDefaultNameTable().AddName(hash, str);
            }

    // write yaml file
    std::string outFilePath = std::string(aampFilePath);
    if (argc == 4)
        outFilePath = std::string(args[3]);
    else
        outFilePath.append(".yaml");

    writeStringToFile(aampFile.ToText(), outFilePath);
    return 0;
}