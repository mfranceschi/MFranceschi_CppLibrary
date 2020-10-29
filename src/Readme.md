# Internal README

## TODO

- Put everything in namespace MF
- Simplify Command:
  - The output and error streams are specified using a filename. Special filenames are "." for retrieving the strings and "" to send to trash.
  - The command line is given in one unique string. If necessary, the arguments are splitted according to the main shell's routine for that.
  - The function returns immediately a unique_ptr to a class. That class can return the exit code and both streams contents. It throws an exception if the execution is not finished yet. It throws an exception if the stream has not been asked for retrieval. That class has a wait method which stops until finishing the execution with a timeout.  
- Connect and simplify "Date" with STL ctime and chrono libraries.
