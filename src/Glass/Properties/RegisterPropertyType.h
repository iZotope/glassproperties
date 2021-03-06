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
#include "Glass/Properties/Private/RegisterPropertyType.h"
#include "Glass/Properties/Types/OptionalProperty.h"

// clang-format off

#define GLASS_REGISTER_PROPERTY_TYPE(Type)                                                                  \
	namespace {                                                                                             \
		[[maybe_unused]] const void* g_PropertyRegistrationData##Type =                                     \
		    reinterpret_cast<const void*>(&Glass::Private::RegisterPropertyType<Type>::staticRegistration);                                       \
		using PropertyRegistrationType_Optional##Type = Glass::OptionalProperty<Type>;                      \
		[[maybe_unused]] const void* g_PropertyRegistrationDataOptional##Type =                             \
		    reinterpret_cast<const void*>(&Glass::Private::RegisterPropertyType<PropertyRegistrationType_Optional##Type>::staticRegistration);    \
	}

// clang-format on
