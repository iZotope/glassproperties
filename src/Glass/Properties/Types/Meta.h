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

namespace Glass {
	template <typename T, typename = std::void_t<>> struct IsPropertyType : std::false_type {};

	template <typename T>
	struct IsPropertyType<T, std::void_t<decltype(T::type, T::name, T::serialize, T::deserialize)>>
	    : std::true_type {};

	template <typename T> constexpr bool IsPropertyType_v = IsPropertyType<T>::value;

	template <typename T, typename = std::void_t<>>
	struct IsBetterEnumProperty : std::false_type {};

	template <typename T>
	struct IsBetterEnumProperty<
	    T,
	    std::void_t<decltype(T::_name(),
	                         std::declval<T>()._to_string(),
	                         T::_from_string_nocase_nothrow(std::declval<const char*>()))>>
	    : std::true_type {};

	template <typename T> constexpr bool IsBetterEnumProperty_v = IsBetterEnumProperty<T>::value;

	template <typename T>
	constexpr bool IsLegacyPropertyType_v = !IsPropertyType_v<T> && !IsBetterEnumProperty_v<T>;
}
