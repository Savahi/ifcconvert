// IFC SDK : IFC2X3 C++ Early Classes  
// Copyright (C) 2009 CSTB
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full license is in Licence.txt file included with this 
// distribution or is available at :
//     http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.



#include <ifc2x3/IfcContextDependentUnit.h>

#include <ifc2x3/CopyOp.h>
#include <ifc2x3/IfcNamedUnit.h>
#include <ifc2x3/Visitor.h>
#include <Step/BaseObject.h>
#include <Step/ClassType.h>
#include <Step/String.h>


#include <string>

#include "precompiled.h"

using namespace ifc2x3;

IfcContextDependentUnit::IfcContextDependentUnit(Step::Id id, Step::SPFData *args) : IfcNamedUnit(id, args) {
    m_name = Step::getUnset(m_name);
}

IfcContextDependentUnit::~IfcContextDependentUnit() {
}

bool IfcContextDependentUnit::acceptVisitor(Step::BaseVisitor *visitor) {
    return static_cast< Visitor * > (visitor)->visitIfcContextDependentUnit(this);
}

const std::string &IfcContextDependentUnit::type() const {
    return IfcContextDependentUnit::s_type.getName();
}

const Step::ClassType &IfcContextDependentUnit::getClassType() {
    return IfcContextDependentUnit::s_type;
}

const Step::ClassType &IfcContextDependentUnit::getType() const {
    return IfcContextDependentUnit::s_type;
}

bool IfcContextDependentUnit::isOfType(const Step::ClassType &t) const {
    return IfcContextDependentUnit::s_type == t ? true : IfcNamedUnit::isOfType(t);
}

IfcLabel IfcContextDependentUnit::getName() {
    if (Step::BaseObject::inited()) {
        return m_name;
    }
    else {
        return Step::getUnset(m_name);
    }
}

const IfcLabel IfcContextDependentUnit::getName() const {
    IfcContextDependentUnit * deConstObject = const_cast< IfcContextDependentUnit * > (this);
    return deConstObject->getName();
}

void IfcContextDependentUnit::setName(const IfcLabel &value) {
    m_name = value;
}

void IfcContextDependentUnit::unsetName() {
    m_name = Step::getUnset(getName());
}

bool IfcContextDependentUnit::testName() const {
    return !Step::isUnset(getName());
}

bool IfcContextDependentUnit::init() {
    bool status = IfcNamedUnit::init();
    std::string arg;
    if (!status) {
        return false;
    }
    arg = m_args->getNext();
    if (arg == "$" || arg == "*") {
        m_name = Step::getUnset(m_name);
    }
    else {
        m_name = Step::String::fromSPF(arg);
    }
    return true;
}

void IfcContextDependentUnit::copy(const IfcContextDependentUnit &obj, const CopyOp &copyop) {
    IfcNamedUnit::copy(obj, copyop);
    setName(obj.m_name);
    return;
}

IFC2X3_EXPORT Step::ClassType IfcContextDependentUnit::s_type("IfcContextDependentUnit");
