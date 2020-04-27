//
// Created by mfran on 27/04/2020.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_COMMANDHELPER_HPP
#define MFRANCESCHI_CPPLIBRARIES_COMMANDHELPER_HPP

// Declarations
class CommandComponent;
class CommandRunner;
class ProcessInputStream;
class ProcessOutputStream;

// Interface
class CommandComponent {
public:
    /// This will be called right before the command is started.
    virtual void beforeStart() = 0;
    /// This will be called right after the command is started.
    virtual void afterStart() = 0;
    /// This will be called right before we initiate the process shutdown procedure.
    virtual void beforeClose() = 0;
    /// This will be called right after the process is shot down.
    virtual void afterClose() = 0;
    /// This cleans up all resources, by anticipation of calling the destructor.
    virtual void cleanUp() = 0;
};

class ProcessInputStream : public CommandComponent {
public:

};

/// This class must be subclassed AAAAAAAAH I M LOST
class CommandRunner : public CommandComponent {

};

#endif //MFRANCESCHI_CPPLIBRARIES_COMMANDHELPER_HPP
