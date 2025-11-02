
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include "utils.h"


void exitOnError(error_type error, const std::string &entityName = "") {
    switch (error) {
        case NO_ERROR:
            // do nothing
            return;

        case FILE_NOT_FOUND:
            std::cerr << "Error: File " << entityName << " doesn't exist. Terminating...\n";
            exit(FILE_NOT_FOUND);

        case UNABLE_TO_OPEN_FILE:
            std::cerr << "Error: Unable to open '" << entityName << "'. Terminating...\n";
            exit(UNABLE_TO_OPEN_FILE);

        case DIR_NOT_FOUND:
            std::cerr << "Error: Directory " << entityName << " doesn't exist. Terminating...\n";
            exit(DIR_NOT_FOUND);

        case UNABLE_TO_OPEN_FILE_FOR_WRITING:
            std::cerr << "Error: Unable to open " << entityName << " for writing. Terminating...\n";
            exit(UNABLE_TO_OPEN_FILE_FOR_WRITING);

        default:
            std::cerr << "Error: Unknown error type. Terminating...\n";
            exit(ERR_TYPE_NOT_FOUND);
    }
}

error_type directoryExists(const std::string &name) {

    if (!std::filesystem::is_directory(name)) {
        return DIR_NOT_FOUND;
    }
    return NO_ERROR;
}

error_type regularFileExists(const std::string &name) {

    if (!std::filesystem::is_regular_file(name)) {
        return FILE_NOT_FOUND;
    }
    return NO_ERROR;
}

error_type regularFileExistsAndIsAvailable(const std::string &filename) {

    if (error_type return_value; (return_value = regularFileExists(filename)) != NO_ERROR ) {
        return return_value;
    }

    std::ifstream infile(filename);

    // Check if the file could be opened
    if (!infile.is_open()) {
        return UNABLE_TO_OPEN_FILE;
    }
    infile.close();
    return NO_ERROR;

}


std::string baseNameWithoutTxt(const std::string& filename) {
    // "filename" is expected to have .txt extension.
    // Return the base-name of the "filename".

    namespace fs = std::filesystem;
    fs::path p(filename);

    // stem() gives filename without extension
    if (p.extension() == ".txt") {
        return p.stem().string();
    }

    return p.filename().string();
}

error_type canOpenForWriting(const std::string& filename) {
    // Determine if "filename" can be opened for writing.

    std::ofstream out(filename, std::ios::out | std::ios::trunc);
    auto is_open = out.is_open();
    out.close();

    return is_open ? NO_ERROR : UNABLE_TO_OPEN_FILE_FOR_WRITING;
}

error_type writeVectorToFile(const std::string& filename,
                       const std::vector<std::string>& data) {
    // Open "fileName" for writing (truncating the file if it already exists).
    // If the file is opened successfully, write each element of "data"
    // to it, placing one element on each line.

    std::ofstream out(filename, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        return UNABLE_TO_OPEN_FILE_FOR_WRITING;
    }

    for (const auto& item : data) {
        out << item << '\n';
        if (!out) {
            std::cerr << "Error: failed while writing to " << filename << "\n";
            return FAILED_TO_WRITE_FILE;
        }
    }

    return NO_ERROR;
}

