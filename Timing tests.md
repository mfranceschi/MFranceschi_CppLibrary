# Summary
During the creation process of MyWorks C++ repository, a thing that I wanted to ensure was that the code I would write will be the fastest possible. 

For instance, there are several ways of getting a file size in Windows: 
* You can get the position of a stream (FILE* or fstream) after positionning it at the end of the file.
* You can use syscalls to retrieve the file attribute: the portable `fstat`, the Windows-only brother `GetFileInformationEx`.
* You can use a syscall to retrieve the file size directly: the Windows-only `GetFileSizeEx`.

Four different ways for getting a file size, each of them are very easy to write and take from one to four lines, and take no longer than a few microseconds to run. But **which one is the shortest**? This simple question was my goal.

In this document, I will present the code I used and the results I obtained. Please note that all timing results are from the same run of the script, without any other process that may slow down the computations. Of course, you could run 

# Basic source code of my tests
You can find the whole source code on this repo but here are the main parts.

I have created a function (called `Timethis`) for measuring the time a function takes to execute. `Timethis` accepts as arguments the number of iterations of the function and the function itself, which should take no argument and return nothing. Here is the source code for it:
```c++
double Timethis(size_t iter, const std::function<void(void)>& func)
{
  using namespace std::chrono;
  high_resolution_clock::time_point beggining = high_resolution_clock::now();
  for (size_t i = 0; i < iter; i++)
  {
    func();
  }
  return (duration<double>(high_resolution_clock::now() - beggining).count()) / double(iter);
}
```
As it has to make two syscalls (both calls to `...::now()`), I had to ensure that it would have a minimal impact on the measures. Thus, each time I run my tests, I also call `Timethis` without any argument to have some kind of average offset of the next measures.

Here is today's value: `xxx`.

```
The duration of the execution of 'Timethis' itself, without anything to do, is: 3.3187e-07

Timing the file existence functions !
Time for PathFileExists: 2.06946e-05
Time for stat: 3.40244e-05
Time for GetFileAttributes: 1.72648e-05

Timing the file size functions !
Time for fseek : 3.92838e-05
Time for GetFileAttributesEx: 1.73985e-05
Time for GetFileSizeEx: 2.93431e-05

Timing char to wchar_t conversion functions!
Time for mbstowcs_s: 5.1667e-07
Time for wstring creation: 7.57953e-06

Timing functions for reading 5 chars in a file!
Time for FILE* with fgets: 4.22333e-05
Time for FILE* with fgetc: 3.84125e-05
Time for ifstream with getline: 4.47927e-05
Time for ifstream with get: 5.11807e-05
Time for Windows syscalls: 3.99883e-05

Timing <ctime> conversion functions between tm and time_t !
Time for localtime: 8.4769e-07
Time for localtime_s: 7.0153e-07
Time for mktime (with local [FR] time tm): 1.66125e-06
Time for gmtime: 4.3435e-07
Time for gmtime_s: 4.6011e-07
Time for mktime (with GMT time tm): 1.69234e-06
```


# Sources:
* http://erikerlandson.github.io/blog/2012/03/19/interaction-between-mktime-and-tm-isdst-a-compute-cycle-landmine/
* https://stackoverflow.com/questions/8558919/mktime-and-tm-isdst
