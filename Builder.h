#ifndef Builder_H
#define Builder_H

#include <ifc2x3/DefinedTypes.h>
#include <ifc2x3/all.h>

namespace IfcConvert {

	struct LengthUnit {
		ifc2x3::IfcSIPrefix prefix; // IfcSIPrefix_UNSET, IfcSIPrefix_MILLI,
		Step::Integer exponent;
		ifc2x3::IfcLabel unitName;
	};

	class Point {
		public:
			double x, y, z;
			Point( void ) {;}
			Point( double x, double y, double z ) : x(x), y(y), z(z) {;}
			~Point(){;}
	};

	class Face {
		public:
			std::vector<Point> points;

			Face() {;}
			~Face() {;}
			int addPoint( double x, double y, double z ) {
				this->points.push_back( Point(x,y,z) );
				return 0;
			}
	};

	class Model {
		public:
			std::vector<Face> faces; 

			Model() {;}
			~Model() {;}
			int addFace() {
				this->faces.push_back( Face() );
				return 0;
			}
			Face* getLastFace() {
				int size = this->faces.size(); 
				if( size == 0 ) { 
					return NULL; 
				} 
				return &(this->faces[size-1]); 
			}
	};


	class Product {
		public:
		std::string geometryType;
		int length, width, height;

		Model model;
		ifc2x3::IfcGloballyUniqueId id;
		std::wstring name;
		std::string ifcType;
		int hierarchy;

		Product( std::wstring id, std::wstring name, std::string ifcType, int productHierarchy ) : 
			length(0), width(0), height(0), geometryType("") 
		{
			this->id = id;
			this->name = name;
			this->ifcType = ifcType;
			this->hierarchy  = productHierarchy ;
		}
		~Product() {;}
	};


	struct MaterialRepresentation {
		ifc2x3::IfcNormalisedRatioMeasure red;
		ifc2x3::IfcNormalisedRatioMeasure green;
		ifc2x3::IfcNormalisedRatioMeasure blue;
	};


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

	struct MaterialLayer {
		Step::Id key; // A key (#NUM) that starts each line of an ".ifc" file.  
	    ifc2x3::IfcLabel name;
	    double thickness;
	};

	typedef std::vector<MaterialLayer> MaterialLayers;

	class Builder
	{
	public:
		LengthUnit lengthUnit;

		std::map<Step::Id, MaterialRepresentation> mRepresentations; // To store color (and later other characteristics) of material
		std::map<ifc2x3::IfcGloballyUniqueId, MaterialLayers> mlAssignments; // To store materials assigned to each object

		std::vector<Placement> placements;

	    std::vector<Product> products;

	    int productHierarchy;
	    int ifcHierarchy;

		Builder() : productHierarchy(0), ifcHierarchy(0) {;}
		~Builder() {;}

		void pushPlacement( ifc2x3::IfcAxis2Placement3D *value );
		void popPlacement();
	    void addProduct( ifc2x3::IfcProduct *value );

		void addFace( ifc2x3::IfcFace *value );
		void addFace( void );

		void addPoint( ifc2x3::IfcCartesianPoint *value );
		void addPoint( double x, double y, double z );


		virtual void addRepresentation( ifc2x3::IfcRepresentationItem *value ) {
			std::cout << "Representation: " << value->getKey() << std::endl;
		}

		virtual void addClosedShell( ifc2x3::IfcClosedShell *value ) {
			std::cout << "Closed Shell: " << value->getKey() << std::endl;
		}

		Face* getCurrentFace( void );
		Product* getCurrentProduct( void );

		void changeProductHierarchy( int change ) {
			this->productHierarchy += change;
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

		int readUnits( ifc2x3::ExpressDataSet* ExpressDataSet ); 
	};


} // End of namespace Spider3d

#endif // BRepBuilder_H
