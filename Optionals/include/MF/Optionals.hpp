//
// Created by MartinF on 25/06/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_OPTIONALS_HPP
#define MFRANCESCHI_CPPLIBRARIES_OPTIONALS_HPP

#include <bitset>
#include <functional>
#include <memory>
#include <stdexcept>

namespace MF
{
    namespace Optionals
    {
        /**
         * Simple exception thrown when trying to access the value of an empty optional.
         */
        class EmptyOptionalException : public std::runtime_error {
           public:
            EmptyOptionalException() : std::runtime_error("No value present in Optional.") {
            }
        };

        template <typename T>
        class Optional;

        template <typename T>
        using OptionalPtr = std::shared_ptr<Optional<T>>;

        template <typename T>
        OptionalPtr<T> empty();

        template <typename T>
        OptionalPtr<T> ofRvalue(const T& rvalue);

        template <typename T>
        OptionalPtr<T> ofLvalue(const T& lvalue);

        template <typename T>
        OptionalPtr<T> ofSharedPointer(const std::shared_ptr<T>& pointer);

        /**
         * An Optional can contain a value, or be empty.
         * The API is inspired from Java's Optional, with some changes.
         *
         * You can build an optional with the functions "empty()" and "of(...)".
         */
        template <typename T>
        class Optional : public std::enable_shared_from_this<Optional<T>> {
           public:
            virtual const T& get() const {
                return getOrThrow();
            }

            virtual bool isPresent() const = 0;

            virtual bool isEmpty() const {
                return !isPresent();
            }

            virtual operator bool() const {
                return isPresent();
            }

            virtual OptionalPtr<T> filter(const std::function<bool(const T&)>& predicate) {
                if (isEmpty()) {
                    return empty<T>();
                }

                return predicate(_getRValueUnchecked()) ? this->shared_from_this() : empty<T>();
            }

            template <typename Destination>
            OptionalPtr<Destination> map(const std::function<Destination(const T&)>& mapper) const {
                return isPresent() ? ofLvalue<Destination>(mapper(_getRValueUnchecked()))
                                   : empty<Destination>();
            }

            template <typename Destination>
            OptionalPtr<Destination> flatMap(
                const std::function<OptionalPtr<Destination>(const T&)>& mapper) const {
                return isPresent() ? mapper(_getRValueUnchecked()) : empty<Destination>();
            }

            virtual OptionalPtr<T> useThisOrRun(
                const std::function<OptionalPtr<T>(void)> supplier) {
                return isPresent() ? this->shared_from_this() : supplier();
            }

            virtual const T& getOrDefault(const T& other) const {
                return isPresent() ? _getRValueUnchecked() : other;
            }

            virtual const T& getOrRun(const std::function<const T&(void)> supplier) {
                return isPresent() ? _getRValueUnchecked() : supplier();
            }

            virtual T getOrRunWithCopy(const std::function<T(void)> supplier) {
                return isPresent() ? _getRValueUnchecked() : supplier();
            }

            virtual const T& getOrThrow() const {
                if (isPresent()) {
                    return _getRValueUnchecked();
                }

                throw EmptyOptionalException();
            }

            template <typename ExceptionToThrow>
            const T& getOrThrow(const std::function<ExceptionToThrow(void)> supplier) const {
                if (isPresent()) {
                    return _getRValueUnchecked();
                }

                throw supplier();
            }

            virtual bool operator==(const Optional<T>& other) const {
                if (isPresent()) {
                    return other.contains(_getRValueUnchecked());
                }

                return other.isEmpty();
            }

            virtual bool contains(const T& other) const {
                return isPresent() ? _getRValueUnchecked() == other : false;
            }

            virtual ~Optional() = default;

           protected:
            /// Returns the contained RValue. Undefined behaviour if isEmpty.
            virtual const T& _getRValueUnchecked() const = 0;

           private:
            // Unique empty optional for a type T.
            // Having it unique is - hopefully - a small optimization.
            static const OptionalPtr<T> EMPTY;

            friend OptionalPtr<T> empty<T>();
        };

        /// Always contains a value, NOT COPIED from the source.
        template <typename T>
        class OptionalFromRvalue : public Optional<T> {
           public:
            static OptionalPtr<T> ofRvalue(const T& rvalue) {
                return OptionalPtr<T>(new OptionalFromRvalue<T>(rvalue));
            }

            bool isPresent() const override {
                return true;
            }

           protected:
            const T& _getRValueUnchecked() const override {
                return value;
            };

           private:
            OptionalFromRvalue(const T& theValue) : value(theValue) {
            }

            const T& value;
        };

        /// Always contains a value, COPIED from the source.
        template <typename T>
        class OptionalFromLvalue : public Optional<T> {
            static_assert(std::is_copy_constructible<T>::value, "Type must be copy constructible.");

           public:
            static OptionalPtr<T> ofLvalue(const T& lvalue) {
                return OptionalPtr<T>(new OptionalFromLvalue<T>(lvalue));
            }

            bool isPresent() const override {
                return true;
            }

           protected:
            const T& _getRValueUnchecked() const override {
                return value;
            };

           private:
            OptionalFromLvalue(const T& theValue) : value(theValue) {
            }

            const T value;
        };

        template <typename T>
        class OptionalFromSharedPtr : public Optional<T> {
           public:
            static OptionalPtr<T> ofSharedPointer(const std::shared_ptr<T>& pointer) {
                return OptionalPtr<T>(new OptionalFromSharedPtr<T>(pointer));
            }

            bool isPresent() const override {
                return bool(value);
            }

            const std::shared_ptr<T>& getSharedPtr() const {
                return value;
            }

           protected:
            const T& _getRValueUnchecked() const override {
                return *value;
            };

           private:
            OptionalFromSharedPtr(const std::shared_ptr<T> theValue) : value(theValue) {
            }

            // Shared_ptr is const to ensure the immutability of the Optional class.
            const std::shared_ptr<T> value;
        };

        /// Optimized: always empty.
        template <typename T>
        class OptionalEmpty : public Optional<T> {
           public:
            const T& get() const override {
                throw EmptyOptionalException();
            }

            bool isPresent() const override {
                return false;
            }

            bool isEmpty() const override {
                return true;
            }

            operator bool() const override {
                return false;
            }

            OptionalPtr<T> filter(const std::function<bool(const T&)>& unused) override {
                (void)unused;
                return this->shared_from_this();
            }

            OptionalPtr<T> useThisOrRun(
                const std::function<OptionalPtr<T>(void)> supplier) override {
                return supplier();
            }

            const T& getOrDefault(const T& other) const override {
                return other;
            }

            const T& getOrRun(const std::function<const T&(void)> supplier) override {
                return supplier();
            }

            T getOrRunWithCopy(const std::function<T(void)> supplier) override {
                return supplier();
            }

            const T& getOrThrow() const override {
                throw EmptyOptionalException();
            }

            bool operator==(const Optional<T>& other) const override {
                return this->isEmpty() == other.isEmpty();
            }

            bool contains(const T& unused) const override {
                (void)unused;
                return false;
            }

            ~OptionalEmpty() = default;

           protected:
            const T& _getRValueUnchecked() const override {
                throw EmptyOptionalException();
            };
        };

        template <typename T>
        OptionalPtr<T> empty() {
            return Optional<T>::EMPTY;
        }

        template <typename T>
        OptionalPtr<T> ofRvalue(const T& rvalue) {
            return OptionalFromRvalue<T>::ofRvalue(rvalue);
        }

        template <typename T>
        OptionalPtr<T> ofLvalue(const T& lvalue) {
            return OptionalFromLvalue<T>::ofLvalue(lvalue);
        }

        template <typename T>
        OptionalPtr<T> ofSharedPointer(const std::shared_ptr<T>& pointer) {
            return OptionalFromSharedPtr<T>::ofSharedPointer(pointer);
        }

        template <typename T>
        const OptionalPtr<T> Optional<T>::EMPTY = std::make_shared<OptionalEmpty<T>>();

        /**
         * Optimized Optional class for booleans.
         * Uses only 2 bits in theory!
         */
        class OptionalBool {
           private:
            const std::bitset<2> theBitset;

            constexpr bool isSet() const {
                return theBitset[0];
            }

            constexpr bool isTrue() const {
                return theBitset[1];
            }

            constexpr OptionalBool(bool value) : theBitset(1 + (static_cast<int>(value) << 1)) {
            }

            constexpr OptionalBool() : theBitset() {
            }

           public:
            static constexpr OptionalBool of(bool value) {
                return OptionalBool(value);
            }

            static constexpr OptionalBool empty() {
                return OptionalBool();
            }

            constexpr bool get() const {
                return getOrThrow();
            }

            constexpr bool isPresent() const {
                return isSet();
            }

            constexpr bool isEmpty() const {
                return !isPresent();
            }

            constexpr operator bool() const {
                return isPresent();
            }

            constexpr OptionalBool filter(const std::function<bool(bool)>& predicate) {
                if (isEmpty()) {
                    return empty();
                }

                return predicate(isTrue()) ? *this : empty();
            }

            OptionalBool useThisOrRun(const std::function<OptionalBool(void)>& supplier) {
                return isPresent() ? *this : supplier();
            }

            constexpr bool getOrDefault(bool other) const {
                return isPresent() ? isTrue() : other;
            }

            bool getOrRun(const std::function<bool(void)>& supplier) {
                return isPresent() ? isTrue() : supplier();
            }

            constexpr bool getOrThrow() const {
                if (!isSet()) {
                    throw EmptyOptionalException();
                }
                return isTrue();
            }

            template <typename ExceptionToThrow>
            constexpr bool getOrThrow(const std::function<ExceptionToThrow(void)> supplier) const {
                if (isPresent()) {
                    return isTrue();
                }

                throw supplier();
            }

            constexpr bool operator==(const OptionalBool& other) const {
                if (isPresent()) {
                    return other.contains(isTrue());
                }

                return other.isEmpty();
            }

            constexpr bool contains(bool other) const {
                return isPresent() ? isTrue() == other : false;
            }

            OptionalPtr<bool> toOptionalPtr() const {
                return isSet() ? Optionals::ofLvalue(isTrue()) : Optionals::empty<bool>();
            }

            ~OptionalBool() = default;
        };
    } // namespace Optionals
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_OPTIONALS_HPP
