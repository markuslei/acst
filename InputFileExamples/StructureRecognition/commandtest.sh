inputfile=SimpleOpAmp1
inputfile=$inputfile.hspice
outputfile=$inputfile.xml
{file path to acst}/acst/build/bin/acst.sh --circuit-netlist $inputfile --device-types-file deviceTypes.xcat --hspice-mapping-file HSpiceMapping.xcat --hspice-supplynet-file supplyNets.xcat --analysis structrec --output-file $outputfile --xml-structrec-library-file {file path to acst}/acst/StructRec/xml/AnalogLibrary.xml --log-level-console DEBUG
