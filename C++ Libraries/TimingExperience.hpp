//
// Created by mfran on 25/12/2019.
//

#ifndef MYWORKS_TEST0_TIMINGEXPERIENCE_H
#define MYWORKS_TEST0_TIMINGEXPERIENCE_H

namespace TimingExperience {
    void RunAll();
    void timingTimeThis();
    void timingTheFileExistence();
    void timingTheFileSize();
    void timingWchar_tConversion();
    void timingFileReading();
    void timingCtimeFunctions();

    constexpr size_t NUMBER_OF_ITERATIONS = 10 * 1000;
};


#endif //MYWORKS_TEST0_TIMINGEXPERIENCE_H
