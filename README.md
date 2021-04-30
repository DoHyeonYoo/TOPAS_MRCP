TOPAS-MRCP
This is TOPAS-MRCP extension repository, a Monte Carlo simulation framework for evlautting the organ dose. 

The TOPAS_MRCP extension doce was developed based on the Geant4 MC simulation code of 
international commission on radiological protection (ICRP) publication 145 "Adult Mesh-type Reference Computatioanl Phantoms"
"https://icrp.org/publication.asp?id=ICRP%20Publication%20145"

GENERAL INFORMATION
TOPAS installed with recommended OS system, C++ and cm ake versions, see topas
https://topas.readthedocs.io/en/latest/getting-started/install.htm

1. I recommend having a global directory for extensions named topas_extension and move in TOPAS-MRCP there
Linux: mkdir ~/topas_extensions
Mac: mkdir /Applications/topas_extensions

2. Unzip TOPAS-MRCP directory in topas_extensions and navigate to the topas directory

3. Unzion the Geant4Headers.zip

4. BUild the extensions
Linux: cmake ./-DTOPAS_EXTENSIONS_DIR=~/topas_extensions/TOPAS-MRCP make -j 4
Mac: cmake ./-DTOPAS_EXTENSIONS=/Applications/topas_extensions/TOPAS-MRCP make -j 4

5. Run the macro file MRCP.txt
topas MRCP.txt
