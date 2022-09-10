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
        class EmptyOptionalException : public std::runtime_error {
           public:
            EmptyOptionalException() : std::runtime_error("No value present in Optional.") {
            }
        };
        template <typename T>
        class Optional;

        template <typename T>
        Optional<T> empty();

        template <typename T>
        Optional<T> of(const T& value);

        template <typename T>
        Optional<T> of(const std::shared_ptr<T>& pointer);

        template <typename T>
        class Optional {
           private:
            const std::shared_ptr<T> value;

            explicit Optional(const std::shared_ptr<T>& ptr = nullptr) : value(ptr) {
            }

            friend Optional<T> empty<T>();

            friend Optional<T> of<T>(const T& value);

            friend Optional<T> of<T>(const std::shared_ptr<T>& pointer);

           public:
            const T& get() const {
                return getOrThrow();
            }

            bool isPresent() const {
                return bool(value);
            }
            bool isEmpty() const {
                return !isPresent();
            }

            void ifPresent(const std::function<void(const T&)>& consumer) const {
                if (isPresent()) {
                    consumer(*value);
                }
            }

            void ifPresentOrElse(
                const std::function<void(const T&)>& consumer,
                const std::function<void(void)>& emptyAction) const {
                if (isPresent()) {
                    consumer(*value);
                } else {
                    emptyAction();
                }
            }

            Optional<T> filter(const std::function<bool(const T&)>& predicate) const {
                if (isPresent()) {
                    if (!predicate(*value)) {
                        return Optional<T>();
                    }
                }
                return *this;
            }

            template <typename Destination>
            Optional<Destination> map(const std::function<Destination(const T&)>& mapper) const {
                return isPresent() ? of<Destination>(mapper(*value)) : empty<Destination>();
            }

            template <typename Destination>
            Optional<Destination> flatMap(
                const std::function<Optional<Destination>(const T&)>& mapper) const {
                return isPresent() ? mapper(*value) : empty<Destination>();
            }

            Optional<T> useThisOrRun(const std::function<Optional<T>(void)> supplier) const {
                return isPresent() ? *this : supplier();
            }

            const T& getOrDefault(const T& other) const {
                return isPresent() ? *value : other;
            }

            const T& getOrRun(const std::function<T(void)> supplier) {
                return isPresent() ? *value : supplier();
            }

            const T& getOrThrow() const {
                if (value) {
                    return *value;
                }

                throw EmptyOptionalException();
            }

            template <typename ExceptionToThrow>
            const T& getOrThrow(const std::function<ExceptionToThrow(void)> supplier) const {
                if (isPresent()) {
                    return *value;
                }

                throw supplier();
            }

            const std::shared_ptr<T>& getSharedPtr() const {
                return value;
            }

            bool operator==(const Optional<T>& other) const {
                return this->value == other.value;
            }

            bool contains(const T& other) const {
                return isPresent() ? *value == other : false;
            }
        };

        template <typename T>
        Optional<T> empty() {
            return Optional<T>();
        }

        template <typename T>
        Optional<T> of(const T& value) {
            return Optional<T>(std::make_shared<T>(value));
        }

        template <typename T>
        Optional<T> of(const std::shared_ptr<T>& pointer) {
            return Optional<T>(pointer);
        }
    } // namespace Optionals
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_OPTIONALS_HPP
