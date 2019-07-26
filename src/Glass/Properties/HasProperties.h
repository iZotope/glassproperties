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
#include "Glass/Properties/Private/getName.h"
#include "Glass/Properties/PropertyList.h"


namespace Glass {
	//! Mixin for object that implement glass properties
	//!
	//! \param U Type inheriting from HasProperties<Ps,U>; must be a subclass of HasPropertiesBase
	//! \param Ps PropertyList type representing the list of properties held by this class
	template <typename U, typename Ps> class HasProperties {
		static_assert(IsPropertyList<Ps>::value, "Ps must be a PropertyList");

	public:
		template <typename P, typename = typename std::enable_if<
		                          PropertyListHasType<Ps, typename P::impl_type>::value,
		                          typename P::property_type::type>::type>
		typename P::property_type::type GetProperty() const {
			return getPropertyHolder().template GetProperty<typename P::property_type::type>(
			    Private::getName<P>(nullptr), typename P::property_type::type{});
		}

		template <typename P, typename = typename std::enable_if<
		                          PropertyListHasType<Ps, typename P::impl_type>::value,
		                          typename P::property_type::type>::type>
		void SetProperty(typename P::property_type::type value) {
			getPropertyHolder().template SetProperty<typename P::property_type::type>(Private::getName<P>(nullptr),
			                                                                          value);
		}


	protected:
		HasProperties() {
			static_assert(std::is_base_of<HasPropertiesBase, U>::value,
			              "U must derive from HasPropertiesBase");

			for (auto& p :
			     CreatePropertyDefinitionListForPropertyList(static_cast<U*>(this), Ps{})) {
				auto defaultValue = p->GetDefaultValue();
				auto property =
				    getPropertyHolder().CreateProperty(p->GetName(),
				                                       p->GetTypeName(),
				                                       std::move(defaultValue.value),
				                                       std::move(defaultValue.scratchSpace));
				auto didSet = p->GetDidSetFn();
				if (didSet) {
					property.GetSignal()->Connect(&getTrackable(), *didSet);
				}
			}
		}

		void didSet(void*) {}

	private:
		const Util::PropertyHolder& getPropertyHolder() const {
			return static_cast<const U*>(this)->m_propertyHolder;
		}

		Util::PropertyHolder& getPropertyHolder() {
			return static_cast<U*>(this)->m_propertyHolder;
		}

		Trackable& getTrackable() { return static_cast<U*>(this)->HasPropertiesBase::m_trackable; }
	};
}
