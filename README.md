======================================================================

TOPAS-MRCP

This is TOPAS-MRCP extension repository, a Monte Carlo simulation framework for evaluting the organ dose from internal and external irradiation using mesh-type reference computational human phantom. 

Thbe TOPAS_MRCP extension code was developed based on the Geant4 MRCP code from the international commission on radiological protection (ICRP) publication 145 "Adult Mesh-type Reference Computational Human Phantom"
The detiled information about the MRCP phantom as followed
"https://icrp.org/publication.asp?id=ICRP%20Publication%20145"

========================================================================

GENERAL INFORMATION

TOPAS installed with recommended OS system, C++ and cm ake versions, see topashttps://topas.readthedocs.io/en/latest/getting-started/install.htm
1.	I recommend having a global directory for extensions named topas_extension and move in TOPAS-MRCP there
    Linux: mkdir ~/topas_extensions 
    Mac: mkdir /Applications/topas_extensions
    
2.  Unzip TOPAS-MRCP directory in topas_extensions and navigate to the topas directory

3.	Unzip the Geant4Headers.zip

4.	Build the extensions 
  	Linux: cmake ./-DTOPAS_EXTENSIONS_DIR=~/topas_extensions/TOPAS-MRCP make -j 4 
	  Mac: cmake ./-DTOPAS_EXTENSIONS=/Applications/topas_extensions/TOPAS-MRCP make -j 4

5.	Run the macro file MRCP.txt

=========================================================================

TOPAS-MRCP contains two main code 

1. TOPAS_MRCP.cc

   Read the phantom data 
   
   A total of 3 files are required in the phantom folder (*.ele, *node, and *.material).
   
   You must change the file name to MRCP_AM.ele, MRCP_AM.node, and MRCP_AM.material. 
   
   Finally, you must check the phantom file path in the MRCP.txt

2. MRCPBinnedScorer.cc

   Calculate the organ dose as deposited energy for each organ.

********
Current version of TOPAS-MRCP dose not supprot Multi-threading
Please use only one CPU per simulation.

