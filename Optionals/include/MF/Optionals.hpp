//
// Created by MartinF on 25/06/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_OPTIONALS_HPP
#define MFRANCESCHI_CPPLIBRARIES_OPTIONALS_HPP

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
         * Wrapper for a shared pointer that can contain a value, or not.
         * API is similar to Java's Optional, with some changes.
         *
         * An optional contains a value, or does not contain a value (in which case it is empty).
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
                } else {
                    return other.isEmpty();
                }
            }

            virtual bool contains(const T& other) const {
                return isPresent() ? _getRValueUnchecked() == other : false;
            }

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

            // friend OptionalPtr<T> ofRvalue<T>(const T& rvalue);

            // Shared_ptr is const to ensure the immutability of the Optional class.
            const T& value;
        };

        /// Always contains a value, COPIED from the source.
        template <typename T>
        class OptionalFromLvalue : public Optional<T> {
            static_assert(std::is_copy_constructible<T>::value, "Type must be copy constructible.");

           public:
            static OptionalPtr<T> ofLvalue(const T& lvalue) {
                // return std::make_shared<OptionalFromLvalue<T>>(lvalue);
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

            // friend OptionalPtr<T> ofLvalue(const T& lvalue);

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

            // friend OptionalPtr<T> ofSharedPointer<T>(const std::shared_ptr<T>& pointer);

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

            OptionalPtr<T> filter(const std::function<bool(const T&)>&) override {
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

            bool contains(const T&) const override {
                return false;
            }

           protected:
            const T& _getRValueUnchecked() const {
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
    } // namespace Optionals
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_OPTIONALS_HPP
