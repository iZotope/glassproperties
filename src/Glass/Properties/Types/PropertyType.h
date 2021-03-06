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

#include "Glass/Properties/Types/Meta.h"
#include "Glass/Properties/Types/ScratchSpaceAndValue.h"

namespace Glass {
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
	//!
	//! # Advanced serialization: Key Path Types
	//!
	//!   Many of our types are meant to be serialized as "Key Paths", i.e., strings that are
	//!   interpreted by the environment to refer to globally registered values. For basic usage,
	//!   where the serialized string is a keypath to a value of the main type of the property,
	//!   simply add a `static const bool is_keypath = true;`.  This provides a hint to any GUI
	//!   design tools that keypaths are valid serialized values.
	//!
	//!   Some types (e.g., EnumOrFloatControllerPropertyType below), can be serialized as keypaths,
	//!   but those paths don't necessarily point to the exact type of the property - for example,
	//!   EnumOrFloatControllerPropertyType supports keypaths to FloatParam* OR keypaths to
	//!   EnumParam* but does NOT support keypaths to EnumOrFloatParam.  To handle this, instead add
	//!   a member type named `allowed_keypath_types`, which should be a std::tuple containing each
	//!   allowed keypath type.
	template <typename T, typename = void> struct PropertyType;

	template <typename T> struct PropertyType<T, std::enable_if_t<!IsBetterEnumProperty_v<T>>> {
		using type = T;
	};

	//! When T is a BETTER_ENUM, name,serialize, and deserialize will be automatically generated
	template <typename T> struct PropertyType<T, std::enable_if_t<IsBetterEnumProperty_v<T>>> {
		using type = T;
		static std::string name() { return std::string{"Enum: "} + T::_name(); }
		static std::string serialize(const T& value) { return value._to_string(); }
		static std::optional<type> deserialize(const std::string& serializedValue) {
			const auto maybeValue = T::_from_string_nocase_nothrow(serializedValue.c_str());
			if (!maybeValue) {
				return std::nullopt;
			}
			return *maybeValue;
		}
	};
}
