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

#include "Glass/Properties/HasPropertiesBase.h"
#include "Glass/Properties/PropertyDefinition.h"
#include "Glass/Properties/Private/CreateProperties.h"
#include "Glass/Properties/Private/getName.h"
#include "Glass/Properties/PropertyList.h"


namespace Glass {
	//! Mixin for object that implement glass properties
	//!
	//! \param U Type inheriting from HasProperties<Ps,U>; must be a subclass of HasPropertiesBase
	//! \param Ps PropertyList type representing the list of properties held by this class
	template <typename U, typename Ps> class HasProperties {
		static_assert(IsPropertyList<Ps>, "Ps must be a PropertyList");

	public:
		template <typename P>
		std::enable_if_t<PropertyListHasType<Ps, P>, typename P::property_type::type>
		GetProperty() const {
			auto maybeValue =
			    getPropertyHolder().template GetProperty<typename P::property_type::type>(
			        Private::getName<P>());
			ZASSERT(maybeValue);
			return *maybeValue;
		}

		template <typename P>
		std::enable_if_t<PropertyListHasType<Ps, P>, void>
		SetProperty(typename P::property_type::type value) {
			const auto success =
			    getPropertyHolder().template SetProperty(Private::getName<P>(), value);
			ZASSERT(success);
		}


	protected:
		HasProperties() {
			static_assert(std::is_base_of<HasPropertiesBase, U>::value,
			              "U must derive from HasPropertiesBase");
			createProperties(Ps{});
		}

		void didSet(void*) {}

	private:
		template <typename P> void createProperty() {
			constexpr bool shouldCallSetNeedsDisplay =
			    Meta::HasSetNeedsDisplay<U> && Meta::IsDisplayProperty<U>;
			constexpr bool shouldCallSetNeedsLayout =
			    Meta::HasSetNeedsLayout<U> && Meta::IsLayoutProperty<U>;

			auto defaultValue = Private::getDefaultValue<P>();
			const auto success =
			    getPropertyHolder().CreateProperty(Private::getName<P>(),
			                                       Private::getName<typename P::property_type>(),
			                                       std::move(defaultValue.value),
			                                       std::move(defaultValue.scratchSpace));
			ZASSERT(success);
			if constexpr (Meta::HasDidSet<U, P> || shouldCallSetNeedsLayout ||
			              shouldCallSetNeedsDisplay) {
				getPropertyHolder()
				    .GetPropertySignal(Private::getName<P>())
				    .Connect(&getTrackable(), [this] {
					    if constexpr (Meta::HasDidSet<U, P>) {
						    static_cast<U*>(this)->didSet(P{});
					    }
					    if constexpr (shouldCallSetNeedsLayout) {
						    static_cast<U*>(this)->SetNeedsLayout();
					    }
					    if constexpr (shouldCallSetNeedsDisplay) {
						    static_cast<U*>(this)->SetNeedsDisplay();
					    }
				    });
			}
		}

		template <typename... P> void createProperties(PropertyList<P...>) {
			(createProperty<P>(), ...);
		}

		const SimplePropertyHolder& getPropertyHolder() const {
			return *static_cast<const U*>(this)->m_propertyHolder;
		}

		SimplePropertyHolder& getPropertyHolder() {
			return *static_cast<U*>(this)->m_propertyHolder;
		}

		Trackable& getTrackable() { return static_cast<U*>(this)->HasPropertiesBase::m_trackable; }
	};
}
