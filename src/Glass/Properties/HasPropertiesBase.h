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
	class HasPropertiesBase {
		template <typename T, typename U, typename Ps> friend class HasProperties;

	public:
		virtual ~HasPropertiesBase();

	protected:
		HasPropertiesBase();

	private:
		Util::PropertyHolder m_propertyHolder;
		Trackable m_trackable;
	};
}
