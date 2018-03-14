/*
 * Copyright 2011-2016 Universiteit Antwerpen
 *
 * Licensed under the EUPL, Version 1.1 or  as soon they will be approved by
 * the European Commission - subsequent versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at: http://ec.europa.eu/idabc/eupl5
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the Licence is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 *
 * Source of code is taken from https://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
 */

#include "Safeline.h"
namespace stride {
    namespace util {
        std::istream &safeGetline(std::istream &is, std::string &t) {
            t.clear();

            // The characters in the stream are read one-by-one using a std::streambuf.
            // That is faster than reading them one-by-one using the std::istream.
            // Code that uses streambuf this way must be guarded by a sentry object.
            // The sentry object performs various tasks,
            // such as thread synchronization and updating the stream state.

            std::istream::sentry se(is, true);
            std::streambuf *sb = is.rdbuf();

            for (;;) {
                int c = sb->sbumpc();
                switch (c) {
                    case '\n':
                        return is;
                    case '\r':
                        if (sb->sgetc() == '\n')
                            sb->sbumpc();
                        return is;
                    case std::streambuf::traits_type::eof():
                        // Also handle the case when the last line has no line ending
                        if (t.empty())
                            is.setstate(std::ios::eofbit);
                        return is;
                    default:
                        t += (char) c;
                }
            }
        }
    }
}