//
// Created by MartinF on 03/07/2023.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_FIXEDLENGTHVECTOR_HPP
#define MFRANCESCHI_CPPLIBRARIES_FIXEDLENGTHVECTOR_HPP

#include <functional>
#include <memory>

namespace MF
{
    namespace Containers
    {
        namespace internals
        {
            template <typename size_type>
            void checkOrThrowOutOfRange(size_type requestedIndex, size_type nbrElements) {
                if (requestedIndex >= nbrElements) {
                    throw nullptr; // TODO
                }
            }

            template <typename T>
            void noDeleter(T *) {
            }
        } // namespace internals

        template <typename T, typename Allocator = std::allocator<T>>
        class FixedLengthVector {
           public:
            using value_type = T;
            using size_type = std::size_t;
            using difference_type = std::ptrdiff_t;
            using reference = value_type &;
            using const_reference = const value_type &;
            using pointer = value_type *;
            using const_pointer = const value_type *;
            using iterator = pointer;
            using const_iterator = const_pointer;

           private:
            T *const ptr;
            const size_type nbrElements;
            const std::function<void(T *)> deleter = std::default_delete<T[]>{};

           public:
            FixedLengthVector() = delete;

            FixedLengthVector(size_type nbrElements)
                : ptr(new T[nbrElements]), nbrElements(nbrElements) {
            }

            FixedLengthVector(
                T *pointer,
                size_type nbrElements,
                std::function<void(T *)> deleter = internals::noDeleter)
                : ptr(pointer), nbrElements(nbrElements), deleter(deleter) {
            }

            ~FixedLengthVector() {
                deleter(ptr);
            }

            constexpr reference at(size_type pos) {
                internals::checkOrThrowOutOfRange(pos, nbrElements);
                return ptr[pos];
            }

            constexpr reference operator[](size_type pos) {
                return ptr[pos];
            }

            constexpr const_reference at(size_type pos) const {
                internals::checkOrThrowOutOfRange(pos, nbrElements);
                return ptr[pos];
            }

            constexpr const_reference operator[](size_type pos) const {
                return ptr[pos];
            }

            constexpr reference front() {
                return *ptr;
            }

            constexpr const_reference front() const {
                return *ptr;
            }

            constexpr reference back() {
                return ptr[nbrElements - 1];
            }

            constexpr const_reference back() const {
                return ptr[nbrElements - 1];
            }

            constexpr pointer data() {
                return ptr;
            }

            constexpr const_pointer data() const {
                return ptr;
            }

            constexpr bool empty() const noexcept {
                return nbrElements == 0;
            }

            constexpr size_type size() const noexcept {
                return nbrElements;
            }

            constexpr size_type max_size() const noexcept {
                return nbrElements;
            }

            constexpr size_type capacity() const noexcept {
                return nbrElements;
            }

            constexpr iterator begin() noexcept {
                return ptr;
            }

            constexpr const_iterator cbegin() const noexcept {
                return ptr;
            }

            constexpr iterator end() noexcept {
                return ptr + nbrElements;
            }

            constexpr const_iterator cend() const noexcept {
                return ptr + nbrElements;
            }
        };
    } // namespace Containers
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_FIXEDLENGTHVECTOR_HPP
