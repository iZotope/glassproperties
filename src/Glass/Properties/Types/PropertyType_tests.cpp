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


#include "iZBase/common/common.h"

#include "Glass/Properties/Types/PropertyType.h"

using namespace Glass;

namespace {
	template <typename T, typename = std::void_t<>>
	struct IsBetterEnumPropertyType : std::false_type {};

	template <typename T>
	struct IsBetterEnumPropertyType<
	    T,
	    std::void_t<decltype(T::name), decltype(&T::serialize), decltype(&T::deserialize)>>
	    : std::true_type {};

	BETTER_ENUM(TestEnum, int32_t, ThingOne, ThingTwo);

	using TestEnumType = PropertyType<TestEnum>;

	static_assert(IsBetterEnumPropertyType<TestEnumType>::value,
	              "TestEnum did not create a specialized PropertyType for a BETTER_ENUM.");
}
