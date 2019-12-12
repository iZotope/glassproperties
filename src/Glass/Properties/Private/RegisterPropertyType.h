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

#include <iostream>

#include "Glass/Properties/Private/GlobalPropertyData.h"
#include "Glass/Properties/Private/getName.h"

namespace Glass::Private {
	template <typename T, typename = void> struct ShouldRegisterPropertyType : std::false_type {};

	template <typename T>
	struct ShouldRegisterPropertyType<
	    T,
	    std::void_t<decltype(getName<T>(),
	                         &T::serialize,
	                         T::deserialize(""),
	                         GlobalPropertyData::GetPropertyTypeSerializationData<T>())>>
	    : std::true_type {};

	template <typename T> struct RegisterPropertyType {
		struct StaticRegistration {
			StaticRegistration() {
				if constexpr (ShouldRegisterPropertyType<T>::value) {
					Glass::Private::GlobalPropertyData::AddPropertyTypeData<T>();
				}
			}
		};

		template <const StaticRegistration&> struct StaticRegistrationRef {};

		static const StaticRegistration staticRegistration;
		static const StaticRegistrationRef<staticRegistration> staticRegistrationRef;
	};

	template <typename T>
	const typename RegisterPropertyType<T>::StaticRegistration
	    RegisterPropertyType<T>::staticRegistration;
}
