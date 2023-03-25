//
// Created by MartinF on 28/01/2023.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_COMMONS_HPP
#define MFRANCESCHI_CPPLIBRARIES_COMMONS_HPP

namespace MF
{
    namespace Commons
    {
        /**
         * Subclass this to disable copy from your class.
         */
        struct NoCopy {
            NoCopy() = default;

            NoCopy(const NoCopy& other) = delete;
            NoCopy& operator=(const NoCopy& other) = delete;

            NoCopy(NoCopy&& other) = default;
            NoCopy& operator=(NoCopy&& other) = default;

            virtual ~NoCopy() = default;
        };

        /**
         * Subclass this to disable move from your class.
         */
        struct NoMove {
            NoMove() = default;

            NoMove(const NoMove& other) = default;
            NoMove& operator=(const NoMove& other) = default;

            NoMove(NoMove&& other) = delete;
            NoMove& operator=(NoMove&& other) = delete;

            virtual ~NoMove() = default;
        };
    } // namespace Commons
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_COMMONS_HPP
