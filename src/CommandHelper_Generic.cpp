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
void CommandComponent::cleanUp() {}

// ///////////////////////////////////////////////////////////////
// ////////////////////// OUTPUT STREAMS /////////////////////////
// ///////////////////////////////////////////////////////////////


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

void CommandRunner::cleanUp() {
    processInputStream->cleanUp();
    processOutputStream->cleanUp();
    processErrorStream->cleanUp();

    delete processInputStream;
    delete processOutputStream;
    delete processErrorStream;

    CloseHandle(processHandle);
}
