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

#include "Decorator.hpp"
#include "../vendor/Debug/Logger.hpp"
#include <boost/beast/http.hpp>

namespace lightlib::mixins {

	template<typename Derived>
	class CORSDecorator : public Decorator<Derived> {
	public:
		
		void before() {
			Logger::log("before() checking cors", "CORSDecorator");
			auto* derived = static_cast<Derived*>(this);
			auto& req = derived->getRequest();

			auto origin = req[http::field::origin];
			if (!origin.empty()) {
				Logger::log("Origin: " + origin.to_string(), "CORSDecorator");
			}
		}

		void after() {
			Logger::log("Adding CORS headers", "CORSDecorator");
			auto* derived = static_cast<Derived*>(this);
			auto& res = derived->getResponse();

			res.set(http::field::access_control_allow_origin, "*");
			res.set(http::field::access_control_allow_methods, "GET, POST, PUT, DELETE");
			res.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
		}
	};
}