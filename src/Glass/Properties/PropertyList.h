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

	//! Type to describe a list of property definitions
	//!
	//! \param Ts List of PropertyDefinition
	template <typename... Ts> struct PropertyList {};

	namespace internal {
		template <typename T, typename... LTs>
		constexpr bool is_type_in_list(PropertyList<LTs...>) {
			return (std::is_same_v<T, LTs> || ...);
		}
	}

	//! Used to determine if a PropertyDefinition is a member of a PropertyList
	//!
	//! \param L PropertyList to verify definition membership
	//! \param T PropertyDefinition to be verified
	template <typename L, typename T>
	constexpr inline bool PropertyListHasType = internal::is_type_in_list<T>(L{});

	template <typename T> constexpr inline bool IsPropertyList = false;

	template <typename... Ts> constexpr inline bool IsPropertyList<PropertyList<Ts...>> = true;


	//! A vector of instantiated PropertyDefinition
	using PropertyDefinitionList = vector<unique_ptr<PropertyDefinitionBase>>;

	template <typename V>
	inline PropertyDefinitionList CreatePropertyDefinitionListForPropertyList(V*, PropertyList<>) {
		PropertyDefinitionList propertyDefinitionList;
		propertyDefinitionList.reserve(20u);
		return propertyDefinitionList;
	}

	template <typename V, typename T, typename... Ts>
	inline PropertyDefinitionList
	CreatePropertyDefinitionListForPropertyList(V* hasProperties, PropertyList<T, Ts...>) {
		auto propertyDefinitionList =
		    CreatePropertyDefinitionListForPropertyList(hasProperties, PropertyList<Ts...>{});
		propertyDefinitionList.emplace_back(T::Create(hasProperties));
		return propertyDefinitionList;
	}


}
