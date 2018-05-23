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

#include "gsl/pointers"


namespace Glass {

	//! Mixin to define a PropertyDefinition that should
	//! call SetNeedsLayout() if available after calling any
	//! didSet method declared for this PropertyDefinition.
	struct LayoutProperty {};
	//! Mixin to define a PropertyDefinition that should
	//! call SetNeedsDisplay() if available after calling any
	//! didSet method declared for this PropertyDefinition.
	struct DisplayProperty {};
	//! Mixin to define a PropertyDefinition that should
	//! call SetNeedsLayout() and SetNeedsDisplay() if available after calling any
	//! didSet method declared for this PropertyDefinition.
	struct UIProperty : LayoutProperty, DisplayProperty {};

	using DidSetType = std::function<void()>;

	class PropertyDefinitionBase {
	public:
		virtual std::string GetName() const = 0;
		virtual std::string GetTypeName() const = 0;
		virtual boost::any GetDefaultValue() const = 0;
		virtual std::optional<DidSetType> GetDidSetFn() const = 0;

		virtual ~PropertyDefinitionBase() = 0;
	};

	namespace internal {
		template<typename T>
		auto getDefaultValue(typename std::enable_if<!std::is_function<decltype(T::defaultValue)>::value, T*>::type) {
			return T::defaultValue;
		}

		template <typename T>
		auto getDefaultValue(
		    typename std::enable_if<std::is_function<decltype(T::defaultValue)>::value, T*>::type) {
			return T::defaultValue();
		}
	}

	namespace Meta {
		template <typename T, typename D, typename = std::void_t<>>
		struct HasDidSet : std::false_type {};

		template <typename T, typename D>
		struct HasDidSet<T, D, std::void_t<decltype(std::declval<T>().didSet(D{}))>>
		    : std::true_type {};

		template <typename T, typename D> constexpr bool HasDidSet_v = HasDidSet<T, D>::value;

		template <typename T, typename = std::void_t<>>
		struct HasSetNeedsLayout : std::false_type {};

		template <typename T>
		struct HasSetNeedsLayout<T, std::void_t<decltype(std::declval<T>().SetNeedsLayout())>>
		    : std::true_type {};

		template <typename T> constexpr bool HasSetNeedsLayout_v = HasSetNeedsLayout<T>::value;

		template <typename T, typename = std::void_t<>>
		struct HasSetNeedsDisplay : std::false_type {};

		template <typename T>
		struct HasSetNeedsDisplay<T, std::void_t<decltype(std::declval<T>().SetNeedsDisplay())>>
		    : std::true_type {};

		template <typename T> constexpr bool HasSetNeedsDisplay_v = HasSetNeedsDisplay<T>::value;

		template <typename D> struct IsLayoutProperty {
			static constexpr bool value = std::is_base_of<LayoutProperty, D>::value;
		};

		template <typename D> constexpr bool IsLayoutProperty_v = IsLayoutProperty<D>::value;

		template <typename D> struct IsDisplayProperty {
			static constexpr bool value = std::is_base_of<DisplayProperty, D>::value;
		};

		template <typename D> constexpr bool IsDisplayProperty_v = IsDisplayProperty<D>::value;
	}

	namespace internal {
		namespace DidSetFactoryHelper {
			template <typename T, typename D,
			          typename = typename std::enable_if<Meta::HasDidSet_v<T, D>, void>::type>
			static void CallDidSet(T* obj, D def = D{}) {
				obj->didSet(std::move(def));
			}
			template <typename W, typename D,
			          typename = typename std::enable_if<!Meta::HasDidSet_v<W, D>, void>::type>
			static void CallDidSet(W*) {}

			template <typename W, typename D,
			          typename = typename std::enable_if<Meta::IsLayoutProperty_v<D>, void>::type>
			static void CallSetNeedsLayout(W* obj, D* = nullptr) {
				static_assert(
				    Meta::HasSetNeedsLayout<W>::value,
				    "W must implement SetNeedsLayout to have a property D of type LayoutProperty.");
				obj->SetNeedsLayout();
			}
			template <typename W, typename D,
			          typename = typename std::enable_if<!Meta::IsLayoutProperty_v<D>, void>::type>
			static void CallSetNeedsLayout(W*) {}

			template <typename W, typename D,
			          typename = typename std::enable_if<Meta::IsDisplayProperty_v<D>, D>::type>
			static void CallSetNeedsDisplay(W* this_, D* = nullptr) {
				static_assert(Meta::HasSetNeedsLayout<W>::value,
				              "W must implement SetNeedsDisplay to have a property D of type "
				              "DisplayProperty.");
				this_->SetNeedsDisplay();
			}
			template <typename W, typename D,
			          typename = typename std::enable_if<!Meta::IsDisplayProperty_v<D>, void>::type>
			static void CallSetNeedsDisplay(W*) {}
		}
		template <typename T, typename D, typename R = DidSetType> struct DidSetFactory {
			static R Create(T*) { return DidSetType{}; }
		};
		template <typename T, typename D>
		struct DidSetFactory<
		    T, D,
		    typename std::enable_if<Meta::HasDidSet_v<T, D> || Meta::IsLayoutProperty_v<D> ||
		                                Meta::IsDisplayProperty_v<D>,
		                            DidSetType>::type> {
			static DidSetType Create(T* hasProperties) {
				return [hasProperties]() {
					DidSetFactoryHelper::CallDidSet<T, D>(hasProperties);
					DidSetFactoryHelper::CallSetNeedsLayout<T, D>(hasProperties);
					DidSetFactoryHelper::CallSetNeedsDisplay<T, D>(hasProperties);
				};
			}
		};
	}

	//! Subclass this template to define a property
	//!
	//! Must conform to the following concept:
	//!   std::string name
	//!   U::type defaultValue or U::type defaultValue() { ... }
	//!   static void didSet(V*) (std::optional, implement and specify V type argument to provide a
	//!   callback)
	//!
	//! \param D The type inheriting from PropertyDefinition
	//! \param U The PropertyType to use for this property
	template <typename D, typename U> class PropertyDefinition : public PropertyDefinitionBase {
	public:
		using property_type = U;
		using impl_type = D;
		template <typename V> static unique_ptr<D> Create(V* hasProperties) {
			auto def = make_unique<D>();
			def->m_didSet = internal::DidSetFactory<V, D>::Create(hasProperties);
			return def;
		}

		PropertyDefinition() = default;

		PropertyDefinition(const PropertyDefinition<D, U>&) = default;
		PropertyDefinition<D, U>& operator=(const PropertyDefinition<D, U>&) = default;
		PropertyDefinition(PropertyDefinition<D, U>&&) = default;
		PropertyDefinition<D, U>& operator=(PropertyDefinition<D, U>&&) = default;

		~PropertyDefinition() override = default;

		std::string GetName() const override final { return impl_type::name; }
		std::string GetTypeName() const override { return property_type::name; }
		boost::any GetDefaultValue() const override final {
			return internal::getDefaultValue<impl_type>(nullptr);
		}
		std::optional<std::function<void()>> GetDidSetFn() const override final {
			if (!m_didSet) {
				return std::nullopt;
			}
			return m_didSet;
		}

	private:
		std::function<void()> m_didSet{};
	};

	//! Subclass this template to define an "std::optional" property
	//!
	//! Must conform to the following concept:
	//! 	std::string name
	//! 	std::optional<U::type> defaultValue or std::optional<U::type> defaultValue() { ... }
	//! 	static void didSet(V*) (std::optional, implement and specify V type argument to provide a
	//! 	callback)
	//!
	//! \param D the type inheriting from PropertyDefinition
	//! \param U the PropertyType to use for this property
	template <typename D, typename U> struct OptionalPropertyDefinition : PropertyDefinition<D, U> {
		struct OptionalPropertyType {
			using type = std::optional<typename U::type>;
		};
		using property_type = OptionalPropertyType;
		std::string GetTypeName() const override {
			return "Optional " + PropertyDefinition<D, U>::GetTypeName();
		}
	};
}
