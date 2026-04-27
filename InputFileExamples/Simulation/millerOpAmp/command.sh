#!/bin/sh

{file path to acst}/acst/build/bin/acst.sh --log-level-console DEBUG  --circuit-netlist SimpleTwoStageOpAmp_SimpleOpAmp8_7.hspice --output-file millerOpAmp.xml --analysis simulation --xml-structrec-library-file {file path to acst}/acst/StructRec/xml/AnalogLibrary.xml --hspice-mapping-file HSpiceMapping.xcat --hspice-supplynet-file supplyNets.xcat --device-types-file deviceTypes.xcat --xml-technologie-file TechnologyFile.xml --xml-circuit-information-file CircuitParameterAndSpecifications.xml --transistor-model EKV1 --scaling 1mum --runtime 3

