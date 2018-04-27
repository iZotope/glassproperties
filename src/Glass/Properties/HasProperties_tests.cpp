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

#include "Glass/Color.h"
#include "Glass/Properties/HasProperties.h"
#include "Glass/Properties/HasPropertiesBase.h"
#include "Glass/Properties/Types.h"

IZ_PUSH_ALL_WARNINGS
#include "gtest/gtest.h"
IZ_POP_ALL_WARNINGS

namespace {
	const int32_t EXPECTED_INT = 65;
	constexpr float EXPECTED_FLOAT = 42.f;
	constexpr Glass::Color EXPECTED_COLOR{1.f, .8f, .2f};
}

namespace {
	struct TestClass;

	struct BackgroundColor : Glass::PropertyDefinition<BackgroundColor, Glass::ColorPropertyType> {
		static constexpr const char* const name = "Background Color";
		static constexpr Glass::ColorPropertyType::type defaultValue = EXPECTED_COLOR;
	};
	struct IntValue2 : Glass::PropertyDefinition<IntValue2, Glass::IntPropertyType> {
		static constexpr const char* const name = "IntValue2";
		static constexpr Glass::IntPropertyType::type defaultValue = EXPECTED_INT;
	};

	using BGProperties = Glass::PropertyList<BackgroundColor, IntValue2>;

	template <typename T>
	struct BackgroundColorMixin : public Glass::HasProperties<BGProperties, T> {

	    };
	struct IntValue : Glass::PropertyDefinition<IntValue, Glass::IntPropertyType, TestClass> {
		static constexpr const char* const name = "IntValue";
		static constexpr Glass::IntPropertyType::type defaultValue = EXPECTED_INT;
		template <typename T> static void didSet(T* this_) {
			this_->latestIntValue = this_->template GetProperty<IntValue>();
		}
	};
	struct FloatValue : Glass::PropertyDefinition<FloatValue, Glass::FloatPropertyType> {
		static constexpr const char* const name = "FloatValue";
		static constexpr const Glass::FloatPropertyType::type defaultValue = EXPECTED_FLOAT;
	};
	using Properties = Glass::PropertyList<IntValue, FloatValue>;

	struct TestClass
	    : public Glass::HasPropertiesBase,
	      public Glass::HasProperties<Properties,TestClass> , public BackgroundColorMixin<TestClass>
	{

		using Glass::HasProperties<Properties,TestClass>::GetProperty;
		using BackgroundColorMixin<TestClass>::GetProperty;
		using Glass::HasProperties<Properties,TestClass>::SetProperty;
		using BackgroundColorMixin<TestClass>::SetProperty;
		using Glass::HasProperties<Properties,TestClass>::GetSerializedValue;
		using BackgroundColorMixin<TestClass>::GetSerializedValue;
		using Glass::HasProperties<Properties,TestClass>::SetSerializedValue;
		using BackgroundColorMixin<TestClass>::SetSerializedValue;

		

		Glass::optional<int32_t> latestIntValue;
		Glass::optional<Glass::Color> latestBackgroundColorValue;
	};
}

class HasPropertiesTests : public ::testing::Test {
public:
	TestClass p;
};

TEST_F(HasPropertiesTests, DefaultValue) {
	EXPECT_EQ(EXPECTED_INT, p.GetProperty<IntValue>());
	EXPECT_EQ(EXPECTED_FLOAT, p.GetProperty<FloatValue>());
}

TEST_F(HasPropertiesTests, SetGet) {
	p.SetProperty<IntValue>(-5);
	p.SetProperty<FloatValue>(5.0f);
	EXPECT_EQ(-5, p.GetProperty<IntValue>());
	EXPECT_EQ(5.0f, p.GetProperty<FloatValue>());
}


 TEST_F(HasPropertiesTests, DidSet) {
	p.SetProperty<IntValue>(-5);
	ASSERT_TRUE(static_cast<bool>(p.latestIntValue));
	EXPECT_EQ(-5, *p.latestIntValue);
}

TEST_F(HasPropertiesTests, SerializedValue) {
	const int serializedInt = 9;
	const float serializedFloat = -3.141f;

	p.SetSerializedValue<IntValue>(serializedInt);
	p.SetSerializedValue<FloatValue>(serializedFloat);

	EXPECT_EQ(serializedInt, p.GetProperty<IntValue>());
	EXPECT_EQ(serializedInt, p.GetSerializedValue<IntValue>());
	EXPECT_EQ(serializedFloat, p.GetProperty<FloatValue>());
	EXPECT_EQ(serializedFloat, p.GetSerializedValue<FloatValue>());
}

TEST_F(HasPropertiesTests, SetSerializedValueDoesNotOverrideLiveValue) {
	const float serializedFloat1 = -3.141f;
	const float serializedFloat2 = -2.4f;
	const float liveFloat = 2.7f;

	p.SetSerializedValue<FloatValue>(serializedFloat1);

	EXPECT_EQ(serializedFloat1, p.GetProperty<FloatValue>());
	EXPECT_EQ(serializedFloat1, p.GetSerializedValue<FloatValue>());

	p.SetProperty<FloatValue>(liveFloat);
	p.SetSerializedValue<FloatValue>(serializedFloat2);

	EXPECT_EQ(liveFloat, p.GetProperty<FloatValue>());
	EXPECT_EQ(serializedFloat2, p.GetSerializedValue<FloatValue>());
}

TEST_F(HasPropertiesTests, Mixin) {
	ASSERT_EQ(EXPECTED_INT, p.GetProperty<IntValue2>());
	ASSERT_EQ(EXPECTED_COLOR, p.GetProperty<BackgroundColor>());
	p.SetProperty<IntValue2>(-2);
	ASSERT_EQ(EXPECTED_INT, p.GetProperty<IntValue>());
	ASSERT_EQ(-2, p.GetProperty<IntValue2>());
}

