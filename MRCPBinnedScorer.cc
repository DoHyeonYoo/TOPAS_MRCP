// Scorer for MRCPBinnedScorer

#include "MRCPBinnedScorer.hh"
#include "G4TransportationManager.hh"

MRCPBinnedScorer::MRCPBinnedScorer(TsParameterManager* pM, TsMaterialManager* mM, TsGeometryManager* gM, TsScoringManager* scM, TsExtensionManager* eM,
                                           G4String scorerName, G4String quantity, G4String outFileName, G4bool isSubScorer)
: TsVBinnedScorer(pM, mM, gM, scM, eM, scorerName, quantity, outFileName, isSubScorer),
fEmCalculator()
{
	outputN = fPm->GetStringParameter(GetFullParmName("Output"));
TotEvtN = 0;
	fScoredHistories=0;
    SetUnit("MeV");
	evtNum = 0;
	G4String MName;
	G4String OrganMass;
	G4int pp=0;
	fNumSteps = 0;
	G4String S_Command;
	G4String S_Equal;
	ifstream MRCPIn;
	MatIn.open("Phantoms/MatList.txt",ios::in);
	MRCPIn.open("Phantoms/MRCP2.txt", ios::in);

	while(true)
	{
		MRCPIn>>S_Command;

		if(std::strncmp("i:So/MRCPsource/NumberOfHistoriesInRun", S_Command, 38) == 0)
		{
			MRCPIn>>S_Equal>>TotEvtN;
			break;
		}
		if(MRCPIn.eof()){break;}

	}

	G4String data_read;

	while(true)
	{

		if(MatIn.eof()){ cout<<"While Break"<<endl;  break;}
		MatIn>>MName;
		MateName[pp]= MName;

		MatIn>>data_read;
		OrganMassMap[pp] = atof(data_read);
		OrganDoseMap[MateName[pp]] = 0;
     	pp ++;
	}

	for(G4int x=0;x<200;x++)
	{
		OrganDoseOut[x] = 0;
		OrganDose2[x] = 0;
		OrganVariance[x] = 0;

		fEdep[x] = 0;
		fEdep2[x] = 0;
		Erms[x] = 0;
		Erms2[x] = 0;
		fUncertainty[x] = 0;

	}

	StepNum = 0;
}


MRCPBinnedScorer::~MRCPBinnedScorer()
{

}

G4bool MRCPBinnedScorer::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{

	fNumSteps ++;

	StepNum ++;

	G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->SetPushVerbosity(0);

    DepositEnergy = (aStep->GetTotalEnergyDeposit()/MeV);

	MatName = aStep->GetPreStepPoint()->GetMaterial()->GetName();

	if(MatName != CurrentVolume) DoubleCounting=false;

	OrganDoseMap[MatName] += DepositEnergy;

	for(G4int x=0;x<187;x++)
	{
		if(MateName[x] == MatName)
		{
			OrganDose2[x] += DepositEnergy;
			OrganDose_Evt[x] += DepositEnergy;
		}
	}
    
    for(G4int x=0;x<187;x++)
    {
        OrganDoseOut[x] = OrganDose2[x];
    }
	CurrentVolume = aStep->GetPreStepPoint()->GetMaterial()->GetName();
}

void MRCPBinnedScorer::UserHookForEndOfEvent()
{

	fScoredHistories++;
//Error propagation
for(G4int x=0;x<187;x++)
{
	fEdep[x] += OrganDose_Evt[x];
	fEdep2[x] += OrganDose_Evt[x]*OrganDose_Evt[x];
}

if(evtNum == TotEvtN-1)
{
	for(G4int x=0;x<187;x++)
	{
		Erms[x] = fEdep2[x]-fEdep[x]*fEdep[x]/(TotEvtN);

		if(Erms[x]>0) Erms2[x] = sqrt(Erms[x]); else Erms2[x] = 0;
	}

	for(G4int x=0;x<187;x++)
	{
		if(fEdep[x] != 0) fUncertainty[x] = 100*Erms2[x]/fEdep[x];
		else fUncertainty[x] = 0;
	}

}


	for(G4int x=0;x<200;x++)
	{
		OrganDose_Evt[x] = 0;
	}


//	if(evtNum%3 == 0){cout<<evtNum<<"th event finished"<<endl;}

	if(evtNum == TotEvtN-1)
	{

	DoseOut.open(outputN, ios::out);

	DoseOut<<"Organ"<<"\t"<<"Absorbed Energy (MeV)"<<"\t"<<"Uncertainty (%)"<<endl;

        for(G4int x=0;x<187;x++)
        {
        OrganDoseOut[x] = OrganDose2[x];
	DoseOut<<MateName[x]<<"\t"<<OrganDoseOut[x]<<"\t"<<fUncertainty[x]<<" %"<<endl;
        }

	DoseOut.close();

	}

	evtNum++;

}

void MRCPBinnedScorer::UserHookForEndOfRun()
{
	cout<<"End Of Run"<<endl;
	cout<<"TOTAL OF "<<evtNum<<" EVENTS WERE DONE"<<endl;
	cout<<"Size of Organ Dose Map   "<<OrganDoseMap.size()<<endl;

    DoseOut.close();
}

