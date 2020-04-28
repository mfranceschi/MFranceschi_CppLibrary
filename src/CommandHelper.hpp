//
// Created by mfran on 27/04/2020.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_COMMANDHELPER_HPP
#define MFRANCESCHI_CPPLIBRARIES_COMMANDHELPER_HPP

#include "File.hpp"
#if defined(_WIN32)
#   include <Windows.h>
#else
#endif

// Declarations
class CommandComponent;
class CommandRunner;
class ProcessInputStream;
class ProcessInputStream_None;
class ProcessInputStream_String;
class ProcessInputStream_File;
class ProcessOutputStream;
class ProcessOutputStream_Keep;
class ProcessOutputStream_Kill;
class ProcessOutputStream_Export;
class ProcessOutputStream_Retrieve;

// BASE INTERFACE
class CommandComponent {
public:
    /// This will be called right before the command is started.
    virtual void beforeStart();
    /// This will be called right after the command is started.
    virtual void afterStart();
    /// This will be called right before we initiate the process stopping procedure.
    virtual void beforeStop();
    /// This will be called right after the process is stopped.
    virtual void afterStop();
    /// This cleans up all resources, by anticipation of calling the destructor.
    virtual void cleanUp();
    virtual ~CommandComponent() = default;
};

// STREAM CLASSES
class ProcessInputStream : public CommandComponent {
public:
#if defined(_WIN32)
    virtual HANDLE getHandle() const = 0;
#endif
};

class ProcessOutputStream : public CommandComponent {
public:
    const bool OUTPUT_AND_NOT_ERROR;
    explicit ProcessOutputStream(bool notError);
#if defined(_WIN32)
    virtual HANDLE getHandle() const = 0;
#endif
};

class ProcessInputStream_None : public ProcessInputStream {
public:
#if defined(_WIN32)
    HANDLE getHandle() const override;
#endif
};

class ProcessOutputStream_Keep : public ProcessOutputStream {
public:
    explicit ProcessOutputStream_Keep(bool notError);
#if defined(_WIN32)
    HANDLE getHandle() const override;
#endif
};

// COMMAND RUNNER
class CommandRunner : public CommandComponent {
public:
    void setInput(ProcessInputStream* stream);
    void setOutput(ProcessOutputStream* stream);
    void setError(ProcessOutputStream* stream);
    void start();
    void stop();
    void cleanUp() override;

protected:
    void internalStart();
    void internalStop();

    File::SFilename_t* executable = nullptr;
    std::vector<File::SFilename_t>* arguments = nullptr;
    ProcessOutputStream* processOutputStream = new ProcessOutputStream_Keep(true);
    ProcessOutputStream* processErrorStream = new ProcessOutputStream_Keep(false);
    ProcessInputStream* processInputStream = new ProcessInputStream_None;

private:
#if defined(_WIN32)
    HANDLE processHandle = INVALID_HANDLE_VALUE;
#else
#endif
};

#endif //MFRANCESCHI_CPPLIBRARIES_COMMANDHELPER_HPP
