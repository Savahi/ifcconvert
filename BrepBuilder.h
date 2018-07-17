#ifndef BRepBuilder_H
#define BRepBuilder_H

#include <ifc2x3/DefinedTypes.h>
#include <ifc2x3/all.h>

namespace Spider3d {

class Vertex {
	public:
		double mX, mY, mZ;
		Vertex( void ) {;}
		Vertex( double x, double y, double z ) : mX(x), mY(y), mZ(z) {;}
		~Vertex(){;}

		int setXYZ( double x, double y, double z ) {
			this->mX = x;
			this->mY = y;
			this->mZ = z;
			return 0;
		}
		int setXYZ( double *x, double *y, double *z ) {
			if( x != NULL ) { this->mX = *x; }
			if( y != NULL ) { this->mY = *y; }
			if( z != NULL ) { this->mZ = *z; }
			return 0;
		}
};

class Facet {
	public:
		std::vector<Vertex> mVertices;

		Facet() {;}
		~Facet() {;}
		int addPoint( double x, double y, double z ) {
			this->mVertices.push_back( Vertex(x,y,z) );
			return 0;
		}
};

class Model {
	public:
		std::vector<Facet> mFacets; 

		Model() {;}
		~Model() {;}
		int addFacet() {
			this->mFacets.push_back( Facet() );
			return 0;
		}
		Facet* getLastFacet() {
			int size = this->mFacets.size(); 
			if( size == 0 ) { 
				return NULL; 
			} 
			return &(this->mFacets[size-1]); 
		}
};


class Product {
	public:
	Model model;
	std::wstring id;
	std::wstring name;
	int hierarchy;

	Product( std::wstring id, std::wstring name, int spiderHierarchy  ) {
		this->id = id;
		this->name = name;
		this->hierarchy  = spiderHierarchy ;
	}
	~Product() {;}
};


struct MaterialRepresentation {
	ifc2x3::IfcNormalisedRatioMeasure red;
	ifc2x3::IfcNormalisedRatioMeasure green;
	ifc2x3::IfcNormalisedRatioMeasure blue;
};

struct MaterialLayer {
	Step::Id key; // A key (#NUM) that starts each line of an ".ifc" file.  
    std::wstring name;
    double thickness;
};

typedef std::wstring MaterialAssignmentKey;
typedef std::vector<MaterialLayer> MaterialLayers;
typedef std::wstring MaterialName;

class Placement {
	public:
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
	}
};


class BRepBuilder
{
public:
	std::map<Step::Id, MaterialRepresentation> mRepresentations; // To store color (and later other characteristics) of material
	std::map<MaterialAssignmentKey, MaterialLayers> mlAssignments; // To store materials assigned to each object

	std::vector<Placement> placements;

    std::vector<Product> products;

    int spiderHierarchy;
    int ifcHierarchy;

	BRepBuilder() : spiderHierarchy(0), ifcHierarchy(0) {
		;
	}
	virtual ~BRepBuilder() {
		;
	}

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
		this->printIfcHierarchy  ( ss.str(), 1 );
	}

	virtual void popPlacement()
	{
		if( this->placements.size() > 0 ) {
			this->placements.pop_back();			
		}
		this->printIfcHierarchy  ( "PLACEMENT poped", -1 );
	}

    virtual void addProduct( ifc2x3::IfcProduct *value )
	{
		std::stringstream ss;
		ss << "PRODUCT: " << value->getKey() << ", GUID=" << value->getGlobalId() << ", NAME=" << value->getName() << ", Representation: " << value->getRepresentation() << std::endl;
		this->printIfcHierarchy  ( ss.str(), 0 );

		this->products.push_back( Product( value->getGlobalId(), value->getName(), this->spiderHierarchy   ) );
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
		this->addFace();
	}

	virtual void addFace( void )
	{
		Product* product = this->getCurrentProductInHierarchy();
		if( product == NULL ) {
			std::cout << "A face cannot be placed outside a product!" << std::endl;
			return;
		}
		product->model.addFacet();
	}


	virtual void addPoint( ifc2x3::IfcCartesianPoint *value )
	{
		Facet* face = getFaceToAddPoint();
		if( face == NULL ) { return; }

		ifc2x3::List_IfcLengthMeasure_1_3 xyz = value->getCoordinates();
		// Going back to the initial coordinates
		for( int i = this->placements.size()-1 ; i >= 0 ; i-- ) {
			this->placements[i].toInitialAxis( xyz[0], xyz[1], xyz[2] );
		}
		face->addPoint(xyz[0], xyz[1], xyz[2]);
	}

	virtual void addPoint( double x, double y, double z )
	{
		Facet* face = getFaceToAddPoint();
		if( face == NULL ) { return; }

		// Going back to the initial coordinates
		for( int i = this->placements.size()-1 ; i >= 0 ; i-- ) {
			this->placements[i].toInitialAxis( x, y, z );
		}
		face->addPoint(x, y, z);
	}

	Facet* getFaceToAddPoint() {
		Product* product = this->getCurrentProductInHierarchy();
		if( product == NULL ) {
			std::cout << "A point cannot be placed outside a product!" << std::endl;
			return NULL;
		}
		Facet *face = product->model.getLastFacet();
		if( face == NULL ) {
			std::cout << "A point cannot be placed outside a face!" << std::endl;
		}		
		return face;
	}

	void printIfcHierarchy( std::string str, int changeHierarchy=0, bool endOfLine=true )
	{
	    for( int i = 0 ; i < this->ifcHierarchy  ; i++ ) {
	        std::cout << "  ";
	    }
	    std::cout << str;
	    if( endOfLine ) {
	        std::cout << std::endl;
	    }
		this->ifcHierarchy  += changeHierarchy;
	}

	void changeSpiderHierarchy( int change ) {
		this->spiderHierarchy += change;
	}

	Product* getCurrentProductInHierarchy( void ) {
		for( int i = this->products.size()-1 ; i >= 0 ; i-- ) {
			if( this->products[i].hierarchy == this->spiderHierarchy  ) {
				return &this->products[i];
			}  
		}
		return NULL;
	}
};


} // End of namespace Spider3d

#endif // BRepBuilder_H
