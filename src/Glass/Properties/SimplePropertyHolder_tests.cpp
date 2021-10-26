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

#include "Glass/Properties/SimplePropertyHolder.h"

IZ_PUSH_ALL_WARNINGS
#include "gtest/gtest.h"
IZ_POP_ALL_WARNINGS

namespace {
	using Glass::SimplePropertyHolder;

	TEST(SimplePropertyHolderTests, CreateProperty) {
		auto ph = SimplePropertyHolder{};
		ASSERT_TRUE(ph.CreateProperty("Foo", "Int", 42));
	}

	TEST(SimplePropertyHolderTests, RecreateCreateProperty) {
		auto ph = SimplePropertyHolder{};
		ph.CreateProperty("Foo", "Int", 42);
		ASSERT_FALSE(ph.CreateProperty("Foo", "Int", 42));
	}

	TEST(SimplePropertyHolderTests, CreateTwoProperty) {
		auto ph = SimplePropertyHolder{};
		ph.CreateProperty("Foo", "Int", 42);
		ASSERT_TRUE(ph.CreateProperty("Bar", "Int", -3));
	}

	TEST(SimplePropertyHolderTests, GetCorrectTypeReturnsValidResult) {
		auto ph = SimplePropertyHolder{};
		ph.CreateProperty("Foo", "Int", 42);
		ASSERT_TRUE(ph.GetProperty<int>("Foo"));
	}

	TEST(SimplePropertyHolderTests, GetDefault) {
		auto ph = SimplePropertyHolder{};
		ph.CreateProperty("Foo", "Int", 42);
		ASSERT_EQ(42, *ph.GetProperty<int>("Foo"));
	}

	TEST(SimplePropertyHolderTests, GetAnotherDefault) {
		auto ph = SimplePropertyHolder{};
		ph.CreateProperty("Bar", "Int", -10);
		ASSERT_EQ(-10, *ph.GetProperty<int>("Bar"));
	}

	TEST(SimplePropertyHolderTests, MissingPropertyReturnsInvalidResult) {
		auto ph = SimplePropertyHolder{};
		ASSERT_FALSE(ph.GetProperty<int>("Foo"));
	}

	TEST(SimplePropertyHolderTests, GetIncorrectTypeReturnsValidResult) {
		auto ph = SimplePropertyHolder{};
		ph.CreateProperty("Foo", "Int", 42);
		ASSERT_FALSE(ph.GetProperty<float>("Foo"));
	}

	TEST(SimplePropertyHolderTests, SetValue) {
		auto ph = SimplePropertyHolder{};
		ph.CreateProperty("Foo", "Int", 42);
		ASSERT_TRUE(ph.SetProperty("Foo", 500));
	}

	TEST(SimplePropertyHolderTests, SetValueMissingProperty) {
		auto ph = SimplePropertyHolder{};
		ASSERT_FALSE(ph.SetProperty("Foo", 500));
	}

	TEST(SimplePropertyHolderTests, SetValueWrongType) {
		auto ph = SimplePropertyHolder{};
		ph.CreateProperty("Foo", "Int", 42);
		ASSERT_FALSE(ph.SetProperty("Foo", 3.14));
	}

	TEST(SimplePropertyHolderTests, SetValueGetValue) {
		auto ph = SimplePropertyHolder{};
		ph.CreateProperty("Foo", "Int", 42);
		ph.SetProperty("Foo", 500);
		ASSERT_EQ(500, *ph.GetProperty<int>("Foo"));
	}

	TEST(SimplePropertyHolderTests, SetDifferentValueGetValue) {
		auto ph = SimplePropertyHolder{};
		ph.CreateProperty("Foo", "Int", 42);
		ph.SetProperty("Foo", 4324);
		ASSERT_EQ(4324, *ph.GetProperty<int>("Foo"));
	}

	TEST(SimplePropertyHolderTests, Signal) {
		auto ph = SimplePropertyHolder{};
		ph.CreateProperty("Foo", "Int", 42);

		bool didFireSignal = false;
		Trackable t{};
		ph.GetPropertySignal("Foo").Connect(&t, [&] { didFireSignal = true; });

		ph.SetProperty("Foo", 123);
		ASSERT_TRUE(didFireSignal);
	}

	TEST(SimplePropertyHolderTests, MutlipleSignals) {
		auto ph = SimplePropertyHolder{};
		ph.CreateProperty("Foo", "Int", 42);

		bool didFireFooSignal = false;
		Trackable t{};
		ph.GetPropertySignal("Foo").Connect(&t, [&] { didFireFooSignal = true; });

		ph.CreateProperty("Bar", "Double", 3.14);

		bool didFireBarSignal = false;
		ph.GetPropertySignal("Bar").Connect(&t, [&] { didFireBarSignal = true; });

		ph.SetProperty("Foo", 123);
		ASSERT_TRUE(didFireFooSignal);
		ASSERT_FALSE(didFireBarSignal);

		ph.SetProperty("Bar", -1.23);
		ASSERT_TRUE(didFireFooSignal);
		ASSERT_TRUE(didFireBarSignal);
	}

}
