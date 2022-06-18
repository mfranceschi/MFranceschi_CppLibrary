//
// Created by Utilisateur on 09/04/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_SHAREDLIBSMACROS_HPP
#define MFRANCESCHI_CPPLIBRARIES_SHAREDLIBSMACROS_HPP

#include <functional>
#include <type_traits>

#if MF_WINDOWS
#    include "MF/LightWindows.hpp"

/**
 * Optional entry point of your shared library. Only 1 per CMake target.
 * Both parameters are optional and are of type std::function<void()>.
 * An empty instance won't be called erroneously.
 * Throwing an exception in the procUnload is undefined behaviour; your process might get killed.
 *
 * @param procLoad Procedure to run when the library is first loaded.
 * @param procUnload Procedure to run when the library is being unloaded.
 */
#    define MF_SHARED_LIB_LOAD_UNLOAD(procLoad, procUnload)                     \
        BOOL WINAPI DllMain(_In_ HINSTANCE, _In_ DWORD reason, _In_ LPVOID) {   \
            static const std::function<void()> procedureLoad = procLoad;        \
            static const std::function<void()> procedureUnload = procUnload;    \
                                                                                \
            if (reason == DLL_PROCESS_ATTACH && bool(procedureLoad)) {          \
                procedureLoad();                                                \
            } else if (reason == DLL_PROCESS_DETACH && bool(procedureUnload)) { \
                procedureUnload();                                              \
            }                                                                   \
                                                                                \
            return TRUE;                                                        \
        }

#    define MF_EXPORT __declspec(dllexport)
#    define MF_IMPORT __declspec(dllimport)

#elif defined(__GNUC__)

// Implementation note.
// For GCC we use the constructor and destructor function attributes - they are tied to the life
// of the shared library.

/**
 * Optional entry point of your shared library. Only 1 per CMake target.
 * Both parameters are optional and are of type std::function<void()>.
 * An empty instance won't be called erroneously.
 * Throwing an exception in the procUnload is undefined behaviour; your process might get killed.
 *
 * @param procLoad Procedure to run when the library is first loaded.
 * @param procUnload Procedure to run when the library is being unloaded.
 */
#    define MF_SHARED_LIB_LOAD_UNLOAD(procLoad, procUnload)            \
        static __attribute__((constructor)) void procedureLoad() {     \
            static const std::function<void()> procedure = procLoad;   \
            if (procedure) {                                           \
                procedure();                                           \
            }                                                          \
        }                                                              \
                                                                       \
        static __attribute__((destructor)) void procedureUnload() {    \
            static const std::function<void()> procedure = procUnload; \
            if (procedure) {                                           \
                procedure();                                           \
            }                                                          \
        }

#    define MF_EXPORT
#    define MF_IMPORT

#else

/**
 * Optional entry point of your shared library. Only 1 per CMake target.
 * Both parameters are optional and are of type std::function<void()>.
 * An empty instance won't be called erroneously.
 * Throwing an exception in the procUnload is undefined behaviour; your process might get killed.
 *
 * @param procLoad Procedure to run when the library is first loaded.
 * @param procUnload Procedure to run when the library is being unloaded.
 */
#    define MF_SHARED_LIB_LOAD_UNLOAD(procLoad, procUnload)                  \
        class MfSharedLibLoadUnloadHandler {                                 \
            static const std::function<void()> procedureLoad = procLoad;     \
            static const std::function<void()> procedureUnload = procUnload; \
                                                                             \
           public:                                                           \
            MfSharedLibLoadUnloadHandler() {                                 \
                if (procedureLoad) {                                         \
                    procedureLoad();                                         \
                }                                                            \
            }                                                                \
                                                                             \
            ~MfSharedLibLoadUnloadHandler() {                                \
                if (procedureUnload) {                                       \
                    procedureUnload();                                       \
                }                                                            \
            }                                                                \
        };                                                                   \
        static MfSharedLibLoadUnloadHandler instanceOfMfSharedLibLoadUnloadHandler;

#    define MF_EXPORT
#    define MF_IMPORT

#endif

#endif // MFRANCESCHI_CPPLIBRARIES_SHAREDLIBSMACROS_HPP
