#!/bin/sh
inputfile=SimpleThreeStageOpAmp_SimpleOpAmp47_1_1
outputfile=$inputfile.xml
inputfile=$inputfile.hspice
{file path to acst}/acst/build/bin/acst.sh --log-level-console DEBUG  --circuit-netlist $inputfile --output-file $outputfile --analysis partitioning --xml-structrec-library-file {file path to acst}/acst/StructRec/xml/AnalogLibrary.xml --hspice-mapping-file HSpiceMapping.xcat --hspice-supplynet-file supplyNets.xcat --device-types-file deviceTypes.xcat

