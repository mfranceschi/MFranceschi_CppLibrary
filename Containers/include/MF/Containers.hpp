//
// Created by MartinF on 05/07/2023.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_CONTAINERS_HPP
#define MFRANCESCHI_CPPLIBRARIES_CONTAINERS_HPP

#include <array>
#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>

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
                T (&&a)[N], std::index_sequence<I...>) {
                return {{std::move(a[I])...}};
            }
        } // namespace detail

        template <class T, std::size_t N>
        constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&a)[N]) {
            return detail::to_array_impl(a, std::make_index_sequence<N>{});
        }

        template <class T, std::size_t N>
        constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&&a)[N]) {
            return detail::to_array_impl(std::move(a), std::make_index_sequence<N>{});
        }

        // --- End of code from https://en.cppreference.com/w/cpp/container/array/to_array

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

        // ----- EARLY DECLARATIONS ----- //
        template <typename T>
        struct Stream;

        template <typename T>
        using StreamPtr = std::shared_ptr<Stream<T>>;

        namespace Streams
        {
            template <typename T>
            using ElementProcessor_t = std::function<void(const T &)>;

            template <typename T>
            using Predicate_t = std::function<bool(const T &)>;

            template <typename Source, typename Target>
            using Mapper_t = std::function<Target(const Source &)>;

            template <typename T>
            struct StreamFromFilter;

            template <typename T>
            struct StreamImpl;

            template <typename T>
            struct StreamFromFilter;

            template <typename Target, typename Source>
            struct StreamFromMap;
        } // namespace Streams

        template <typename T>
        struct Stream {
           public:
            virtual StreamPtr<T> filter(const Streams::Predicate_t<T> &predicate) = 0;

            template <typename Target>
            inline StreamPtr<Target> map(const Streams::Mapper_t<T, Target> &mapper);

            virtual std::vector<T> collectToVector() = 0;

            virtual std::size_t size() = 0;

            virtual void browse(const Streams::ElementProcessor_t<T> &elementProcessor) = 0;

            // virtual ~Stream() = default;
        };

        template <typename T>
        template <typename Target>
        StreamPtr<Target> Stream<T>::map(const Streams::Mapper_t<T, Target> &mapper) {
            return std::make_shared<Streams::StreamFromMap<Target, T>>(this, mapper);
        }

        namespace Streams
        {
            template <typename T>
            struct StreamImpl : public Stream<T> {
                std::size_t size() override {
                    std::size_t result = 0;
                    this->browse([&result](const T &) {
                        result++;
                    });
                    return result;
                }

                StreamPtr<T> filter(const Streams::Predicate_t<T> &predicate) override {
                    return std::make_shared<StreamFromFilter<T>>(this, predicate);
                }

                std::vector<T> collectToVector() override {
                    std::vector<T> result{};
                    this->browse([&result](const T &element) {
                        result.push_back(element);
                    });
                    return result;
                }
            };

            template <typename T>
            struct StreamFromFilter : public StreamImpl<T> {
               protected:
               public:
                StreamFromFilter(Stream<T> *source, const Streams::Predicate_t<T> &predicate)
                    : previous(source), predicate(predicate) {
                }

                void browse(const Streams::ElementProcessor_t<T> &elementProcessor) override {
                    const auto &_pred = this->predicate;
                    const auto &_proc = elementProcessor;

                    Streams::ElementProcessor_t<T> nestedElementProcessor =
                        [&_pred, &_proc](const T &element) {
                            if (_pred(element)) {
                                _proc(element);
                            }
                        };
                    previous->browse(nestedElementProcessor);
                }

               private:
                Stream<T> *const previous;
                const Streams::Predicate_t<T> predicate;
            };

            template <typename Target, typename Source>
            struct StreamFromMap : public StreamImpl<Target> {
               protected:
                using _Mapper_t = Streams::Mapper_t<Source, Target>;

               public:
                StreamFromMap(Stream<Source> *source, const _Mapper_t &mapper)
                    : previous(source), mapper(mapper) {
                }

                void browse(const Streams::ElementProcessor_t<Target> &elementProcessor) override {
                    const auto &_mapper = this->mapper;
                    const auto &_proc = elementProcessor;

                    Streams::ElementProcessor_t<Target> nestedElementProcessor =
                        [&_mapper, &_proc](const Target &element) {
                            _proc(_mapper(element));
                        };
                    previous->browse(nestedElementProcessor);
                }

               private:
                Stream<Source> *const previous;
                const _Mapper_t mapper;
            };

            template <typename T, typename Collection_t>
            struct StreamFromCollection : public StreamImpl<T> {
               public:
                StreamFromCollection(const Collection_t &collection) : collection(collection) {
                }

                std::vector<T> collectToVector() override {
                    return std::vector<T>(collection);
                }

                std::size_t size() override {
                    return collection.size();
                }

                void browse(const Streams::ElementProcessor_t<T> &elementProcessor) override {
                    for (const auto &element : collection) {
                        elementProcessor(element);
                    }
                }

               private:
                const Collection_t &collection;
            };

            template <typename T>
            StreamPtr<T> fromCollection(const std::vector<T> &source) {
                return std::make_shared<StreamFromCollection<T, std::vector<T>>>(source);
            }
        } // namespace Streams
    } // namespace Containers
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_CONTAINERS_HPP
