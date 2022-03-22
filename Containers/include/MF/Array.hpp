//
// Created by mfran on 03/09/2020.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_ARRAY_HPP
#define MFRANCESCHI_CPPLIBRARIES_ARRAY_HPP

#include <array> // TODO remove
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace MF
{
    template <typename T, std::size_t N>
    class Array;

    namespace detail
    {

        template <typename T, std::size_t N>
        class ArrayIterator {
           private:
            Array<T, N> &refToArray;
            std::size_t elementIndex;

           public:
            // ----- MEMBER TYPES ----- //
            using iterator_category = std::random_access_iterator_tag;
            using value_type = typename Array<T, N>::value_type;
            using difference_type = typename Array<T, N>::difference_type;
            using pointer = typename Array<T, N>::pointer;
            using reference = typename Array<T, N>::reference;

            // ----- MEMBER FUNCTIONS ----- //

            // ----- Constructors & destructors
            explicit constexpr ArrayIterator(Array<T, N> &theArray, std::size_t index)
                : refToArray(theArray), elementIndex(index) {
            }

            ArrayIterator() = delete;

            ArrayIterator(const ArrayIterator<T, N> &) = default;

            ArrayIterator &operator=(const ArrayIterator<T, N> &) = default;

            ArrayIterator(ArrayIterator<T, N> &&) noexcept = default;

            ArrayIterator &operator=(ArrayIterator<T, N> &&) noexcept = default;

            ~ArrayIterator() = default;

            // ----- Member functions

            // ----- Operators
            constexpr reference operator*() const {
                return refToArray[elementIndex];
            }

            constexpr pointer operator->() const {
                return &refToArray[elementIndex];
            }

            constexpr reference operator[](difference_type diff) const {
                return refToArray[elementIndex + diff];
            } // TODO ensure it is correct

            constexpr ArrayIterator &operator++() {
                ++elementIndex;
                return *this;
            }

            constexpr ArrayIterator &operator--() {
                --elementIndex;
                return *this;
            }

            constexpr ArrayIterator operator++(int) {
                auto copy = *this;
                ++elementIndex;
                return copy;
            }

            constexpr ArrayIterator operator--(int) {
                auto copy = *this;
                --elementIndex;
                return copy;
            }

            constexpr ArrayIterator operator+(difference_type n) const {
                return ArrayIterator<T, N>(refToArray, elementIndex + n);
            }

            constexpr ArrayIterator operator-(difference_type n) const {
                return ArrayIterator<T, N>(refToArray, elementIndex - n);
            }

            constexpr ArrayIterator &operator+=(difference_type n) {
                elementIndex += n;
                return *this;
            }

            constexpr ArrayIterator &operator-=(difference_type n) {
                elementIndex -= n;
                return *this;
            }
        };
    } // namespace detail

    template <typename T, std::size_t N>
    struct Array {
        // ----- MEMBER TYPES ----- //
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type &;
        using const_reference = const value_type &;
        using pointer = value_type *;
        using const_pointer = const value_type *;
        // Usings for iterators come later.

        // ----- MEMBER FUNCTIONS ----- //

        // ----- ELEMENT ACCESS
        constexpr reference at(size_type pos) {
            if (pos >= size()) {
                throw std::out_of_range("out of range"); // TODO
            }
            return _array[pos];
        }

        constexpr const_reference at(size_type pos) const {
            if (pos >= size()) {
                throw std::out_of_range("out of range"); // TODO
            }
            return _array[pos];
        }

        constexpr reference operator[](size_type pos) {
            return _array[pos];
        }

        constexpr const_reference operator[](size_type pos) const {
            return _array[pos];
        }

        constexpr reference front() {
            return _array[0];
        }

        constexpr const_reference front() const {
            return _array[0];
        }

        constexpr reference back() {
            return _array[N - 1];
        }

        constexpr const_reference back() const {
            return _array[N - 1];
        }

        constexpr pointer data() noexcept {
            return &_array;
        }

        constexpr const_pointer data() const noexcept {
            return &_array;
        }

        // ----- CAPACITY

        constexpr bool empty() const noexcept {
            return N == 0;
        }

        constexpr size_type size() const noexcept {
            return N;
        }

        constexpr size_type max_size() const noexcept {
            return N;
        }

        // ----- OPERATIONS

        constexpr void fill(const T &value) {
            for (size_type i = 0; i < N; i++) {
                _array[i] = value;
            }
        }

        constexpr void swap(Array &other) noexcept(
#if __cplusplus < 201703L
            noexcept(swap(std::declval<T &>(), std::declval<T &>()))
#else
            std::is_nothrow_swappable_v<T>
#endif
        ) {
            // TODO
        }

       public:
        T _array[N];
    };

    // ----- NON-MEMBER FUNCTIONS ----- //

    // ----- BOOLEAN COMPARISON OPERATORS

    template <typename T, std::size_t N>
    constexpr bool operator==(const Array<T, N> &lhs, const Array<T, N> &rhs) {
        for (typename decltype(lhs)::size_type i = 0; i < N; i++) {
            if (lhs[i] != rhs[i]) {
                return false;
            }
        }
        return true;
    }

    template <typename T, std::size_t N>
    constexpr bool operator!=(const Array<T, N> &lhs, const Array<T, N> &rhs) {
        return !(lhs == rhs);
    }

    template <typename T, std::size_t N>
    constexpr bool operator<(const Array<T, N> &lhs, const Array<T, N> &rhs) {
        for (typename decltype(lhs)::size_type i = 0; i < N; i++) {
            if (lhs[i] >= rhs[i]) {
                return false;
            }
        }
        return true;
    }

    template <typename T, std::size_t N>
    constexpr bool operator<=(const Array<T, N> &lhs, const Array<T, N> &rhs) {
        for (typename decltype(lhs)::size_type i = 0; i < N; i++) {
            if (lhs[i] > rhs[i]) {
                return false;
            }
        }
        return true;
    }

    template <typename T, std::size_t N>
    constexpr bool operator>(const Array<T, N> &lhs, const Array<T, N> &rhs) {
        return !(lhs <= rhs);
    }

    template <typename T, std::size_t N>
    constexpr bool operator>=(const Array<T, N> &lhs, const Array<T, N> &rhs) {
        return !(lhs < rhs);
    }

    template <std::size_t I, typename T, std::size_t N>
    constexpr T &get(Array<T, N> &ar) {
        static_assert(I < N, "I must be inferior to N.");
        return ar[I];
    }

    template <std::size_t I, typename T, std::size_t N>
    constexpr const T &get(const Array<T, N> &ar) {
        static_assert(I < N, "I must be inferior to N.");
        return ar[I];
    }

    // TODO T&& overloads of get

    template <typename T, std::size_t N>
    constexpr void swap(Array<T, N> &lhs, Array<T, N> &rhs)
#if __cplusplus >= 201703L
        noexcept(noexcept(lhs.swap(rhs)))
#endif
    {
        lhs.swap(lhs);
    }

    // --- Code from https://en.cppreference.com/w/cpp/container/array/to_array
    namespace detail
    {
        template <class T, std::size_t N, std::size_t... I>
        constexpr std::array<std::remove_cv_t<T>, N> to_array_impl(
            T (&a)[N], std::index_sequence<I...>) {
            return {{a[I]...}};
        }

        template <class T, std::size_t N, std::size_t... I>
        constexpr std::array<std::remove_cv_t<T>, N> to_array_impl(
            T(&&a)[N], std::index_sequence<I...>) {
            return {{std::move(a[I])...}};
        }
    } // namespace detail

    template <class T, std::size_t N>
    constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&a)[N]) {
        return detail::to_array_impl(a, std::make_index_sequence<N>{});
    }

    template <class T, std::size_t N>
    constexpr std::array<std::remove_cv_t<T>, N> to_array(T(&&a)[N]) {
        return detail::to_array_impl(std::move(a), std::make_index_sequence<N>{});
    }

    // --- End of code from https://en.cppreference.com/w/cpp/container/array/to_array

} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_ARRAY_HPP
