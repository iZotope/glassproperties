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
#include "Glass/Properties/Types.h"

IZ_PUSH_ALL_WARNINGS
#include "gtest/gtest.h"
IZ_POP_ALL_WARNINGS

namespace {
	const int32_t EXPECTED_INT = 65;
	const float EXPECTED_FLOAT = 42.f;
}

class PropertyHolderTests : public ::testing::Test {
public:
	struct PropertyHolder : public Glass::HasProperties<PropertyHolder> {
		struct IntValue
		    : Glass::PropertyDefinition<IntValue, Glass::IntPropertyType, PropertyHolder> {
			inline static const auto name = "IntValue";
			inline static const Glass::IntPropertyType::type defaultValue = EXPECTED_INT;
			static void didSet(PropertyHolder* this_) {
				this_->latestIntValue = this_->GetProperty<IntValue>();
			}
		};
		struct FloatValue : Glass::PropertyDefinition<FloatValue, Glass::FloatPropertyType> {
			inline static const auto name = "FloatValue";
			inline static const Glass::FloatPropertyType::type defaultValue = EXPECTED_FLOAT;
		};
		using Properties = Glass::PropertyList<IntValue, FloatValue>;

		Glass::optional<int32_t> latestIntValue;
	};

	PropertyHolder p;
};

TEST_F(PropertyHolderTests, DefaultValue) {
	EXPECT_EQ(EXPECTED_INT, p.GetProperty<PropertyHolder::IntValue>());
	EXPECT_EQ(EXPECTED_FLOAT, p.GetProperty<PropertyHolder::FloatValue>());
}

TEST_F(PropertyHolderTests, SetGet) {
	p.SetProperty<PropertyHolderTests::PropertyHolder::IntValue>(-5);
	p.SetProperty<PropertyHolderTests::PropertyHolder::FloatValue>(5.0f);
	EXPECT_EQ(-5, p.GetProperty<PropertyHolderTests::PropertyHolder::IntValue>());
	EXPECT_EQ(5.0f, p.GetProperty<PropertyHolderTests::PropertyHolder::FloatValue>());
}

TEST_F(PropertyHolderTests, DidSet) {
	p.SetProperty<PropertyHolderTests::PropertyHolder::IntValue>(-5);
	ASSERT_TRUE(static_cast<bool>(p.latestIntValue));
	EXPECT_EQ(-5, *p.latestIntValue);
}

TEST_F(PropertyHolderTests, SerializedValue) {
	const int serializedInt = 9;
	const float serializedFloat = -3.141f;

	p.SetSerializedValue<PropertyHolderTests::PropertyHolder::IntValue>(serializedInt);
	p.SetSerializedValue<PropertyHolderTests::PropertyHolder::FloatValue>(serializedFloat);

	EXPECT_EQ(serializedInt, p.GetProperty<PropertyHolderTests::PropertyHolder::IntValue>());
	EXPECT_EQ(serializedInt, p.GetSerializedValue<PropertyHolderTests::PropertyHolder::IntValue>());
	EXPECT_EQ(serializedFloat, p.GetProperty<PropertyHolderTests::PropertyHolder::FloatValue>());
	EXPECT_EQ(serializedFloat,
	          p.GetSerializedValue<PropertyHolderTests::PropertyHolder::FloatValue>());
}

TEST_F(PropertyHolderTests, SetSerializedValueDoesNotOverrideLiveValue) {
	const float serializedFloat1 = -3.141f;
	const float serializedFloat2 = -2.4f;
	const float liveFloat = 2.7f;

	p.SetSerializedValue<PropertyHolderTests::PropertyHolder::FloatValue>(serializedFloat1);

	EXPECT_EQ(serializedFloat1, p.GetProperty<PropertyHolderTests::PropertyHolder::FloatValue>());
	EXPECT_EQ(serializedFloat1,
	          p.GetSerializedValue<PropertyHolderTests::PropertyHolder::FloatValue>());

	p.SetProperty<PropertyHolderTests::PropertyHolder::FloatValue>(liveFloat);
	p.SetSerializedValue<PropertyHolderTests::PropertyHolder::FloatValue>(serializedFloat2);

	EXPECT_EQ(liveFloat, p.GetProperty<PropertyHolderTests::PropertyHolder::FloatValue>());
	EXPECT_EQ(serializedFloat2,
	          p.GetSerializedValue<PropertyHolderTests::PropertyHolder::FloatValue>());
}
