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

#include <unordered_map>

#include "Glass/Properties/Private/getName.h"

namespace Util {
	class PropertySerializer;
}

namespace Glass {
	namespace Private {
		namespace GlobalPropertyData {
			struct PropertyTypeDataBase;

			using PropertySerializeFn = std::function<std::string(const boost::any&)>;
			using PropertyDeserializeFn = std::function<boost::any(const std::string&)>;
			using PropertySerializationMap =
			    std::unordered_map<std::string, unique_ptr<PropertyTypeDataBase>>;

			PropertySerializationMap& getPropertySerializationMap();

			//! Register all property types that have been globally registered for serialization
			//! with AddPropertyTypeData<T>() or GLASS_REGISTER_PROPERTY_TYPE.
			void registerGlobalPropertyTypes(Util::PropertySerializer&);

			//! Register a property type with a property serializer.
			//! Normally this shouldn't be called directly, but instead proeprties should be
			//! registered with AddProeprtyTypeData<T>() or GLASS_REGISTER_PROPERTY_TYPE and
			//! registered with registerGlobalPropertyTypes on startup
			void registerPropertyType(Util::PropertySerializer&, const std::string& typeName,
			                          const PropertyTypeDataBase&);
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

			struct PropertyTypeDataBase {
				virtual PropertySerializeFn GetSerializeFn() const = 0;
				virtual PropertyDeserializeFn GetDeserializeFn() const = 0;
				virtual std::optional<vector<std::string>> GetNames() const = 0;
			};

			template <typename T> struct PropertyTypeData : PropertyTypeDataBase {
				PropertySerializeFn GetSerializeFn() const override {
					auto serialize = [](const boost::any& value) -> std::string {
						try {
							typename T::type typedValue = boost::any_cast<typename T::type>(value);
							return T::serialize(typedValue);
						} catch (const boost::bad_any_cast&) {
							ZERROR("Attempting to serialize an invalid type. Type to searialize "
							       "must be type T.");
							return "";
						}
					};

					return serialize;
				}
				PropertyDeserializeFn GetDeserializeFn() const override {
					auto deserialize = [](const string& serializedValue) -> boost::any {
						return T::deserialize(serializedValue);
					};

					return deserialize;
				}
				std::optional<vector<std::string>> GetNames() const override {
					return EnumPropertyNames<T>::Get();
				}
			};

			template <typename T> void* AddPropertyTypeData(T* = nullptr) {
				auto& map = getPropertySerializationMap();

				map.emplace(std::make_pair(std::string{Private::getName<T>()},
				                           make_unique<PropertyTypeData<T>>()));

				return nullptr;
			}
		}
	}
}
