// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <xlog/internal/check_op.h>

#include <cstring>
#include <sstream>
#include <string>

#ifdef _MSC_VER
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

namespace xlog {
namespace log_internal {

    CheckOpMessageBuilder::CheckOpMessageBuilder(const char *exprtext) {
        stream_ << exprtext << " (";
    }

    std::ostream &CheckOpMessageBuilder::ForVar2() {
        stream_ << " vs. ";
        return stream_;
    }

    std::string *CheckOpMessageBuilder::NewString() {
        stream_ << ")";
        return new std::string(stream_.str());
    }

    void MakeCheckOpValueString(std::ostream &os, char v) {
        if (v >= 32 && v <= 126) {
            os << "'" << v << "'";
        } else {
            os << "char value " << int{v};
        }
    }

    void MakeCheckOpValueString(std::ostream &os, signed char v) {
        if (v >= 32 && v <= 126) {
            os << "'" << v << "'";
        } else {
            os << "signed char value " << int{v};
        }
    }

    void MakeCheckOpValueString(std::ostream &os, unsigned char v) {
        if (v >= 32 && v <= 126) {
            os << "'" << v << "'";
        } else {
            os << "unsigned char value " << int{v};
        }
    }

    void MakeCheckOpValueString(std::ostream &os, const void *p) {
        if (p == nullptr) {
            os << "(null)";
        } else {
            os << p;
        }
    }

    void MakeCheckOpValueString(std::ostream &os, const char *p) {
        if (p == nullptr) {
            os << "(null)";
        } else {
            os << p;
        }
    }

    void MakeCheckOpValueString(std::ostream &os, const signed char *p) {
        MakeCheckOpValueString(os, reinterpret_cast<const char *>(p));
    }

    void MakeCheckOpValueString(std::ostream &os, const unsigned char *p) {
        MakeCheckOpValueString(os, reinterpret_cast<const char *>(p));
    }

    void MakeCheckOpValueString(std::ostream &os, std::nullptr_t) {
        os << "(null)";
    }

#define XLOG_DEFINE_CHECK_STROP_IMPL(func, expected)                         \
    std::string *Check##func##expected##Impl(const char *s1, const char *s2, \
                                             const char *exprtext) {         \
        const bool equal = s1 == s2 || (s1 && s2 && !func(s1, s2));          \
        if (equal == expected) {                                             \
            return nullptr;                                                  \
        }                                                                    \
        std::ostringstream oss;                                              \
        oss << exprtext << " (" << (s1 ? s1 : "(null)") << " vs. "           \
            << (s2 ? s2 : "(null)") << ")";                                  \
        return new std::string(oss.str());                                   \
    }

    XLOG_DEFINE_CHECK_STROP_IMPL(strcmp, true)
    XLOG_DEFINE_CHECK_STROP_IMPL(strcmp, false)
    XLOG_DEFINE_CHECK_STROP_IMPL(strcasecmp, true)
    XLOG_DEFINE_CHECK_STROP_IMPL(strcasecmp, false)

#undef XLOG_DEFINE_CHECK_STROP_IMPL

}  // namespace log_internal
}  // namespace xlog
