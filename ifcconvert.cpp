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

#include "BrepBuilder.h"
#include "BrepReaderVisitor.h"

using namespace Spider3d;

class ConsoleCallBack : public Step::CallBack
{
public:
    ConsoleCallBack() : _max(100) {}
    virtual void setMaximum(size_t max) { _max = max; }
    virtual void setProgress(size_t progress) { std::cerr << int(double(progress)/double(_max)*100.0) << "%..."; }
    virtual bool stop() const {return false;}

protected:
    size_t _max;
};

static const char *cpInputFileKey = "IfcFile";
static const char *cpOutputPathKey = "OutputPath";
static int loadIni( const char *configFile, std::map<std::string, std::string>& configParameters );

static const char *cpFileOper = "oper.txt";
static const char *cpFileMat = "mat.txt";
static const char *cpFileMod = "mod.txt";

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

    std::cout << "Reading the model..." << std::endl;

    // ** Getting the model
    ifc2x3::ExpressDataSet * expressDataSet = dynamic_cast<ifc2x3::ExpressDataSet*>(reader.getExpressDataSet());

    if( expressDataSet == NULL ) {
        std::cout << "There is no ExpressDataSet. Exiting...\n" << std::endl;
        return (2);
    }

    BRepBuilder brepBuilder;
    
    BrepReaderVisitor visitor( &brepBuilder );

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


    // Opening the 'operations' file 
    std::ofstream fsOper;
    fsOper.open( (configParameters[cpOutputPathKey] + std::string(cpFileOper)).c_str() );    
    if( fsOper.fail() ) {
        std::cout << "Can't write into the " << cpFileOper << " (operations) file. Exiting..." << std::endl; 
        return 0;
    }
    // Writing operations...
    fsOper.close();

    // Opening the 'models' file 
    std::ofstream fsMod;
    fsMod.open( (configParameters[cpOutputPathKey] + std::string(cpFileMod)).c_str() );    
    if( fsMod.fail() ) {
        std::cout << "Can't write into the " << cpFileMod << " (models) file. Exiting..." << std::endl; 
        return 0;
    }
    // Writing materials...
    fsMod.close();

    // Opening the 'materials' file...
    std::ofstream fsMat;
    fsMat.open( (configParameters[cpOutputPathKey] + std::string(cpFileMat)).c_str() );    
    if( fsMat.fail() ) {
        std::cout << "Can't write into the " << cpFileMat << " (materials) file. Exiting..." << std::endl; 
        return 0;
    }
    // Writing materials...
    fsMat.close();

    return 0;
}


static int loadIni( const char *configFile, std::map<std::string, std::string>& configParameters ) {
    std::ifstream infile( configFile );
    std::string line;

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
    return 0;
}
