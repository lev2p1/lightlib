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

#include <functional>

namespace lightlib::mixins {

	template<typename Derived>
    class Decorator {
    protected:

        virtual void decorate() {}

    public:

        template<typename Func>
        auto wrap(Func&& func) {
            return [this, func = std::forward<Func>(func)](auto&&... args) {
                static_cast<Derived*>(this)->before();
                auto result = func(std::forward<decltype(args)>(args)...);
                static_cast<Derived*>(this)->after();

                return result;
                };
        }

        virtual void before() {}
        virtual void after() {}

        virtual ~Decorator() = default;
    };
}