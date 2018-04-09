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
	constexpr float EXPECTED_FLOAT = 42.f;
}

class HasPropertiesTests : public ::testing::Test {
public:
	struct TestClass : public Glass::HasProperties<TestClass> {
		struct IntValue
		    : Glass::PropertyDefinition<IntValue, Glass::IntPropertyType, TestClass> {
			static constexpr const char* const name = "IntValue";
			static constexpr Glass::IntPropertyType::type defaultValue = EXPECTED_INT;
			static void didSet(TestClass* this_) {
				this_->latestIntValue = this_->GetProperty<IntValue>();
			}
		};
		struct FloatValue : Glass::PropertyDefinition<FloatValue, Glass::FloatPropertyType> {
			static constexpr const char* const name = "FloatValue";
			static constexpr const Glass::FloatPropertyType::type defaultValue = EXPECTED_FLOAT;
		};
		using Properties = Glass::PropertyList<IntValue, FloatValue>;

		Glass::optional<int32_t> latestIntValue;
	};

	TestClass p;
};

TEST_F(HasPropertiesTests, DefaultValue) {
	EXPECT_EQ(EXPECTED_INT, p.GetProperty<TestClass::IntValue>());
	EXPECT_EQ(EXPECTED_FLOAT, p.GetProperty<TestClass::FloatValue>());
}

TEST_F(HasPropertiesTests, SetGet) {
	p.SetProperty<TestClass::IntValue>(-5);
	p.SetProperty<TestClass::FloatValue>(5.0f);
	EXPECT_EQ(-5, p.GetProperty<TestClass::IntValue>());
	EXPECT_EQ(5.0f, p.GetProperty<TestClass::FloatValue>());
}

TEST_F(HasPropertiesTests, DidSet) {
	p.SetProperty<TestClass::IntValue>(-5);
	ASSERT_TRUE(static_cast<bool>(p.latestIntValue));
	EXPECT_EQ(-5, *p.latestIntValue);
}

TEST_F(HasPropertiesTests, SerializedValue) {
	const int serializedInt = 9;
	const float serializedFloat = -3.141f;

	p.SetSerializedValue<TestClass::IntValue>(serializedInt);
	p.SetSerializedValue<TestClass::FloatValue>(serializedFloat);

	EXPECT_EQ(serializedInt, p.GetProperty<TestClass::IntValue>());
	EXPECT_EQ(serializedInt, p.GetSerializedValue<TestClass::IntValue>());
	EXPECT_EQ(serializedFloat, p.GetProperty<TestClass::FloatValue>());
	EXPECT_EQ(serializedFloat,
	          p.GetSerializedValue<TestClass::FloatValue>());
}

TEST_F(HasPropertiesTests, SetSerializedValueDoesNotOverrideLiveValue) {
	const float serializedFloat1 = -3.141f;
	const float serializedFloat2 = -2.4f;
	const float liveFloat = 2.7f;

	p.SetSerializedValue<TestClass::FloatValue>(serializedFloat1);

	EXPECT_EQ(serializedFloat1, p.GetProperty<TestClass::FloatValue>());
	EXPECT_EQ(serializedFloat1,
	          p.GetSerializedValue<TestClass::FloatValue>());

	p.SetProperty<TestClass::FloatValue>(liveFloat);
	p.SetSerializedValue<TestClass::FloatValue>(serializedFloat2);

	EXPECT_EQ(liveFloat, p.GetProperty<TestClass::FloatValue>());
	EXPECT_EQ(serializedFloat2,
	          p.GetSerializedValue<TestClass::FloatValue>());
}
