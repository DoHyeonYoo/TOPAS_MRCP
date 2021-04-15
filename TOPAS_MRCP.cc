// Component for TOPAS_MRCP
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
// TOPAS_MRCP was developed by Dohyeon Yoo. 

#include "TOPAS_MRCP.hh"
#include "TsParameterManager.hh"
#include "G4Box.hh"

using namespace std;
ofstream Matout;

TOPAS_MRCP::TOPAS_MRCP(TsParameterManager* pM, TsExtensionManager* eM, TsMaterialManager* mM, TsGeometryManager* gM,
			 TsVGeometryComponent* parentComponent, G4VPhysicalVolume* parentVolume, G4String& name) :

TsVGeometryComponent(pM, eM, mM, gM, parentComponent, parentVolume, name),
worldPhysical(0), tetLogic(0)
{
	// Initialisation of the variables for phantom information
	Matout.open("MatList.txt",ios::out);
	phantomDataPath = fPm->GetStringParameter(GetFullParmName("Path"));
	phantomName = fPm->GetStringParameter(GetFullParmName("Sex"));
	
	if(phantomName=="M") phantomName = "MRCP_AM";
	else if(phantomName=="F") phantomName = "MRCP_AF";
	else{
	G4cout<< "change the sex initial (M or F)"<<G4endl;
	G4cout<< "MRCP_AM will be implemeted in this simulation" <<G4endl;
	phantomName = "MRCP_AM";
	}
	G4cout << "================================================================================"<<G4endl;
	G4cout << "\t" << phantomName << " was implemented in this CODE!!   "<< G4endl;
	G4cout << "================================================================================"<<G4endl;

	G4String eleFile      =  phantomName + ".ele";
	G4String nodeFile     =  phantomName + ".node";
	G4String materialFile =  phantomName + ".material";

	// Read phantom data files (*. ele, *.node)
	DataRead(eleFile, nodeFile);
	
	// Read material file (*.material)
	MaterialRead(materialFile);

	PrintMaterialInfomation();
	phantomSize     =  GetPhantomSize();
	phantomBoxMin   =  GetPhantomBoxMin();
	phantomBoxMax   =  GetPhantomBoxMax();
	nOfTetrahedrons =  GetNumTetrahedron();
}

TOPAS_MRCP::~TOPAS_MRCP()
{
	
}

G4VPhysicalVolume* TOPAS_MRCP::Construct()
{
	BeginConstruction();
    
    fTETs.clear();

    //
    // Define the phantom container (10-cm margins form the. ouding of phantom)
    //
	G4double totalHLX =  phantomSize.x()/2 + 10.*cm;
	G4double totalHLY =  phantomSize.y()/2 + 10.*cm;
	G4double totalHLZ =  phantomSize.z()/2 + 10.*cm;

    G4Material* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");

    G4String envelopMaterialName = fParentComponent->GetResolvedMaterialName();
   
	G4Box* envelopeSolid = new G4Box(fName, totalHLX, totalHLY, totalHLZ);
	envelopeLog = CreateLogicalVolume(envelopeSolid);

	G4VPhysicalVolume* envelopePhys = CreatePhysicalVolume(envelopeLog);

	//
	// Define the tetrahedral mesh phantom as a parameterised geometry
	//

    G4VSolid* tetraSolid = new G4Tet("TetSolid",
                                     G4ThreeVector(),
                                     G4ThreeVector(1.*cm,0,0),
                                     G4ThreeVector(0,1.*cm,0),
                                     G4ThreeVector(0,0,1.*cm));
    
    tetLogic = CreateLogicalVolume(tetraSolid);
    
    // Solid and logical volume to be used for parameterised geometry
	// 
    new G4PVParameterised("TET", tetLogic, envelopePhys, kUndefined, GetNumTetrahedron(), new MRCPParameterisation(this));
	PrintPhantomInformation();
    InstantiateChildren(envelopePhys);
    return envelopePhys;
}
void TOPAS_MRCP::ConstructPhantom()
{
	
}

void TOPAS_MRCP::PrintPhantomInformation()
{
	//
	// Print brief information on the imported phantom
	//
	G4cout<< G4endl;
	G4cout.precision(3);
	G4cout<<"================================================================================================================"<<G4endl;
	G4cout<<"   Phantom name               "<<GetPhantomName() << " TET phantom"<<G4endl;
	G4cout<<"   Phantom size               "<<phantomSize.x()<<" * "<<phantomSize.y()<<" * "<<phantomSize.z()<<" mm3"<<G4endl;
	G4cout<<"   Phantom box position (min) "<<phantomBoxMin.x()<<" mm, "<<phantomBoxMin.y()<<" mm, "<<phantomBoxMin.z()<<" mm"<<G4endl;
	G4cout<<"   Phantom box position (max) "<<phantomBoxMax.x()<<" mm, "<<phantomBoxMax.y()<<" mm, "<<phantomBoxMax.z()<<" mm"<<G4endl;
	G4cout<<"   Number of tetrahedrons     "<<nOfTetrahedrons<<G4endl<<G4endl;
}


void TOPAS_MRCP::DataRead(G4String eleFile, G4String nodeFile)
{
	G4String tempStr;
	G4int tempInt;

	//
	// Read *.node file
	//
	std::ifstream ifpNode;

	ifpNode.open((phantomDataPath + "/" + nodeFile).c_str());
	if(!ifpNode.is_open()) {
	
	// exception for the case when there is no *.node file
	G4Exception("TOPAS_MRCP::DataRead","",FatalErrorInArgument,
	G4String("      There is no " + nodeFile ).c_str());
	}
	G4cout << "  Opening TETGEN node (vertex points: x y z) file '" << nodeFile << "'" <<G4endl;

	G4int numVertex;
	G4double xPos, yPos, zPos;
	G4double xMin(DBL_MAX), yMin(DBL_MAX), zMin(DBL_MAX);
	G4double xMax(DBL_MIN), yMax(DBL_MIN), zMax(DBL_MIN);

	ifpNode >> numVertex >> tempInt >> tempInt >> tempInt;

	for(G4int i=0; i<numVertex; i++)
	{
		ifpNode >> tempInt >> xPos >> yPos >> zPos;

		// set the unit
		xPos*=mm;
		yPos*=mm;
		zPos*=mm;

		// save the node data as the form of std::vector<G4ThreeVector>
		vertexVector.push_back(G4ThreeVector(xPos, yPos, zPos));

		// to get the information of the bounding box of phantom
		if (xPos < xMin) xMin = xPos;
		if (xPos > xMax) xMax = xPos;
		if (yPos < yMin) yMin = yPos;
		if (yPos > yMax) yMax = yPos;
		if (zPos < zMin) zMin = zPos;
		if (zPos > zMax) zMax = zPos;
	}

	// set the variables for the bounding box and phantom size
	boundingBox_Min = G4ThreeVector(xMin,yMin,zMin);
	boundingBox_Max = G4ThreeVector(xMax,yMax,zMax);
	phantomSize = G4ThreeVector(xMax-xMin,yMax-yMin,zMax-zMin);

	ifpNode.close();

	// Read *.ele file
	//
	std::ifstream ifpEle;

	ifpEle.open((phantomDataPath + "/" + eleFile).c_str());
	if(!ifpEle.is_open()) {
		// exception for the case when there is no *.ele file
		G4Exception("TOPAS_MRCP::DataRead","",FatalErrorInArgument,
				G4String("      There is no " + eleFile ).c_str());
	}
	G4cout << "  Opening TETGEN elements (tetrahedron with node No.) file '" << eleFile << "'" <<G4endl;

	G4int numEle;
	ifpEle >> numEle  >> tempInt >> tempInt;

	G4int* ele =new G4int[4];
	for(G4int i=0; i<numEle; i++)
	{
		ifpEle >> tempInt;
	
		for(G4int j=0;j<4;j++){
			ifpEle >> tempInt;
			ele[j]=tempInt;
		}
		eleVector.push_back(ele);
		ifpEle >> tempInt;
		materialVector.push_back(tempInt);

		// save the element (tetrahedron) data as the form of std::vector<G4Tet*>
		tetVector.push_back(new G4Tet("Tet_Solid",
							   		  vertexVector[ele[0]],
									  vertexVector[ele[1]],
									  vertexVector[ele[2]],
									  vertexVector[ele[3]]));

		// calculate the total volume and the number of tetrahedrons for each organ
		std::map<G4int, G4double>::iterator FindIter = volumeMap.find(materialVector[i]);

		if(FindIter!=volumeMap.end()){
			FindIter->second += tetVector[i]->GetCubicVolume();
			numTetMap[materialVector[i]]++;
		}
		else {
			volumeMap[materialVector[i]] = tetVector[i]->GetCubicVolume();
			numTetMap[materialVector[i]] = 1;
		}
	}
	ifpEle.close();
}

void TOPAS_MRCP::MaterialRead(G4String materialFile)
{
	// Read material file (*.material)
	//
	std::ifstream ifpMat;

	ifpMat.open((phantomDataPath + "/" + materialFile).c_str());
	if(!ifpMat.is_open()) {
		// exception for the case when there is no *.material file
		G4Exception("TOPAS_MRCP::DataRead","",FatalErrorInArgument,
				G4String("      There is no " + materialFile ).c_str());
	}

	G4cout << "  Opening material file '" << materialFile << "'" <<G4endl;

	char read_data[50];
	char* token;
	G4double zaid;
	G4double fraction;
	G4String MaterialName;
	G4double density;

	while(!ifpMat.eof())
	{
		ifpMat >> read_data;                   //ex) 'C' RBM
		ifpMat >> MaterialName;                //ex)  C 'RBM'

		Matout<<MaterialName<<endl;

		ifpMat >> read_data;
		density = std::atof(read_data);        //ex) 1.30
		ifpMat >> read_data;                   //ex) g/cm3
		ifpMat >> read_data;
		token = std::strtok(read_data,"m");
		G4int matID = std::atoi(token);        //ex) m'10'
		materialIndex.push_back(matID);
		organNameMap[matID]= MaterialName;
		densityMap[matID] = density*g/cm3;

		Matout<<density*(volumeMap[matID]/cm3)<<endl;

		for(G4int i=0 ;  ; i++)
		{
			ifpMat >> read_data;
			if(std::strcmp(read_data, "C")==0 || ifpMat.eof()) break;

			zaid = (G4int)(std::atoi(read_data)/1000);
			ifpMat >> read_data;
			fraction = -1.0 * std::atof(read_data);
			materialIndexMap[matID].push_back(std::make_pair(G4int(zaid), fraction));
		}
	}
	ifpMat.close();

	// Construct materials for each organ
	//
	G4NistManager* nistManager = G4NistManager::Instance();

	for(G4int i=0;i<(G4int)materialIndex.size();i++){
		G4int idx = materialIndex[i];
		G4Material* mat = new G4Material(organNameMap[idx], densityMap[idx], G4int(materialIndexMap[idx].size()), kStateSolid, NTP_Temperature, STP_Pressure);
		for(G4int j=0;j<G4int(materialIndexMap[idx].size());j++){
			mat->AddElement(nistManager->FindOrBuildElement(materialIndexMap[idx][j].first), materialIndexMap[idx][j].second);
		}
		materialMap[idx]=mat;
		massMap[idx]=densityMap[idx]*volumeMap[idx];
	}
}

void TOPAS_MRCP::PrintMaterialInfomation()
{
	// Print the overall information for each organ
	//
	G4cout << G4endl
		   << std::setw(9)  << "Organ ID"
		   << std::setw(11) << "# of Tet"
		   << std::setw(11) << "vol [cm3]"
		   << std::setw(11) << "d [g/cm3]"
		   << std::setw(11) << "mass [g]"
		   << "\t" << "organ/tissue"<< G4endl ;
	G4cout << "--------------------------------------------------------------------------------"<<G4endl;

	std::map<G4int, G4Material*>::iterator matIter;
	G4cout<<std::setiosflags(std::ios::fixed);
	G4cout.precision(3);
	for(matIter=materialMap.begin(); matIter!=materialMap.end();matIter++)
	{
		G4int idx = matIter->first;

		G4cout << std::setw(9)  << idx                         // organ ID
			   << std::setw(11) << numTetMap[idx]              // # of tetrahedrons
			   << std::setw(11) << volumeMap[idx]/cm3          // organ volume
			   << std::setw(11) << materialMap[idx]
			                       ->GetDensity()/(g/cm3)      // organ density
			   << std::setw(11) << massMap[idx]/g              // organ mass
			   << "\t"<<materialMap[idx]->GetName() << G4endl; // organ name
	}
}

MRCPParameterisation::MRCPParameterisation(TOPAS_MRCP* data)
: G4VPVParameterisation()
{
TETData=data;
}

MRCPParameterisation::~MRCPParameterisation()
{

}

G4VSolid* MRCPParameterisation::ComputeSolid(
    		       const G4int copyNo, G4VPhysicalVolume* )
{

return TETData->GetTetrahedron(copyNo);
	
}

void MRCPParameterisation::ComputeTransformation(
                   const G4int,G4VPhysicalVolume*) const
{

}

G4Material* MRCPParameterisation::ComputeMaterial(const G4int copyNo,
                                                 G4VPhysicalVolume* phy,
                                                 const G4VTouchable* )
{
	// return the material data for each material index
	return TETData->GetMaterial(TETData->GetMaterialIndex(copyNo));
}
