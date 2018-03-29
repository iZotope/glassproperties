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

	template <typename H, typename T> struct PropertyList {
		using head = H;
		using tail = T;
	};

	template <typename T> struct PropertyListBuilder {};

	template <typename H, typename T>
	struct PropertyListBuilder<PropertyList<H, T>> : PropertyListBuilder<T> {
		static vector<unique_ptr<internal::PropertyDefinition>> Build() {
			auto propertyDefinitionList = PropertyListBuilder<T>::Build();
			propertyDefinitionList.emplace_back(new H{});
			return propertyDefinitionList;
		}
	};

	template <typename H> struct PropertyListBuilder<PropertyList<H, NullPropertyList>> {
		static vector<unique_ptr<internal::PropertyDefinition>> Build() {
			vector<unique_ptr<internal::PropertyDefinition>> propertyDefinitionList;
			propertyDefinitionList.emplace_back(new H{});
			return propertyDefinitionList;
		}
	};
}
