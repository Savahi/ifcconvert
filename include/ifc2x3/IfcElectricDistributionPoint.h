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

#ifndef IFC2X3_IFCELECTRICDISTRIBUTIONPOINT_H
#define IFC2X3_IFCELECTRICDISTRIBUTIONPOINT_H
#include <ifc2x3/DefinedTypes.h>
#include <ifc2x3/Export.h>

#include <ifc2x3/IfcFlowController.h>
#include <Step/BaseVisitor.h>
#include <Step/ClassType.h>
#include <Step/SPFData.h>
#include <Step/String.h>
#include <string>

namespace ifc2x3 {

    class CopyOp;

    /**
     * Generated class for the IfcElectricDistributionPoint Entity.
     * 
     */
    class IFC2X3_EXPORT IfcElectricDistributionPoint : public IfcFlowController {
    public:
        /**
         * Accepts a read/write Step::BaseVisitor.
         * 
         * @param visitor the read/write Step::BaseVisitor to accept
         */
        virtual bool acceptVisitor(Step::BaseVisitor *visitor);
        /**
         * Returns the class type as a human readable std::string.
         * 
         */
        virtual const std::string &type() const;
        /**
         * Returns the Step::ClassType of this specific class. Useful to compare with the isOfType method for example.
         * 
         */
        static const Step::ClassType &getClassType();
        /**
         * Returns the Step::ClassType of the instance of this class. (might be a subtype since it is virtual and overloaded).
         * 
         */
        virtual const Step::ClassType &getType() const;
        /**
         * Compares this instance's Step::ClassType with the one passed as parameter. Checks the type recursively (to the mother classes).
         * 
         * @param t
         */
        virtual bool isOfType(const Step::ClassType &t) const;
        /**
         * Gets the value of the explicit attribute 'DistributionPointFunction'.
         * 
         */
        virtual IfcElectricDistributionPointFunctionEnum getDistributionPointFunction();
        /**
         * (const) Returns the value of the explicit attribute 'DistributionPointFunction'.
         * 
         * @return the value of the explicit attribute 'DistributionPointFunction'
         */
        virtual const IfcElectricDistributionPointFunctionEnum getDistributionPointFunction() const;
        /**
         * Sets the value of the explicit attribute 'DistributionPointFunction'.
         * 
         * @param value
         */
        virtual void setDistributionPointFunction(IfcElectricDistributionPointFunctionEnum value);
        /**
         * unset the attribute 'DistributionPointFunction'.
         * 
         */
        virtual void unsetDistributionPointFunction();
        /**
         * Test if the attribute 'DistributionPointFunction' is set.
         * 
         * @return true if set, false if unset
         */
        virtual bool testDistributionPointFunction() const;
        /**
         * Gets the value of the explicit attribute 'UserDefinedFunction'.
         * 
         */
        virtual IfcLabel getUserDefinedFunction();
        /**
         * (const) Returns the value of the explicit attribute 'UserDefinedFunction'.
         * 
         * @return the value of the explicit attribute 'UserDefinedFunction'
         */
        virtual const IfcLabel getUserDefinedFunction() const;
        /**
         * Sets the value of the explicit attribute 'UserDefinedFunction'.
         * 
         * @param value
         */
        virtual void setUserDefinedFunction(const IfcLabel &value);
        /**
         * unset the attribute 'UserDefinedFunction'.
         * 
         */
        virtual void unsetUserDefinedFunction();
        /**
         * Test if the attribute 'UserDefinedFunction' is set.
         * 
         * @return true if set, false if unset
         */
        virtual bool testUserDefinedFunction() const;
        friend class ExpressDataSet;

    protected:
        /**
         * @param id
         * @param args
         */
        IfcElectricDistributionPoint(Step::Id id, Step::SPFData *args);
        virtual ~IfcElectricDistributionPoint();
        /**
         */
        virtual bool init();
        /**
         * @param obj
         * @param copyop
         */
        virtual void copy(const IfcElectricDistributionPoint &obj, const CopyOp &copyop);

    private:
        /**
         */
        static Step::ClassType s_type;
        /**
         */
        IfcElectricDistributionPointFunctionEnum m_distributionPointFunction;
        /**
         */
        Step::String m_userDefinedFunction;

    };

}

#endif // IFC2X3_IFCELECTRICDISTRIBUTIONPOINT_H
