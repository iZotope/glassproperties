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

#include "Glass/Properties/Private/GlobalPropertyData.h"

#include "Glass/Properties/Types.h"
#include "iZBase/Util/PropertySerializer.h"

IZ_PUSH_ALL_WARNINGS
#include "gtest/gtest.h"
IZ_POP_ALL_WARNINGS

namespace {
	BETTER_ENUM(TestEnum, int32_t, ThingOne, ThingTwo, Cat);
	using TestEnumType = Glass::PropertyType<TestEnum>;
}

TEST(GlobalPropertyTypeRegistration, IntRegistration) {
	Util::PropertySerializer serializer{};

	Glass::Private::GlobalPropertyData::registerGlobalPropertyTypes(serializer);

	ASSERT_TRUE(serializer.IsTypeRegistered(Glass::IntPropertyType::name));
}

TEST(GlobalPropertyTypeRegistration, EnumPropertyNameRegistration) {
	Util::PropertySerializer serializer{};

	Glass::Private::GlobalPropertyData::registerPropertyType(
	    serializer,
	    TestEnumType::name,
	    Glass::Private::GlobalPropertyData::PropertyTypeData<TestEnumType>{});

	ASSERT_TRUE(serializer.IsTypeRegistered(TestEnumType::name));
	ASSERT_TRUE(serializer.IsTypeNamedEnum(TestEnumType::name));

	auto names = serializer.GetNamedEnumTypeNames(TestEnumType::name);
	ASSERT_EQ(TestEnum::_size_constant, names.size());
	for (size_t i = 0; i < names.size(); ++i) {
		ASSERT_EQ(TestEnum::_names()[i], names[i]);
	}
}
