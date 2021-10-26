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

#include "Glass/Properties/Private/GlobalPropertyData.h"

namespace Glass {
	namespace Private {

		template <typename T, typename = void> constexpr inline bool HasDefaultValue = false;

		template <typename T>
		constexpr inline bool HasDefaultValue<T, std::void_t<decltype(T::defaultValue)>> = true;

		template <typename T> auto getDefaultValue() {
			static_assert(HasDefaultValue<T>,
			              "T must have a defaultValue static data member or function.");

			if constexpr (std::is_function_v<decltype(T::defaultValue)>) {
				static_assert(std::is_convertible_v<decltype(T::defaultValue()),
				                                    typename T::property_type::type>,
				              "T::defaultValue() must be convertible to the property type.");
				return PropertyDefaultValue{
				    boost::any{},
				    static_cast<typename T::property_type::type>(T::defaultValue())};
			} else if constexpr (std::is_convertible_v<decltype(T::defaultValue),
			                                           typename T::property_type::type>) {
				return PropertyDefaultValue{
				    boost::any{},
				    static_cast<typename T::property_type::type>(T::defaultValue)};
			} else {
				//! This branch is for types with complex default values using strings.  This will
				//! use the deserializer to deserialize the default value.  Note that this isn't
				//! supported if the value type is actually a string - in that case, we take the
				//! string literal as the default value rather than running it through the
				//! deserializer.
				static_assert(std::is_convertible_v<decltype(T::defaultValue), std::string>);
				auto serializationData = GlobalPropertyData::GetPropertyTypeSerializationData<
				    typename T::property_type>();
				// At this point, we don't have a context, so hope this works without.
				auto deserialized = serializationData.deserialize(T::defaultValue, boost::any{});
				ZVERIFYRETURN(
				    deserialized,
				    PropertyDefaultValue{boost::any{}, typename T::property_type::type{}});
				return PropertyDefaultValue{std::move(deserialized->scratchSpace),
				                            std::move(deserialized->value)};
			}
		}


		//		template <typename T>
		//		auto getDefaultValue(
		//		    typename std::enable_if<!std::is_function<decltype(T::defaultValue)>::value &&
		//		                                std::is_convertible<decltype(T::defaultValue),
		//		                                                    typename
		//T::property_type::type>::value, 		                            T*>::type) { 			return PropertyDefaultValue{ 			    boost::any{},
		//static_cast<typename T::property_type::type>(T::defaultValue)};
		//		}
		//
		//		template <typename T>
		//		auto getDefaultValue(
		//		    typename std::enable_if<std::is_function<decltype(T::defaultValue)>::value,
		//T*>::type) { 			return PropertyDefaultValue{ 			    boost::any{}, static_cast<typename
		//T::property_type::type>(T::defaultValue())};
		//		}
		//
		//		//! This overload is for types with complex default values using strings.  This will
		//use the
		//		//! deserializer to deserialize the default value.  Note that this isn't supported if
		//the
		//		//! value type is actually a string - in that case, we take the string literal as
		//the
		//		//! default value rather than running it through the deserializer.
		//		template <typename T>
		//		PropertyDefaultValue getDefaultValue(
		//		    typename std::enable_if<
		//		        !std::is_function<decltype(T::defaultValue)>::value &&
		//		            !std::is_convertible<std::string, typename T::property_type::type>::value
		//&& 		            std::is_convertible<decltype(T::defaultValue), std::string>::value, 		        T*>::type) { 			auto
		//serializationData = 			    GlobalPropertyData::GetPropertyTypeSerializationData<typename
		//T::property_type>();
		//			// At this point, we don't have a context, so hope this works without.
		//			auto deserialized = serializationData.deserialize(T::defaultValue,
		//boost::any{}); 			ZVERIFYRETURN( 			    deserialized, 			    PropertyDefaultValue{boost::any{}, 			                                                typename
		//T::property_type::type{}}); 			return PropertyDefaultValue{
		//			    std::move(deserialized->scratchSpace), std::move(deserialized->value)};
		//		}
	}
}
