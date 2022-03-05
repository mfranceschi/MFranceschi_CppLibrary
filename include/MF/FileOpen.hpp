//
// Created by mfran on 24/04/2020.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_FILEOPEN_HPP
#define MFRANCESCHI_CPPLIBRARIES_FILEOPEN_HPP

#include "MF/Filesystem.hpp"

namespace File {
    /// Data structure used to store information about files opened with Open.
    struct ReadFileData {
        const char *contents = nullptr;
        Filesize_t size = 0ul;

        virtual ~ReadFileData() = default; // For polymorphic reasons.
    };

    /**
     * Stores the entire contents of file "filename" in a read-only C-string.
     * Also, that C-string does not end with '\0'.
     * It is advised to use an "InCharArrayStream".
     * To clean up memory, please call "Read_Close" with the structure returned from there.
     * The structure must remain a pointer (in reality, it is an instance of a subclass of ReadFileData).
     * The purpose of this function is to offer the fastest way to read an entire file.
     * @param filename Name of the file to open.
     * @return "nullptr" if anything failed or the file is empty, or a new structure.
     */
    const ReadFileData *Read(Filename_t filename);

    /// Please use this simple tool to clean up any memory associated with something returned by "Read".
    void Read_Close(const ReadFileData *content);

    /**
     * Wrapper function: fills a C++ string with the whole file contents, internally using the Read function.
     * @param filename Name of the file to read.
     * @param string A string with the whole contents of the file, not modified if an error occurred.
     * @return True on success, false on error.
     */
    bool ReadToString(Filename_t filename, std::string &string);
}


#endif //MFRANCESCHI_CPPLIBRARIES_FILEOPEN_HPP
