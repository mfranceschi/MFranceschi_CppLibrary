//
// Created by Utilisateur on 27/03/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_FILESYSTEM_TESTS_COMMONS_HPP
#define MFRANCESCHI_CPPLIBRARIES_FILESYSTEM_TESTS_COMMONS_HPP

#include "MF/Filesystem.hpp"
#include "tests_data.hpp"

using namespace MF::Filesystem;

const Filename_t FILENAME_MIDDLE_SIZE = MF_FILESYSTEM_TESTS_FILE_MEDIUM;
const Filename_t FILENAME_NOT_EXISTING = MF_FILESYSTEM_TESTS_NON_EXISTING;
const Filename_t FILENAME_SMALL_UTF16LE = MF_FILESYSTEM_TESTS_FILE_SMALL_UTF16LE;
const Filename_t FILENAME_TEMP = MF_FILESYSTEM_TESTS_TEMP;

// Structure that holds file information.
struct file_info_data {
    Filesize_t size;
    Filename_t name;
    char firstByte, lastByte;
    bool exists;
    FileEncoding_e encoding;
    size_t offset = 0;
};

static const file_info_data fid_middle_size{287815, FILENAME_MIDDLE_SIZE,       'l', '\xEF',
                                            true,   FileEncoding_e::ENC_DEFAULT};
static const file_info_data fid_not_existing{0,     FILENAME_NOT_EXISTING,      0, 0,
                                             false, FileEncoding_e::ENC_DEFAULT};
static const file_info_data fid_smallfile_utf16le{38,   FILENAME_SMALL_UTF16LE,      '\xFF', '\x00',
                                                  true, FileEncoding_e::ENC_UTF16LE, 2};

#endif // MFRANCESCHI_CPPLIBRARIES_FILESYSTEM_TESTS_COMMONS_HPP
