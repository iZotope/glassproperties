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
#include <unordered_map>

#include "Glass/Properties/Private/getName.h"
#include "Glass/Properties/Types.h"
#include <boost/callable_traits/is_invocable.hpp>
#include <boost/tti/has_type.hpp>

namespace Util {
	class PropertySerializer;
}

namespace Glass {
	namespace Private {
		namespace GlobalPropertyData {
			struct PropertyTypeDataBase;

			struct PropertyDeserializationResult {
				boost::any scratchSpace;
				boost::any value;
			};
			// This is the most general possible serialize function - it takes the value and the
			// scratch space, and could fail.
			using PropertySerializeFn =
			    std::function<std::optional<std::string>(const boost::any&, const boost::any&)>;

			// This is the most general possible deserialize function - it takes a string a context,
			// and returns the value and the scratch space.
			using PropertyDeserializeFn =
			    std::function<std::optional<PropertyDeserializationResult>(const std::string&,
			                                                          const boost::any&)>;

			struct PropertyTypeSerializationData {
				std::string typeName;
				PropertySerializeFn serialize;
				PropertyDeserializeFn deserialize;
				std::optional<std::type_index> requiredContext;
				std::optional<vector<string>> enumNames;
			};

			using PropertySerializationMap =
			    std::unordered_map<std::string, std::function<PropertyTypeSerializationData()>>;

			PropertySerializationMap& getPropertySerializationMap();

			//! Register all property types that have been globally registered for serialization
			//! with AddPropertyTypeData<T>() or GLASS_REGISTER_PROPERTY_TYPE.
			void registerGlobalPropertyTypes(Util::PropertySerializer&);

			//! Register a property type with a property serializer.
			//! Normally this shouldn't be called directly, but instead proeprties should be
			//! registered with AddProeprtyTypeData<T>() or GLASS_REGISTER_PROPERTY_TYPE and
			//! registered with registerGlobalPropertyTypes on startup
			void registerPropertyType(Util::PropertySerializer&, PropertyTypeSerializationData);

			template <typename T, typename = std::void_t<>> struct EnumPropertyNames {
				static std::nullopt_t Get() { return std::nullopt; }
			};
			template <typename T>
			struct EnumPropertyNames<T, std::void_t<decltype(T::type::_names())>> {
				static vector<std::string> Get() {
					const auto nameIterator = T::type::_names();
					return vector<std::string>(begin(nameIterator), end(nameIterator));
				}
			};


			BOOST_TTI_HAS_TYPE(scratch_type);
			BOOST_TTI_HAS_TYPE(context_type);

			//! Overload for basic serializers with neither context nor scratch types
			template <typename T>
			typename std::enable_if<!has_type_scratch_type<T>::value &&
			                            !has_type_context_type<T>::value,
			                        PropertyTypeSerializationData>::type
			GetPropertyTypeSerializationData(T* = nullptr) {
				static_assert(
				    boost::callable_traits::is_invocable_r<std::optional<std::string>,
				                                           decltype(T::serialize),
				                                           const typename T::type&>::value,
				    "serialize must have a signature compatible with std::optional<std::string>(const "
				    "type&)");
				static_assert(boost::callable_traits::is_invocable_r<std::optional<typename T::type>,
				                                                     decltype(T::deserialize),
				                                                     const std::string&>::value,
				              "deserialize must have a signature compatible with "
				              "std::optional<type>(const std::string&)");
				auto serialize = [](const boost::any& value,
				                    const boost::any&) -> std::optional<std::string> {
					const typename T::type* typedValue = boost::any_cast<typename T::type>(&value);
					if (!typedValue) {
						ZERROR("Attempting to serialize an invalid type. Type to serialize "
						       "must be type T.");
						return std::nullopt;
					}
					return T::serialize(*typedValue);
				};
				auto deserialize = [](const string& serializedValue,
				                      const auto&) -> std::optional<PropertyDeserializationResult> {
					auto ret = T::deserialize(serializedValue);
					if (!ret) {
						return std::nullopt;
					}
					return PropertyDeserializationResult{boost::any{}, std::move(*ret)};
				};

				return PropertyTypeSerializationData{
				    std::string{Private::getName<T>()},
				    std::move(serialize),
				    std::move(deserialize),
				    std::nullopt,
				    EnumPropertyNames<T>::Get(),
				};
			}

			//! Overload for scratch space serializers without context
			template <typename T>
			typename std::enable_if<has_type_scratch_type<T>::value &&
			                            !has_type_context_type<T>::value,
			                        PropertyTypeSerializationData>::type
			GetPropertyTypeSerializationData(T* = nullptr) {
				static_assert(
				    boost::callable_traits::is_invocable_r<std::optional<std::string>,
				                                           decltype(T::serialize),
				                                           const typename T::type&,
				                                           const typename T::scratch_type*>::value,
				    "serialize must have a signature compatible with std::optional<std::string>(const "
				    "T::type&, const T::scratch_type*)");

				static_assert(
				    boost::callable_traits::is_invocable_r<
				        std::optional<Glass::ScratchSpaceAndValue<typename T::scratch_type,
				                                             typename T::type>>,
				        decltype(T::deserialize),
				        const std::string&>::value,
				    "deserialize must have a signature compatible with "
				    "std::optional<ScratchSpaceAndValue<T::scratch_type, T::type>>(const std::string&)");
				auto serialize = [](const boost::any& value,
				                    const boost::any& scratch) -> std::optional<std::string> {
					const typename T::type* typedValue = boost::any_cast<typename T::type>(&value);
					if (!typedValue) {
						ZERROR("Attempting to serialize an invalid type. Type to serialize "
						       "must be type T.");
						return std::nullopt;
					}
					const typename T::scratch_type* typedScratch =
					    boost::any_cast<typename T::scratch_type>(&scratch);
					return T::serialize(*typedValue, typedScratch);
				};
				auto deserialize = [](const string& serializedValue,
				                      const auto&) -> std::optional<PropertyDeserializationResult> {
					std::optional<
					    Glass::ScratchSpaceAndValue<typename T::scratch_type, typename T::type>>
					    ret = T::deserialize(serializedValue);
					if (ret == std::nullopt) {
						return std::nullopt;
					}
					auto anyScratch = ret->scratchSpace
					                      ? boost::any{std::move(*(ret->scratchSpace))}
					                      : boost::any{};
					return PropertyDeserializationResult{std::move(anyScratch),
					                                     std::move(ret->value)};
				};

				return PropertyTypeSerializationData{
				    std::string{Private::getName<T>()},
				    std::move(serialize),
				    std::move(deserialize),
				    std::nullopt,
				    EnumPropertyNames<T>::Get(),
				};
			}

			//! overload for context serializers without scratch space
			template <typename T>
			typename std::enable_if<!has_type_scratch_type<T>::value &&
			                            has_type_context_type<T>::value,
			                        PropertyTypeSerializationData>::type
			GetPropertyTypeSerializationData(T* = nullptr) {
				static_assert(
				    boost::callable_traits::is_invocable_r<std::optional<std::string>,
				                                           decltype(T::serialize),
				                                           const typename T::type&>::value,
				    "serialize must have a signature compatible with std::optional<std::string>(const "
				    "T::type&)");

				static_assert(
				    boost::callable_traits::is_invocable_r<std::optional<typename T::type>,
				                                           decltype(T::deserialize),
				                                           const std::string&,
				                                           const typename T::context_type*>::value,
				    "deserialize must have a signature compatible with "
				    "std::optional<T::type>(const std::string&, const T::context_type*)");
				auto serialize = [](const boost::any& value,
				                    const boost::any&) -> std::optional<std::string> {
					const typename T::type* typedValue = boost::any_cast<typename T::type>(&value);
					if (!typedValue) {
						ZERROR("Attempting to serialize an invalid type. Type to serialize "
						       "must be type T.");
						return std::nullopt;
					}
					return T::serialize(*typedValue);
				};
				auto deserialize =
				    [](const string& serializedValue,
				       const boost::any& context) -> std::optional<PropertyDeserializationResult> {
					const typename T::context_type* typedContext =
					    boost::any_cast<typename T::context_type>(&context);
					std::optional<typename T::type> ret = T::deserialize(serializedValue, typedContext);
					if (ret == std::nullopt) {
						return std::nullopt;
					}
					return PropertyDeserializationResult{boost::any{}, std::move(*ret)};
				};

				return PropertyTypeSerializationData{
				    std::string{Private::getName<T>()},
				    std::move(serialize),
				    std::move(deserialize),
				    std::type_index{typeid(typename T::context_type)},
				    EnumPropertyNames<T>::Get(),
				};
			}

			//! overload for serializers with both context and scratch space
			template <typename T>
			typename std::enable_if<has_type_scratch_type<T>::value &&
			                            has_type_context_type<T>::value,
			                        PropertyTypeSerializationData>::type
			GetPropertyTypeSerializationData(T* = nullptr) {
				static_assert(
				    boost::callable_traits::is_invocable_r<std::optional<std::string>,
				                                           decltype(T::serialize),
				                                           const typename T::type&,
				                                           const typename T::scratch_type*>::value,
				    "serialize must have a signature compatible with std::optional<std::string>(const "
				    "T::type&, const T::scratch_type*)");

				static_assert(boost::callable_traits::is_invocable_r<
				                  std::optional<Glass::ScratchSpaceAndValue<typename T::scratch_type,
				                                                       typename T::type>>,
				                  decltype(T::deserialize),
				                  const std::string&,
				                  const typename T::context_type*>::value,
				              "deserialize must have a signature compatible with "
				              "std::optional<ScratchSpaceAndValue<T::scratch_type, T::type>>(const "
				              "std::string&, const T::context_type*)");
				auto serialize = [](const boost::any& value,
				                    const boost::any& scratch) -> std::optional<std::string> {
					const typename T::type* typedValue = boost::any_cast<typename T::type>(&value);
					if (!typedValue) {
						ZERROR("Attempting to serialize an invalid type. Type to serialize "
						       "must be type T.");
						return std::nullopt;
					}
					const typename T::scratch_type* typedScratch =
					    boost::any_cast<typename T::scratch_type>(&scratch);
					return T::serialize(*typedValue, typedScratch);
				};
				auto deserialize =
				    [](const string& serializedValue,
				       const boost::any& context) -> std::optional<PropertyDeserializationResult> {
					const typename T::context_type* typedContext =
					    boost::any_cast<typename T::context_type>(&context);
					std::optional<
					    Glass::ScratchSpaceAndValue<typename T::scratch_type, typename T::type>>
					    ret = T::deserialize(serializedValue, typedContext);
					if (ret == std::nullopt) {
						return std::nullopt;
					}
					auto anyScratch = ret->scratchSpace
					                      ? boost::any{std::move(*(ret->scratchSpace))}
					                      : boost::any{};
					return PropertyDeserializationResult{std::move(anyScratch),
					                                     std::move(ret->value)};
				};

				return PropertyTypeSerializationData{
				    std::string{Private::getName<T>()},
				    std::move(serialize),
				    std::move(deserialize),
				    std::type_index{typeid(typename T::context_type)},
				    EnumPropertyNames<T>::Get(),
				};
			}


			template <typename T> void* AddPropertyTypeData(T* t = nullptr) {
				auto& map = getPropertySerializationMap();
				auto data = GetPropertyTypeSerializationData(t);
				map.emplace(std::make_pair(data.typeName,
				                           [] { return GetPropertyTypeSerializationData<T>(); }));
				return nullptr;
			}
		}
	}
}
