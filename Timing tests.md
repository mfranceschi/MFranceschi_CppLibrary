# Summary
During the creation process of MyWorks C++ repository, a thing that I wanted to ensure was that the code I would write will be the fastest possible. 

For instance, there are several ways of getting a file size in Windows: 
* You can get the position of a stream (FILE* or fstream) after positionning it at the end of the file.
* You can use syscalls to retrieve the file attribute: the portable `fstat`, the Windows-only brother `GetFileInformationEx`.
* You can use a syscall to retrieve the file size directly: the Windows-only `GetFileSizeEx`.

Four different ways for getting a file size, each of them are very easy to write and take from one to four lines, and take no longer than a few microseconds to run. But **which one is the shortest**? This simple question was my goal.

In this document, I will present the code I used and the results I obtained.

# Sources:
* http://erikerlandson.github.io/blog/2012/03/19/interaction-between-mktime-and-tm-isdst-a-compute-cycle-landmine/
* https://stackoverflow.com/questions/8558919/mktime-and-tm-isdst
