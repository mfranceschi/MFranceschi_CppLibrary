//
// Created by mfran on 27/04/2020.
//

#include "CommandHelper.hpp"

void CommandComponent::beforeStart() {}
void CommandComponent::afterStart() {}
void CommandComponent::beforeStop() {}
void CommandComponent::afterStop() {}
void CommandComponent::cleanUp() {}

ProcessOutputStream::ProcessOutputStream(bool notError) :
        OUTPUT_AND_NOT_ERROR(notError) {}

ProcessOutputStream_Keep::ProcessOutputStream_Keep(bool notError) : ProcessOutputStream(notError) {}

void CommandRunner::setInput(ProcessInputStream* stream) {
    processInputStream = stream;
}

void CommandRunner::setOutput(ProcessOutputStream* stream) {
    processOutputStream = stream;
}

void CommandRunner::setError(ProcessOutputStream* stream) {
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
}
