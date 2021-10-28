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
	namespace Private {
		inline std::string ToStringStripZeros(float value, int precision) {
			auto s = fmt::format("{:.{}f}", value, precision);
			s.erase(s.find_last_not_of('0') + 1, std::string::npos);
			if (s.find('.') == s.length() - 1) {
				s.erase(s.find('.'));
			}
			return s;
		};
	}
}
