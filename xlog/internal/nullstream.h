#pragma once

#include <cstdlib>

namespace xlog {
namespace internal {

class NullStream {
public:
    NullStream &internal_stream() { return *this; }
    NullStream &at_location(const char *, int) { return *this; }
    NullStream &no_prefix() { return *this; }
    NullStream &no_newline() { return *this; }
    NullStream &with_verbosity(int) { return *this; }
    NullStream &with_timestamp(void *) { return *this; }
    NullStream &with_perror() { return *this; }
    template <typename T>
    NullStream &to_sink_also(T *) { return *this; }
    template <typename T>
    NullStream &to_sink_only(T *) { return *this; }
    template <typename T>
    NullStream &operator<<(const T &) { return *this; }
    NullStream &operator<<(const char *) { return *this; }
    NullStream &operator<<(char) { return *this; }
};

class NullStreamFatal {
public:
    NullStreamFatal() = default;
    NullStreamFatal(const NullStreamFatal &) = default;
    ~NullStreamFatal() { std::_Exit(1); }
    NullStreamFatal &internal_stream() { return *this; }
    template <typename T>
    NullStreamFatal &operator<<(const T &) { return *this; }
    NullStreamFatal &operator<<(const char *) { return *this; }
    NullStreamFatal &operator<<(char) { return *this; }
};

class NullStreamMaybeFatal {
public:
    explicit NullStreamMaybeFatal(bool fatal) : fatal_(fatal) {}
    NullStreamMaybeFatal(const NullStreamMaybeFatal &) = default;
    ~NullStreamMaybeFatal() {
        if (fatal_) std::_Exit(1);
    }
    NullStreamMaybeFatal &internal_stream() { return *this; }
    template <typename T>
    NullStreamMaybeFatal &operator<<(const T &) { return *this; }
    NullStreamMaybeFatal &operator<<(const char *) { return *this; }
    NullStreamMaybeFatal &operator<<(char) { return *this; }

private:
    bool fatal_;
};

} // namespace internal
} // namespace xlog
