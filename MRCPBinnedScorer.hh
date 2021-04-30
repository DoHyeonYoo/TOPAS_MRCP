
#ifndef MRCPBinnedScorer_HH
#define MRCPBinnedScorer_HH

#include "TsVBinnedScorer.hh"
#include "G4EmCalculator.hh"
#include <iostream>

using namespace std;

class MRCPBinnedScorer : public TsVBinnedScorer
{
public:
        MRCPBinnedScorer(TsParameterManager* pM, TsMaterialManager* mM, TsGeometryManager* gM, TsScoringManager* scM, TsExtensionManager* eM,
                                           G4String scorerName, G4String quantity, G4String outFileName, G4bool isSubScorer);
        ~MRCPBinnedScorer();

        G4bool ProcessHits(G4Step* aStep, G4TouchableHistory*);

	void UserHookForEndOfEvent();

	void UserHookForEndOfRun();

private:
//      G4TouchableHandle touchable;
G4String outputN;
	G4int StepNum, evtNum;
    ofstream DoseOut;
	ifstream MatIn;
	G4double OrganDose_Evt[200];
	G4double OrganDose2[200];
	G4double OrganDoseOut[200];

	G4double OrganVariance[200];
	G4double OrganStd[200];

	G4int fScoredHistories;

	G4String MateName[190];
	G4double OrganMassMap[190];
    G4EmCalculator fEmCalculator;
    G4double DepositEnergy;

	G4double delta;
	G4double mean[200];
	G4double mom2[200];
	G4double Uncert[200];

	G4double fEdep[200];
	G4double fEdep2[200];
	G4double Erms[200];
	G4double Erms2[200];
	G4double fUncertainty[200];

    G4String MatName;
	G4int TotEvtN;
	G4String CurrentVolume;
	G4bool DoubleCounting;

	map<G4String, G4double>		OrganDoseMap;
	G4int fNumSteps;
};

#endif
