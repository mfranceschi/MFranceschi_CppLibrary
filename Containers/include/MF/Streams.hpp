//
// Created by Utilisateur on 25/03/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_STREAMS_HPP
#define MFRANCESCHI_CPPLIBRARIES_STREAMS_HPP

#include <functional>
#include <memory>
#include <vector>

namespace MF
{
    namespace Containers
    {
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

#endif // MFRANCESCHI_CPPLIBRARIES_STREAMS_HPP
