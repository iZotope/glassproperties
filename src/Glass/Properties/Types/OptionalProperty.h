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

#include "Glass/Properties/Private/has_type.h"
#include "Glass/Properties/Types/PropertyType.h"

namespace Glass {
	namespace Private {
		static constexpr auto NulloptString = "std::nullopt";

		template <typename T,
		          bool = has_static_member_data_is_keypath<T, const bool>::value,
		          bool = has_type_allowed_keypath_types<T>::value>
		struct AllowedKeypathTypes;

		template <typename T> struct AllowedKeypathTypes<T, true, false> {
			using type = std::tuple<typename T::type>;
		};

		template <typename T> struct AllowedKeypathTypes<T, false, true> {
			using type = typename T::allowed_keypath_types;
		};

		template <typename T>
		struct AllowedKeypathTypes<T, true, true> : AllowedKeypathTypes<T, false, true> {};

		template <typename T,
		          bool = has_static_member_data_is_keypath<T, const bool>::value ||
		                 has_type_allowed_keypath_types<T>::value>
		struct OptionalPropertyBase : T {
			using type = std::optional<typename T::type>;
			static std::string name() { return std::string{"Optional: "} + getName<T>(); }
		};

		template <typename T> struct OptionalPropertyBase<T, true> : T {
			using type = std::optional<typename T::type>;
			static std::string name() { return std::string{"Optional: "} + getName<T>(); }

			using allowed_keypath_types = typename AllowedKeypathTypes<T>::type;
		};
	}

	template <typename T, typename = std::void_t<>>
	struct OptionalProperty : PropertyType<OptionalProperty<T>>, Private::OptionalPropertyBase<T> {
		using type = typename Private::OptionalPropertyBase<T>::type;
		// using Private::OptionalPropertyBase<T>::name;
		using Private::RegisterPropertyType<OptionalProperty<T>>::staticRegistration;
		struct NoContextTag {};

		static auto serialize(const type& value) -> decltype(T::serialize(*value)) {
			if (!value) {
				return std::string{Private::NulloptString};
			}
			return T::serialize(*value);
		}

		template <typename U = T>
		static std::optional<type>
		deserialize(const std::string& serializedValue,
		            std::enable_if_t<!Private::has_type_context_type<U>::value, NoContextTag> =
		                NoContextTag{}) {
			if (serializedValue == std::string{Private::NulloptString}) {
				return type{};
			}
			if (auto deserialized = T::deserialize(serializedValue)) {
				return std::optional<type>{type{std::move(*deserialized)}};
			}
			return std::nullopt;
		}

		template <typename U = T>
		static std::optional<type>
		deserialize(const std::string& serializedValue,
		            std::enable_if_t<Private::has_type_context_type<U>::value,
		                             const typename U::context_type*> context = nullptr) {
			if (serializedValue == std::string{Private::NulloptString}) {
				return type{};
			}
			if (auto deserialized = T::deserialize(serializedValue, context)) {
				return std::optional<type>{type{std::move(*deserialized)}};
			}
			return std::nullopt;
		}
	};

	template <typename T>
	struct OptionalProperty<
	    T,
	    std::void_t<typename std::enable_if<Private::has_type_scratch_type<T>::value, T>::type>>
	    : PropertyType<OptionalProperty<T>>, Private::OptionalPropertyBase<T> {
		using type = typename Private::OptionalPropertyBase<T>::type;
		using scratch_type = std::optional<typename T::scratch_type>;
		using Deserialized = ScratchSpaceAndValue<scratch_type, type>;
		// using Private::OptionalPropertyBase<T>::name;
		using Private::RegisterPropertyType<OptionalProperty<T>>::staticRegistration;
		struct NoContextTag {};

		static auto serialize(const type& value, const scratch_type* scratch)
		    -> decltype(T::serialize(*value, &**scratch)) {
			if (!value) {
				return std::string{Private::NulloptString};
			}
			if (scratch) {
				ZASSERT(*scratch);
				return T::serialize(*value, &**scratch);
			}

			return T::serialize(*value, nullptr);
		}

		template <typename U = T>
		static std::optional<Deserialized>
		deserialize(const std::string& serializedValue,
		            std::enable_if_t<!Private::has_type_context_type<U>::value, NoContextTag> =
		                NoContextTag{}) {
			if (serializedValue == std::string{Private::NulloptString}) {
				return Deserialized{};
			} else if (auto deserialized = T::deserialize(serializedValue)) {
				return Deserialized{(*deserialized).scratchSpace, ((*deserialized).value)};
			}
			return std::nullopt;
		}

		template <typename U = T>
		static std::optional<Deserialized>
		deserialize(const std::string& serializedValue,
		            std::enable_if_t<Private::has_type_context_type<U>::value,
		                             const typename U::context_type*> context = nullptr) {
			if (serializedValue == std::string{Private::NulloptString}) {
				return Deserialized{};
			} else if (auto deserialized = T::deserialize(serializedValue, context)) {
				return Deserialized{(*deserialized).scratchSpace, ((*deserialized).value)};
			}
			return std::nullopt;
		}
	};
}
