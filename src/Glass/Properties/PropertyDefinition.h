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


namespace Glass {
	namespace internal {
		struct PropertyDefinition {
			virtual std::string GetName() const = 0;
			virtual std::string GetTypeName() const = 0;
			virtual boost::any GetDefaultValue() const = 0;
			virtual std::optional<std::function<void()>> GetDidSetFn(boost::any) const = 0;
		};
	}

	//! Subclass to define a property
	//!
	//! Must conform to the following concept:
	//!   std::string name
	//!   U::type defaultValue
	//!   static void didSet(V*) (std::optional, implement and specify V type argument to provide a
	//!   callback)
	//!
	//! typename T: The type inheriting from PropertyDefinition
	//! typename U: The PropertyType to use for this property
	//! typename V: The type that has this property, only necessary if providing a didSet(V*) static
	//! function
	template <typename T, typename U, typename V = void>
	struct PropertyDefinition : internal::PropertyDefinition {
	public:
		using property_type = U;
		using impl_type = T;

		std::string GetName() const override final { return impl_type::name; }
		std::string GetTypeName() const override final { return property_type::name; }
		boost::any GetDefaultValue() const override final { return impl_type::defaultValue; }
		std::optional<std::function<void()>> GetDidSetFn(boost::any this_) const override final {
			if constexpr (std::is_void<V>::value) {
				return std::nullopt;
			} else {
				V* vThis;
				try {
					vThis = boost::any_cast<V*>(this_);
					std::function<void()> fn = std::bind(&T::didSet, vThis);
					return fn;
				} catch (boost::bad_any_cast&) {
					ZERROR("Failed to cast this to correct type to generate didSet callback.");
					return std::nullopt;
				}
			}
		}
	};
}
