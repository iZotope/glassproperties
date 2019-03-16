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

#include "Glass/Properties/HasPropertiesBase.h"

using namespace Glass;


HasPropertiesBase::HasPropertiesBase()
    : m_managedPropertyHolder{make_unique<Util::PropertyHolder>()}
    , m_propertyHolder{*m_managedPropertyHolder.get()} {}

HasPropertiesBase::HasPropertiesBase(Util::PropertyHolder& propertyHolder)
    : m_propertyHolder{propertyHolder} {}

HasPropertiesBase::~HasPropertiesBase() = default;
