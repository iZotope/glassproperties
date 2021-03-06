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

#include "iZBase/Util/PropertySerializer.h"

#include "Glass/Properties/Private/getName.h"
#include "Glass/Properties/Private/has_type.h"
#include "Glass/Properties/Types/ScratchSpaceAndValue.h"

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

			// This is the most general possible deserialize function - it takes a std::string a
			// context, and returns the value and the scratch space.
			using PropertyDeserializeFn =
			    std::function<std::optional<PropertyDeserializationResult>(const std::string&,
			                                                               const boost::any&)>;

			using PropertyTypeSerializationData =
			    Util::PropertySerializer::AdvancedTypeRegistrationInfo;

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
			void registerPropertyType(Util::PropertySerializer&,
			                          std::string name,
			                          PropertyTypeSerializationData data);

			template <typename T, typename = std::void_t<>> struct EnumPropertyNames {
				static void addToSerializationData(PropertyTypeSerializationData&) {}
			};

			template <typename T>
			struct EnumPropertyNames<T, std::void_t<decltype(T::type::_names())>> {
				static void addToSerializationData(PropertyTypeSerializationData& data) {
					const auto nameIterator = T::type::_names();
					data.SetEnumValueNames(
					    vector<std::string>(begin(nameIterator), end(nameIterator)));
				}
			};

			template <typename T>
			void maybeRegisterEnumPropertyNames(PropertyTypeSerializationData& data) {
				EnumPropertyNames<T>::addToSerializationData(data);
			}

			template <typename T,
			          bool = has_static_member_data_is_keypath<T, const bool>::value,
			          bool = has_type_allowed_keypath_types<T>::value>
			struct KeyPathInfo;

			template <typename T> struct KeyPathInfo<T, false, false> {
				static void addToSerializationData(PropertyTypeSerializationData&) {}
			};

			template <typename T> struct KeyPathInfo<T, true, false> {
				static void addToSerializationData(PropertyTypeSerializationData& data) {
					if (T::is_keypath) {
						data.template SetExpectedKeyPathType<typename T::type>();
					}
				}
			};

			inline bool isAnyOneOf(const boost::any&, std::tuple<>* = nullptr) { return false; }

			template <typename T, typename... Ts>
			bool isAnyOneOf(const ::Controller::KVCAny& value, std::tuple<T, Ts...>* = nullptr) {
				if (::Controller::KeyValueCoding::CastFromAny<T>(value)) {
					return true;
				}
				return isAnyOneOf(value, static_cast<std::tuple<Ts...>*>(nullptr));
			}

			template <typename T> struct KeyPathInfo<T, false, true> {
				static void addToSerializationData(PropertyTypeSerializationData& data) {
					data.designAidInfo = Util::PropertySerializer::DesignAidKVCPathInfo{
					    [](const ::Controller::KVCAny& maybeValueReporter) -> bool {
						    return isAnyOneOf(
						        maybeValueReporter,
						        static_cast<typename std::add_pointer<
						            typename T::allowed_keypath_types>::type>(nullptr));
					    }};
				}
			};

			template <typename T> struct KeyPathInfo<T, true, true> {
				static void addToSerializationData(PropertyTypeSerializationData& data) {
					if (T::is_keypath) {
						KeyPathInfo<T, false, true>::addToSerializationData(data);
					}
				}
			};

			template <typename T> void maybeAddKeyPathInfo(PropertyTypeSerializationData& data) {
				KeyPathInfo<T>::addToSerializationData(data);
			}

			BOOST_TTI_HAS_TYPE(scratch_type);
			BOOST_TTI_HAS_TYPE(context_type);


			//! Overload for basic serializers with neither context nor scratch types
			template <typename T>
			typename std::enable_if<!has_type_scratch_type<T>::value &&
			                            !has_type_context_type<T>::value,
			                        PropertyTypeSerializationData>::type
			GetPropertyTypeSerializationData(T* = nullptr) {
				static_assert(std::is_convertible_v<decltype(T::serialize(
				                                        std::declval<const typename T::type&>())),
				                                    std::optional<std::string>>,
				              "serialize must have a signature compatible with "
				              "std::optional<std::string>(const "
				              "type&)");
				static_assert(std::is_convertible_v<decltype(T::deserialize(
				                                        std::declval<const std::string&>())),
				                                    std::optional<typename T::type>>,
				              "deserialize must have a signature compatible with "
				              "std::optional<type>(const std::string&)");
				auto serialize = [](const boost::any& value,
				                    const boost::any&) -> boost::optional<std::string> {
					const typename T::type* typedValue = boost::any_cast<typename T::type>(&value);
					if (!typedValue) {
						ZERROR("Attempting to serialize an invalid type. Type to serialize "
						       "must be type T.");
						return boost::none;
					}
					auto serialized = std::optional<std::string>(T::serialize(*typedValue));
					return serialized ? boost::optional<std::string>(*serialized) : boost::none;
				};
				auto deserialize =
				    [](const std::string& serializedValue,
				       const auto&) -> boost::optional<Util::PropertyDeserializationResult> {
					auto ret = T::deserialize(serializedValue);
					if (!ret) {
						return boost::none;
					}
					return Util::PropertyDeserializationResult{boost::any{}, std::move(*ret)};
				};

				auto serializationData =
				    PropertyTypeSerializationData()
				        .SerializationFunction(std::move(serialize))
				        .DeserializationFunction(std::move(deserialize))
				        .template SetTypeForValueReporterConnection<typename T::type>();
				maybeRegisterEnumPropertyNames<T>(serializationData);
				maybeAddKeyPathInfo<T>(serializationData);
				return serializationData;
			}

			//! Overload for scratch space serializers without context
			template <typename T>
			typename std::enable_if<has_type_scratch_type<T>::value &&
			                            !has_type_context_type<T>::value,
			                        PropertyTypeSerializationData>::type
			GetPropertyTypeSerializationData(T* = nullptr) {
				static_assert(
				    std::is_convertible_v<decltype(T::serialize(
				                              std::declval<const typename T::type&>(),
				                              std::declval<const typename T::scratch_type*>())),
				                          std::optional<std::string>>,
				    "serialize must have a signature compatible with std::optional<std::string>(const "
				    "T::type&, const T::scratch_type*)");

				static_assert(
				    std::is_convertible_v<
				        decltype(T::deserialize(std::declval<const std::string&>())),
				        std::optional<
				            ScratchSpaceAndValue<typename T::scratch_type, typename T::type>>>,
				    "deserialize must have a signature compatible with "
				    "std::optional<ScratchSpaceAndValue<T::scratch_type, T::type>>(const std::string&)");
				auto serialize = [](const boost::any& value,
				                    const boost::any& scratch) -> boost::optional<std::string> {
					const typename T::type* typedValue = boost::any_cast<typename T::type>(&value);
					if (!typedValue) {
						ZERROR("Attempting to serialize an invalid type. Type to serialize "
						       "must be type T.");
						return boost::none;
					}
					const typename T::scratch_type* typedScratch =
					    boost::any_cast<typename T::scratch_type>(&scratch);
					auto serialized =
					    std::optional<std::string>(T::serialize(*typedValue, typedScratch));
					return serialized ? boost::optional<std::string>(*serialized) : boost::none;
				};
				auto deserialize =
				    [](const std::string& serializedValue,
				       const auto&) -> boost::optional<Util::PropertyDeserializationResult> {
					std::optional<
					    Glass::ScratchSpaceAndValue<typename T::scratch_type, typename T::type>>
					    ret = T::deserialize(serializedValue);
					if (ret == std::nullopt) {
						return boost::none;
					}
					auto anyScratch = ret->scratchSpace
					                      ? boost::any{std::move(*(ret->scratchSpace))}
					                      : boost::any{};
					return Util::PropertyDeserializationResult{std::move(anyScratch),
					                                           std::move(ret->value)};
				};

				auto serializationData =
				    PropertyTypeSerializationData()
				        .SerializationFunction(std::move(serialize))
				        .DeserializationFunction(std::move(deserialize))
				        .template SetTypeForValueReporterConnection<typename T::type>();
				maybeRegisterEnumPropertyNames<T>(serializationData);
				maybeAddKeyPathInfo<T>(serializationData);

				return serializationData;
			}

			//! overload for context serializers without scratch space
			template <typename T>
			typename std::enable_if<!has_type_scratch_type<T>::value &&
			                            has_type_context_type<T>::value,
			                        PropertyTypeSerializationData>::type
			GetPropertyTypeSerializationData(T* = nullptr) {
				static_assert(
				    std::is_convertible_v<decltype(T::serialize(
				                              std::declval<const typename T::type&>())),
				                          std::optional<std::string>>,
				    "serialize must have a signature compatible with std::optional<std::string>(const "
				    "T::type&)");

				static_assert(
				    std::is_convertible_v<decltype(T::deserialize(
				                              std::declval<const std::string&>(),
				                              std::declval<const typename T::context_type*>())),
				                          std::optional<typename T::type>>,
				    "deserialize must have a signature compatible with "
				    "std::optional<T::type>(const std::string&, const T::context_type*)");
				auto serialize = [](const boost::any& value,
				                    const boost::any&) -> boost::optional<std::string> {
					const typename T::type* typedValue = boost::any_cast<typename T::type>(&value);
					if (!typedValue) {
						ZERROR("Attempting to serialize an invalid type. Type to serialize "
						       "must be type T.");
						return boost::none;
					}
					auto serialized = std::optional<std::string>(T::serialize(*typedValue));
					return serialized ? boost::optional<std::string>(*serialized) : boost::none;
				};
				auto deserialize = [](const std::string& serializedValue, const boost::any& context)
				    -> boost::optional<Util::PropertyDeserializationResult> {
					const typename T::context_type* typedContext =
					    boost::any_cast<typename T::context_type>(&context);
					std::optional<typename T::type> ret =
					    T::deserialize(serializedValue, typedContext);
					if (ret == std::nullopt) {
						return boost::none;
					}
					return Util::PropertyDeserializationResult{boost::any{}, std::move(*ret)};
				};


				auto serializationData =
				    PropertyTypeSerializationData()
				        .SerializationFunction(std::move(serialize))
				        .DeserializationFunction(std::move(deserialize))
				        .template ContextType<typename T::context_type>()
				        .template SetTypeForValueReporterConnection<typename T::type>();
				maybeRegisterEnumPropertyNames<T>(serializationData);
				maybeAddKeyPathInfo<T>(serializationData);

				return serializationData;
			}

			//! overload for serializers with both context and scratch space
			template <typename T>
			typename std::enable_if<has_type_scratch_type<T>::value &&
			                            has_type_context_type<T>::value,
			                        PropertyTypeSerializationData>::type
			GetPropertyTypeSerializationData(T* = nullptr) {
				static_assert(
				    std::is_convertible_v<decltype(T::serialize(
				                              std::declval<const typename T::type&>(),
				                              std::declval<const typename T::scratch_type*>())),
				                          std::optional<std::string>>,
				    "serialize must have a signature compatible with std::optional<std::string>(const "
				    "T::type&, const T::scratch_type*)");

				static_assert(
				    std::is_convertible_v<
				        decltype(T::deserialize(std::declval<const std::string&>(),
				                                std::declval<const typename T::context_type*>())),
				        std::optional<
				            ScratchSpaceAndValue<typename T::scratch_type, typename T::type>>>,
				    "deserialize must have a signature compatible with "
				    "std::optional<ScratchSpaceAndValue<T::scratch_type, T::type>>(const "
				    "std::string&, const T::context_type*)");
				auto serialize = [](const boost::any& value,
				                    const boost::any& scratch) -> boost::optional<std::string> {
					const typename T::type* typedValue = boost::any_cast<typename T::type>(&value);
					if (!typedValue) {
						ZERROR("Attempting to serialize an invalid type. Type to serialize "
						       "must be type T.");
						return boost::none;
					}
					const typename T::scratch_type* typedScratch =
					    boost::any_cast<typename T::scratch_type>(&scratch);
					auto serialized =
					    std::optional<std::string>(T::serialize(*typedValue, typedScratch));
					return serialized ? boost::optional<std::string>(*serialized) : boost::none;
				};
				auto deserialize = [](const std::string& serializedValue, const boost::any& context)
				    -> boost::optional<Util::PropertyDeserializationResult> {
					const typename T::context_type* typedContext =
					    boost::any_cast<typename T::context_type>(&context);
					std::optional<
					    Glass::ScratchSpaceAndValue<typename T::scratch_type, typename T::type>>
					    ret = T::deserialize(serializedValue, typedContext);
					if (ret == std::nullopt) {
						return boost::none;
					}
					auto anyScratch = ret->scratchSpace
					                      ? boost::any{std::move(*(ret->scratchSpace))}
					                      : boost::any{};
					return Util::PropertyDeserializationResult{std::move(anyScratch),
					                                           std::move(ret->value)};
				};

				auto serializationData =
				    PropertyTypeSerializationData()
				        .SerializationFunction(std::move(serialize))
				        .DeserializationFunction(std::move(deserialize))
				        .template ContextType<typename T::context_type>()
				        .template SetTypeForValueReporterConnection<typename T::type>();
				maybeRegisterEnumPropertyNames<T>(serializationData);
				maybeAddKeyPathInfo<T>(serializationData);

				return serializationData;
			}


			template <typename T> void* AddPropertyTypeData(T* = nullptr) {
				auto& map = getPropertySerializationMap();
				if (map.find(Private::getName<T>()) != map.end()) {
					return nullptr;
				}
				// Setup property serialization data for T
				map.emplace(std::make_pair(Private::getName<T>(),
				                           [] { return GetPropertyTypeSerializationData<T>(); }));

				return &map.at(Private::getName<T>());
			}
		}
	}
}
