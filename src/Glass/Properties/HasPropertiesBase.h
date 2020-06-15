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

#include "Glass/Properties/Private/getName.h"
#include "iZBase/Util/PropertyHolder.h"

namespace Burlap {
	template <typename V> class View;
}

namespace Glass {
	class ViewDesignInterface;

	//! Base class for classes that have properties.
	//!
	//! Through multiple inheritance it is possible to inherit from two different
	//! HasProperties<Ps,U>, so the actual property holder is a member of HasPropertiesBase. Any
	//! derived type that inherits from some HasProperties<Ps,U> must also inherit from
	//! HasPropertiesBase and must provide the type deriving from HasPropertiesBase as type argument
	//! U.
	class HasPropertiesBase {
		friend class Flex;
		template <typename V> friend class Burlap::View;
		template <typename Ps, typename U> friend class HasProperties;
		friend class ViewDesignInterface;
		friend class HasToolTip;

	public:
		virtual ~HasPropertiesBase() = 0;

		void SetStyleSheet(shared_ptr<Util::StyleSheet> styleSheet);
		void AddClassName(const std::string& className);
		void SetClassNames(const vector<std::string>& classNames);

	protected:
		HasPropertiesBase();

	private:
		HasPropertiesBase(Util::PropertyHolder&);

		unique_ptr<Util::PropertyHolder> m_managedPropertyHolder;
		Util::PropertyHolder& m_propertyHolder;
		Trackable m_trackable;
	};
}
