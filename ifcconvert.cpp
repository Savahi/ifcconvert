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

#include "BrepBuilder.h"
#include "BrepReaderVisitor.h"

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


int main(int argc, char **argv)
{
    std::cout << "Reading Brep geometry of Ifc2x3..." << std::endl;

    if( argc < 3 ) {
        std::cout << "Invalid command line arguments. Use " << argv[0] << " <input-ifc-file-name> <output-file-name>" << std::endl;
        return 1;
    }

    // ** open, load, close the file
    std::ifstream ifcFile;
    ifcFile.open(argv[1]);

    ifc2x3::SPFReader reader;
    ConsoleCallBack cb;
    reader.setCallBack(&cb);

    if ( ifcFile.is_open() ) {
        std::cout << "reading file ..." << std::endl;
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
        std::cout << "OK!!" << std::endl;
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

    std::cout << "Reading the model...\n";

    // ** Getting the model
    ifc2x3::ExpressDataSet * expressDataSet = dynamic_cast<ifc2x3::ExpressDataSet*>(reader.getExpressDataSet());

    if( expressDataSet == NULL ) {
        std::cout << "There is no ExpressDataSet. Exiting..." << std::endl;
        return (2);
    }

    std::cout << "Reading building elements...\n";

    // ** Reading building elements
    std::ofstream outputFile;
    outputFile.open( argv[2] );    
    if( outputFile.fail() ) {
        std::cout << "Can't write into the output file. Exiting..." << std::endl; 
    }

    std::cout << "Extracting materials...\n";    

    Step::RefLinkedList< ifc2x3::IfcRelAssociatesMaterial >::iterator ramIt = expressDataSet->getAllIfcRelAssociatesMaterial().begin();
    for( ; ramIt != expressDataSet->getAllIfcRelAssociatesMaterial().end(); ++ramIt ) {

        std::vector<std::wstring> materialsNames;        

        std::cout << " IfcRelAssociatesMaterial" << std::endl;
        ifc2x3::IfcMaterialSelect* rm = ramIt->getRelatingMaterial();
        if( rm != NULL ) {
            std::cout <<  " Relaing material type: " << rm->currentTypeName() << std::endl;
            if( rm->currentType() == ifc2x3::IfcMaterialSelect::IFCMATERIALLAYERSETUSAGE ) {
                ifc2x3::IfcMaterialLayerSetUsage *mlsu = rm->getIfcMaterialLayerSetUsage();
                if( mlsu != NULL ) {
                    std::cout <<  "  Material Layer Usage " << std::endl;
                    ifc2x3::IfcMaterialLayerSet *mls = mlsu->getForLayerSet(); // Getting a MATERIAL LAYER SET
                    if( mls != NULL ) {
                        std::cout <<  "   Material Layer Set " << std::endl;
                        ifc2x3::List_IfcMaterialLayer_1_n layers = mls->getMaterialLayers();
                        ifc2x3::List_IfcMaterialLayer_1_n::iterator layersIt = layers.begin();
                        for( ; layersIt != layers.end() ; ++layersIt ) {
                            ifc2x3::IfcMaterial* material = (*layersIt)->getMaterial();
                            if( material != NULL ) {
                                std::cout << "    Material name: " << material->getName() << std::endl;
                                materialsNames.push_back( material->getName() );
                            }
                        }
                    }
                }
            }
        }

        ifc2x3::Set_IfcRoot_1_n relObjs = ramIt->getRelatedObjects();
        ifc2x3::Set_IfcRoot_1_n::iterator relObjsIt = relObjs.begin();
        for( ; relObjsIt != relObjs.end() ; ++relObjsIt ) {   
            std::cout << " Related object: type=" << (*relObjsIt)->type() << ", id=" << (*relObjsIt)->getGlobalId() << ", name= " << (*relObjsIt)->getName() << "\n";
        }
        //it->acceptVisitor(&visitor);
    }

    std::cout << "Running visitors...\n";
    BRepBuilder brepBuilder( &outputFile );
    BrepReaderVisitor visitor( &brepBuilder );

    Step::RefLinkedList< ifc2x3::IfcProject >::iterator projIt = expressDataSet->getAllIfcProject().begin();
    for( ; projIt != expressDataSet->getAllIfcProject().end(); ++projIt ) {
        projIt->acceptVisitor(&visitor);
    }

    brepBuilder.closeTags();

    outputFile.close();

    return 0;
}
