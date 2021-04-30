====================================================================================================
TOPAS-MRCP

This is TOPAS-MRCP extension repository, a Monte Carlo simulation framework for evaluting the organ dose from internal and external irradiation using mesh-type reference computational human phantom. 

Thbe TOPAS_MRCP extension code was developed based on the Geant4 MRCP code from the international commission on radiological protection (ICRP) publication 145 "Adult Mesh-type Reference Computational Human Phantom"
The detiled information about the MRCP phantom as followed
"https://icrp.org/publication.asp?id=ICRP%20Publication%20145"

========================================================================================================================================================
GENERAL INFORMATION

TOPAS installed with recommended OS system, C++ and cm ake versions, see topashttps://topas.readthedocs.io/en/latest/getting-started/install.htm
1.	I recommend having a global directory for extensions named topas_extension and move in TOPAS-MRCP there
    Linux: mkdir ~/topas_extensions 
    Mac: mkdir /Applications/topas_extensions
    
2.  Unzip TOPAS-MRCP directory in topas_extensions and navigate to the topas directory

3.	Unzion the Geant4Headers.zip

4.	BUild the extensions 
  	Linux: cmake ./-DTOPAS_EXTENSIONS_DIR=~/topas_extensions/TOPAS-MRCP make -j 4 
	  Mac: cmake ./-DTOPAS_EXTENSIONS=/Applications/topas_extensions/TOPAS-MRCP make -j 4

5.	Run the macro file MRCP.txt topas MRCP.txt

========================================================================================================================================================
TOPAS-MRCP contains two main code 
1. TOPAS_MRCP.cc
2. 


