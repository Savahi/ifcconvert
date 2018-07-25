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


	int Builder::readUnits( ifc2x3::ExpressDataSet *expressDataSet ) 
	{
		this->lengthUnit.exponent = 1;
		this->lengthUnit.prefix = ifc2x3::IfcSIPrefix_UNSET;
		this->lengthUnit.unitName = "m";

		// Creating a map "siUnitPrefixes": SI-unit key -> prefix
	    std::map<Step::Id,ifc2x3::IfcSIPrefix> siUnitPrefixes;
	    Step::RefLinkedList< ifc2x3::IfcSIUnit >::iterator siUnitIter = expressDataSet->getAllIfcSIUnit().begin();
	    for( ; siUnitIter != expressDataSet->getAllIfcSIUnit().end() ; ++siUnitIter ) {
	        if( siUnitIter->testPrefix() ) {
	            siUnitPrefixes.insert( std::pair<Step::Id,ifc2x3::IfcSIPrefix>( siUnitIter->getKey(), siUnitIter->getPrefix() ) );
	            std::cout << "Prefix added: " << siUnitIter->getPrefix() << std::endl;
	        }
	    }

	    // Reading global unit assignments
	    Step::RefLinkedList< ifc2x3::IfcUnitAssignment >::iterator unitIter = expressDataSet->getAllIfcUnitAssignment().begin();
	    for( ; unitIter != expressDataSet->getAllIfcUnitAssignment().end() ; ++unitIter ) {
	        std::cout << "Type:" << unitIter->type() << std::endl;
	        ifc2x3::Set_IfcUnit_1_n unitSet = unitIter->getUnits();
	        ifc2x3::Set_IfcUnit_1_n::iterator unitSetIter = unitSet.begin();
	        for( ; unitSetIter != unitSet.end() ; ++unitSetIter ) {
	            Step::RefPtr< ifc2x3::IfcUnit > unit = *unitSetIter;
	            if( unit->currentType() == ifc2x3::IfcUnit::IFCDERIVEDUNIT ) { // Just for fun yet
	                ifc2x3::IfcDerivedUnit *derivedUnit = unit->getIfcDerivedUnit();
	                std::cout << "DERIVED UNIT: type=" << derivedUnit->getUnitType() << ", key=" << derivedUnit->getKey() << std::endl;
	            } else if( unit->currentType() == ifc2x3::IfcUnit::IFCNAMEDUNIT ) {
	                ifc2x3::IfcNamedUnit *namedUnit = unit->getIfcNamedUnit();
	                std::cout << "NAMED UNIT: type=" << namedUnit->getUnitType() << ", key=" << namedUnit->getKey();
	                if( namedUnit->getUnitType() == ifc2x3::IfcUnitEnum_LENGTHUNIT ) { // Length unit found
		                ifc2x3::IfcDimensionalExponents *exps = namedUnit->getDimensions();
		                if( exps != NULL ) {
	    	                if( exps->testLengthExponent() ) {
								this->lengthUnit.exponent = exps->getLengthExponent(); // Must be "1"
	                	   	}
	                	}
		              	std::map<Step::Id,ifc2x3::IfcSIPrefix>::iterator siUnitIter = siUnitPrefixes.find(namedUnit->getKey());
		                if( siUnitIter != siUnitPrefixes.end() ) {
		                    this->lengthUnit.prefix = siUnitIter->second;
		                    if( this->lengthUnit.prefix == ifc2x3::IfcSIPrefix_MILLI ) {
		                    	this->lengthUnit.unitName = "mm";
		                    }                     
		                }
		                break;
	                }
	            } else if( unit->currentType() == ifc2x3::IfcUnit::IFCMONETARYUNIT ) { // Just for fun yet
	                ifc2x3::IfcMonetaryUnit *monetaryUnit = unit->getIfcMonetaryUnit();
	                std::cout << "MONETARY UNIT: key=" << monetaryUnit->getKey() << std::endl;
	            }
	        } 
	    }
	    return 0;
	}
}