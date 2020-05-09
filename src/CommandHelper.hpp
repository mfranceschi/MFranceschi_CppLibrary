//
// Created by mfran on 27/04/2020.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_COMMANDHELPER_HPP
#define MFRANCESCHI_CPPLIBRARIES_COMMANDHELPER_HPP

#include "File.hpp"
#if defined(_WIN32)
#   include <Windows.h>
using StreamItem = HANDLE;
using ProcessItem = HANDLE;
constexpr StreamItem STREAM_ITEM_DEFAULT = nullptr;
#else
#   include <unistd.h>
using FD_t = int;
using StreamItem = int;
using ProcessItem = pid_t;
constexpr StreamItem STREAM_ITEM_DEFAULT = -1;
#endif

// Declarations
class CommandComponent;
class CommandRunner;
class ProcessInputStream;
class ProcessInputStream_None;
class ProcessInputStream_String;
class ProcessInputStream_FromFile;
class ProcessOutputStream;
class ProcessOutputStream_Keep;
class ProcessOutputStream_Kill;
class ProcessOutputStream_Export;
class ProcessOutputStream_Retrieve;
class ProcessErrorStream_Keep;

// ///////////////////////////////////////////////////////////////
// ///////////// COMMAND COMPONENT interface /////////////////////
// ///////////////////////////////////////////////////////////////

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
    virtual ~CommandComponent() = default;
};

class ProcessStream : public CommandComponent {
public:
    virtual StreamItem getStreamItem() const = 0;

#if !defined(_WIN32)
    virtual void closeOnFork();
#endif
};

// ///////////////////////////////////////////////////////////////
// /////////////////////// INPUT STREAMS /////////////////////////
// ///////////////////////////////////////////////////////////////

class ProcessInputStream : public ProcessStream {};

class ProcessInputStream_None : public ProcessInputStream {
public:
    StreamItem getStreamItem() const override;
};

class ProcessInputStream_String : public ProcessInputStream {
public:
    explicit ProcessInputStream_String(const File::SFilename_t& string);
    void beforeStart() override;
    void afterStart() override;
    void afterStop() override;
    StreamItem getStreamItem() const override;
#if !defined(_WIN32)
    void closeOnFork() override;
#endif
protected:
    const File::SFilename_t& inputString;
    StreamItem readStream = STREAM_ITEM_DEFAULT;
    StreamItem writeToStream = STREAM_ITEM_DEFAULT;
};

class ProcessInputStream_FromFile : public ProcessInputStream {
public:
    explicit ProcessInputStream_FromFile(const File::SFilename_t& filename);
    void beforeStart() override;
    void afterStop() override;
    StreamItem getStreamItem() const override;
#if !defined(_WIN32)
    void closeOnFork() override;
#endif
protected:
    const File::SFilename_t& filename;
    StreamItem fileStream = STREAM_ITEM_DEFAULT;
};

// ///////////////////////////////////////////////////////////////
// ////////////////////// OUTPUT STREAMS /////////////////////////
// ///////////////////////////////////////////////////////////////

class ProcessOutputStream : public ProcessStream {
public:
    virtual std::string retrieveOutput();
};

class ProcessOutputStream_Keep : public ProcessOutputStream {
public:
    StreamItem getStreamItem() const override;
};

class ProcessErrorStream_Keep : public ProcessOutputStream {
public:
    StreamItem getStreamItem() const override;
};

class ProcessOutputStream_Export : public ProcessOutputStream {
public:
    const bool APPEND;
    explicit ProcessOutputStream_Export(bool append, const File::SFilename_t& filename);
    void beforeStart() override;
    void afterStop() override;
    StreamItem getStreamItem() const override;
#if !defined(_WIN32)
    void closeOnFork() override;
#endif

protected:
    const File::SFilename_t& filename;
    StreamItem fileStream = STREAM_ITEM_DEFAULT;
};

class ProcessOutputStream_Kill : public ProcessOutputStream_Export {
public:
    ProcessOutputStream_Kill();
    void beforeStart() override;
};

class ProcessOutputStream_Retrieve : public ProcessOutputStream {
public:
    void beforeStart() override;
    void beforeStop() override;
    void afterStop() override;
    std::string retrieveOutput() override;
    StreamItem getStreamItem() const override;
#if !defined(_WIN32)
    void closeOnFork() override;
#endif
protected:
    std::ostringstream oss;
    StreamItem readStream = STREAM_ITEM_DEFAULT;
    StreamItem writeStream = STREAM_ITEM_DEFAULT;
};

// ///////////////////////////////////////////////////////////////
// ////////////////////// COMMAND RUNNER /////////////////////////
// ///////////////////////////////////////////////////////////////

class CommandRunner {
public:
    void setInput(ProcessInputStream* stream);
    void setOutput(ProcessOutputStream* stream);
    void setError(ProcessOutputStream* stream);
    void start();
    void stop();
    std::string getOutput();
    std::string getError();
    int getStatusCode();
    ~CommandRunner();

    const File::SFilename_t* executable = nullptr;
    const std::vector<File::SFilename_t>* arguments = nullptr;

protected:
    void internalStart();
    void internalStop();
    int internalGetStatusCode();

    ProcessOutputStream* processOutputStream = new ProcessOutputStream_Keep;
    ProcessOutputStream* processErrorStream = new ProcessErrorStream_Keep;
    ProcessInputStream* processInputStream = new ProcessInputStream_None;

private:
    void internalOSCleanUp();
    ProcessItem childProcessItem = (void*)(-1);
};

#endif //MFRANCESCHI_CPPLIBRARIES_COMMANDHELPER_HPP
