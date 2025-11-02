
#ifndef HUFFMANDECODER_UTILS_H
#define HUFFMANDECODER_UTILS_H
#pragma once

#include <string>

enum error_type {
    NO_ERROR,
    FILE_NOT_FOUND,
    DIR_NOT_FOUND,
    UNABLE_TO_OPEN_FILE,
    ERR_TYPE_NOT_FOUND,
    UNABLE_TO_OPEN_FILE_FOR_WRITING,
    FAILED_TO_WRITE_FILE,
};

void exitOnError(error_type error, const std::string& entityName);
error_type regularFileExistsAndIsAvailable(const std::string &fileName);
error_type fileExists(const std::string &name);
error_type directoryExists(const std::string &name);
error_type regularFileExists(const std::string &name);
std::string baseNameWithoutTxt(const std::string& filename);
error_type canOpenForWriting(const std::string& filename);
error_type writeVectorToFile(const std::string& filename,
                             const std::vector<std::string> & lines);

#endif //HUFFMANDECODER_UTILS_H