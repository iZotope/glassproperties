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

	struct PropertyDefaultValue {
		boost::any scratchSpace;
		boost::any value;
	};

	class PropertyDefinitionBase {
	public:
		virtual std::string GetName() const = 0;
		virtual std::string GetTypeName() const = 0;

		virtual PropertyDefaultValue GetDefaultValue() const = 0;
		virtual std::optional<DidSetType> GetDidSetFn() const = 0;

		virtual ~PropertyDefinitionBase() = 0;
	};
}

#include "Glass/Properties/Private/DidSetFactory.h"
#include "Glass/Properties/Private/getDefaultValue.h"
#include "Glass/Properties/Private/getName.h"

namespace Glass {
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
			def->m_didSet = Private::DidSetFactory<V, D>::Create(hasProperties);
			return def;
		}

		PropertyDefinition() = default;

		PropertyDefinition(const PropertyDefinition<D, U>&) = default;
		PropertyDefinition<D, U>& operator=(const PropertyDefinition<D, U>&) = default;
		PropertyDefinition(PropertyDefinition<D, U>&&) = default;
		PropertyDefinition<D, U>& operator=(PropertyDefinition<D, U>&&) = default;

		~PropertyDefinition() override = default;

		std::string GetName() const override final { return Private::getName<impl_type>(); }
		std::string GetTypeName() const override { return Private::getName<property_type>(); }
		PropertyDefaultValue GetDefaultValue() const override final {
			return Private::getDefaultValue<impl_type>();
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
}
