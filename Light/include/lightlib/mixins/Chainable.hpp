/*
 * Copyright (c) 2026 Kirill Sergeev, Nikolay Sugonyako, Andrey Agarkov, Gleb Safyannikov
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * This file is part of lightlib.
 *
 * lightlib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * lightlib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lightlib; if not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <memory>
#include <boost/beast/http.hpp>

namespace lightlib::mixins {
    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;

    template<typename Derived>
    class Chainable {
    private:
        std::unique_ptr<Derived> next;

    public:
        Derived* setNext(std::unique_ptr<Derived> handler) {
            next = std::move(handler);
            return next.get();
        }

        Derived* append(std::unique_ptr<Derived> handler) {
            if (!next) {
                return setNext(std::move(handler));
            }

            auto* current = next.get();
            while (current->getNext()) {
                current = current->getNext();
            }
            return current->setNext(std::move(handler));
        }

        Derived* getNext() const {
            return next.get();
        }

        bool hasNext() const {
            return next != nullptr;
        }

    protected:
        template<typename Req, typename Res>
        bool passToNext(Req& req, Res& res) {
            if (next) {
                return next->handle(req, res);
            }

            return true;
        }

        template<typename Req, typename Res>
        bool passToNextIfExists(Req& req, Res& res) {
            if (hasNext()) {
                return next->handle(req, res);
            }
            return true;
        }

        template<typename Req, typename Res>
        bool passToNextWithCheck(Req& req, Res& res) {
            if (!hasNext()) {
                return true;
            }

            bool result = next->handle(req, res);

            return result;
        }

        template<typename Req, typename Res, typename Func>
        bool processAndPass(Req& req, Res& res, Func&& handler) {
            bool result = handler();

            if (!result) {
                return false;
            }

            if (hasNext()) {
                return next->handle(req, res);
            }

            return true;
        }

        virtual bool handle(Request& req, Response& res) = 0;
    };
}