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
	//! Use PropertyType to define a named property for serialization
	//!
	//! typename T: type backing the property
	template <typename T> struct PropertyType { using type = T; };

	struct IntPropertyType : PropertyType<int32_t> {
		static constexpr const char* const name = "Int";
	};
	struct FloatPropertyType : PropertyType<float> {
		static constexpr const char* const name = "Float";
	};
}
