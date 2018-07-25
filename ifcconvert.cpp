// IFC SDK : IFC2X3 C++ Early Classes
// Copyright (C) 2009 CSTB
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full license is in Licence.txt file included with this
// distribution or is available at :
//     http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
#include <ifc2x3/SPFReader.h>
#include <ifc2x3/SPFWriter.h>
#include <ifc2x3/ExpressDataSet.h>
#include <ifc2x3/IfcProject.h>
#include <ifc2x3/IfcLocalPlacement.h>
#include <ifc2x3/IfcAxis2Placement.h>
#include <ifc2x3/IfcAxis2Placement2D.h>
#include <ifc2x3/IfcAxis2Placement3D.h>

#include <Step/CallBack.h>

#include <iostream>
#include <map>

#include "Builder.h"
#include "Visitor.h"

using namespace IfcConvert;

class ConsoleCallBack : public Step::CallBack
{
public:
    ConsoleCallBack() : max(100), done(false) {;}
    virtual void setMaximum( size_t max ) { this->max = max; }
    virtual void setProgress( size_t progress ) { 
        if( !this->done ) {  
            size_t progressPct = int( double(progress)/double(this->max)*100.0 + 0.5 );
            std::cerr << "\r" << progressPct << "%...   ";
            if( progressPct >= 100 ) { std::cerr << "\r" << "Done importing!     " << std::endl; this->done = true; }
        }
    }
    virtual bool stop() const {return false;}

protected:
    size_t max;
    bool done;
};

struct ExportedMaterial {
    ifc2x3::IfcLabel name;
};


static const char *cpInputFileKey = "IfcFile";
static const char *cpOutputPathKey = "TextFilesDir";
static int loadIni( const char *configFile, std::map<std::string, std::string>& configParameters );

static const char *cpFileOper = "oper.txt";
static const char *cpFileMat = "mat.txt";
static const char *cpFileMod = "model.txt";
static const char *cpFileOperMat = "oper_mat.txt";

int main(int argc, char **argv)
{
    if( argc < 2 ) {
        std::cout << "Invalid command line arguments. Use " << argv[0] << " <input-ifc-file-name>" << std::endl;
        return 1;
    }

    std::map<std::string, std::string> configParameters;
    loadIni( argv[1], configParameters );
    if( configParameters.find(cpInputFileKey) == configParameters.end() ) {
        std::cout << "An input file hasn't been specified!\nExiting...\n";
        return(1);
    } 
    if( configParameters.find(cpOutputPathKey) == configParameters.end() ) {
        std::cout << "An output path hasn't been specified!\nExiting...\n";
        return(1);
    }

    std::cout << "Importing:" << std::endl;

    std::ifstream ifcFile;
    ifcFile.open( configParameters[cpInputFileKey].c_str() );

    ifc2x3::SPFReader reader;
    ConsoleCallBack cb;
    reader.setCallBack(&cb);

    if ( ifcFile.is_open() ) {
        std::cout << "reading " << argv[1] << "..." << std::endl;
    } else {
        std::cout << "ERROR: failed to open <" << argv[1] << ">" << std::endl;
        return 1;
    }

    // get length of file
    ifcFile.seekg (0, ifcFile.end);
    std::ifstream::pos_type length = ifcFile.tellg();
    ifcFile.seekg (0, ifcFile.beg);

    bool result = reader.read( ifcFile, (std::ifstream::pos_type)length );
    ifcFile.close();

    if (result) {
        std::cout << "Read Ok." << std::endl;
    } else {
        std::cout << "An error occured:" << std::endl;
        std::vector<std::string> errors = reader.errors();
        std::vector<std::string>::iterator it = errors.begin();
        while(it != errors.end()) {
            std::cout << *it << std::endl;
            ++it;
        }
        std::cout << "Exiting..." << std::endl;
        return 1;
    }

    std::cout << "Getting Express data set..." << std::endl;

    // ** Getting the model
    ifc2x3::ExpressDataSet* expressDataSet = dynamic_cast<ifc2x3::ExpressDataSet*>(reader.getExpressDataSet());

    if( expressDataSet == NULL ) {
        std::cout << "There is no ExpressDataSet. Exiting...\n" << std::endl;
        return (2);
    }

    Builder builder;

    std::cout << "\n**** Reading (measure) units...\n";
    builder.readUnits( expressDataSet );
    
    Visitor visitor( &builder );

    std::cout << "\n****Reading material definition representations...\n";
    Step::RefLinkedList< ifc2x3::IfcMaterialDefinitionRepresentation >::iterator mdefIt = 
        expressDataSet->getAllIfcMaterialDefinitionRepresentation().begin();
    for( ; mdefIt != expressDataSet->getAllIfcMaterialDefinitionRepresentation().end(); ++mdefIt ) {
        mdefIt->acceptVisitor(&visitor);
    }

    std::cout << "\n****Reading materials...\n";
    Step::RefLinkedList< ifc2x3::IfcRelAssociatesMaterial >::iterator ramIt = expressDataSet->getAllIfcRelAssociatesMaterial().begin();
    for( ; ramIt != expressDataSet->getAllIfcRelAssociatesMaterial().end(); ++ramIt ) {
        ramIt->acceptVisitor(&visitor);
    }

    std::cout << "\n****Reading building elements...\n";
    Step::RefLinkedList< ifc2x3::IfcProject >::iterator projIt = expressDataSet->getAllIfcProject().begin();
    for( ; projIt != expressDataSet->getAllIfcProject().end(); ++projIt ) {
        projIt->acceptVisitor(&visitor);
    }

    std::cout << "\n\n********* PRODUCTS:\n";
    for( int i = 0 ; i < builder.products.size() ; i++ ) {
        Product* p = &builder.products[i];
        std::cout << "id:" << p->id << ", name: " << p->name;
        std::cout << ", hierarchy: " << p->hierarchy << ", ifcname" << p->ifcType << std::endl;
    }

    std::map<Step::Id, ExportedMaterial> exportedMaterials; // Exported materials: to build the "mat.txt" file
    std::cout << "\n\n********* MATERIALS:\n";
    std::map<ifc2x3::IfcGloballyUniqueId, MaterialLayers>::const_iterator assignIter = builder.mlAssignments.begin();
    for( ; assignIter != builder.mlAssignments.end() ; ++assignIter ) {
        MaterialLayers::const_iterator layerIter = assignIter->second.begin();
        std::cout << "material assignment key: " << assignIter->first << " #" << assignIter->second.size() << std::endl; 
        for( ; layerIter != assignIter->second.end() ; ++layerIter ) {
            std::cout << "name: " << layerIter->name << ", material key=" << layerIter->key << std::endl;

            // Exported materials: to build the "mat.txt" file
            std::map<Step::Id, ExportedMaterial>::iterator iter = exportedMaterials.find( layerIter->key );
            if( iter == exportedMaterials.end() ) {
                ExportedMaterial em = { layerIter->name };
                exportedMaterials.insert( std::pair<Step::Id,ExportedMaterial>(layerIter->key, em ) );
            }
        }
    }    

    // Opening the 'operations' file 
    std::ofstream fsOper;
    fsOper.open( (configParameters[cpOutputPathKey] + std::string(cpFileOper)).c_str() );    
    if( fsOper.fail() ) {
        std::cout << "Can't write into the " << cpFileOper << " (operations) file. Exiting..." << std::endl; 
        return 0;
    }

    // ****************************************************************************************************************************
    // Writing operations...
    
    // Iterating through all the products to split out ones with and without "children" 
    for( int i = 0 ; i < builder.products.size()-1 ; i++ ) {
        Product* pi = &builder.products[i];
        if( pi->hierarchy >= builder.products[i+1].hierarchy ) {
            pi->firstChild = -1;
        } else {
            pi->firstChild = i+1;
        }
    }
    fsOper << "Level\tCode\tName\tDPH\tType\tVolPlan\tUnit\tPrior\tAsapStart\tAsapFin\tFactStart\tFactFin\tDurPlanD\tDurPlan\tModel" << std::endl;
    for( int i = 0 ; i < builder.products.size() ; i++ ) {
        Product* p = &builder.products[i];

        if( p->name.size() == 0 ) {
            if( p->ifcType.size() > 0 ) {
                p->name.assign( p->ifcType.begin(), p->ifcType.end() );
            } else {
                std::string unnamed = "Unnamed";
                p->name.assign( unnamed.begin(), unnamed.end() );
            }
        } 
        if( p->firstChild != -1 ) {
            fsOper << p->hierarchy;
        } 
        fsOper << "\t" << p->id << "\t" << p->name << "\t\t\t\t\t\t\t\t\t\t\t\t" << p->id << std::endl;
    }    
    fsOper.close();

    // Opening the 'models' file 
    std::ofstream fsMod;
    fsMod.open( (configParameters[cpOutputPathKey] + std::string(cpFileMod)).c_str() );    
    if( fsMod.fail() ) {
        std::cout << "Can't write into the " << cpFileMod << " (models) file. Exiting..." << std::endl; 
        return 0;
    }
    // Writing models...
    fsMod << "Code\tName\tDescription" << std::endl;
    for( int i = 0 ; i < builder.products.size() ; i++ ) {
        Product* p = &builder.products[i];
        Model* m = &(p->model);
        fsMod << p->id << "\t" << p->name << "\t";
        for( int iM = 0 ; iM < m->faces.size() ; iM++ ) {
            fsMod << "<facet>";            
            Face* f = &m->faces[iM];
            for( int iP = 0 ; iP < f->points.size() ; iP++ ) {
                Point* p = &f->points[iP];
                fsMod << "<point>" << p->x << "," << p->y << "," << p->z << "</point>";
            }
            fsMod << "</facet>";
        }
        fsMod << std::endl;
    }
    fsMod.close();

    // Opening the 'materials' file...
    std::ofstream fsMat;
    fsMat.open( (configParameters[cpOutputPathKey] + std::string(cpFileMat)).c_str() );    
    if( fsMat.fail() ) {
        std::cout << "Can't write into the " << cpFileMat << " (materials) file. Exiting..." << std::endl; 
        return 0;
    }
    // Writing materials...
    fsMat << "Code\tName\tType\tUnit" << std::endl;
    std::map<Step::Id, ExportedMaterial>::const_iterator iter = exportedMaterials.begin();
    for( ; iter != exportedMaterials.end() ; ++iter ) {
        fsMat << iter->first << "\t" << iter->second.name << "\t\t" << std::endl;
    }
    fsMat.close();

    // Opening the 'oper_mat' file...
    std::ofstream fsOperMat;
    fsOperMat.open( (configParameters[cpOutputPathKey] + std::string(cpFileOperMat)).c_str() );    
    if( fsOperMat.fail() ) {
        std::cout << "Can't write into the " << cpFileOperMat << " (materials) file. Exiting..." << std::endl; 
        return 0;
    }
    // Writing materials...
    fsOperMat << "OperCode\tMatCode\tFix" << std::endl;
    assignIter = builder.mlAssignments.begin(); // assignIter is defined earlier...
    for( ; assignIter != builder.mlAssignments.end() ; ++assignIter ) {
        if( assignIter->second.size() == 0 ) { // No layers assigned to the product
            continue;
        }
        // Searching for the product assigned
        Product *p=NULL; 
        for( int i = 0 ; i < builder.products.size() ; i++ ) {
            if( builder.products[i].id == assignIter->first ) {
                p = &builder.products[i];
                break;
            }
        }
        if( p == NULL ) {
            continue;
        }
        // Iterating layer by layer
        MaterialLayers::const_iterator layerIter = assignIter->second.begin();
        for( ; layerIter != assignIter->second.end() ; ++layerIter ) {
            double number=0.0;
            if( p->geometryType == "IfcRectangleProfileDef" ) {
                number = layerIter->thickness * p->height;
            }
            fsOperMat << assignIter->first << "\t" << layerIter->key << "\t" << number << std::endl;
        }
    }
    fsOperMat.close();

    return 0;
}


static int loadIni( const char *configFile, std::map<std::string, std::string>& configParameters ) {
    std::ifstream infile( configFile );
    std::string line;

    if( infile.is_open() ) {
        while( std::getline( infile, line ) )   {
            std::istringstream iss_line( line );
            std::string key;
            if( std::getline( iss_line, key, '=' ) ) {
                std::string value;
                if( std::getline(iss_line, value ) ) {
                    configParameters[key] = value;
                }
            }
        }
        infile.close();
        return 0;
    }
    return -1;
}
