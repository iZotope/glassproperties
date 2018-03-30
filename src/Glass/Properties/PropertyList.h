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

#include "Glass/Properties/PropertyDefinition.h"

namespace Glass {

	struct NullPropertyList {};

	template <typename... Ts> struct PropertyList {};

	template <typename T> struct PropertyListBuilder {};

	using PropertyDefinitionList = vector<unique_ptr<internal::PropertyDefinition>>;

	inline PropertyDefinitionList CreatePropertyDefinitionListForPropertyList(PropertyList<>) {
		PropertyDefinitionList propertyDefinitionList;
		propertyDefinitionList.reserve(20u);
		return propertyDefinitionList;
	}

	template <typename T, typename... Ts>
	inline PropertyDefinitionList
	    CreatePropertyDefinitionListForPropertyList(PropertyList<T, Ts...>) {
		auto propertyDefinitionList =
		    CreatePropertyDefinitionListForPropertyList(PropertyList<Ts...>{});
		propertyDefinitionList.emplace_back(make_unique<T>());
		return propertyDefinitionList;
	}
}
