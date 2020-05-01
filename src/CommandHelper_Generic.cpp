//
// Created by mfran on 27/04/2020.
//

#include "CommandHelper.hpp"

// ///////////////////////////////////////////////////////////////
// ///////////// COMMAND COMPONENT interface /////////////////////
// ///////////////////////////////////////////////////////////////

void CommandComponent::beforeStart() {}
void CommandComponent::afterStart() {}
void CommandComponent::beforeStop() {}
void CommandComponent::afterStop() {}

// ///////////////////////////////////////////////////////////////
// /////////////////////// INPUT STREAMS /////////////////////////
// ///////////////////////////////////////////////////////////////

ProcessInputStream_String::ProcessInputStream_String(const File::SFilename_t& str) :
        inputString(str) {}

ProcessInputStream_FromFile::ProcessInputStream_FromFile(const File::SFilename_t& file_name) :
        filename(file_name) {}

// ///////////////////////////////////////////////////////////////
// ////////////////////// OUTPUT STREAMS /////////////////////////
// ///////////////////////////////////////////////////////////////

std::string ProcessOutputStream::retrieveOutput() {
    return std::string();
}

ProcessOutputStream_Export::ProcessOutputStream_Export(bool append, const File::SFilename_t& filename)  :
        APPEND(append), filename(filename) {}

// ///////////////////////////////////////////////////////////////
// ////////////////////// COMMAND RUNNER /////////////////////////
// ///////////////////////////////////////////////////////////////

void CommandRunner::setInput(ProcessInputStream* stream) {
    delete processInputStream;
    processInputStream = stream;
}

void CommandRunner::setOutput(ProcessOutputStream* stream) {
    delete processOutputStream;
    processOutputStream = stream;
}

void CommandRunner::setError(ProcessOutputStream* stream) {
    delete processErrorStream;
    processErrorStream = stream;
}

void CommandRunner::start() {
    processInputStream->beforeStart();
    processOutputStream->beforeStart();
    processErrorStream->beforeStart();

    internalStart();

    processInputStream->afterStart();
    processOutputStream->afterStart();
    processErrorStream->afterStart();
}

void CommandRunner::stop() {
    processInputStream->beforeStop();
    processOutputStream->beforeStop();
    processErrorStream->beforeStop();

    internalStop();

    processInputStream->afterStop();
    processOutputStream->afterStop();
    processErrorStream->afterStop();
}

CommandRunner::~CommandRunner() {
    delete processInputStream;
    delete processOutputStream;
    delete processErrorStream;

    internalOSCleanUp();
}

std::string CommandRunner::getOutput() {
    return processOutputStream->retrieveOutput();
}

std::string CommandRunner::getError() {
    return processErrorStream->retrieveOutput();
}

int CommandRunner::getStatusCode() {
    return internalGetStatusCode();
}
