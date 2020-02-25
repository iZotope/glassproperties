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

#include "Glass/Properties/HasProperties.h"
#include "Glass/Properties/Macros.h"
#include "Glass/Properties/PropertyListMeta.h"
#include "Glass/Properties/Types.h"

IZ_PUSH_ALL_WARNINGS
#include "gtest/gtest.h"
IZ_POP_ALL_WARNINGS

namespace {
	const int32_t DEFAULT_INT = 1;

	struct TestClass;

	GLASS_PROPERTIES(TestClassProperties,
	                 (Property, IntValue1, Glass::IntPropertyType, DEFAULT_INT),
	                 (Property, IntValue2, Glass::IntPropertyType, DEFAULT_INT),
	                 (Property, IntValue3, Glass::IntPropertyType, DEFAULT_INT));

	struct TestClass : public Glass::HasPropertiesBase,
	                   public Glass::HasProperties<TestClass, TestClassProperties::List> {

		using Glass::HasProperties<TestClass, TestClassProperties::List>::GetProperty;
		using Glass::HasProperties<TestClass, TestClassProperties::List>::SetProperty;
	};
}

class PropertiesMetaTest : public ::testing::Test {
public:
	TestClass p;
};

TEST_F(PropertiesMetaTest, SetAllProps) {
	const int32_t EXPECTED_INT = 5;
	EXPECT_EQ(DEFAULT_INT, p.GetProperty<TestClassProperties::IntValue1>());
	EXPECT_EQ(DEFAULT_INT, p.GetProperty<TestClassProperties::IntValue2>());
	EXPECT_EQ(DEFAULT_INT, p.GetProperty<TestClassProperties::IntValue3>());
	Glass::Meta::SetAllProps(p,
	                         EXPECTED_INT,
	                         Glass::PropertyList<TestClassProperties::IntValue1,
	                                             TestClassProperties::IntValue2,
	                                             TestClassProperties::IntValue3>{});
	EXPECT_EQ(EXPECTED_INT, p.GetProperty<TestClassProperties::IntValue1>());
	EXPECT_EQ(EXPECTED_INT, p.GetProperty<TestClassProperties::IntValue2>());
	EXPECT_EQ(EXPECTED_INT, p.GetProperty<TestClassProperties::IntValue3>());
}
