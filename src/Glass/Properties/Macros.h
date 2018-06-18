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

#include "Glass/Properties/Private/Macros.h"

//! Declare a set of Glass::PropertyDefinition and a Glass::PropertyList in a namespace.
//!
//! To declare a set of PropertyDefinitions, use the following macro by providing
//! a namespace and at least one tuple defining a property definition.
//!
//! This macro cannot be used to create a template subtype of PropertyDefinitions.
//!
//! GLASS_PROPERTIES(<NameSpace>, <PropertyDefinitionTuple>...)
//!
//! <NameSpace> namespace name used to declare PropertyDefinitions
//!
//! <PropertyDefinitionTuple> -> (<BaseType>, <DefType>, <PropType>, <DefaultValue) |
//!                              (<BaseType>, <DefType>, <DefName>, <PropType>, <DefaultName>)
//!
//! <BaseType> -> Property | OptionalProperty | DisplayProperty | OptionalDispayProperty |
//!               LayoutProperty | OptionalLayoutProperty | UIProperty | OptionalUIProperty
//!
//!               The base type specified will create the following type of defintion:
//!               - Property: A standard property
//!               - OptionalProperty: A property with that stores an std::optional value
//!               - DisplayProperty: A property that calls SetNeedsDisplay after didSet on change
//!               - OptionalDisplayProperty: A property with std::optional value that calls
//!                   SetNeedsDisplay after any didSet method on property value change
//!               - LayoutProperty: A property that calls SetNeedsLayout after didSet on change
//!               - OptionalLayoutProperty: A property with std::optional value that calls
//!                   SetNeedsLayout after any didSet method on property value change
//!               - UIProperty: A property that calls SetNeedsDisplay after didSet on change
//!               - OptionalUIProperty: A property with std::optional value that calls SetNeedsDisplay
//!                   and SetNeedsLayout after any didSet method on property value change
//!
//! <DefType> typename for a PropertyDefinition derived type declaration
//!
//! <DefName> const char* literal human readable name for the property; if specified, this
//!           name will be used as the property name in layout files and the DesignAid.
//!           If DefName is not provided, DefType will be used as the property name
//!           in layout files and the DesignAid.
//!
//! <PropType> The type of the property (derived from Glass::PropertyType)
//!
//! <DefaultValue> The property's default value
#define GLASS_PROPERTIES(NameSpace, ...)                                                           \
	namespace NameSpace {                                                                          \
		GLASS_PS(BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__))                                           \
		using List = GLASS_PL(__VA_ARGS__);                                                        \
	}
