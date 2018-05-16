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

		template<typename T>
		auto getDefaultValue(typename std::enable_if<!std::is_function<decltype(T::defaultValue)>::value, T*>::type) {
			return T::defaultValue;
		}
		
		template<typename T>
		auto getDefaultValue(typename std::enable_if<std::is_same<decltype((T::defaultValue(), 4)), decltype(4)>::value, T*>::type) {
			return T::defaultValue();
		}
	}

	//! Subclass this template to define a property
	//!
	//! Must conform to the following concept:
	//!   std::string name
	//!   U::type defaultValue or U::type defaultValue() { ... }
	//!   static void didSet(V*) (std::optional, implement and specify V type argument to provide a
	//!   callback)
	//!
	//! \param T The type inheriting from PropertyDefinition
	//! \param U The PropertyType to use for this property
	//! \param V The type that has this property, only necessary if providing a didSet(V*) static
	//! function
	template <typename T, typename U, typename V = void>
	struct PropertyDefinition : internal::PropertyDefinition {
		using property_type = U;
		using impl_type = T;

		std::string GetName() const override final { return impl_type::name; }
		std::string GetTypeName() const override { return property_type::name; }
		boost::any GetDefaultValue() const override final { return internal::getDefaultValue<impl_type>(nullptr); }
		std::optional<std::function<void()>> GetDidSetFn(boost::any this_) const override final {
			return getDidSetFn<V>(this_);
		}

	private:
		template <typename W, typename R = typename std::enable_if<std::is_void<W>::value, std::nullopt_t>::type>
		R getDidSetFn(boost::any, float = 0) const {
			return std::nullopt;
		}

		template <typename W, typename R = typename std::enable_if<!std::is_void<W>::value,
		                                      std::optional<std::function<void()>>>::type>
		R getDidSetFn(boost::any this_, int = 0) const {
			V** pvThis{nullptr};
            pvThis = boost::any_cast<V*>(&this_);
			if (!pvThis) {
				ZERROR("Failed to cast this to correct type to generate didSet callback.");
				return std::nullopt;
			}
			std::function<void()> fn = [vThis = *pvThis]() { T::didSet(vThis); };

			return fn;
		}
	};

	//! Subclass this template to define an "std::optional" property
	//!
	//! Must conform to the following concept:
	//! 	std::string name
	//! 	std::optional<U::type> defaultValue or std::optional<U::type> defaultValue() { ... }
	//! 	static void didSet(V*) (std::optional, implement and specify V type argument to provide a
	//! 	callback)
	//!
	//! \param T the type inheriting from PropertyDefinition
	//! \param U the PropertyType to use for this property
	//! \param V the type that has this property, only necessary if providing a didSet(V*) static
	//! function
	template <typename T, typename U, typename V = void>
	struct OptionalPropertyDefinition : PropertyDefinition<T, U, V> {
		struct OptionalPropertyType {
			using type = std::optional<typename U::type>;
		};
		using property_type = OptionalPropertyType;
		std::string GetTypeName() const override {
			return "Optional " + PropertyDefinition<T, U, V>::GetTypeName();
		}
	};
}
