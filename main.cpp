#include <iostream>
#include "HuffmanTree.h"
// main.cpp — test harness for buildFromHeader (.hdr + .code in input_output/)
#include <filesystem>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

// ---- simple, spec-compliant header reader: "word<space>code" per line ----
static bool readHeaderPairs(const fs::path& hdrPath,
                            std::vector<std::pair<std::string,std::string>>& out,
                            std::string& err)
{
    out.clear();
    std::ifstream in(hdrPath);
    if (!in) { err = "Cannot open header file."; return false; }

    std::string line;
    bool saw_content = false;
    while (std::getline(in, line)) {
        if (line.empty()) continue;

        if (!saw_content) {
            // Early validation: header should start with a letter
            char c = line[0];
            if (c < 'a' || c > 'z') { err = "Header must start with a letter."; return false; }
            saw_content = true;
        }

        auto pos = line.find(' ');
        if (pos == std::string::npos) { err = "Malformed header line (no space)."; return false; }

        std::string word = line.substr(0, pos);
        std::string code = line.substr(pos + 1);

        if (word.empty() || code.empty()) { err = "Empty word or code in header."; return false; }

        out.emplace_back(std::move(word), std::move(code));
    }
    return true;
}

// ---- usage helper ----
static void usage(const char* prog) {
    std::cerr << "Usage: " << prog << " <base>.hdr <base>.code\n"
              << "  (files must be located in ./input_output)\n";
    std::exit(1);
}

int main(int argc, char* argv[]) {
    if (argc != 3) usage(argv[0]);

    fs::path dir = "input_output";
    if (!fs::exists(dir)) {
        std::cerr << "Error: input directory does not exist: " << dir << "\n";
        return 2;
    }

    // Enforce files are looked up only inside input_output/
    fs::path hdrPath  = dir / fs::path(argv[1]).filename();
    fs::path codePath = dir / fs::path(argv[2]).filename();

    // Basic file checks
    {
        std::ifstream hdr(hdrPath);
        if (!hdr) {
            std::cerr << "Error: cannot open header file: " << hdrPath << "\n";
            return 3;
        }
    }
    std::ifstream code_in(codePath);
    if (!code_in) {
        std::cerr << "Error: cannot open code file: " << codePath << "\n";
        return 4;
    }

    // Simple sanity: code file should start with 0 or 1 (skipping newlines)
    {
        code_in.clear();
        code_in.seekg(0);
        int ch;
        bool ok = false;
        while ((ch = code_in.get()) != EOF) {
            if (ch == '\n' || ch == '\r') continue;
            ok = (ch == '0' || ch == '1');
            break;
        }
        if (!ok) {
            std::cerr << "Error: code file must start with 0 or 1.\n";
            return 5;
        }
        code_in.clear();
        code_in.seekg(0);
    }

    // Parse header -> (word,code) pairs
    std::vector<std::pair<std::string,std::string>> header_pairs;
    std::string err;
    if (!readHeaderPairs(hdrPath, header_pairs, err)) {
        std::cerr << "Error reading header: " << err << "\n";
        return 6;
    }

    // Build the decoding Huffman tree from the header
    HuffmanTree decoder;
    if (!decoder.buildFromHeader(header_pairs, err)) {
        std::cerr << "Error building Huffman tree: " << err << "\n";
        return 7;
    }

    // If we got here, buildFromHeader succeeded
    std::cout << "buildFromHeader: OK ("
              << header_pairs.size() << " words)\n";

    return 0;
}

