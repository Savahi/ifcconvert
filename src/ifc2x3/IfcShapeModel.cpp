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



#include <ifc2x3/IfcShapeModel.h>

#include <ifc2x3/CopyOp.h>
#include <ifc2x3/IfcRepresentation.h>
#include <ifc2x3/IfcShapeAspect.h>
#include <ifc2x3/Visitor.h>
#include <Step/BaseExpressDataSet.h>
#include <Step/BaseObject.h>
#include <Step/ClassType.h>


#include <string>
#include <vector>

#include "precompiled.h"

using namespace ifc2x3;

IfcShapeModel::IfcShapeModel(Step::Id id, Step::SPFData *args) : IfcRepresentation(id, args) {
}

IfcShapeModel::~IfcShapeModel() {
}

bool IfcShapeModel::acceptVisitor(Step::BaseVisitor *visitor) {
    return static_cast< Visitor * > (visitor)->visitIfcShapeModel(this);
}

const std::string &IfcShapeModel::type() const {
    return IfcShapeModel::s_type.getName();
}

const Step::ClassType &IfcShapeModel::getClassType() {
    return IfcShapeModel::s_type;
}

const Step::ClassType &IfcShapeModel::getType() const {
    return IfcShapeModel::s_type;
}

bool IfcShapeModel::isOfType(const Step::ClassType &t) const {
    return IfcShapeModel::s_type == t ? true : IfcRepresentation::isOfType(t);
}

Inverse_Set_IfcShapeAspect_0_1 &IfcShapeModel::getOfShapeAspect() {
    if (Step::BaseObject::inited()) {
        return m_ofShapeAspect;
    }
    else {
        m_ofShapeAspect.setUnset(true);
        return m_ofShapeAspect;
    }
}

const Inverse_Set_IfcShapeAspect_0_1 &IfcShapeModel::getOfShapeAspect() const {
    IfcShapeModel * deConstObject = const_cast< IfcShapeModel * > (this);
    return deConstObject->getOfShapeAspect();
}

bool IfcShapeModel::testOfShapeAspect() const {
    return !m_ofShapeAspect.isUnset();
}

bool IfcShapeModel::init() {
    bool status = IfcRepresentation::init();
    std::string arg;
    std::vector< Step::Id > *inverses;
    if (!status) {
        return false;
    }
    inverses = m_args->getInverses(IfcShapeAspect::getClassType(), 0);
    if (inverses) {
        unsigned int i;
        m_ofShapeAspect.setUnset(false);
        for (i = 0; i < inverses->size(); i++) {
            m_ofShapeAspect.insert(static_cast< IfcShapeAspect * > (m_expressDataSet->get((*inverses)[i])));
        }
    }
    return true;
}

void IfcShapeModel::copy(const IfcShapeModel &obj, const CopyOp &copyop) {
    IfcRepresentation::copy(obj, copyop);
    return;
}

IFC2X3_EXPORT Step::ClassType IfcShapeModel::s_type("IfcShapeModel");
