#include "BrepReaderVisitor.h"

#include <ifc2x3/ExpressDataSet.h>

namespace Spider3d {

BrepReaderVisitor::BrepReaderVisitor(BRepBuilder* brepBuilder) : _brepBuilder(brepBuilder), _fatherIsOpeningEl(false)
{
}

bool BrepReaderVisitor::visitIfcMaterialDefinitionRepresentation(ifc2x3::IfcMaterialDefinitionRepresentation *mdr) {
//IfcMaterial *IfcMaterialDefinitionRepresentation::getRepresentedMaterial();

ifc2x3::List_IfcRepresentation_1_n repres = mdr->getRepresentations(); // Getting representations
ifc2x3::List_IfcRepresentation_1_n::iterator represIter = repres.begin(); 
for( ; represIter != repres.end() ; ++represIter ) { // Iterating representations
    std::cout << "(*represIter)[1] Key=" << (*represIter)->getKey() << std::endl; 
    ifc2x3::Set_IfcRepresentationItem_1_n represItems = (*represIter)->getItems(); // Getting representation items
    ifc2x3::Set_IfcRepresentationItem_1_n::iterator represItemsIter = represItems.begin();
    for( ; represItemsIter != represItems.end() ; ++represItemsIter ) { // Iterating representation items
        std::cout << "(*represItemsIter)[2] Key=" << (*represItemsIter)->getKey();
        std::cout << ", type=" << (*represItemsIter)->type() << std::endl;
        Step::RefPtr<ifc2x3::IfcStyledItem> si = (*represItemsIter); // Representation item == style item
        ifc2x3::Set_IfcPresentationStyleAssignment_1_n psa = si->getStyles(); // Getting style assignments
        ifc2x3::Set_IfcPresentationStyleAssignment_1_n::iterator psaIter = psa.begin();
        for( ; psaIter != psa.end() ; ++psaIter ) {
            std::cout << "(*psaIter)[3] Key=" << (*psaIter)->getKey() << std::endl;
            ifc2x3::Set_IfcPresentationStyleSelect_1_n pss = (*psaIter)->getStyles(); // Getting styles
            ifc2x3::Set_IfcPresentationStyleSelect_1_n::iterator pssIter = pss.begin();
            if( pssIter != pss.end() ) {
                Step::RefPtr<ifc2x3::IfcPresentationStyleSelect> pss = (*pssIter); // Getting style
                if( pss->currentType() == ifc2x3::IfcPresentationStyleSelect::IFCSURFACESTYLE ) { // This style is a surface style?
                    ifc2x3::IfcSurfaceStyle* ss = pss->getIfcSurfaceStyle();
                    if( ss != NULL ) {
                        ifc2x3::Set_IfcSurfaceStyleElementSelect_1_5 sses = ss->getStyles();
                        ifc2x3::Set_IfcSurfaceStyleElementSelect_1_5::iterator ssesIter = sses.begin(); // Getting style elements
                        for( ; ssesIter != sses.end() ; ++ssesIter ) {
                            if( (*ssesIter)->currentType() == ifc2x3::IfcSurfaceStyleElementSelect::IFCSURFACESTYLESHADING ) { // Shading style?
                                ifc2x3::IfcSurfaceStyleShading* sss = (*ssesIter)->getIfcSurfaceStyleShading();
                                ifc2x3::IfcColourRgb* rgb = sss->getSurfaceColour(); // Getting rgb
                                if( rgb != NULL ) {
                                    ifc2x3::IfcNormalisedRatioMeasure r = rgb->getRed();
                                    ifc2x3::IfcNormalisedRatioMeasure g = rgb->getGreen();
                                    ifc2x3::IfcNormalisedRatioMeasure b = rgb->getBlue();
                                    std::cout << "SURFACE STYLE SHADING rgb: " << r << ", " << g << ", " << b << "\n";
                                }
                            }  
                        }
                    }
                }
            }                
        }
        /*
        ifc2x3::Inverse_Set_IfcPresentationLayerAssignment_0_n psa = (*represItemsIter)->getLayerAssignments();
        ifc2x3::Inverse_Set_IfcPresentationLayerAssignment_0_n::iterator psaIter = psa.begin();
        for( ; psaIter != psa.end() ; ++psaIter ) {
            std::cout << "(*psaIter)[3] Key=" << (*psaIter)->getKey() << std::endl;
        }
        ifc2x3::Inverse_Set_IfcStyledItem_0_1 sbi = (*represItemsIter)->getStyledByItem();
        ifc2x3::Inverse_Set_IfcStyledItem_0_1::iterator sbiIter = sbi.begin();
        if( sbiIter != sbi.end() ) {
            std::cout << "(*sbiIter)[3] Key=" << (*sbiIter)->getKey() << std::endl;
        }
        */
/*
            ifc2x3::Set_IfcPresentationStyleSelect_1_n pss = (*psaIt)->getStyles();
            ifc2x3::Set_IfcPresentationStyleSelect_1_n::iterator pssIt = pss.begin();
            if( pssIt != pss.end() ) {
                    printf("\nHERE5!\n");
                Step::RefPtr<ifc2x3::IfcPresentationStyleSelect> pss = (*pssIt);
                if( pss->currentType() == ifc2x3::IfcPresentationStyleSelect::IFCSURFACESTYLE ) {
                    printf("\nHERE6!\n");
                }
            }                
        }
*/
    }
}
/*
-List_IfcRepresentation_1_n &IfcProductRepresentation::getRepresentations()
-IfcRepresentation 
-IfcProductRepresentation 
-virtual Set_IfcRepresentationItem_1_n &getItems();
-IfcRepresentationItem
-virtual Inverse_Set_IfcStyledItem_0_1 &getStyledByItem();
-IfcStyledItem
-virtual Set_IfcPresentationStyleAssignment_1_n &getStyles();
IfcPresentationStyleAssignment
IfcPresentationStyleSelect
union IfcPresentationStyleSelect_union {
            IfcNullStyle m_IfcNullStyle; IfcCurveStyle *m_IfcCurveStyle; IfcSymbolStyle *m_IfcSymbolStyle;
            IfcFillAreaStyle *m_IfcFillAreaStyle; IfcTextStyle *m_IfcTextStyle; IfcSurfaceStyle *m_IfcSurfaceStyle;
        };
IfcSurfaceStyle
virtual Set_IfcSurfaceStyleElementSelect_1_5 &getStyles();
IfcSurfaceStyleElementSelect
union IfcSurfaceStyleElementSelect_union {
            IfcSurfaceStyleShading *m_IfcSurfaceStyleShading; IfcSurfaceStyleLighting *m_IfcSurfaceStyleLighting;
            IfcSurfaceStyleWithTextures *m_IfcSurfaceStyleWithTextures; IfcExternallyDefinedSurfaceStyle *m_IfcExternallyDefinedSurfaceStyle;
            IfcSurfaceStyleRefraction *m_IfcSurfaceStyleRefraction;
        };
IfcSurfaceStyleShading
virtual IfcColourRgb *getSurfaceColour();
IfcColourRgb
virtual IfcNormalisedRatioMeasure getBlue();
*/
;
}


bool BrepReaderVisitor::visitIfcRelAssociatesMaterial(ifc2x3::IfcRelAssociatesMaterial *ifcRelAssociatesMaterial) {

    std::vector<MaterialName> mlNames; // To store the names of the materials read.    
    std::vector<double> mlThickness; // To store the thickness of the material layers read.
    std::vector<Step::Id> mlKeys; // To store the keys () of the materials read.    

    std::cout << " IfcRelAssociatesMaterial" << std::endl;
    ifc2x3::IfcMaterialSelect* rm = ifcRelAssociatesMaterial->getRelatingMaterial(); // The relating material of type 'IfcMaterialSelect'
    if( rm != NULL ) {
        std::cout <<  " Relaing material type: " << rm->currentTypeName() << std::endl;
        if( rm->currentType() == ifc2x3::IfcMaterialSelect::IFCMATERIALLAYERSETUSAGE ) {
            ifc2x3::IfcMaterialLayerSetUsage *mlsu = rm->getIfcMaterialLayerSetUsage();
            if( mlsu != NULL ) {
                ifc2x3::IfcMaterialLayerSet *mls = mlsu->getForLayerSet(); // Getting a MATERIAL LAYER SET
                if( mls != NULL ) {
                    std::cout <<  "  Material Layer Set " << std::endl;
                    ifc2x3::List_IfcMaterialLayer_1_n layers = mls->getMaterialLayers(); // Getting the LAYERS
                    ifc2x3::List_IfcMaterialLayer_1_n::iterator layersIt = layers.begin();
                    for( ; layersIt != layers.end() ; ++layersIt ) {                     // Iterating the LAYERS
                        double thickness = (*layersIt)->getLayerThickness();
                        ifc2x3::IfcMaterial* material = (*layersIt)->getMaterial();
                        if( material != NULL ) {
                            std::cout << "   Material name: " << material->getName();
                            std::cout << ", Material key: " << material->getKey();
                            std::cout << ", thickness=" << thickness << std::endl;
                            mlNames.push_back( material->getName() );
                            mlThickness.push_back( thickness );
                            mlKeys.push_back( material->getKey() );
                        }
                    }
                }
            }
        }
    }

    ifc2x3::Set_IfcRoot_1_n relObjs = ifcRelAssociatesMaterial->getRelatedObjects(); // Getting all the objects related to the materials.
    ifc2x3::Set_IfcRoot_1_n::iterator relObjsIt = relObjs.begin(); // Retrieving the iterator
    for( ; relObjsIt != relObjs.end() ; ++relObjsIt ) {  // Iterating...
        std::cout << " Related object: type=" << (*relObjsIt)->type();
        std::cout << ", id=" << (*relObjsIt)->getGlobalId() << ", name= " << (*relObjsIt)->getName() << "\n";
        MaterialAssignmentKey objectGlobalId = (*relObjsIt)->getGlobalId(); // An id of an object the material is assigned to.

        std::map<MaterialAssignmentKey, MaterialLayers>::iterator objectFound = 
            _brepBuilder->mlAssignments.find( objectGlobalId ); // Searching for the key in the map...
        if( objectFound == _brepBuilder->mlAssignments.end() ) { // ...if not found...
            MaterialLayers mls; // ...adding one...
            bool success;
            std::pair<std::map<MaterialAssignmentKey, MaterialLayers>::iterator, bool>(objectFound, success) = 
                _brepBuilder->mlAssignments.insert( std::pair<MaterialAssignmentKey, MaterialLayers>(objectGlobalId, mls ) );
            if( !success ) { // If failed to insert a new key-value pair...
                continue; // ... continuing the cycle without adding a layer.
            }
        }
        for( int i = 0 ; i < mlNames.size() ; i++ ) { // Adding a new material (layer thickness and name) to all the objects related...
            MaterialLayer ml = { mlKeys[i], mlNames[i], mlThickness[i] };
            objectFound->second.push_back( ml ); // For each object pushing a yet another material's name and layer thickness.
        }
    }
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

} // End of namespace Spider3d