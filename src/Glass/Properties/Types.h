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

	//! Properties that require scratch space should return the following type from
	//! deserialize (see below)
	template <typename Scratch, typename Type> struct ScratchSpaceAndValue {
		std::optional<Scratch> scratchSpace;
		Type value;
	};

	//! Use PropertyType to define a named property for serialization
	//!
	//! \param T type backing the property
	//!
	//! # Serialization/Deserialization
	//!   You can also define `serialize` and `deserialize` functions here.
	//!
	//!   The basic form of `serialize` is:
	//!   std::string serialize(const type& value)
	//!
	//!   If serialization can fail, you can use:
	//!   std::optional<std::string> serialize(const type& value)
	//!
	//!   `deserialize` should look like:
	//!   std::optional<T> deserialize(const std::string& serializedValue)
	//!
	//! # Advanced serialization: scratch space
	//!
	//!   In some contexts, deserializers need to create data that will be owned by the object that
	//!   has the properties.  For example, if `type` is a pointer (that is typically owned by an
	//!   external entity), if we want to deserialize a string, we need a place to put the value
	//!   being pointed to.  "Scratch Space" is the proper mechanism for this.
	//!   Serializers/Deserializers can opt-in to having "scratch space" as follows:
	//!
	//!   1) define a member type named `scratch_type`
	//!   2) modify the type of `deserialize` to be
	//!      std::optional<ScratchSpaceAndValue<scratch_type, type>> deserialize(const std::string&
	//!      serializedValue)
	//!   3) modify the type of `serialize` to be
	//!      std::optional<std::string> serialize(const T& value, const scratch_type* scratch)
	//!
	//!   Note that `serialize` may receive a nullptr scratch space, for example if the value of the
	//!   property wasn't deserialized or if someone explicitly cleared the scratch space.
	//!
	//! # Advanced serialization: Deserialization Context.
	//!
	//!   Sometimes, the deserializer needs access to context data in order to properly deserialize.
	//!   For example, we sometimes want to give serialize connections from widgets to DSP
	//!   parameters as KVC paths - this requires the deserializer to have access to the KVC in
	//!   order to properly interpret the path.  Note that the context of appropriate type must be
	//!   given to the PropertySerializer using `SetDeserializationContext` for this to work
	//!   properly.  In order to gain access to a deserialization context:
	//!
	//!   1) define a member type named `context_type`
	//!   2) modify the type of `deserialize` to be
	//!      std::optional<T> deserialize(const std::string& serializedValue, const context_type*
	//!      context)
	//!   3) ensure that the property serializer is given a context with
	//!   SetDeserializationContext(context_type) at some point.
	//!
	//!   Note that sometimes the deserialize function will be given a nullptr context, for example
	//!   if the PropertySerializer was never given a context of the correct type.
	//!
	//! # Advanced serialization: mixing Deserialization Contexts with Scratch Spaces
	//!
	//!   Some deserializers will need both a context and a scratch space.  To support this,
	//!
	//!   1) define a member type named `context_type`
	//!   2) define a member type named `scratch_type`
	//!   3) modify the type of `deserialize` to be
	//!      std::optional<ScratchSpaceAndValue<scratch_type, type>> deserialize(const std::string&
	//!      serializedValue, const context_type* context)
	//!   4) modify the type of `serialize` to be
	//!      std::optional<std::string> serialize(const T& value, const scratch_type* scratch)
	//!   See FloatControllerPropertyType below for an example of a deserializer with a context and
	//!   scratch.
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

    template <typename T> struct VectorProperty : T {
		using type = vector<typename T::type>;
		static std::string name() { return std::string{"Vector<"} + Private::getName<T>() + std::string{">"}; }
		static std::string serialize(const type& value) {
            UNREF_PARAM(value);
			ZFAILASSERT(); // Not implemented yet
            return std::string();
		}
		static type deserialize(const std::string& serializedValue) {
            UNREF_PARAM(serializedValue);
			ZFAILASSERT(); // Not implemented yet
            return std::string();
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
}
