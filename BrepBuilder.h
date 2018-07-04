#ifndef BRepBuilder_H
#define BRepBuilder_H

#include <ifc2x3/DefinedTypes.h>
#include <ifc2x3/all.h>

namespace Spider3d {

struct MaterialLayer {
    std::wstring name;
    double thickness;
};

typedef std::wstring MaterialAssignmentKey;
typedef std::vector<MaterialLayer> MaterialLayers;
typedef std::wstring MaterialName;

struct Placement {
	double iEX, jEX, kEX; 
	double iEY, jEY, kEY;
	double iEZ, jEZ, kEZ;
	double iO, jO, kO;

	Placement( double iEX, double jEX, double kEX, 
		double iEZ, double jEZ, double kEZ, double iO, double jO, double kO ) {
		this->iEX = iEX; this->jEX = jEX; this->kEX = kEX;
		this->iEZ = iEZ; this->jEZ = jEZ, this->kEZ = kEZ;
		this->iEY = jEX * kEZ - jEZ * kEX;
		this->jEY = iEX * kEZ - iEZ * kEX;
		this->kEY = iEX * jEZ - iEZ * jEX;		
		this->iO = iO, this->jO = jO, this->kO = kO;
	}

	~Placement() {;}

	void toInitialAxis( double& x, double& y, double& z ) {
		double xCopy = x, yCopy = y, zCopy = z;

		x = this->iEX * xCopy + this->iEY * yCopy + this->iEZ * zCopy + this->iO;
		y = this->jEX * xCopy + this->jEY * yCopy + this->jEZ * zCopy + this->jO;
		z = this->kEX * xCopy + this->kEY * yCopy + this->kEZ * zCopy + this->kO;
		/*
		double xCopy = x+this->iO, yCopy = y+this->jO, zCopy = z+this->kO;

		x = this->iEX * xCopy + this->iEY * yCopy + this->iEZ * zCopy;
		y = this->jEX * xCopy + this->jEY * yCopy + this->jEZ * zCopy;
		z = this->kEX * xCopy + this->kEY * yCopy + this->kEZ * zCopy;
		*/
	}
};

class BRepBuilder
{
public:
	std::map<MaterialAssignmentKey, MaterialLayers> mlAssignments; // To store materials assigned to each object

	std::ofstream *pOutputFile;
	int nProductsOpened;
	int nProductsClosed;
	int nFacesOpened;
	int nFacesClosed;
	int nPoints;

	std::vector<Placement> placements;

    int hierarchy;
    void printHierarchy( std::string str, bool endOfLine=true );	
	
	BRepBuilder( std::ofstream *pOutputFile ) : pOutputFile(pOutputFile), hierarchy(0),
		nProductsOpened(0), nProductsClosed(0), nFacesOpened(0), nFacesClosed(0), nPoints(0) {
		;
	}

	virtual ~BRepBuilder() {;}

	virtual void pushPlacement( ifc2x3::IfcAxis2Placement3D *value )
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
		this->printHierarchy( ss.str(), 1 );
	}

	virtual void popPlacement()
	{
		if( this->placements.size() > 0 ) {
			this->placements.pop_back();			
		}
		this->printHierarchy( "PLACEMENT poped", -1 );
	}

    virtual void addProduct( ifc2x3::IfcProduct *value )
	{
		std::stringstream ss;
		ss << "PRODUCT: " << value->getKey() << ", GUID=" << value->getGlobalId() << ", NAME=" << value->getName() << ", Representation: " << value->getRepresentation() << std::endl;
		this->printHierarchy( ss.str(), 0 );

		this->closeFaceIfRequired();
		if( this->nProductsOpened > this->nProductsClosed ) {
			(*this->pOutputFile) << std::endl;	
			this->nProductsClosed++;
		}
		(*this->pOutputFile) << value->getGlobalId() << ";" << value->getName() << ";";
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
		if( !isAddPoint() ) { return; }

		ifc2x3::List_IfcLengthMeasure_1_3 xyz = value->getCoordinates();
		// Going back to the initial coordinates
		for( int i = this->placements.size()-1 ; i >= 0 ; i-- ) {
			//std::cout << "OLD POINT: " << xyz[0] << "," << xyz[1] << "," << xyz[2] << std::cout;
			this->placements[i].toInitialAxis( xyz[0], xyz[1], xyz[2] );
			//std::cout << "NEW POINT: " << xyz[0] << "," << xyz[1] << "," << xyz[2] << std::cout;
		}

		(*this->pOutputFile) << "<point>" << xyz[0] << "," << xyz[1] << "," << xyz[2] << "</point>";
		this->nPoints++;
	}

	virtual void addFace( void )
	{
		if( !(this->nProductsOpened > this->nProductsClosed) ) {
			std::cout << "A face cannot be placed outside a product!" << std::endl;
			return;
		}		
		this->closeFaceIfRequired();
		(*this->pOutputFile) << "<facet>";
		this->nFacesOpened++;
	}

	virtual void addPoint( double x, double y, double z )
	{
		if( !isAddPoint() ) { return; }
		//std::cout << "BEFORE: x=" << x << ", y=" << y << ", z=" << z << std::endl;

		// Going back to the initial coordinates
		for( int i = this->placements.size()-1 ; i >= 0 ; i-- ) {
			this->placements[i].toInitialAxis( x, y, z );
		}
		(*this->pOutputFile) << "<point>" << x << "," << y << "," << z << "</point>";
		this->nPoints++;

		//std::cout << "AFTER: x=" << x << ", y=" << y << ", z=" << z << std::endl;
	}

	bool isAddPoint() {
		if( !(this->nProductsOpened > this->nProductsClosed) ) {
			std::cout << "A point cannot be placed outside a product!" << std::endl;
			return false;
		}		
		if( !(this->nFacesOpened > this->nFacesClosed) ) {
			std::cout << "A point cannot be placed outside a face!" << std::endl;
			return false;
		}		
		return true;
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

	void printHierarchy( const std::string str, int changeHierarchy=0, bool endOfLine=true ) 
	{
	    for( int i = 0 ; i < this->hierarchy ; i++ ) {
	        std::cout << "  ";
	    }
	    std::cout << str;
	    if( endOfLine ) {
	        std::cout << std::endl;
	    }
		this->hierarchy += changeHierarchy;
	}
};

} // End of namespace Spider3d

#endif // BRepBuilder_H
