#include <ifc2x3/DefinedTypes.h>
#include <ifc2x3/all.h>
#include "Builder.h"

namespace IfcConvert {


	void Builder::pushPlacement( ifc2x3::IfcAxis2Placement3D *value )
	{
		double iEX=1.0, jEX=0.0, kEX=0.0, iEZ=0.0, jEZ=0.0, kEZ=1.0, iO=0.0, jO=0.0, kO=0.0;

		ifc2x3::IfcDirection *axis = value->getAxis();
		if( axis != NULL ) {
			ifc2x3::List_Real_2_3 EZ = axis->getDirectionRatios(); 
			iEZ = EZ[0]; jEZ = EZ[1]; kEZ = EZ[2];
		}
		ifc2x3::IfcDirection *refDirection = value->getRefDirection();
		if( refDirection != NULL  ) {
			ifc2x3::List_Real_2_3 EX = refDirection->getDirectionRatios(); 
			iEX = EX[0]; jEX = EX[1]; kEX = EX[2];
		} 
		ifc2x3::IfcCartesianPoint *location = value->getLocation();
		if( location != NULL ) { 
			ifc2x3::List_IfcLengthMeasure_1_3 center = location->getCoordinates();
			iO = center[0]; jO = center[1]; kO = center[2];
		}
		this->placements.push_back( Placement( iEX, jEX, kEX, iEZ, jEZ, kEZ, iO, jO, kO ) );
		std::stringstream ss;
		ss << "PLACEMENT pushed : " << value->getKey() << ", iEX=" << iEX << ", jEX=" << jEX << ", kEX=" << kEX;
		ss << ", iEZ=" << iEZ << ", jEZ=" << jEZ << ", kEZ=" << kEZ << ", iO=" << iO << ", jO=" << jO << ", kO=" << kO << std::endl;
		this->printIfcHierarchy( ss.str(), 1 );
	}

	void Builder::popPlacement()
	{
		if( this->placements.size() > 0 ) {
			this->placements.pop_back();			
		}
		this->printIfcHierarchy( "PLACEMENT poped", -1 );
	}

    void Builder::addProduct( ifc2x3::IfcProduct *value )
	{
		std::stringstream ss;
		ss << "PRODUCT: " << value->getKey() << ", GUID=" << value->getGlobalId() << ", NAME=" << value->getName() << ", Representation: " << value->getRepresentation() << std::endl;
		this->printIfcHierarchy( ss.str(), 0 );

		this->products.push_back( Product( value->getGlobalId(), value->getName(), value->type(), this->productHierarchy ) );
	}


	void Builder::addFace( ifc2x3::IfcFace *value )
	{
		this->addFace();
	}

	void Builder::addFace( void )
	{
		Product* product = this->getCurrentProduct();
		if( product == NULL ) {
			std::cout << "A face cannot be placed outside a product!" << std::endl;
			return;
		}
		product->model.addFace();
	}

	void Builder::addPoint( ifc2x3::IfcCartesianPoint *value )
	{
		Face* face = this->getCurrentFace();
		if( face == NULL ) { return; }

		ifc2x3::List_IfcLengthMeasure_1_3 xyz = value->getCoordinates();
		// Going back to the initial coordinates
		for( int i = this->placements.size()-1 ; i >= 0 ; i-- ) {
			this->placements[i].toInitialAxis( xyz[0], xyz[1], xyz[2] );
		}
		face->addPoint(xyz[0], xyz[1], xyz[2]);
	}

	void Builder::addPoint( double x, double y, double z )
	{
		Face* face = getCurrentFace();
		if( face == NULL ) { return; }

		// Going back to the initial coordinates
		for( int i = this->placements.size()-1 ; i >= 0 ; i-- ) {
			this->placements[i].toInitialAxis( x, y, z );
		}
		face->addPoint(x, y, z);
	}

	Face* Builder::getCurrentFace() {
		Product* product = this->getCurrentProduct();
		if( product == NULL ) {
			std::cout << "A point cannot be placed outside a product!" << std::endl;
			return NULL;
		}
		Face *face = product->model.getLastFace();
		if( face == NULL ) {
			std::cout << "A point cannot be placed outside a face!" << std::endl;
		}		
		return face;
	}

	Product* Builder::getCurrentProduct( void ) {
		for( int i = this->products.size()-1 ; i >= 0 ; i-- ) {
			if( this->products[i].hierarchy == this->productHierarchy  ) {
				return &this->products[i];
			}  
		}
		return NULL;
	}


}