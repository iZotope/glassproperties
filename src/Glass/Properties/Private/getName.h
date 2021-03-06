// Copyright 2017-2021 iZotope, Inc.
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


#pragma once

#include <type_traits>

namespace Glass::Private {

	template <typename T, typename = void> constexpr inline bool HasName = false;

	template <typename T> constexpr inline bool HasName<T, std::void_t<decltype(T::name)>> = true;


	template <typename T> constexpr auto getName() {
		static_assert(HasName<T>, "T must have a `name` static member variable or function.");
		if constexpr (std::is_function_v<decltype(T::name)>) {
			return T::name();
		} else {
			return T::name;
		}
	}
}
