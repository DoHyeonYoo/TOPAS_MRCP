//
// ********************************************************************
// *                                                                  *
// *                                                                  *
// * This file was obtained from Topas MC Inc under the license       *
// * agreement set forth at http://www.topasmc.org/registration       *
// * Any use of this file constitutes full acceptance of              *
// * this TOPAS MC license agreement.                                 *
// *                                                                  *
// ********************************************************************
//

#ifndef TOPAS_MRCP_hh
#define TOPAS_MRCP_hh
#include "TsVGeometryComponent.hh"
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include "G4PVParameterised.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UIExecutive.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4ThreeVector.hh"
#include "G4String.hh"
#include "G4Tet.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Colour.hh"
#include "G4VPVParameterisation.hh"

class TOPAS_MRCP : public TsVGeometryComponent
{    

public:

	TOPAS_MRCP(TsParameterManager* pM, TsExtensionManager* eM, TsMaterialManager* mM, TsGeometryManager* gM,
				  TsVGeometryComponent* parentComponent, G4VPhysicalVolume* parentVolume, G4String& name);
	~TOPAS_MRCP();
	
	G4VPhysicalVolume* Construct();

	G4String      GetPhantomName()           { return phantomName; };
	G4Material*   GetMaterial(G4int idx)     { return materialMap[idx];}
	G4int         GetNumTetrahedron()        { return tetVector.size();}
	G4int         GetMaterialIndex(G4int idx){ return materialVector[idx]; }
	G4Tet*        GetTetrahedron(G4int idx)  { return tetVector[idx]; }
	G4double      GetVolume(G4int idx)       { return volumeMap[idx]; }
	std::map<G4int, G4double> GetMassMap()   { return massMap; }
	std::map<G4int, G4Colour> GetColourMap() { return colourMap; }
	G4ThreeVector GetPhantomSize()           { return phantomSize; }
	G4ThreeVector GetPhantomBoxMin()         { return boundingBox_Min; }
	G4ThreeVector GetPhantomBoxMax()         { return boundingBox_Max; }

private:

	void ConstructPhantom();
	void PrintPhantomInformation();

	G4VPhysicalVolume* worldPhysical;
             G4LogicalVolume* envelopeLog;

	G4ThreeVector      phantomSize;
	G4ThreeVector      phantomBoxMin, phantomBoxMax;
	G4int              nOfTetrahedrons;

	G4LogicalVolume*   tetLogic;

	// private methods
	void DataRead(G4String, G4String);
	void MaterialRead(G4String);
	void ColourRead();
	void PrintMaterialInfomation();

	G4String phantomDataPath;
	G4String phantomName;

	G4ThreeVector boundingBox_Min;
	G4ThreeVector boundingBox_Max;
	
	std::vector<G4ThreeVector> vertexVector;
	std::vector<G4Tet*>        tetVector;
	std::vector<G4int*>        eleVector;
	std::vector<G4int>         materialVector;
	std::map<G4int, G4int>     numTetMap;
	std::map<G4int, G4double>  volumeMap;
	std::map<G4int, G4double>  massMap;
	std::map<G4int, G4Colour>  colourMap;

	std::map<G4int, std::vector<std::pair<G4int, G4double>>> materialIndexMap;
	std::vector<G4int>                                       materialIndex;
	std::map<G4int, G4Material*>                             materialMap;
	std::map<G4int, G4double>                                densityMap;
	std::map<G4int, G4String>                                organNameMap;
    std::vector<G4VPhysicalVolume*> fTETs;
};

class MRCPParameterisation : public G4VPVParameterisation
{
 public:
    MRCPParameterisation(TOPAS_MRCP*);
    virtual ~MRCPParameterisation();
    virtual G4VSolid* ComputeSolid(
    		       const G4int copyNo, G4VPhysicalVolume* );
    virtual void ComputeTransformation(
                   const G4int,G4VPhysicalVolume*) const;
    virtual G4Material* ComputeMaterial(const G4int copyNo,
                                        G4VPhysicalVolume* phy,
                                        const G4VTouchable*);
 private:
        TOPAS_MRCP* TETData;
};
#endif
