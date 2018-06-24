#ifndef BRepBuilder_H
#define BRepBuilder_H

#include <ifc2x3/DefinedTypes.h>
#include <ifc2x3/all.h>

struct BRepBuilderPlacement {
	double iEX, jEX, kEX; 
	double iEY, jEY, kEY;
	double iEZ, jEZ, kEZ;
	double iO, jO, kO;

	BRepBuilderPlacement( double iEX, double jEX, double kEX, 
		double iEZ, double jEZ, double kEZ, double iO, double jO, double kO ) {
		this->iEX = iEX; this->jEX = jEX; this->kEX = kEX;
		this->iEZ = iEZ; this->jEZ = jEZ, this->kEZ = kEZ;
		this->iEY = jEX * kEZ - jEZ * kEX;
		this->jEY = iEX * kEZ - iEZ * kEX;
		this->kEY = iEX * jEZ - iEZ * jEX;		
		this->iO = iO, this->iO = iO, this->iO = iO;
	}

	~BRepBuilderPlacement() {;}

	void toInitialAxis( double& x, double& y, double& z ) {
		double xCopy = x, yCopy = y, zCopy = z;

		x = this->iEX * xCopy + this->iEY * yCopy + this->iEZ * zCopy + this->iO;
		y = this->jEX * xCopy + this->jEY * yCopy + this->jEZ * zCopy + this->jO;
		z = this->kEX * xCopy + this->kEY * yCopy + this->kEZ * zCopy + this->kO;
	}
};

class BRepBuilder
{
public:
	std::ofstream *pOutputFile;
	int nProductsOpened;
	int nProductsClosed;
	int nFacesOpened;
	int nFacesClosed;
	int nPoints;

	std::vector<BRepBuilderPlacement> placements;
	
	BRepBuilder( std::ofstream *pOutputFile ) : pOutputFile(pOutputFile), 
		nProductsOpened(0), nProductsClosed(0), nFacesOpened(0), nFacesClosed(0), nPoints(0) {
		;
	}

	virtual ~BRepBuilder() {;}

	virtual void pushPlacement( ifc2x3::IfcAxis2Placement3D *value )
	{
		ifc2x3::IfcDirection *axis = value->getAxis();
		ifc2x3::IfcDirection *refDirection = value->getRefDirection();
		ifc2x3::IfcCartesianPoint *location = value->getLocation();
		if( axis != NULL && refDirection != NULL && location != NULL ) {
			ifc2x3::List_Real_2_3 EZ, EX;
			EZ = axis->getDirectionRatios(); 
			EX = refDirection->getDirectionRatios(); // Here is the error!			
			ifc2x3::List_IfcLengthMeasure_1_3 center = location->getCoordinates();
			this->placements.push_back( BRepBuilderPlacement( EX[0], EX[1], EX[2], EZ[0], EZ[1], EZ[2], center[0], center[1], center[2] ) );
			std::cout << "PLACEMENT pushed : " << value->getKey() << ", iEX=" << EX[0] << ", jEX=" << EX[1] << ", kEX=" << EX[2] << ", iEZ=" << EZ[0] << ", jEZ=" << EZ[1] << ", kEZ=" << EZ[2] << ", iO" << center[0] << ", jO" << center[1] << ", kO" << center[2] << std::endl;
		} else {
			this->placements.push_back( BRepBuilderPlacement( 1, 0, 0, 0, 0, 1, 0, 0, 0 ) );			
			std::cout << "PLACEMENT pushed : " << value->getKey() << " <default>" << std::endl;
		}
	}

	virtual void popPlacement()
	{
		if( this->placements.size() > 0 ) {
			this->placements.pop_back();			
		}
		std::cout << "PLACEMENT poped" << std::endl;
	}

    virtual void addProduct( ifc2x3::IfcProduct *value )
	{
		std::cout << "PRODUCT: " << value->getKey() << ", GUID=" << value->getGlobalId() << ", NAME=" << value->getName() << ", Representation: " << value->getRepresentation() << std::endl;
		this->closeFaceIfRequired();
		if( this->nProductsOpened > this->nProductsClosed ) {
			(*this->pOutputFile) << std::endl;	
			this->nProductsClosed++;
		}
		(*this->pOutputFile) << value->getGlobalId() << ";";
		this->nProductsOpened++;
	}

	virtual void addRepresentation( ifc2x3::IfcRepresentationItem *value )
	{
		std::cout << "Representation: " << value->getKey() << std::endl;
	}

	virtual void addClosedShell( ifc2x3::IfcClosedShell *value )
	{
		std::cout << "Closed Shell: " << value->getKey() << std::endl;
	}

	virtual void addFace( ifc2x3::IfcFace *value )
	{
		//std::cout << "Face: " << value->getKey() << std::endl;
		if( !(this->nProductsOpened > this->nProductsClosed) ) {
			std::cout << "A face cannot be placed outside a product!" << std::endl;
			return;
		}		
		this->closeFaceIfRequired();
		(*this->pOutputFile) << "<facet>";
		this->nFacesOpened++;
	}

	virtual void addPoint( ifc2x3::IfcCartesianPoint *value )
	{
		ifc2x3::List_IfcLengthMeasure_1_3 xyz = value->getCoordinates();
		// std::cout << "Point: " << value->getKey() << ", x=" << xyz[0] << ", y=" << xyz[1] << ", z=" << xyz[2] <<  std::endl;
		if( !(this->nProductsOpened > this->nProductsClosed) ) {
			std::cout << "A point cannot be placed outside a product!" << std::endl;
			return;
		}		
		if( !(this->nFacesOpened > this->nFacesClosed) ) {
			std::cout << "A point cannot be placed outside a face!" << std::endl;
			return;
		}		

		// Going back to the initial coordinates
		for( int i = this->placements.size()-1 ; i >= 0 ; i-- ) {
			//std::cout << "OLD POINT: " << xyz[0] << "," << xyz[1] << "," << xyz[2] << std::cout;
			this->placements[i].toInitialAxis( xyz[0], xyz[1], xyz[2] );
			//std::cout << "NEW POINT: " << xyz[0] << "," << xyz[1] << "," << xyz[2] << std::cout;
		}

		(*this->pOutputFile) << "<point>" << xyz[0] << "," << xyz[1] << "," << xyz[2] << "</point>";
		this->nPoints++;
	}

	void closeFaceIfRequired( void ) {
		if( this->nFacesOpened > this->nFacesClosed ) {
			(*this->pOutputFile) << "</facet>";
			this->nFacesClosed++;
		}		
	}

	void closeTags( void ) {
		closeFaceIfRequired();
	}

};

#endif // BRepBuilder_H
