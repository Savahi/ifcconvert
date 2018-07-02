#include "BrepReaderVisitor.h"

#include <ifc2x3/ExpressDataSet.h>

BrepReaderVisitor::BrepReaderVisitor(BRepBuilder* brepBuilder) 
    : _brepBuilder(brepBuilder),
      _fatherIsOpeningEl(false)
{
}

bool BrepReaderVisitor::visitIfcObjectDefinition(ifc2x3::IfcObjectDefinition *value)
{
    _brepBuilder->printHierarchy( "BrepReaderVisitor.visitIfcObjectDefinition", 1 );

    ifc2x3::Inverse_Set_IfcRelDecomposes_0_n::iterator itDecomposedBy = value->getIsDecomposedBy().begin();
    while (itDecomposedBy != value->getIsDecomposedBy().end())
    {
        ifc2x3::IfcRelDecomposes* rd = const_cast<ifc2x3::IfcRelDecomposes*> (itDecomposedBy->get());
        //std::cout << "getRelatingObject(): " << rd->getRelatingObject() << rd->type() << "\n";
        rd->acceptVisitor(this);
        ++itDecomposedBy;
    }
    _brepBuilder->printHierarchy( "end of BrepReaderVisitor.visitIfcObjectDefinition", -1 );
    return true;
}

bool BrepReaderVisitor::visitIfcRelDecomposes(ifc2x3::IfcRelDecomposes *value) {
    std::cout << "visitIfcRelDecomposes()!\n";
}

bool BrepReaderVisitor::visitIfcRelAggregates(ifc2x3::IfcRelAggregates *value)
{
    _brepBuilder->printHierarchy( "BrepReaderVisitor.visitIfcRelAggregates()", 1 );
    ifc2x3::Set_IfcObjectDefinition_1_n::iterator it = value->getRelatedObjects().begin();
    while (it != value->getRelatedObjects().end())
    {
        std::stringstream ss;
        ss << "getRelatedObjects() item: " << (*it)->type();
        _brepBuilder->printHierarchy( ss.str(), 0 );        

        (*it)->acceptVisitor(this);
        ++it;
    }
    _brepBuilder->printHierarchy( "end of BrepReaderVisitor.visitIfcRelAggregates", -1 );
    return true;
}

bool BrepReaderVisitor::pushPlacementAndComputeRepresentation(ifc2x3::IfcProduct *value, bool addProduct)
{
    //push placement
    bool hasPlacement = false;
    if(value->testObjectPlacement())
    {
        hasPlacement = value->getObjectPlacement()->acceptVisitor(this);
    }

    if(addProduct)
        _brepBuilder->addProduct(value);
    
    // work on representation
    ifc2x3::IfcProductRepresentation * pr = value->getRepresentation();
    if(pr)
    {
        pr->acceptVisitor(this);
    }

    return hasPlacement;
}

bool BrepReaderVisitor::visitIfcProduct(ifc2x3::IfcProduct *value)
{
    //push placement
    bool hasPlacement = pushPlacementAndComputeRepresentation(value);
    
    // visit decomposed by object
    visitIfcObjectDefinition(value);

    // pop placement
    if(hasPlacement)
        _brepBuilder->popPlacement();

    return true;
}

bool BrepReaderVisitor::visitIfcElement(ifc2x3::IfcElement *value)
{
    // test if this element fills something; its father must be an opening element
    if(value->getFillsVoids().size() > 0 && !_fatherIsOpeningEl)
        return true;

    // push placement
    bool hasPlacement = pushPlacementAndComputeRepresentation(value);

    // visit opening elements
    ifc2x3::Inverse_Set_IfcRelVoidsElement_0_n::iterator it = value->getHasOpenings().begin();
    while (it != value->getHasOpenings().end())
    {     
        ifc2x3::IfcFeatureElementSubtraction* elt = const_cast<ifc2x3::IfcFeatureElementSubtraction*> ((*it)->getRelatedOpeningElement());
        elt->acceptVisitor(this); 
        ++it;        
    }
    
    // visit decomposed by object
    visitIfcObjectDefinition(value);

    // pop placement
    if(hasPlacement)
        _brepBuilder->popPlacement();
    
    return true;
}

bool BrepReaderVisitor::visitIfcOpeningElement(ifc2x3::IfcOpeningElement *value)
{
    _brepBuilder->printHierarchy( "BrepReaderVisitor.visitIfcOpeningElement()", 1 );

    // push placement
    bool hasPlacement = pushPlacementAndComputeRepresentation(value, false);

    // visit fillings
    _fatherIsOpeningEl = true;
    ifc2x3::Inverse_Set_IfcRelFillsElement_0_n::iterator it = value->getHasFillings().begin();
    while (it != value->getHasFillings().end())
    {     
        ifc2x3::IfcElement *elt = const_cast<ifc2x3::IfcElement*>((*it)->getRelatedBuildingElement());
        elt->acceptVisitor(this);
        ++it;        
    }
    _fatherIsOpeningEl = false;

    // pop placement
    if(hasPlacement)
        _brepBuilder->popPlacement();

    _brepBuilder->printHierarchy( "end of BrepReaderVisitor.visitIfcOpeningElement()", -1 );
    return true;
}

bool BrepReaderVisitor::visitIfcSpatialStructureElement(ifc2x3::IfcSpatialStructureElement *value)
{
    _brepBuilder->printHierarchy( "BrepReaderVisitor.visitIfcSpatialStructureElement()", 1 );

    // push placement
    bool hasPlacement = pushPlacementAndComputeRepresentation(value);

    // visit contained elements
    ifc2x3::Inverse_Set_IfcRelContainedInSpatialStructure_0_n::iterator itContainsElements = value->getContainsElements().begin();
    while (itContainsElements != value->getContainsElements().end())
    {
        ifc2x3::IfcRelContainedInSpatialStructure* rel = const_cast<ifc2x3::IfcRelContainedInSpatialStructure *> (itContainsElements->get());
        rel->acceptVisitor(this);
        ++itContainsElements;
    }
    
    // visit decomposed by object
    visitIfcObjectDefinition(value);

    // pop placement
    if(hasPlacement)
        _brepBuilder->popPlacement();

    _brepBuilder->printHierarchy( "end of BrepReaderVisitor.visitIfcSpatialStructureElement()", -1 );
    return true;
}

bool BrepReaderVisitor::visitIfcRelContainedInSpatialStructure(ifc2x3::IfcRelContainedInSpatialStructure *value)
{
    _brepBuilder->printHierarchy( "BrepReaderVisitor.visitIfcRelContainedInSpatialStructure()", 1 );

    ifc2x3::Set_IfcProduct_1_n::iterator itRelatedElement = value->getRelatedElements().begin();
    while (itRelatedElement != value->getRelatedElements().end())
    {
        std::stringstream ss;
        ss << "BrepReaderVisitor.visitIfcRelContainedInSpatialStructure() item: " << (*itRelatedElement)->type();
        _brepBuilder->printHierarchy( ss.str(), 0 );

        (*itRelatedElement)->acceptVisitor(this);
        ++itRelatedElement;
    }

    _brepBuilder->printHierarchy( "end of BrepReaderVisitor.visitIfcRelContainedInSpatialStructure()", -1 );
    return true;
}

bool BrepReaderVisitor::visitIfcMaterialDefinitionRepresentation(ifc2x3::IfcMaterialDefinitionRepresentation *value) {
    printf("\nHERE!!!!\n");
}


bool BrepReaderVisitor::visitIfcProductRepresentation(ifc2x3::IfcProductRepresentation *value)
{
    _brepBuilder->printHierarchy( "BrepReaderVisitor.visitIfcProductRepresentation", 1 );
    ifc2x3::List_IfcRepresentation_1_n::iterator it = value->getRepresentations().begin();
    while (it != value->getRepresentations().end())
    {
        Step::String representationType = (*it)->getRepresentationType();
        std::stringstream ss;
        ss << "representationtype: " << representationType;
        _brepBuilder->printHierarchy( ss.str(), 0 );        
        if (representationType == Step::String("Brep") || true )  // "|| true" .
        {
            (*it)->acceptVisitor(this);
        }
        ++it;
    }
    _brepBuilder->printHierarchy( "end of visitIfcProductRepresentation", -1 );

    return true;
}

bool BrepReaderVisitor::visitIfcRepresentation(ifc2x3::IfcRepresentation *value)
{
    _brepBuilder->printHierarchy( "BrepReaderVisitor.visitIfcRepresentation", 1 );
    ifc2x3::Set_IfcRepresentationItem_1_n::iterator it = value->getItems().begin();
    while (it != value->getItems().end())
    {
        std::stringstream ss;
        ss << "key=" << (*it)->getKey();
        _brepBuilder->printHierarchy( ss.str(), 0 );
        (*it)->acceptVisitor(this);
        ++it;
    }
    _brepBuilder->printHierarchy( "end of visitIfcRepresentation", -1 );
    return true;
}


bool BrepReaderVisitor::visitIfcCurve(ifc2x3::IfcCurve *value)
{
    //_brepBuilder->printHierarchy( "BrepReaderVisitor.visitIfcCurve" );
    //_brepBuilder->addRepresentation(value);
 
    return true;
}

bool BrepReaderVisitor::visitIfcPolyline(ifc2x3::IfcPolyline *value)
{
    return true;
    _brepBuilder->printHierarchy( "BrepReaderVisitor.visitIfcPolyline", 0 );
    ifc2x3::List_IfcCartesianPoint_3_n::iterator it = value->getPoints().begin();
    while (it != value->getPoints().end())
    {
        (*it)->acceptVisitor(this);
        ++it;
    }
    return true;
}

bool BrepReaderVisitor::visitIfcProfileDef(ifc2x3::IfcProfileDef *value) {
    std::stringstream ss;
    if( value->getType().getName() == "IfcRectangleProfileDef" ) {
        ifc2x3::IfcRectangleProfileDef* profileDef = (ifc2x3::IfcRectangleProfileDef*)value;
        double xDim = profileDef->getXDim();
        double yDim = profileDef->getYDim();
        ss << "BrepReaderVisitor.visitIfcProfileDef: " << value->type() << ", xdim=" << xDim << ", ydim=" << yDim; 
        _brepBuilder->printHierarchy( ss.str(), 0 );
    }
    return true;
}

bool BrepReaderVisitor::visitIfcSweptAreaSolid(ifc2x3::IfcSweptAreaSolid *value) {
    //ifc2x3::IfcAxis2Placement3D *position = this->getPosition();

    _brepBuilder->printHierarchy( "BrepReaderVisitor.visitIfcSweptAreaSolid->", 0 );
    return true;
}

bool BrepReaderVisitor::visitIfcExtrudedAreaSolid(ifc2x3::IfcExtrudedAreaSolid *value) {
    _brepBuilder->pushPlacement(value->getPosition());

    ifc2x3::IfcDirection *extrudedDirection = value->getExtrudedDirection();
    if( extrudedDirection ) {
        ifc2x3::List_Real_2_3 ijk = extrudedDirection->getDirectionRatios();
        double depth = value->getDepth();
        ifc2x3::IfcProfileDef *sweptArea = value->getSweptArea();
        if( sweptArea ) {
            if( sweptArea->getType().getName() == "IfcRectangleProfileDef" ) {
                ifc2x3::IfcRectangleProfileDef *rectangleProfileDef = (ifc2x3::IfcRectangleProfileDef *)sweptArea;
                double xDim = rectangleProfileDef->getXDim();
                double yDim = rectangleProfileDef->getYDim();
                std::stringstream ss;
                ss << "BrepReaderVisitor.visitIfcExtrudedAreaSolid: depth=" << depth;
                ss << ", i=" << ijk[0] << ", j=" << ijk[1] << ", k=" << ijk[2];
                ss << ", xdim=" << xDim << ", ydim" << yDim; 
                _brepBuilder->printHierarchy( ss.str(), 0 );
                buildExtrudedArea( depth, xDim, yDim, ijk[0], ijk[1], ijk[2] );
            }
        }
    }

    _brepBuilder->popPlacement();
    return true;
}

bool BrepReaderVisitor::visitIfcFacetedBrep(ifc2x3::IfcFacetedBrep *value)
{
    _brepBuilder->addRepresentation(value);
 
    ifc2x3::IfcClosedShell* closedShell = value->getOuter();
    if(closedShell)
    {
        closedShell->acceptVisitor(this);
    }

    return true;
}

bool BrepReaderVisitor::visitIfcClosedShell(ifc2x3::IfcClosedShell *value)
{
    _brepBuilder->addClosedShell(value);

    ifc2x3::Set_IfcFace_1_n::iterator it = value->getCfsFaces().begin();
    while (it != value->getCfsFaces().end())
    {
        (*it)->acceptVisitor(this);
        ++it;
    }

    return true;
}

bool BrepReaderVisitor::visitIfcFace(ifc2x3::IfcFace *value)
{
    _brepBuilder->addFace(value);

    ifc2x3::Set_IfcFaceBound_1_n::iterator it = value->getBounds().begin();
    while (it != value->getBounds().end())
    {
        ifc2x3::IfcPolyLoop * poly = dynamic_cast<ifc2x3::IfcPolyLoop*>((*it)->getBound());
        if(poly)
        {
            poly->acceptVisitor(this);
        }

        ++it;
    }

    return true;
}

bool BrepReaderVisitor::visitIfcPolyLoop(ifc2x3::IfcPolyLoop *value)
{
    ifc2x3::List_IfcCartesianPoint_3_n::iterator it = value->getPolygon().begin();
    while (it != value->getPolygon().end())
    {
        (*it)->acceptVisitor(this);
        ++it;
    }

    return true;
}

bool BrepReaderVisitor::visitIfcCartesianPoint(ifc2x3::IfcCartesianPoint *value)
{
    _brepBuilder->addPoint(value);
    return true;
}

// placement
bool BrepReaderVisitor::visitIfcLocalPlacement(ifc2x3::IfcLocalPlacement *value)
{
    return value->getRelativePlacement()->acceptVisitor(this);
}

bool BrepReaderVisitor::visitIfcAxis2Placement(ifc2x3::IfcAxis2Placement * value)
{
    ifc2x3::IfcAxis2Placement3D * axis3placement3d = value->getIfcAxis2Placement3D();
    if(axis3placement3d)
    {
        return axis3placement3d->acceptVisitor(this);
    }

    return true;
}

bool BrepReaderVisitor::visitIfcAxis2Placement3D(ifc2x3::IfcAxis2Placement3D * value)
{
    _brepBuilder->pushPlacement(value);
    return true;
}

void BrepReaderVisitor::buildExtrudedArea( double depth, double xDim, double yDim, double i, double j, double k ) {
    
    double depthSquared = depth * depth;
    if( !(depthSquared > 0.0) ) {
        return;
    }
    double depthE = i*i + j*j + k*k;
    if( !(depthE > 0.0) ) {
        return;
    }

    double a = sqrt(depthSquared / depthE);
    double x1=0.0;
    double x2=xDim;
    double y1=0.0;
    double y2=yDim;
    double z1=0;
    double x1new = i * a + x1;
    double x2new = i * a + x2;
    double y1new = j * a + y1;
    double y2new = j * a + y2;
    double z = 0;
    double znew = k * a + z;

    //std::cout << "\n" << "x1=" << x1 << ", x2=" << x2 << "y1=" << y1 << ", y2=" << y2 << ", z1=" << z;
    //std::cout << "\n" << "x1new=" << x1new << ", x2new=" << x2new << ", y1new=" << y1new << ", y2new=" << y2new << ", znew=" << znew << "\n";

    _brepBuilder->addFace();
    _brepBuilder->addPoint(x1,y1,z);
    _brepBuilder->addPoint(x2,y1,z);
    _brepBuilder->addPoint(x2,y2,z);
    _brepBuilder->addPoint(x1,y2,z);

    _brepBuilder->addFace();
    _brepBuilder->addPoint(x2,y1,z);
    _brepBuilder->addPoint(x2new,y1new,znew);
    _brepBuilder->addPoint(x2new,y2new,znew);
    _brepBuilder->addPoint(x2,y2,z);

    _brepBuilder->addFace();
    _brepBuilder->addPoint(x1,y2,z);
    _brepBuilder->addPoint(x2,y2,z);
    _brepBuilder->addPoint(x2new,y2new,znew);
    _brepBuilder->addPoint(x1new,y2new,znew);

    _brepBuilder->addFace();
    _brepBuilder->addPoint(x1,y1,z);
    _brepBuilder->addPoint(x1,y2,z);
    _brepBuilder->addPoint(x1new,y2new,znew);
    _brepBuilder->addPoint(x1new,y1new,znew);

    _brepBuilder->addFace();
    _brepBuilder->addPoint(x1,y1,z);
    _brepBuilder->addPoint(x1new,y1new,znew);
    _brepBuilder->addPoint(x2new,y1new,znew);
    _brepBuilder->addPoint(x2,y1,z);

    _brepBuilder->addFace();
    _brepBuilder->addPoint(x1new,y1new,znew);
    _brepBuilder->addPoint(x2new,y1new,znew);
    _brepBuilder->addPoint(x2new,y2new,znew);
    _brepBuilder->addPoint(x1new,y2new,znew);
}
