#include "Visitor.h"

#include <ifc2x3/ExpressDataSet.h>

namespace IfcConvert {

    Visitor::Visitor(Builder* builder) : _builder(builder), _fatherIsOpeningEl(false) {
        ;
    }

    bool Visitor::visitIfcMaterialDefinitionRepresentation(ifc2x3::IfcMaterialDefinitionRepresentation *mdr) {
        Step::Id materialKey;
        ifc2x3::IfcNormalisedRatioMeasure red;
        ifc2x3::IfcNormalisedRatioMeasure green;
        ifc2x3::IfcNormalisedRatioMeasure blue;

        ifc2x3::IfcMaterial* material = mdr->getRepresentedMaterial();
        if( material == NULL ) {
            return true;
        }
        materialKey = material->getKey();
        std::cout << "Material key: " << materialKey << std::endl;

        bool rgbFound = false;
        ifc2x3::List_IfcRepresentation_1_n repres = mdr->getRepresentations(); // Getting representations
        ifc2x3::List_IfcRepresentation_1_n::iterator represIter = repres.begin(); 
        for( ; represIter != repres.end() ; ++represIter ) { // Iterating representations
            std::cout << "[1] List_IfcRepresentation_1_n iterator: key=" << (*represIter)->getKey() << std::endl; 
            ifc2x3::Set_IfcRepresentationItem_1_n represItems = (*represIter)->getItems(); // Getting representation items
            ifc2x3::Set_IfcRepresentationItem_1_n::iterator represItemsIter = represItems.begin();
            for( ; represItemsIter != represItems.end() ; ++represItemsIter ) { // Iterating representation items
                std::cout << "[2] Set_IfcRepresentationItem_1_n iterator: key=" << (*represItemsIter)->getKey(); 
                std::cout << ", type=" << (*represItemsIter)->type() << std::endl;
                
                Step::RefPtr<ifc2x3::IfcStyledItem> si = (*represItemsIter); // Representation item == style item (friend class)
                ifc2x3::Set_IfcPresentationStyleAssignment_1_n psa = si->getStyles(); // Getting style assignments
                ifc2x3::Set_IfcPresentationStyleAssignment_1_n::iterator psaIter = psa.begin();
                for( ; psaIter != psa.end() ; ++psaIter ) {
                    std::cout << "[3] Set_IfcPresentationStyleAssignment_1_n iterator: key=" << (*psaIter)->getKey() << std::endl;
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
                                            red = rgb->getRed();
                                            green = rgb->getGreen();
                                            blue = rgb->getBlue();
                                            std::cout << "SURFACE STYLE SHADING rgb: " << red << ", " << green << ", " << blue << "\n";
                                            rgbFound = true;
                                            break;
                                        }
                                    }  
                                }
                            }
                        }
                    }                
                }
                if( rgbFound ) { break; }
            }
            if( rgbFound ) { break; }
        }
        if( rgbFound ) {
            MaterialRepresentation materialRepresentation = { red, green, blue };
            _builder->mRepresentations.insert( std::pair<Step::Id, MaterialRepresentation>(materialKey, materialRepresentation) );
        }
        return true;
    }


    bool Visitor::visitIfcRelAssociatesMaterial(ifc2x3::IfcRelAssociatesMaterial *ifcRelAssociatesMaterial) {

        std::vector<ifc2x3::IfcLabel> mlNames; // To store the names of the materials read.    
        std::vector<double> mlThickness; // To store the thickness of the material layers read.
        std::vector<Step::Id> mlKeys; // To store the keys () of the materials read.    

        std::cout << "Inside 'visitIfcRelAssociatesMaterial'" << std::endl;
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
            ifc2x3::IfcGloballyUniqueId objectGlobalId = (*relObjsIt)->getGlobalId(); // An id of an object the material is assigned to.

            std::map<ifc2x3::IfcGloballyUniqueId, MaterialLayers>::iterator objectFound = 
                _builder->mlAssignments.find( objectGlobalId ); // Searching for the key in the map...
            if( objectFound == _builder->mlAssignments.end() ) { // ...if not found...
                MaterialLayers mls; // ...adding one...
                //bool success;
                //std::pair<std::map<ifc2x3::IfcGloballyUniqueId, MaterialLayers>::iterator, bool>(objectFound, success) = 
                //    _builder->mlAssignments.insert( std::pair<ifc2x3::IfcGloballyUniqueId, MaterialLayers>(objectGlobalId, mls ) );

                std::pair<std::map<ifc2x3::IfcGloballyUniqueId, MaterialLayers>::iterator, bool> insRes;

                insRes = _builder->mlAssignments.insert( std::pair<ifc2x3::IfcGloballyUniqueId, MaterialLayers>(objectGlobalId, mls ) );

                if( !insRes.second ) { // If failed to insert a new key-value pair...
                    std::cout << "Error inserting a new element into the mlAssigment map!\n";
                    continue; // ... continuing the cycle without adding a layer.
                }
                objectFound = insRes.first;
            }
            for( int i = 0 ; i < mlNames.size() ; i++ ) { // Adding a new material (layer thickness and name) to all the objects related...
                MaterialLayer ml = { mlKeys[i], mlNames[i], mlThickness[i] };
                objectFound->second.push_back( ml ); // For each object pushing a yet another material's name and layer thickness.
            }
        }
        return true;
    }


    bool Visitor::visitIfcObjectDefinition(ifc2x3::IfcObjectDefinition *value)
    {
        _builder->printIfcHierarchy( "Visitor.visitIfcObjectDefinition", 0 );

        ifc2x3::Inverse_Set_IfcRelDecomposes_0_n::iterator itDecomposedBy = value->getIsDecomposedBy().begin();
        while (itDecomposedBy != value->getIsDecomposedBy().end())
        {
            ifc2x3::IfcRelDecomposes* rd = const_cast<ifc2x3::IfcRelDecomposes*> (itDecomposedBy->get());
            rd->acceptVisitor(this);
            ++itDecomposedBy;
        }
        _builder->printIfcHierarchy( "end of Visitor.visitIfcObjectDefinition", 0 );
        return true;
    }

    bool Visitor::visitIfcRelDecomposes(ifc2x3::IfcRelDecomposes *value) {
        std::cout << "visitIfcRelDecomposes()!\n";
    }

    bool Visitor::visitIfcRelAggregates(ifc2x3::IfcRelAggregates *value)
    {
        _builder->printIfcHierarchy(  "Visitor.visitIfcRelAggregates()", 0 );
        ifc2x3::Set_IfcObjectDefinition_1_n::iterator it = value->getRelatedObjects().begin();
        while (it != value->getRelatedObjects().end())
        {
            std::stringstream ss;
            ss << "getRelatedObjects() item: " << (*it)->type();
            _builder->printIfcHierarchy(  ss.str(), 0 );        

            (*it)->acceptVisitor(this);
            ++it;
        }
        _builder->printIfcHierarchy(  "end of Visitor.visitIfcRelAggregates", 0 );
        return true;
    }

    bool Visitor::pushPlacementAndComputeRepresentation(ifc2x3::IfcProduct *value, bool addProduct)
    {
        //push placement
        bool hasPlacement = false;
        if( value->testObjectPlacement() ) {
            ifc2x3::IfcLocalPlacement *lp = (ifc2x3::IfcLocalPlacement*)value->getObjectPlacement();
            ifc2x3::IfcAxis2Placement *a2p = lp->getRelativePlacement();
            ifc2x3::IfcAxis2Placement3D *a2p3d = a2p->getIfcAxis2Placement3D();
            if( a2p3d ) {
                _builder->pushPlacement(a2p3d);
                hasPlacement = true;
            }
            //hasPlacement = value->getObjectPlacement()->acceptVisitor(this);
        }

        if( hasPlacement ) {
            _builder->changeProductHierarchy(1);
        }

        if( addProduct ) {
            _builder->addProduct(value);
        }

        // work on representation
        ifc2x3::IfcProductRepresentation* pr = value->getRepresentation();
        if(pr)
        {
            pr->acceptVisitor(this);
        }

        return hasPlacement;
    }

    bool Visitor::popPlacement( bool hadPlacement ) {
        if( hadPlacement ) {
            _builder->popPlacement();
            _builder->changeProductHierarchy(-1);
        }
    }

    bool Visitor::visitIfcProduct(ifc2x3::IfcProduct *value)
    {
        //push placement
        bool hasPlacement = pushPlacementAndComputeRepresentation(value);
        
        // visit decomposed by object
        visitIfcObjectDefinition(value);

        // pop placement
        popPlacement(hasPlacement);

        return true;
    }

    bool Visitor::visitIfcElement(ifc2x3::IfcElement *value)
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
        popPlacement(hasPlacement);
        
        return true;
    }

    bool Visitor::visitIfcOpeningElement(ifc2x3::IfcOpeningElement *value)
    {
        _builder->printIfcHierarchy(  "Visitor.visitIfcOpeningElement()", 1 );

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
        popPlacement(hasPlacement);

        _builder->printIfcHierarchy(  "end of Visitor.visitIfcOpeningElement()", -1 );
        return true;
    }

    bool Visitor::visitIfcSpatialStructureElement(ifc2x3::IfcSpatialStructureElement *value)
    {
        _builder->printIfcHierarchy(  "Visitor.visitIfcSpatialStructureElement()", 1 );

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
        popPlacement(hasPlacement);

        _builder->printIfcHierarchy(  "end of Visitor.visitIfcSpatialStructureElement()", -1 );
        return true;
    }

    bool Visitor::visitIfcRelContainedInSpatialStructure(ifc2x3::IfcRelContainedInSpatialStructure *value)
    {
        _builder->printIfcHierarchy(  "Visitor.visitIfcRelContainedInSpatialStructure()", 1 );

        ifc2x3::Set_IfcProduct_1_n::iterator itRelatedElement = value->getRelatedElements().begin();
        while (itRelatedElement != value->getRelatedElements().end())
        {
            std::stringstream ss;
            ss << "Visitor.visitIfcRelContainedInSpatialStructure() item: " << (*itRelatedElement)->type();
            _builder->printIfcHierarchy(  ss.str(), 0 );

            (*itRelatedElement)->acceptVisitor(this);
            ++itRelatedElement;
        }

        _builder->printIfcHierarchy(  "end of Visitor.visitIfcRelContainedInSpatialStructure()", -1 );
        return true;
    }


    bool Visitor::visitIfcProductRepresentation(ifc2x3::IfcProductRepresentation *value)
    {
        _builder->printIfcHierarchy(  "Visitor.visitIfcProductRepresentation", 1 );
        ifc2x3::List_IfcRepresentation_1_n::iterator it = value->getRepresentations().begin();
        while (it != value->getRepresentations().end())
        {
            Step::String representationType = (*it)->getRepresentationType();
            std::stringstream ss;
            ss << "representationtype: " << representationType;
            _builder->printIfcHierarchy(  ss.str(), 0 );        
            if (representationType == Step::String("Brep") || true )  // "|| true" .
            {
                (*it)->acceptVisitor(this);
            }
            ++it;
        }
        _builder->printIfcHierarchy(  "end of visitIfcProductRepresentation", -1 );

        return true;
    }

    bool Visitor::visitIfcRepresentation(ifc2x3::IfcRepresentation *value)
    {
        _builder->printIfcHierarchy(  "Visitor.visitIfcRepresentation", 1 );
        ifc2x3::Set_IfcRepresentationItem_1_n::iterator it = value->getItems().begin();
        while (it != value->getItems().end())
        {
            std::stringstream ss;
            ss << "key=" << (*it)->getKey() << ", type=" << (*it)->type();
            _builder->printIfcHierarchy(  ss.str(), 0 );
            (*it)->acceptVisitor(this);
            ++it;
        }
        _builder->printIfcHierarchy(  "end of visitIfcRepresentation", -1 );
        return true;
    }


    bool Visitor::visitIfcCurve(ifc2x3::IfcCurve *value)
    {
        //_builder->printIfcHierarchy(  "Visitor.visitIfcCurve" );
        //_builder->addRepresentation(value);
     
        return true;
    }

    bool Visitor::visitIfcPolyline(ifc2x3::IfcPolyline *value)
    {
        return true;
        _builder->printIfcHierarchy(  "Visitor.visitIfcPolyline", 0 );
        ifc2x3::List_IfcCartesianPoint_3_n::iterator it = value->getPoints().begin();
        while (it != value->getPoints().end())
        {
            (*it)->acceptVisitor(this);
            ++it;
        }
        return true;
    }

    bool Visitor::visitIfcProfileDef(ifc2x3::IfcProfileDef *value) {
        std::stringstream ss;
        if( value->getType().getName() == "IfcRectangleProfileDef" ) {
            ifc2x3::IfcRectangleProfileDef* profileDef = (ifc2x3::IfcRectangleProfileDef*)value;
            double xDim = profileDef->getXDim();
            double yDim = profileDef->getYDim();
            ss << "Visitor.visitIfcProfileDef: " << value->type() << ", xdim=" << xDim << ", ydim=" << yDim; 
            _builder->printIfcHierarchy(  ss.str(), 0 );
        }
        return true;
    }

    bool Visitor::visitIfcSweptAreaSolid(ifc2x3::IfcSweptAreaSolid *value) {
        //ifc2x3::IfcAxis2Placement3D *position = this->getPosition();

        _builder->printIfcHierarchy(  "Visitor.visitIfcSweptAreaSolid->", 0 );
        return true;
    }

    bool Visitor::visitIfcExtrudedAreaSolid(ifc2x3::IfcExtrudedAreaSolid *value) {
        _builder->pushPlacement(value->getPosition());

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
                    ss << "Visitor.visitIfcExtrudedAreaSolid: depth=" << depth;
                    ss << ", i=" << ijk[0] << ", j=" << ijk[1] << ", k=" << ijk[2];
                    ss << ", xdim=" << xDim << ", ydim" << yDim; 
                    _builder->printIfcHierarchy(  ss.str(), 0 );

                    // Storing the dimensions of the product for future use...
                    Product* p = _builder->getCurrentProduct();
                    if( p != NULL ) {
                        p->geometryType.assign("IfcRectangleProfileDef"); 
                        p->length = xDim; p->width = yDim; p->height = depth;
                    }
                    
                    buildExtrudedArea( depth, xDim, yDim, ijk[0], ijk[1], ijk[2] );
                }
            }
        }

        _builder->popPlacement();
        return true;
    }

    bool Visitor::visitIfcFacetedBrep(ifc2x3::IfcFacetedBrep *value)
    {
        _builder->addRepresentation(value);
     
        ifc2x3::IfcClosedShell* closedShell = value->getOuter();
        if(closedShell)
        {
            closedShell->acceptVisitor(this);
        }

        return true;
    }

    bool Visitor::visitIfcClosedShell(ifc2x3::IfcClosedShell *value)
    {
        _builder->addClosedShell(value);

        ifc2x3::Set_IfcFace_1_n::iterator it = value->getCfsFaces().begin();
        while (it != value->getCfsFaces().end())
        {
            (*it)->acceptVisitor(this);
            ++it;
        }

        return true;
    }

    bool Visitor::visitIfcFace(ifc2x3::IfcFace *value)
    {
        _builder->addFace(value);

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

    bool Visitor::visitIfcPolyLoop(ifc2x3::IfcPolyLoop *value)
    {
        ifc2x3::List_IfcCartesianPoint_3_n::iterator it = value->getPolygon().begin();
        while (it != value->getPolygon().end())
        {
            (*it)->acceptVisitor(this);
            ++it;
        }

        return true;
    }

    bool Visitor::visitIfcCartesianPoint(ifc2x3::IfcCartesianPoint *value)
    {
        _builder->addPoint(value);
        return true;
    }

    // placement
    bool Visitor::visitIfcLocalPlacement(ifc2x3::IfcLocalPlacement *value)
    {
        _builder->printIfcHierarchy( "visitIfcLocalPlacement" );
        return value->getRelativePlacement()->acceptVisitor(this);
    }

    bool Visitor::visitIfcAxis2Placement(ifc2x3::IfcAxis2Placement * value)
    {
        _builder->printIfcHierarchy( "visitIfcAxis2Placement" );
        ifc2x3::IfcAxis2Placement3D * axis3placement3d = value->getIfcAxis2Placement3D();
        if(axis3placement3d)
        {
            return axis3placement3d->acceptVisitor(this);
        }

        return true;
    }

    bool Visitor::visitIfcAxis2Placement3D(ifc2x3::IfcAxis2Placement3D * value)
    {
        _builder->printIfcHierarchy( "visitIfcAxis2Placement3D" );
        _builder->pushPlacement(value);
        return true;
    }

    void Visitor::buildExtrudedArea( double depth, double xDim, double yDim, double i, double j, double k ) {
        
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

        _builder->addFace();
        _builder->addPoint(x1,y1,z);
        _builder->addPoint(x2,y1,z);
        _builder->addPoint(x2,y2,z);
        _builder->addPoint(x1,y2,z);

        _builder->addFace();
        _builder->addPoint(x2,y1,z);
        _builder->addPoint(x2new,y1new,znew);
        _builder->addPoint(x2new,y2new,znew);
        _builder->addPoint(x2,y2,z);

        _builder->addFace();
        _builder->addPoint(x1,y2,z);
        _builder->addPoint(x2,y2,z);
        _builder->addPoint(x2new,y2new,znew);
        _builder->addPoint(x1new,y2new,znew);

        _builder->addFace();
        _builder->addPoint(x1,y1,z);
        _builder->addPoint(x1,y2,z);
        _builder->addPoint(x1new,y2new,znew);
        _builder->addPoint(x1new,y1new,znew);

        _builder->addFace();
        _builder->addPoint(x1,y1,z);
        _builder->addPoint(x1new,y1new,znew);
        _builder->addPoint(x2new,y1new,znew);
        _builder->addPoint(x2,y1,z);

        _builder->addFace();
        _builder->addPoint(x1new,y1new,znew);
        _builder->addPoint(x2new,y1new,znew);
        _builder->addPoint(x2new,y2new,znew);
        _builder->addPoint(x1new,y2new,znew);
    }

} // End of namespace Spider3d