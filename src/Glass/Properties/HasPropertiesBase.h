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

#include "iZBase/Util/PropertyHolder.h"

namespace Glass {
	//! Base class for classes that have properties.
	//!
	//! Through multiple inheritance it is possible to inherit from two different
	//! HasProperties<Ps,U>, so the actual property holder is a member of HasPropertiesBase. Any
	//! derived type that inherits from some HasProperties<Ps,U> must also inherit from
	//! HasPropertiesBase and must provide the type deriving from HasPropertiesBase as type argument
	//! U.
	class HasPropertiesBase {
		template <typename Ps, typename U> friend class HasProperties;

	public:
		virtual ~HasPropertiesBase() = 0;

	protected:
		HasPropertiesBase();

	private:
		Util::PropertyHolder m_propertyHolder;
		Trackable m_trackable;
	};
}
