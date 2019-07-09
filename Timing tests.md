# Summary
During the creation process of MyWorks C++ repository, a thing that I wanted to ensure was that the code I would write will be the fastest possible. 

For instance, there are several ways of getting a file size in Windows: 
* You can get the position of a stream (FILE* or fstream) after positionning it at the end of the file.
* You can use syscalls to retrieve the file attribute: the portable `fstat`, the Windows-only brother `GetFileInformationEx`.
* You can use a syscall to retrieve the file size directly: the Windows-only `GetFileSizeEx`.

Four different ways for getting a file size, each of them are very easy to write and take from one to four lines, and take no longer than a few microseconds to run. But **which one is the shortest**? This simple question was my goal.

In this document, I will present the code I used and the results I obtained. Please note that all timing results are from the same run of the script, without any other process that may slow down the computations.

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


# Sources:
* http://erikerlandson.github.io/blog/2012/03/19/interaction-between-mktime-and-tm-isdst-a-compute-cycle-landmine/
* https://stackoverflow.com/questions/8558919/mktime-and-tm-isdst
