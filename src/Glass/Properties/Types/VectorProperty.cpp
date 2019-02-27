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


#include "iZBase/common/common.h"

#include "Glass/Properties/Types/VectorProperty.h"

#include "boost/algorithm/string.hpp"

vector<std::string>
Glass::Private::parseSerializedVectorProperty(const std::string& serializedValue) {
	auto results = serializedValue;
	results.erase(results.find("vector("), 7);
	results.erase(results.find(")"));

	vector<std::string> vectorResults;
	boost::split(vectorResults, results, [](char c) { return c == ','; });

	int index = 0;
	for (auto iter = vectorResults.begin(); iter != vectorResults.end();) {
		if (iter[0][0] == ' ') {
			iter->erase(0, 1);
		}
		if (iter->find("\\") != std::string::npos) {
			iter->erase(iter->find("\\"), 2);
			iter->insert(iter->length(), ", ");
			iter->insert(iter->length() - 1, vectorResults[index + 1]);
			if (iter->find_last_of(' ') == iter->length() - 1) {
				iter->erase(iter->find_last_of(' '), 1);
			}
			vectorResults.erase(iter + 1);
		}
		++iter;
		++index;
	}
	return vectorResults;
}
