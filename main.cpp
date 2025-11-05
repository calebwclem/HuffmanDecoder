
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include "utils.h"
#include "HuffmanTree.h"

namespace fs = std::filesystem;

// Parse .hdr into (word, code) pairs; apply the “first line starts with a letter” sanity check.
static error_type readHeaderPairs(const fs::path& hdrPath,
                                  std::vector<std::pair<std::string,std::string>>& out,
                                  std::string& errMsg)
{
    out.clear();
    std::ifstream in(hdrPath);
    if (!in) return UNABLE_TO_OPEN_FILE;

    std::string line;
    bool saw_content = false;
    while (std::getline(in, line)) {
        if (line.empty()) continue;

        if (!saw_content) {
            // spec sanity: header should start with a letter (not 0 or 1)
            unsigned char c = static_cast<unsigned char>(line[0]);
            if (!std::isalpha(c)) { errMsg = "Header must start with a letter."; return ERR_TYPE_NOT_FOUND; }
            saw_content = true;
        }

        auto pos = line.find(' ');
        if (pos == std::string::npos) { errMsg = "Malformed header line (no space)."; return ERR_TYPE_NOT_FOUND; }

        std::string word = line.substr(0, pos);
        std::string code = line.substr(pos + 1);
        if (word.empty() || code.empty()) { errMsg = "Empty word or code in header."; return ERR_TYPE_NOT_FOUND; }

        out.emplace_back(std::move(word), std::move(code));
    }
    return NO_ERROR;
}

static void usage(const char* prog) {
    std::cerr << "Usage: " << prog << " <base>.hdr <base>.code\n"
              << "  (files must be in ./input_output)\n";
    std::exit(1);
}

int main(int argc, char* argv[]) {
    if (argc != 3) usage(argv[0]);

    fs::path dir = "input_output";
    exitOnError(directoryExists(dir.string()), dir.string());

    // Resolve to files only within input_output/
    fs::path hdrPath  = dir / fs::path(argv[1]).filename();
    fs::path codePath = dir / fs::path(argv[2]).filename();

    // Preflight I/O using utils
    exitOnError(regularFileExistsAndIsAvailable(hdrPath.string()),  hdrPath.string());
    exitOnError(regularFileExistsAndIsAvailable(codePath.string()), codePath.string());

    // Sanity (spec): code file should start with 0 or 1 (skipping newlines)
    {
        std::ifstream code_chk(codePath);
        if (!code_chk) exitOnError(UNABLE_TO_OPEN_FILE, codePath.string());
        int ch; bool ok = false;
        while ((ch = code_chk.get()) != EOF) {
            if (ch == '\n' || ch == '\r') continue;
            ok = (ch == '0' || ch == '1');
            break;
        }
        if (!ok) {
            std::cerr << "Error: code file must start with 0 or 1.\n";
            return 5;
        }
    }

    // Parse header -> (word, code)
    std::vector<std::pair<std::string,std::string>> header_pairs;
    std::string parseErr;
    {
        error_type st = readHeaderPairs(hdrPath, header_pairs, parseErr);
        if (st != NO_ERROR) {
            if (!parseErr.empty()) std::cerr << "Error reading header: " << parseErr << "\n";
            exitOnError(st, hdrPath.string());
        }
    }

    // Build decoding tree
    HuffmanTree decoder;
    {
        std::string buildErr;
        bool ok = decoder.buildFromHeader(header_pairs, buildErr);
        if (!ok) {
            std::cerr << "Error building Huffman tree: " << buildErr << "\n";
            return 7;
        }
    }

    // Output path: use base of the .hdr filename (stem), not baseNameWithoutTxt (that’s for .txt)
    fs::path outPath = dir / (hdrPath.stem().string() + ".tokens_decoded");
    exitOnError(canOpenForWriting(outPath.string()), outPath.string());

    // Stream decode: ignore newlines in .code; write one word per line to .tokens_decoded
    std::ifstream code_in(codePath);
    if (!code_in) exitOnError(UNABLE_TO_OPEN_FILE, codePath.string());
    std::ofstream out(outPath);
    if (!out) exitOnError(UNABLE_TO_OPEN_FILE_FOR_WRITING, outPath.string());

    std::string decodeErr;
    if (!decoder.decode(code_in, out, decodeErr)) {
        std::cerr << "Error decoding: " << decodeErr << "\n";
        return 9;
    }
    // Success: per spec, no extra output required.
    return 0;
}
