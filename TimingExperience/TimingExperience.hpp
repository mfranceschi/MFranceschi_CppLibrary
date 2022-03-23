//
// Created by mfran on 25/12/2019.
//

#ifndef MYWORKS_TEST0_TIMINGEXPERIENCE_H
#define MYWORKS_TEST0_TIMINGEXPERIENCE_H

#include <cstdlib>

namespace TimingExperience
{
    void RunAll();
    void timingTimeThis();
    void timingTheFileExistence();
    void timingTheFileSize();
    void timingWchar_tConversion();
    void timingFileReading();
    void timingCtimeFunctions();

    constexpr std::size_t NUMBER_OF_ITERATIONS = 10 * 1000;
} // namespace TimingExperience

#endif // MYWORKS_TEST0_TIMINGEXPERIENCE_H
