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

#include "Glass/Properties/Private/getName.h"

namespace Glass {
	//! Use PropertyType to define a named property for serialization
	//!
	//! \param T type backing the property
	template <typename T, typename = std::void_t<>> struct PropertyType { using type = T; };

	//! When T is a BETTER_ENUM, name,serialize, and deserialize will be automatically generated
	template <typename T>
	struct PropertyType<
	    T, std::void_t<decltype(T::_name(), std::declval<T>()._to_string(),
	                            T::_from_string_nocase_nothrow(std::declval<const char*>()))>> {
		using type = T;
		static constexpr auto name = T::_name();
		static std::string serialize(const T& value) { return value._to_string(); }
		static std::optional<T> deserialize(const std::string& serializedValue) {
			const auto maybeValue = T::_from_string_nocase_nothrow(serializedValue.c_str());
			if (!maybeValue) {
				return std::nullopt;
			}
			return *maybeValue;
		}
	};


	template <typename T> struct OptionalProperty : T {
		using type = std::optional<typename T::type>;
		static constexpr auto std::nulloptString = "std::nullopt";
		static std::string name() { return std::string{"Optional: "} + Private::getName<T>(); }
		static std::string serialize(const type& value) {
			if (!value) {
				return std::nulloptString;
			}
			return T::serialize(*value);
		}
		static type deserialize(const std::string& serializedValue) {
			if (serializedValue == std::string{std::nulloptString}) {
				return std::nullopt;
			}
			return T::deserialize(serializedValue);
		}
	};


	struct IntPropertyType : PropertyType<int32_t> {
		static constexpr auto name = "Int";
		static std::string serialize(int32_t value);
		static std::optional<int32_t> deserialize(const std::string& serializedValue);
	};

	struct FloatPropertyType : PropertyType<float> {
		static constexpr auto name = "Float";
	};

	struct BoolPropertyType : PropertyType<bool> {
		static constexpr auto name = "Bool";
	};

	struct StringPropertyType : PropertyType<std::string> {
		static constexpr auto name = "String";
	};

	template <typename T>
	struct EnumPropertyType : PropertyType<T> {
		static constexpr auto name = "Enum";
	};
}
