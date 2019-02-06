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



#include "iZBase/Controller/FloatParam.h"
#include "iZBase/Controller/KeyValueCoding.h"

#include "Glass/Properties/Scale.h"
#include "Glass/Float4Dim.h"
#include "Glass/Color.h"
#include "Glass/Font.h"
#include "Glass/Image.h"
#include "Glass/SpriteSheet.h"

#include "Glass/Properties/Private/getName.h"

#include "fmt/format.h"

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
	
	struct Float4DimPropertyType : PropertyType<Float4Dim> {
		static constexpr auto name = "Float4Dim";
		static std::string serialize(const type& value) {
			return boost::apply_visitor(
			    ::Util::overload<std::string>(
			        [](float rad) {
						return fmt::format("{}", rad);
				    },
			        [](std::array<float, 4> rad) {
						return fmt::format("[{}, {}, {}, {}]", rad[0], rad[1], rad[2], rad[3]);
				    }),
			    value);
		}
	};

	struct BoolPropertyType : PropertyType<bool> {
		static constexpr auto name = "Bool";
	};

	struct StringPropertyType : PropertyType<std::string> {
		static constexpr auto name = "String";
	};

	struct PointPropertyType : PropertyType<Point> {
		static constexpr auto name = "Glass::Point";
	};

	struct SizePropertyType : PropertyType<Size> {
		static constexpr auto name = "Glass::Size";
	};

	struct RectPropertyType : PropertyType<Rect> {
		static constexpr auto name = "Glass::Rect";
	};

	struct ColorPropertyType : PropertyType<Color> {
		static constexpr auto name = "Glass::Color";
	};

	struct FontPropertyType : PropertyType<Typeface> {
		static constexpr auto name = "Glass::Font";
	};

	struct ImagePropertyType : PropertyType<Image> {
		static constexpr auto name = "Glass::Image";
	};

	struct SpriteSheetPropertyType : PropertyType<SpriteSheet> {
		static constexpr auto name = "Glass::SpriteSheet";
	};

	struct FloatControllerPropertyType : PropertyType<::Controller::FloatParam*> {
		static constexpr auto name = "FloatController";

		struct scratch_type {
			std::string serializedRepresentation;
			shared_ptr<::Controller::FloatParam> paramBacking;
		};
		using context_type = ::Controller::KeyValueCoding*;
		static std::optional<std::string> serialize(::Controller::FloatParam* controller,
		                                       const scratch_type* scratch);
		using Deserialized = ScratchSpaceAndValue<scratch_type, type>;
		static std::optional<Deserialized> deserialize(const std::string& value,
		                                          const context_type* kvc);
		static const bool is_keypath = true;
	};

	struct EnumControllerPropertyType : PropertyType<::Controller::EnumParam*> {
		static constexpr auto name = "EnumController";

		struct scratch_type {
			std::string serializedRepresentation;
			shared_ptr<::Controller::EnumParam> paramBacking;
		};
		using context_type = ::Controller::KeyValueCoding*;
		static std::optional<std::string> serialize(::Controller::EnumParam* controller,
		                                       const scratch_type* scratch);
		using Deserialized = ScratchSpaceAndValue<scratch_type, type>;
		static std::optional<Deserialized> deserialize(const std::string& value,
												  const context_type* kvc);
		static const bool is_keypath = true;
	};

	struct BoolControllerPropertyType : PropertyType<::Controller::BoolParam*> {
		static constexpr auto name = "BoolController";

		struct scratch_type {
			std::string serializedRepresentation;
			shared_ptr<::Controller::BoolParam> paramBacking;
		};
		using context_type = ::Controller::KeyValueCoding*;
		static std::optional<std::string> serialize(::Controller::BoolParam* controller,
		                                       const scratch_type* scratch);
		using Deserialized = ScratchSpaceAndValue<scratch_type, type>;
		static std::optional<Deserialized> deserialize(const std::string& value,
		                                          const context_type* kvc);
		static const bool is_keypath = true;
	};

	struct FloatScalePropertyType : PropertyType<FloatScale> {
		static constexpr auto name = "FloatScale";
		static std::string serialize(const FloatScale& value) { return value.ToString(); }
		static std::optional<FloatScale> deserialize(const std::string& serializedValue) {
			return FloatScale::MakeFromString(serializedValue);
		}
	};

	struct FloatIntervalPropertyType : PropertyType<FloatInterval> {
		static constexpr auto name = "FloatInterval";
	};

	struct DirectionalFloatIntervalPropertyType : PropertyType<DirectionalFloatInterval> {
		static constexpr auto name = "DirectionalFloatInterval";
	};

	using EnumOrFloatParam = boost::variant<::Controller::FloatParam*, ::Controller::EnumParam*>;
	struct EnumOrFloatControllerPropertyType : PropertyType<EnumOrFloatParam> {
		static constexpr auto name = "EnumOrFloatController";

		struct scratch_type {
			std::string serializedRepresentation;
			boost::variant<shared_ptr<::Controller::FloatParam>, shared_ptr<::Controller::EnumParam>> paramBacking;
		};
		using context_type = ::Controller::KeyValueCoding*;
		static std::optional<std::string> serialize(EnumOrFloatParam controller,
			const scratch_type* scratch);
		using Deserialized = ScratchSpaceAndValue<scratch_type, type>;
		static std::optional<Deserialized> deserialize(const std::string& value,
			const context_type* kvc);
		using allowed_keypath_types =
		    std::tuple<::Controller::FloatParam*, ::Controller::EnumParam*>;
	};
}
