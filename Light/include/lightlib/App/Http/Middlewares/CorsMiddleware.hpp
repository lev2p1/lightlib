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
#include "Middleware.hpp"
#include "../../../mixins/CorsDecorator.hpp"
#include "../../../mixins/Chainable.hpp"

namespace lightlib {

    class CorsMiddleware : public Middleware,
        public mixins::CORSDecorator<CorsMiddleware>,
        public mixins::Chainable<CorsMiddleware> {

        Request req;
        Response res;

    public:
        bool handle(Request& request, Response& response) override {
            req = std::move(request);
            res = Response{ http::status::ok, req.version() };

            auto wrapped = this->wrap([&]() -> bool {
                Logger::log("CORS: Processing request", "CORSMiddleware");

                return this->processAndPass(req, res, [&]() -> bool {
                    if (!this->hasNext()) {
                        res.body() = "OK";
                        res.prepare_payload();
                    }
                    return true;
                    });
                });

            bool result = wrapped();

            response = std::move(res);
            request = std::move(req);

            return result;
        }

        Request& getRequest() { return req; }
        Response& getResponse() { return res; }
    };
}