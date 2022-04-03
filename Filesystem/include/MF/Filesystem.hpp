/* Martin Franceschi - MyWorks package - File module.
 *
 * This module gives various information about a given file.
 * You can check if it exists, check its size and encoding.
 * You can also open it by applying automatically a locale
 * (UTF-8 and UTF-16LE only).
 * You are in charge of providing the right C-string filename (wide or not).
 */

//---------- Interface of module <File> (file File.h)
#if !defined(FILE_H)
#    define FILE_H

//--------------------------------------------------------------- Includes
#    include <fstream>
#    include <memory>
#    include <string>
#    include <vector>

namespace MF
{
    namespace Filesystem
    {
        // Represents a file encoding.
        // It is an implementation detail that may change. Please use "..._t" instead.
        enum class Encoding_e {
            ENC_UTF16LE, // Normal UTF-16LE
            ENC_UTF8, // Normal UTF-8
            ENC_DEFAULT, // If no encoding is false, we assume the default locale.
            ENC_ERROR // A problem occurred while looking for the encoding.
        };

        // Represents a file encoding.
        using Encoding_t = Encoding_e;

        // Type used to deal with file sizes of any weight (GBs are okay).
        using Filesize_t = unsigned long;

#    if MF_WINDOWS && defined(UNICODE)
#        define MAKE_FILE_NAME L""
        using Filename_t = const wchar_t *;
        using SFilename_t = std::wstring;
#    else
#        define MAKE_FILE_NAME ""
        using Filename_t = const char *;
        using SFilename_t = std::string;
#    endif

#    if MF_WINDOWS
#        define FILE_SEPARATOR MAKE_FILE_NAME "\\"
#        define LINE_END "\r\n"
#    else
#        define FILE_SEPARATOR MAKE_FILE_NAME "/"
#        define LINE_END "\n"
#    endif

        //////////////////////////////////////////////////////////////////  PUBLIC
        //------------------------------------------------------- Public functions

        /**
         * Generates a file name by concatenating each argument from the list.
         * Arguments must be of "const char[]" type, so it is advised to use literal strings.
         * If number is <= 0 then this function returns an empty string.
         * @param absolute If true and we are not on Windows, we prepend a directory separator.
         * @param isDirectory If true then we append a directory separator at the very end.
         * @param number Number of variadic arguments.
         * @param ... List of "number" C-strings.
         * @return A concatenation of the arguments and the default directory separator.
         */
        SFilename_t MakeFilename(bool absolute, bool isDirectory, int number, ...);

        /**
         * This function removes the given file or directory.
         * @param filename Name of the file or directory to remove. Often, directories require to be
         * empty.
         * @param fileOnly If true, the removal is done if and only if the "filename" points to a
         * file.
         * @return True if something was removed successfully.
         */
        bool Delete(Filename_t filename, bool fileOnly = true);

        /// Deletes a file.
        bool DeleteFile(Filename_t filename);

        /// Deletes an empty directory.
        bool DeleteDirectory(Filename_t filename);

        /**
         * Determines the encoding of the file, accordingly to the first two or three bytes in the
         * file. Special results: ENC_ERROR if reading the first three bytes failed; ENC_DEFAULT if
         * no supported encoding could be determined.
         * @param filename Path to the file to test.
         * @return An "encoding_t" value.
         */
        Encoding_t GetFileEncoding(Filename_t filename);

        /// Returns true if "filename" points to an existing file, false otherwise.
        bool IsFile(Filename_t filename);

        /// Returns true if "filename" points to an existing directory, false otherwise.
        bool IsDir(Filename_t filename);

        /**
         * Attempts to read a few bytes in the file, and simply say if it was a success or not.
         * @param filename Name of the file to read.
         * @param charsToRead Number of bytes to read. If <= 0, we simply return "Exist(filename)".
         * @return True if it was a success, false if we could not read exactly "charsToRead" bytes.
         */
        bool IsFileReadable(Filename_t filename, unsigned char charsToRead = 3);

        /**
         * Extraordinary function.
         *
         * 1. It closes "ifs".
         * 2. If unknown, it calls "GetFileEncoding(filename)" to determine the encoding.
         * 3. If the coding could be processed, we call "ifs.open(filename)" with a specific policy
         * (locale, starting offset, etc.). In all cases, the function returns.
         * @param ifs The stream to open with the given file and opening policy. It is closed before
         * doing anything.
         * @param filename The path to the file to open.
         * @param encoding (optional) If you already know the encoding, you can set this parameter;
         *                            otherwise it will be determined inside the function.
         * @return False if and only if trying to determine the encoding failed --> file cannot be
         * processed correctly.
         */
        bool OpenFile(
            std::ifstream &ifs, Filename_t filename, Encoding_t encoding = Encoding_e::ENC_ERROR);

        /// Returns the size of the file pointed by "filename" in bytes, or 0 if anything failed.
        Filesize_t GetFileSize(Filename_t filename);

        /// Returns the cumulated size of all regular files of the given directory.
        Filesize_t GetDirectorySize(Filename_t filename);

        /// Simple helper function, for use during debugging.
        std::ostream &operator<<(std::ostream &theOstream, const Encoding_t &enc);

        /**
         * Creates an empty directory. No particular setting (such as permissions) is applied.
         * @param filename Name of the directory to create.
         * @return True if the directory actually got created.
         */
        bool CreateDirectory(Filename_t filename);

        bool CreateFile(Filename_t filename, Filesize_t sizeToAllocate = 0);

        inline bool TouchFile(Filename_t filename) {
            return CreateFile(filename);
        }

        /// Returns the "Current Working Directory" as a folder path. It ends with a FILE_SEPARATOR.
        SFilename_t GetCWD();

        /**
         * Generates the complete list of files and directories that are direct children of the
         * given folder. Names are returned relative to the "folder". Directories have an ending
         * PATH_SEPARATOR.
         * > ListFilesInDirectory("myFolder/") -> ("file.txt", "image.png", "subfolder/")
         * @param folder Name or path to the folder. It must end with a PATH_SEPARATOR character.
         * @return List of files and directories names, or empty vector if anything failed.
         */
        std::vector<SFilename_t> ListFilesInDirectory(Filename_t folder);

        /// Data structure used to store information about files opened with OpenFile.
        struct WholeFileData {
            const char *contents = nullptr;
            Filesize_t size = 0UL;

            virtual ~WholeFileData() = default; // For polymorphic reasons.
        };

        /**
         * Stores the entire contents of file "filename" in a read-only C-string.
         * Also, that C-string does not end with '\0'.
         * It is advised to use an "InCharArrayStream".
         * The structure must remain a pointer (in reality, it is an instance of a subclass of
         * WholeFileData). The purpose of this function is to offer the fastest way to read an
         * entire file.
         * @param filename Name of the file to open.
         * @return "nullptr" if anything failed or the file is empty, or a new structure.
         */
        std::unique_ptr<const WholeFileData> ReadWholeFile(Filename_t filename);

        /**
         * Wrapper function: fills a C++ string with the whole file contents, internally using the
         * Read function.
         * @param filename Name of the file to read.
         * @param string A string with the whole contents of the file, not modified if an error
         * occurred.
         * @return True on success, false on error.
         */
        bool ReadWholeFileToString(Filename_t filename, std::string &string);
    } // namespace Filesystem
} // namespace MF
#endif // FILE_H
