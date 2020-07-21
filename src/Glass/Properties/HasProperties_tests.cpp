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
#include "iZBase/Util/PropertySerializer.h"

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

	struct GlobalIntProperty
	    : Glass::PropertyDefinition<GlobalIntProperty, Glass::IntPropertyType> {
		static constexpr const char* const name = "IntValue";
		static constexpr Glass::IntPropertyType::type defaultValue = EXPECTED_INT;
	};

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
	struct BackgroundColorMixin : public Glass::HasProperties<T, BGProperties> {};


	using IntValue = GlobalIntProperty;


	struct FloatValue : Glass::PropertyDefinition<FloatValue, Glass::FloatPropertyType> {
		static constexpr const char* const name = "FloatValue";
		static constexpr const Glass::FloatPropertyType::type defaultValue = EXPECTED_FLOAT;
	};


	using Properties = Glass::PropertyList<IntValue, FloatValue>;

	struct TestClass : public Glass::HasPropertiesBase,
	                   public Glass::HasProperties<TestClass, Properties>,
	                   public BackgroundColorMixin<TestClass> {

		using Glass::HasProperties<TestClass, Properties>::GetProperty;
		using BackgroundColorMixin<TestClass>::GetProperty;
		using Glass::HasProperties<TestClass, Properties>::SetProperty;
		using BackgroundColorMixin<TestClass>::SetProperty;

		void didSet(IntValue) { latestIntValue = GetProperty<IntValue>(); }

		Glass::optional<int32_t> latestIntValue;
		Glass::optional<Glass::Color> latestBackgroundColorValue;
	};

	String serializeInt(int nValue) { return String("%1").Arg(nValue); }
	checked_int deserializeInt(const String& strValue) { return strValue.ToInt(); }
}

static_assert(!Glass::Meta::HasDidSet_v<TestClass, FloatValue>,
              "TestClass has a didSet function for FloatValue");
static_assert(Glass::Meta::HasDidSet_v<TestClass, IntValue>,
              "TestClass does not have a didSet function for IntValue");
static_assert(std::is_assignable<Glass::PropertyDefinitionBase, IntValue>::value,
              "Can't assign IntValue to Glass::PropertyDefinitionBase");

class HasPropertiesTests : public ::testing::Test {
public:
	void SetUp() override {
		stylesheet = make_shared<Util::StyleSheet>(&serializer);
		serializer.RegisterType<int>("Int", &serializeInt, &deserializeInt);
		// serializer.RegisterType<String>("String", &serializeString, &deserializeString);
	}

	TestClass p;
	Util::PropertySerializer serializer;
	shared_ptr<Util::StyleSheet> stylesheet;
	std::string className = "Class";
	std::string anotherClassName = "Another Class";
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

TEST_F(HasPropertiesTests, Mixin) {
	ASSERT_EQ(EXPECTED_INT, p.GetProperty<IntValue2>());
	ASSERT_EQ(EXPECTED_COLOR, p.GetProperty<BackgroundColor>());
	p.SetProperty<IntValue2>(-2);
	ASSERT_EQ(EXPECTED_INT, p.GetProperty<IntValue>());
	ASSERT_EQ(-2, p.GetProperty<IntValue2>());
}

TEST_F(HasPropertiesTests, BasicStylesheet) {
    stylesheet->AddProperty(className, "IntValue", "Int", "5");

    p.SetStyleSheet(stylesheet);
	p.SetClassNames({className});
    EXPECT_EQ(5, p.GetProperty<IntValue>());
}

TEST_F(HasPropertiesTests, DoesOrderMatter) {
    stylesheet->AddProperty(className, "IntValue", "Int", "5");

	p.SetClassNames({className});
    p.SetStyleSheet(stylesheet);
    EXPECT_EQ(5, p.GetProperty<IntValue>());
}

TEST_F(HasPropertiesTests, MultipleClasses) {
	stylesheet->AddProperty(className, "IntValue", "Int", "5");
	stylesheet->AddProperty(anotherClassName, "IntValue2", "Int", "50");

	p.SetClassNames({className, anotherClassName});
	p.SetStyleSheet(stylesheet);
	EXPECT_EQ(5, p.GetProperty<IntValue>());
	EXPECT_EQ(50, p.GetProperty<IntValue2>());
}
