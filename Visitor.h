#ifndef VISITOR_H
#define VISITOR_H

#include "Builder.h"

#include <ifc2x3/InheritVisitor.h>

namespace IfcConvert {

    /*!
    ** \class Visitor Visitor.h
    ** \brief Class used as interface for the BrepBuilder
    ** 
    ** From a given ifc item, scan all the child items.
    ** Through the Builder attribut:
    ** placements are pushed (at the start) and popped (at the end) when an ifcproduct is visited
    ** ifcproduct, ifcrepresentation, ifcclosedshell, ifcface and ifcpoint are added when they are visited
    */
    class Visitor : public ifc2x3::InheritVisitor
    {
    public:
        Visitor(Builder* builder);

        virtual bool visitIfcMaterialDefinitionRepresentation(ifc2x3::IfcMaterialDefinitionRepresentation *value);

        virtual bool visitIfcRelAssociatesMaterial(ifc2x3::IfcRelAssociatesMaterial *value);
    	
        /*!
        ** \brief Visit "decomposedby" items of an IfcObjectDefinition
        */
        virtual bool visitIfcObjectDefinition(ifc2x3::IfcObjectDefinition *value);
        /*!
        ** \brief Visit "relatedObjects" items of an IfcRelAggregates
        */
        virtual bool visitIfcRelAggregates(ifc2x3::IfcRelAggregates *value);
        
        virtual bool visitIfcRelDecomposes(ifc2x3::IfcRelDecomposes *value);

        /*!
        ** \brief Visit an IfcProduct
        ** If it has a placement, push it at the start
        ** Add the product to the Builder
        ** Visit "decomposedby" items
        ** If it has a placement, pop it at the end 
        */
        virtual bool visitIfcProduct(ifc2x3::IfcProduct *value);
        /*!
        ** \brief Visit an IfcSpatialStructureElement
        ** If it has a placement, push it at the start
        ** Add the product to the Builder
        ** Visit "containsElements" items
        ** Visit "decomposedby" items
        ** If it has a placement, pop it at the end 
        */
        virtual bool visitIfcSpatialStructureElement(ifc2x3::IfcSpatialStructureElement *value);
        /*!
        ** \brief Visit an IfcElement
        ** If the element fills something and its parent visitor function is not an IfcOpeningElement, do not visit the IfcElement
        ** If it has a placement, push it at the start
        ** Add the product to the Builder
        ** Visit "openings" items
        ** Visit "decomposedby" items
        ** If it has a placement, pop it at the end 
        */
        virtual bool visitIfcElement(ifc2x3::IfcElement *value);
        /*!
        ** \brief Visit an IfcOpeningElement
        ** If it has a placement, push it at the start
        ** Add the product to the Builder
        ** Visit "fillings" items
        ** If it has a placement, pop it at the end 
        */
        virtual bool visitIfcOpeningElement(ifc2x3::IfcOpeningElement *value);
        /*!
        ** \brief Visit "relatedElements" items of an IfcRelContainedInSpatialStructure
        */
        virtual bool visitIfcRelContainedInSpatialStructure(ifc2x3::IfcRelContainedInSpatialStructure *value);
        /*!
        ** \brief Visit the "Brep" representations of an IfcProductRepresentation
        */
        virtual bool visitIfcProductRepresentation(ifc2x3::IfcProductRepresentation *value);
        /*!
        ** \brief Visit "representationItems" items of an IfcRepresentation
        */
        virtual bool visitIfcRepresentation(ifc2x3::IfcRepresentation *value);
        
        /* Mine */
        virtual bool visitIfcCurve(ifc2x3::IfcCurve *value);
        virtual bool visitIfcPolyline(ifc2x3::IfcPolyline *value);
        virtual bool visitIfcProfileDef(ifc2x3::IfcProfileDef *value);
        virtual bool visitIfcSweptAreaSolid(ifc2x3::IfcSweptAreaSolid *value);
        virtual bool visitIfcExtrudedAreaSolid(ifc2x3::IfcExtrudedAreaSolid *value);
        virtual bool visitIfcBooleanClippingResult(ifc2x3::IfcBooleanClippingResult *value);
        /* The end of "Mine" */

        /*!
        ** \brief Visit an IfcFacetedBrep
        ** Add the representation to the Builder
        ** Visit the "closedShell" item if there is one
        */
        virtual bool visitIfcFacetedBrep(ifc2x3::IfcFacetedBrep *value);
        /*!
        ** \brief Visit an IfcClosedShell
        ** Add the closedShell to the Builder
        ** Visit "faces" items
        */
        virtual bool visitIfcClosedShell(ifc2x3::IfcClosedShell *value);
        /*!
        ** \brief Visit an IfcFace
        ** Add the face to the Builder
        ** Visit "polyLoop" items
        */
        virtual bool visitIfcFace(ifc2x3::IfcFace *value);
        /*!
        ** \brief Visit "cartesianPoints" items of an IfcPolyLoop
        */
        virtual bool visitIfcPolyLoop(ifc2x3::IfcPolyLoop *value);
        /*!
        ** \brief Visit an IfcCartesianPoint
        ** Add the point to the Builder
        */
        virtual bool visitIfcCartesianPoint(ifc2x3::IfcCartesianPoint *value);
        
        /*!
        ** \brief Visit the relative placement of an IfcLocalPlacement
        */
        virtual bool visitIfcLocalPlacement(ifc2x3::IfcLocalPlacement *value);
        /*!
        ** \brief Visit the IfcAxis2Placement3D of an IfcAxis2Placement if there is one
        */
        virtual bool visitIfcAxis2Placement(ifc2x3::IfcAxis2Placement *value);
        /*!
        ** \brief Add the axis2Placement3D to the Builder
        */
        virtual bool visitIfcAxis2Placement3D(ifc2x3::IfcAxis2Placement3D *value);
        
        /*!
        ** \brief Do not visit IfcDoor
        ** If you want to visit doors, remove or comment this method
        */
        virtual bool visitIfcDoor(ifc2x3::IfcDoor * /*value*/)
        {
            return true;
        };
        /*!
        ** \brief Do not visit IfcWindow
        ** If you want to visit windows, remove or comment this method
        */
        virtual bool visitIfcWindow(ifc2x3::IfcWindow * /*value*/)
        {
            return true;
        };

    protected:
        /*!
        ** \brief Visit an IfcFace
        ** Push the object placement if there is one
        ** Add the product to the Builder
        ** Visit the representation if there is one
        ** \return if the product has an object placement
        */
        bool pushPlacementAndComputeRepresentation(ifc2x3::IfcProduct *value, bool addProduct = true);
        bool popPlacement( bool hadPlacement );

    protected:
    	Builder* _builder;
        bool _fatherIsOpeningEl;

        void buildRectangleProfileDef( double depth, double i, double j, double k, double xDim, double yDim );
        void buildArbitraryClosedProfileDef( double depth, double i, double j, double k, 
            std::vector<double>& x, std::vector<double>& y );
    };

} // End of namespace Spider3d

#endif // BREPREADERVISITOR_H
