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


#include "Glass/Properties/PropertyList.h"

#include "Glass/Properties/HasProperties.h"
#include "Glass/Properties/PropertyDefinition.h"
#include "Glass/Properties/Types.h"

using namespace Glass;

namespace detail {
	struct ThingOne : public PropertyDefinition<ThingOne, IntPropertyType> {
		static constexpr auto name = "Thing 1";
		static constexpr Glass::IntPropertyType::type defaultValue = 1;
	};
	struct ThingTwo : public PropertyDefinition<ThingTwo, IntPropertyType> {
		static constexpr auto name = "Thing 2";
		static constexpr Glass::IntPropertyType::type defaultValue = 2;
	};
	struct Cat : public PropertyDefinition<ThingOne, IntPropertyType> {
		static constexpr auto name = "Cat";
		static constexpr Glass::IntPropertyType::type defaultValue = 0;
	};

	using Properties = PropertyList<ThingOne, ThingTwo>;

	template <typename... T>
	struct TestClassT : public HasPropertiesBase, public HasProperties<TestClassT<T...>> {};
}

namespace Glass {
	template <typename... T> struct PropertiesOf<detail::TestClassT<T...>> {
		using type = detail::Properties;
	};
}

namespace detail {
	template struct TestClassT<>;
}
using TestClass = detail::TestClassT<>;

static_assert(PropertyListHasType<typename PropertiesOf<TestClass>::type, detail::ThingOne>::value,
              "PropertyListHasType<Properties, ThingOne>::value == false, "
              "should be true.");
static_assert(PropertyListHasType<typename PropertiesOf<TestClass>::type, detail::ThingTwo>::value,
              "PropertyListHasType<Properties, ThingOne>::value == false, "
              "should be true.");
static_assert(!PropertyListHasType<typename PropertiesOf<TestClass>::type, detail::Cat>::value,
              "PropertyListHasType<Properties, ThingOne>::value == true, "
              "should be false.");
