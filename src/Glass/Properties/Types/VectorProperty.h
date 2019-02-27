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

namespace Glass {
	namespace Private {
		//! Parse a serialied VectorProperty<T>, returning a list of serialied T
		vector<std::string> parseSerializedVectorProperty(const std::string&);
	}

	template <typename T> struct VectorProperty : T {
		using type = vector<typename T::type>;
		static std::string name() {
			return std::string{"vector<"} + Private::getName<T>() + std::string{">"};
		}
		static std::string serialize(const type& value) {
			UNREF_PARAM(value);
			std::string sVectorElements{"vector("};
			for (auto& result : value) {
				auto serializedResult = T::serialize(result);
				if (serializedResult.find(',') != std::string::npos) {
					auto resultIter = serializedResult.cbegin();
					while (resultIter != serializedResult.end()) {
						if (*resultIter == ',') {
							serializedResult.insert(resultIter, '\\');
							++resultIter;
						}
						++resultIter;
					}
				}
				sVectorElements += serializedResult + ", ";
			}
			sVectorElements.erase(sVectorElements.find_last_of(","));
			sVectorElements += ")";

			return sVectorElements;
		}
		static std::optional<type> deserialize(const std::string& serializedValue) {

			type returnVector;
			const vector<std::string> vectorResults =
			    Private::parseSerializedVectorProperty(serializedValue);

			for (auto& result : vectorResults) {
				auto item = T::deserialize(result);
				if (item) {
					returnVector.push_back(*item);
				} else {
					return std::optional<type>();
				}
			}

			return returnVector;
		}
	};
}
