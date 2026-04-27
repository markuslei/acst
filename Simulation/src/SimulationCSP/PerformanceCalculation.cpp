/**************************************************************************************************
 *  Main authors:
 *     Inga Abel <inga.abel@tum.de>, 
 *	   Maximilian Neuner <maximilian.neuner@tum.de>, 
 *     Michael Eick <michael.eick@tum.de>
 *
 * 
 *  Copyright (C) 2021
 *  Chair of Electronic Design Automation
 *  Univ.-Prof. Dr.-Ing. Ulf Schlichtmann
 *  TU Muenchen
 *  Arcisstrasse 21
 *  D-80333 Muenchen
 *  Germany
 *
 *  This file is part of acst, a analog circuit analysis, sizing and synthesis enviroment:
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *************************************************************************************************/



 #include "Simulation/incl/SimulationCSP/ComponentToIntVarMap.h"
 #include "Simulation/incl/SimulationCSP/ComponentToIntVarInputMap.h"
 #include "Partitioning/incl/Results/Result.h"
 #include "Simulation/incl/SimulationCSP/Graph/Graph.h"
 #include "Simulation/incl/SimulationCSP/Graph/Path.h"
 #include "Simulation/incl/SimulationCSP/Graph/Node.h"
 #include "Simulation/incl/SimulationCSP/Graph/Edge.h"
 #include "Simulation/incl/CircuitInformation/CircuitInformation.h"
 #include "Simulation/incl/CircuitInformation/DesignAttributes.h"
 #include "Simulation/incl/CircuitInformation/Pin.h"
 #include "Simulation/incl/CircuitInformation/LoadCapacity.h"
 #include "Simulation/incl/SimulationCSP/NetToIntVarMap.h"
 #include "Simulation/incl/SimulationCSP/SimulationSpace.h"
 #include "StructRec/incl/StructureCircuit/Structure/Pair.h"
 #include "StructRec/incl/StructureCircuit/Structure/Pin/StructurePin.h"
 
 #include "Core/incl/Circuit/Net/NetId/NetId.h"
 #include "Core/incl/Circuit/Net/NetId/NetName.h"
 #include "Core/incl/Circuit/Device/DeviceType/MosfetDeviceType.h"
 #include "Core/incl/Common/BacktraceAssert.h"
 
 #include <math.h>
 #include <cmath>
 #include <numeric>
 #include <vector>
 #include <gecode/minimodel.hh>
 #include <gecode/float.hh>
 #include <algorithm>
 #include <utility>
 #include <unordered_set>
 
 #include "Simulation/incl/SimulationCSP/PerformanceCalculation.h"
 #include "Simulation/incl/SimulationCSP/Constraints/SimulatedPolesAndZeros.h"
 #include "StructRec/incl/StructureCircuit/StructureCircuit.h"
 
 
 namespace Simulation 
 {
 
	 PerformanceCalculation::PerformanceCalculation() :
			 partitioningResult_(NULL),
			 structRecResult_(NULL),
			 polesAndZeros_(NULL),
			 circuitInformation_(NULL),
			 graph_(NULL),
			 transistorToWidthInputMap_(NULL),
			 transistorToMultiplierInputMap_(NULL),
			 transistorToLengthInputMap_(NULL),
			 transistorToCurrentMap_(NULL),
			 twoPortToValueInputMap_(NULL),
			 netToVoltageMap_(NULL),
			 space_(NULL)
	 {
	 }
 
	 void PerformanceCalculation::setPartitioningResult(
		 const Partitioning::Result& result)
	 {
		 partitioningResult_ = & result;
	 }
 
	 void PerformanceCalculation::setStructureRecognitionResult(const StructRec::StructureCircuits& circuits)
	 {
		 structRecResult_ = &circuits;
	 }
 
	 void PerformanceCalculation::setGraph(
		 const Graph& graph)
	 {
		 graph_ = & graph;
	 }
 
	 void PerformanceCalculation::setCircuitInformation(
		 const CircuitInformation& information)
	 {
		 circuitInformation_ = & information;
	 }
 
 
 
	 void PerformanceCalculation::setTransistorToWidthInputMap(ComponentToIntVarInputMap& widthMap )
	 {
		 transistorToWidthInputMap_ = & widthMap;
	 }
	 void PerformanceCalculation::setTransistorToLengthInputMap(ComponentToIntVarInputMap& lenghtMap)
	 {
		 transistorToLengthInputMap_ = & lenghtMap;
	 }	
	 void PerformanceCalculation::setTransistorToMultiplierInputMap(ComponentToIntVarInputMap& multiplierMap)
	 {
		 transistorToMultiplierInputMap_ = & multiplierMap;
	 }
 
	 void PerformanceCalculation::setTransistorToCurrentMap(
		 ComponentToIntVarMap& currentMap)
	 {
		 transistorToCurrentMap_ = & currentMap;
	 }
 
	
	 void PerformanceCalculation::setNetToVoltageMap(
		 NetToIntVarMap& voltageMap)
	 {
		 netToVoltageMap_ = & voltageMap;
	 }
 
	 
 
	 void PerformanceCalculation::setTwoPortToValueInputMap(
			 ComponentToIntVarInputMap& valueMap)
	 {
		 twoPortToValueInputMap_ = & valueMap;
	 }
 
 
	 const ComponentToIntVarInputMap& PerformanceCalculation::getTwoPortToValueInputMap() const
	 {
		 assert(twoPortToValueInputMap_ != NULL);
		 return * twoPortToValueInputMap_;
	 }
 
	 const Partitioning::Result& PerformanceCalculation::getPartitioningResult() const
	 {
		 assert(partitioningResult_ != NULL);
		 return * partitioningResult_;
	 }
 
	 const Graph& PerformanceCalculation::getGraph() const
	 {
		 assert(graph_ != NULL);
		 return * graph_;
	 }
 
	 const CircuitInformation& PerformanceCalculation::getCircuitInformation() const
	 {
		 assert(circuitInformation_ != NULL);
		 return * circuitInformation_;
	 }
 
	 const ComponentToIntVarInputMap& PerformanceCalculation::getTransistorToWidthInputMap() const{
		 assert(transistorToWidthInputMap_ != NULL);
		 return *transistorToWidthInputMap_;
	 }

	 const ComponentToIntVarInputMap& PerformanceCalculation::getTransistorToMultiplierInputMap() const{
		 assert(transistorToMultiplierInputMap_ != NULL);
		 return *transistorToMultiplierInputMap_;
	 }

	 const ComponentToIntVarInputMap& PerformanceCalculation::getTransistorToLengthInputMap() const{
		 assert(transistorToLengthInputMap_ != NULL);
		 return *transistorToLengthInputMap_;
	 }
 
 
	 const ComponentToIntVarMap& PerformanceCalculation::getTransistorToCurrentMap() const
	 {
		 assert(transistorToCurrentMap_ != NULL);
		 return *transistorToCurrentMap_;
	 }
 
	 
	 const NetToIntVarMap& PerformanceCalculation::getNetToVoltageMap() const
	 {
		 assert(netToVoltageMap_ != NULL);
		 return *netToVoltageMap_;
	 }
 
 
	 
 
	 
	 
	 void PerformanceCalculation::setSpace(SimulationSpace& space)
	 {
		 space_ = & space;
	 }
 
	 // Performance Variables
 
	 SimulationSpace& PerformanceCalculation::getSpace() const
	 {
		 assert(space_ != NULL);
		 return *space_;
	 }
 
	 
	 float PerformanceCalculation::getGain()
	 {
		 return GAIN_;
	 }
 
	 void PerformanceCalculation::setGain(float gain)
	 {
		 GAIN_ = gain;
	 } 
 
 
	 float PerformanceCalculation::getDominantPole()
	 {
		 return DOMINANTPOLE_;
	 }
 
	 void PerformanceCalculation::setDominantPole(float dominantPole)
	 {
		 DOMINANTPOLE_ = dominantPole;
	 }
 
 
	 float PerformanceCalculation::getPositiveZero()
	 {
		 return POSITIVEZERO_;
	 }
 
	 void PerformanceCalculation::setPositiveZero(float positivezero)
	 {
		 POSITIVEZERO_ = positivezero;
	 }
 
 
 
	 std::vector<float> PerformanceCalculation::getImportantNonDominantPoles() 
	 {
 
	 return ImportantNonDominantPoles_;
 
	 }
 
	 void PerformanceCalculation::setImportantNonDominantPoles( std::vector<float> ImportantNonDominantPoles )
	 {
 
	 ImportantNonDominantPoles_ = ImportantNonDominantPoles;
 
	 }
 
	 std::vector<float> PerformanceCalculation::getImportantZeros()
	 {
	 return ImportantZeros_;
	 }
 
	 void PerformanceCalculation::setImportantZeros(std::vector<float> ImportantZeros)
	 {
	 ImportantZeros_ = ImportantZeros;
	 }
 
 
 
	 float PerformanceCalculation::getPowerConsumption()
	 {
	 
		return POWER_;
 
	 }
 
	 void PerformanceCalculation::setPowerConsumption(float power)
	 {
	 
		POWER_ = power;
 
	 }	
 
	 float PerformanceCalculation::getTransitFrequency()
	 {
		 return TRANSITFREQUENCY_;
	 }
 
	 void PerformanceCalculation::setTransitFrequency(float TransitFrequency)
	 {
		 TRANSITFREQUENCY_ = TransitFrequency ;
	 }	
	 
	 float PerformanceCalculation::getTransitFrequencyWithErrorFactor()
	 {
		 return TRANSIT_FREQUENCY_WITH_ERROR_FACTOR_;
	 }
 
	 void PerformanceCalculation::setTransitFrequencyWithErrorFactor(float TransitFrequencyWithErrorFactor)
	 {
		 TRANSIT_FREQUENCY_WITH_ERROR_FACTOR_ = TransitFrequencyWithErrorFactor ;
	 }
 
	 float PerformanceCalculation::getArea()
	 {
		 return AREA_;
	 }
 
	 void PerformanceCalculation::setArea(float area)
	 {
		 AREA_ = area;
	 }
 
 
	 float PerformanceCalculation::getPhaseMargin()
	 {
		 return PHASE_MARGIN_;
	 }
 
 
	 void PerformanceCalculation::setPhaseMargin(float phaseMargin)
	 {
		 PHASE_MARGIN_ = phaseMargin;
	 }	
	 
	 float PerformanceCalculation::getSlewRate()
	 {
		 return SLEW_RATE_;
	 }
 
	 void PerformanceCalculation::setSlewRate(float slewRate)
	 {
		 SLEW_RATE_ = slewRate;
	 }	
 
	 float PerformanceCalculation::getCMRR()
	 {
		 return CMRR_;
	 }
 
 
	 void PerformanceCalculation::setCMRR(float CMRR)
	 {
		 CMRR_ = CMRR;
	 }	
				 
	 float PerformanceCalculation::getNegPSRR()
	 {
		 return NEG_PSRR_;
	 }
 
	 void PerformanceCalculation::setNegPSRR(float negPSRR)
	 {
		 NEG_PSRR_ = negPSRR ;
	 }	
		 
	 float PerformanceCalculation::getPosPSRR()
	 {
		 return POS_PSRR_;
	 }
 
	 void PerformanceCalculation::setPosPSRR(float posPSRR)
	 {
		 POS_PSRR_ = posPSRR ;
	 }	
 
	 float PerformanceCalculation::getMaxOutputVoltage()
	 {
		 return VOUT_MAX_;
	 }
 
	 void PerformanceCalculation::setMaxOutputVoltage(float voutMax)
	 {
		 VOUT_MAX_ = voutMax;
	 }	
 
	 float PerformanceCalculation::getMinOutputVoltage()
	 {
		 return VOUT_MIN_;
	 }
 
	 void PerformanceCalculation::setMinOutputVoltage(float voutMin)
	 {
		 VOUT_MIN_ = voutMin;
	 }			
 
	 float PerformanceCalculation::getMaxCommonModeInputVoltage()
	 {
		 return VCM_MAX_;
	 }
 
	 void PerformanceCalculation::setMaxCommonModeInputVoltage(float vcmMax)
	 {
		 VCM_MAX_ = vcmMax;
	 }
 
	 float PerformanceCalculation::getMinCommonModeInputVoltage()
	 {
		 return VCM_MIN_;
	 }
 
	 void PerformanceCalculation::setMinCommonModeInputVoltage(float vcmMin)
	 {
		 VCM_MIN_ = vcmMin;
	 }
 
 
	 // Intermediate Performance variables
 
	 std::vector<std::pair<Core::NetId, float>> PerformanceCalculation::getNetcapacitances()
	 {
		 return netCapacitances_;
	 }
 
	 void PerformanceCalculation::setNetCapacitances(const std::vector<std::pair<Core::NetId, float>>& netCapacitances)
	 {
		 netCapacitances_ = netCapacitances;
	 }
 
	 float PerformanceCalculation::getOutputResistanceFirstStage()
	 {
		 return Output_Resistance_FirstStage_;
	 }
 
	 void PerformanceCalculation::setOutputResistanceFirstStage(float outputResistanceFirstStage)
	 {
		 Output_Resistance_FirstStage_ = outputResistanceFirstStage;
	 }
 
	 float PerformanceCalculation::getOutputResistancePrimarySecondStage()
	 {
		 return Output_Resistance_PrimarySecondStage_;
	 }
 
	 float PerformanceCalculation::getOutputResistanceSecondarySecondStage()
	 {
		 return Output_Resistance_SecondarySecondStage_;
	 }
 
	 void PerformanceCalculation::setOutputResistancePrimarySecondStage(float outputResistancePrimarySecondStage)
	 {
		 Output_Resistance_PrimarySecondStage_ = outputResistancePrimarySecondStage;
	 }
 
	 void PerformanceCalculation::setOutputResistanceSecondarySecondStage(float outputResistanceSecondarySecondStage)
	 {
		 Output_Resistance_SecondarySecondStage_ = outputResistanceSecondarySecondStage;
	 }
 
	 float PerformanceCalculation::getOutputResistanceThirdStage()
	 {
		 return Output_Resistance_ThirdStage_;
	 }
 
	 void PerformanceCalculation::setOutputResistanceThirdStage(float outputResistanceThirdStage)
	 {
		 Output_Resistance_ThirdStage_ = outputResistanceThirdStage;
	 }
 
	 float PerformanceCalculation::getGainFirstStage()
	 {
		 return gainFirstStage_;
	 }
 
	 void PerformanceCalculation::setGainFirstStage(float gainFirstStage)
	 {
		 gainFirstStage_ = gainFirstStage;
	 }
 
	 float PerformanceCalculation::getGainSecondStage()
	 {
		 return gainSecondStage_;
	 }
 
	 void PerformanceCalculation::setGainSecondStage(float gainSecondStage)
	 {
		 gainSecondStage_ = gainSecondStage;
	 }
 
	 float PerformanceCalculation::getGainThirdStage()
	 {
		 return gainThirdStage_;
	 }
 
	 void PerformanceCalculation::setGainThirdStage(float gainThirdStage)
	 {
		 gainThirdStage_ = gainThirdStage;
	 }
 
	 float PerformanceCalculation::getFirstStageTransconductance()
	 {
		 return firstStageTransconductance_;
	 }
 
	 void PerformanceCalculation::setFirstStageTransconductance(float firstStageTransconductance)
	 {
		 firstStageTransconductance_ = firstStageTransconductance;
	 }
 
	 std::vector<std::pair<std::string, float>> PerformanceCalculation::getFirstStageComponentTransconductancesArray()
	 {
		 return FirstStagecomponentTransconductancesArray_;
	 }
 
	 void PerformanceCalculation::setFirstStageComponentTransconductancesArray(const std::vector<std::pair<std::string, float>>& firstStageComponentTransconductancesArray)
	 {
		 FirstStagecomponentTransconductancesArray_ = firstStageComponentTransconductancesArray;
	 }
 
	 float PerformanceCalculation::getPrimarySecondStageTransconductance()
	 {
		 return PrimarySecondStageTransconductance_;
	 }
 
	 void PerformanceCalculation::setPrimarySecondStageTransconductance(float primarySecondStageTransconductance)
	 {
		 PrimarySecondStageTransconductance_ = primarySecondStageTransconductance;
	 }
 
	 std::vector<std::pair<std::string, float>> PerformanceCalculation::getPrimarySecondStageComponentTransconductancesArray()
	 {
		 return PrimarySecondStagecomponentTransconductancesArray_;
	 }
 
	 void PerformanceCalculation::setPrimarySecondStageComponentTransconductancesArray(const std::vector<std::pair<std::string, float>>& primarySecondStageComponentTransconductancesArray)
	 {
		 PrimarySecondStagecomponentTransconductancesArray_ = primarySecondStageComponentTransconductancesArray;
	 }
 
	 float PerformanceCalculation::getSecondarySecondStageTransconductance()
	 {
		 return SecondarySecondStageTransconductance_;
	 }
 
	 void PerformanceCalculation::setSecondarySecondStageTransconductance(float secondarySecondStageTransconductance)
	 {
		 SecondarySecondStageTransconductance_ = secondarySecondStageTransconductance;
	 }
 
	 std::vector<std::pair<std::string, float>> PerformanceCalculation::getSecondarySecondStageComponentTransconductancesArray()
	 {
		 return SecondarySecondStagecomponentTransconductancesArray_;
	 }
 
	 void PerformanceCalculation::setSecondarySecondStageComponentTransconductancesArray(const std::vector<std::pair<std::string, float>>& secondarySecondStageComponentTransconductancesArray)
	 {
		 SecondarySecondStagecomponentTransconductancesArray_ = secondarySecondStageComponentTransconductancesArray;
	 }
 
	 float PerformanceCalculation::getThirdStageTransconductance()
	 {
		 return ThirdStageTransconductance_;
	 }
 
	 void PerformanceCalculation::setThirdStageTransconductance(float thirdStageTransconductance)
	 {
		 ThirdStageTransconductance_ = thirdStageTransconductance;
	 }
 
	 std::vector<std::pair<std::string, float>> PerformanceCalculation::getThirdStageComponentTransconductancesArray()
	 {
		 return ThirdStagecomponentTransconductancesArray_;
	 }
 
	 void PerformanceCalculation::setThirdStageComponentTransconductancesArray(const std::vector<std::pair<std::string, float>>& thirdStageComponentTransconductancesArray)
	 {
		 ThirdStagecomponentTransconductancesArray_ = thirdStageComponentTransconductancesArray;
	 }
 
	 float PerformanceCalculation::getOutputConductanceLoadPartFirstStage()
	 {
		 return Output_Conductance_loadpart_first_stage_;
	 }
 
	 void PerformanceCalculation::setOutputConductanceLoadPartFirstStage(float outputConductanceLoadPartFirstStage)
	 {
		 Output_Conductance_loadpart_first_stage_ = outputConductanceLoadPartFirstStage;
	 }
 
	 float PerformanceCalculation::getOutputConductanceBiasPartFirstStage()
	 {
		 return Output_Conductance_biasPart_first_stage_;
	 }
 
	 void PerformanceCalculation::setOutputConductanceBiasPartFirstStage(float outputConductanceBiasPartFirstStage)
	 {
		 Output_Conductance_biasPart_first_stage_ = outputConductanceBiasPartFirstStage;
	 }
 
	 std::vector<std::pair<std::string, float>> PerformanceCalculation::getComponentOutputConductancesArrayFirstStage()
	 {
		 return ComponentOutputConductancesArrayFirstStage_;
	 }
 
	 void PerformanceCalculation::setComponentOutputConductancesArrayFirstStage(const std::vector<std::pair<std::string, float>>& componentOutputConductancesArrayFirstStage)
	 {
		 ComponentOutputConductancesArrayFirstStage_ = componentOutputConductancesArrayFirstStage;
	 }
 
	 float PerformanceCalculation::getOutputConductanceLoadPartPrimarySecondStage()
	 {
		 return Output_Conductance_loadpart_Primary_Second_Stage_;
	 }
 
	 void PerformanceCalculation::setOutputConductanceLoadPartPrimarySecondStage(float outputConductanceLoadPartPrimarySecondStage)
	 {
		 Output_Conductance_loadpart_Primary_Second_Stage_ = outputConductanceLoadPartPrimarySecondStage;
	 }
 
	 float PerformanceCalculation::getOutputConductanceBiasPartPrimarySecondStage()
	 {
		 return Output_Conductance_biasPart_Primary_Second_Stage_;
	 }
 
	 void PerformanceCalculation::setOutputConductanceBiasPartPrimarySecondStage(float outputConductanceBiasPartPrimarySecondStage)
	 {
		 Output_Conductance_biasPart_Primary_Second_Stage_ = outputConductanceBiasPartPrimarySecondStage;
	 }
 
	 std::vector<std::pair<std::string, float>> PerformanceCalculation::getComponentOutputConductancesArrayPrimarySecondStage()
	 {
		 return ComponentOutputConductancesArrayPrimarySecondStage_;
	 }
 
	 void PerformanceCalculation::setComponentOutputConductancesArrayPrimarySecondStage(const std::vector<std::pair<std::string, float>>& componentOutputConductancesArrayPrimarySecondStage)
	 {
		 ComponentOutputConductancesArrayPrimarySecondStage_ = componentOutputConductancesArrayPrimarySecondStage;
	 }
 
	 float PerformanceCalculation::getOutputConductanceLoadPartSecondarySecondStage()
	 {
		 return Output_Conductance_loadpart_Secondary_Second_Stage_;
	 }
 
	 void PerformanceCalculation::setOutputConductanceLoadPartSecondarySecondStage(float outputConductanceLoadPartSecondarySecondStage)
	 {
		 Output_Conductance_loadpart_Secondary_Second_Stage_ = outputConductanceLoadPartSecondarySecondStage;
	 }
 
	 float PerformanceCalculation::getOutputConductanceBiasPartSecondarySecondStage()
	 {
		 return Output_Conductance_biasPart_Secondary_Second_Stage_;
	 }
 
	 void PerformanceCalculation::setOutputConductanceBiasPartSecondarySecondStage(float outputConductanceBiasPartSecondarySecondStage)
	 {
		 Output_Conductance_biasPart_Secondary_Second_Stage_ = outputConductanceBiasPartSecondarySecondStage;
	 }
 
	 std::vector<std::pair<std::string, float>> PerformanceCalculation::getComponentOutputConductancesArraySecondarySecondStage()
	 {
		 return ComponentOutputConductancesArraySecondarySecondStage_;
	 }
 
	 void PerformanceCalculation::setComponentOutputConductancesArraySecondarySecondStage(const std::vector<std::pair<std::string, float>>& componentOutputConductancesArraySecondarySecondStage)
	 {
		 ComponentOutputConductancesArraySecondarySecondStage_ = componentOutputConductancesArraySecondarySecondStage;
	 }
 
	 float PerformanceCalculation::getOutputConductanceLoadPartThirdStage()
	 {
		 return Output_Conductance_loadpart_Third_Stage_;
	 }
 
	 void PerformanceCalculation::setOutputConductanceLoadPartThirdStage(float outputConductanceLoadPartThirdStage)
	 {
		 Output_Conductance_loadpart_Third_Stage_ = outputConductanceLoadPartThirdStage;
	 }
 
	 float PerformanceCalculation::getOutputConductanceBiasPartThirdStage()
	 {
		 return Output_Conductance_biasPart_Third_Stage_;
	 }
 
	 void PerformanceCalculation::setOutputConductanceBiasPartThirdStage(float outputConductanceBiasPartThirdStage)
	 {
		 Output_Conductance_biasPart_Third_Stage_ = outputConductanceBiasPartThirdStage;
	 }
 
	 std::vector<std::pair<std::string, float>> PerformanceCalculation::getComponentOutputConductancesArrayThirdStage()
	 {
		 return ComponentOutputConductancesArrayThirdStage_;
	 }
 
	 void PerformanceCalculation::setComponentOutputConductancesArrayThirdStage(const std::vector<std::pair<std::string, float>>& componentOutputConductancesArrayThirdStage)
	 {
		 ComponentOutputConductancesArrayThirdStage_ = componentOutputConductancesArrayThirdStage;
	 }
 
 
 
	 float PerformanceCalculation::calculateInputArea()
	 {
 
		 std::vector<Partitioning::Transistor*> transistors = getPartitioningResult().getAllTransistors();
		 float calculatedArea=0;
		 for(std::vector<Partitioning::Transistor*>::const_iterator it_tran = transistors.begin(); it_tran != transistors.end(); it_tran++)
		 {
			 Partitioning::Transistor* tran = *it_tran;
 
			 calculatedArea+=getTransistorToWidthInputMap().find(*tran)* getTransistorToMultiplierInputMap().find(*tran)* getTransistorToLengthInputMap().find(*tran);
		 }
		 setArea(calculatedArea);
		 return calculatedArea;
	 }
 
		
 
	  float PerformanceCalculation::computeSign(const Edge & edge, const Path & path) const
	  {
		  float sign = -1;
		  const std::vector<const Edge*> edges = path.getEdges();
		  if(edge.getEdgeKey() == (*edges.begin())->getEdgeKey())
		  {
			  if(edge.getStartNodeId() == path.getStartNode().getNetId())
			  {
				  sign = 1;
			  }
		  }
		  else
		  {
			  for(std::vector<const Edge*>::const_iterator it = edges.begin(); it != edges.end(); it++)
			  {
				  const Edge* posEdge = * std::next(it);
				  if(edge.getEdgeKey() == posEdge->getEdgeKey())
				  {
					  const Edge*  previousEdge = *it;
					  if(edge.getStartNodeId() == previousEdge->getEndNodeId() || edge.getStartNodeId() == previousEdge->getStartNodeId())
					  {
						  sign = 1;
					  }
				  }
			  }
		  }
		  return sign;
	  }
 
	 void PerformanceCalculation::setTransistorModel(std::string model)
	 {
		 transistorModel_ = model;
	 }
 
	 std::string PerformanceCalculation::getTransistorModel() const
	 {
		 assert(transistorModel_ =="EKV" || transistorModel_ == "SHM", "Transistor model not supported");
		 return transistorModel_;
	 }

	 void PerformanceCalculation::setEKVVersion(int version)
	 {
		 ekvVersion_ = version;
	 }

	 int PerformanceCalculation::getEKVVersion() const
	 {
		 assert(ekvVersion_ == 1 || ekvVersion_ == 2 || ekvVersion_ == 3, "EKV version not supported");
		 return ekvVersion_;
	 }
 
	 const StructRec::StructureCircuits&  PerformanceCalculation::getStructureRecognitionResult() const
	 {
		 assert(structRecResult_ != NULL);
		 return *structRecResult_;
	 }
 
	 const SimulatedPolesAndZeros& PerformanceCalculation::getPolesAndZeros() const
	 {
		 assert(polesAndZeros_ != NULL);
		 return *polesAndZeros_;
	 }
 
 
		 bool PerformanceCalculation::isOutputNetFirstStage(Core::NetId outputNet) const
		 {
			 bool isOutputNet = false;
 
			 if(getPartitioningResult().hasSecondStage())
			 {
				 for(auto & secondStage : getPartitioningResult().getSecondStages())
				 {
					 Partitioning::Component & compSecondStage = **getPartitioningResult().getBelongingComponents(*secondStage).begin();
					 if(compSecondStage.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")).getIdentifier() == outputNet)
					 {
						 isOutputNet = true;
					 }
				 }
			 }
			 else
			 {
				 if(getCircuitInformation().getCircuitParameter().isFullyDifferential())
				 {
					 if(getCircuitInformation().getCircuitParameter().getOutputPinMinus().getNetId() == outputNet
							 || getCircuitInformation().getCircuitParameter().getOutputPinPlus().getNetId() == outputNet)
					 {
						 isOutputNet = true;
					 }
				 }
				 else
				 {
					 if(getCircuitInformation().getCircuitParameter().getOutputPin().getNetId() == outputNet)
					 {
						 isOutputNet = true;
					 }
				 }
			 }
 
			 return isOutputNet;
		 }
 
		 bool PerformanceCalculation::compensationCapacityConnectedBetween(Partitioning::TransconductancePart &stage1, Partitioning::TransconductancePart &stage2) const
		 {
			 bool isConnected = false;
			 const StructRec::StructureNet * minusNet = nullptr;
			 const StructRec::StructureNet * plusNet = nullptr;
 
			 for(auto & it : getPartitioningResult().getCompensationCapacitors())
			 {
			 Partitioning::CapacitancePart & compCap = *it;
			 Partitioning::Component & compCompCap = **getPartitioningResult().getBelongingComponents(compCap).begin();
			 const StructRec::StructureNet & minusNetCap = compCompCap.getArray().findNet(StructRec::StructurePinType("CapacitorArray","Minus"));
			 const StructRec::StructureNet & plusNetCap = compCompCap.getArray().findNet(StructRec::StructurePinType("CapacitorArray","Plus"));
 
			 if(getPartitioningResult().hasCompensationResistor())
			 {
				 Partitioning::ResistorPart & compRes = getPartitioningResult().getCompensationResistor();
				 Partitioning::Component & compCompRes = **getPartitioningResult().getBelongingComponents(compRes).begin();
				 if(compCompRes.getArray().getStructureName().toStr() == "ResistorArray")
				 {
					 const StructRec::StructureNet & minusNetRes = compCompRes.getArray().findNet(StructRec::StructurePinType("ResistorArray","Minus"));
					 const StructRec::StructureNet & plusNetRes = compCompRes.getArray().findNet(StructRec::StructurePinType("ResistorArray","Plus"));
					 if(minusNetRes.getIdentifier() == minusNetCap.getIdentifier())
					 {
						 plusNet = & plusNetRes;
						 minusNet = & plusNetCap;
					 }
					 else if(minusNetRes.getIdentifier() == plusNetCap.getIdentifier())
					 {
						 plusNet = & plusNetRes;
						 minusNet = & minusNetCap;
					 }
					 else if(plusNetRes.getIdentifier() == plusNetCap.getIdentifier())
					 {
						 plusNet = & minusNetRes;
						 minusNet = & minusNetCap;
					 }
					 else
					 {
						 plusNet = & minusNetRes;
						 minusNet = & plusNetCap;
					 }
				 }
				 else
				 {
					 const StructRec::StructureNet & minusNetRes = compCompRes.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray","Drain"));
					 const StructRec::StructureNet & plusNetRes = compCompRes.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray","Source"));
					 if(minusNetRes.getIdentifier() == minusNetCap.getIdentifier())
					 {
						 plusNet = & plusNetRes;
						 minusNet = & plusNetCap;
					 }
					 else
					 {
						 plusNet = & plusNetRes;
						 minusNet = & minusNetCap;
					 }
				 }
			 }
			 else
			 {
				 minusNet = & minusNetCap;
				 plusNet = & plusNetCap;
			 }
 
			 if((stage1.isFirstStage()) && stage1.hasHelperStructure())
			 {
				 Partitioning::Component & diffPair1Comp = **getPartitioningResult().getBelongingComponents(stage1).begin();
				 Partitioning::Component & diffPair2Comp = **std::next(getPartitioningResult().getBelongingComponents(stage1).begin());
				 const StructRec::StructureNet & outputNetDiffPair1 =  diffPair1Comp.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));
				 const StructRec::StructureNet & outputNetDiffPair2 =  diffPair2Comp.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));
 
 
				 isConnected = (isOutputNetStage(*minusNet, stage1) && isOutputNetStage(*plusNet, stage2))
						 || (isOutputNetStage(*plusNet, stage1) && isOutputNetStage(*minusNet, stage2))
						 || (outputNetDiffPair1.getIdentifier() == minusNet->getIdentifier() && (isOutputNetStage(*plusNet, stage2)))
						 || (outputNetDiffPair1.getIdentifier() == plusNet->getIdentifier() && (isOutputNetStage(*minusNet, stage2)))
						 || (outputNetDiffPair2.getIdentifier() == minusNet->getIdentifier() && (isOutputNetStage(*plusNet, stage2)))
						 || (outputNetDiffPair2.getIdentifier() == plusNet->getIdentifier() && (isOutputNetStage(*minusNet, stage2)));
 
			 }
			 else if((stage2.isFirstStage()) && stage2.hasHelperStructure())
			 {
				 Partitioning::Component & diffPair1Comp = **getPartitioningResult().getBelongingComponents(stage2).begin();
				 Partitioning::Component & diffPair2Comp = **std::next(getPartitioningResult().getBelongingComponents(stage2).begin());
				 const StructRec::StructureNet & outputNetDiffPair1 =  diffPair1Comp.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));
				 const StructRec::StructureNet & outputNetDiffPair2 =  diffPair2Comp.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));
 
 
				 isConnected = (isOutputNetStage(*minusNet, stage1) && isOutputNetStage(*plusNet,stage2))
							 || (isOutputNetStage(*plusNet, stage1) && isOutputNetStage(*minusNet,stage2))
							 ||	(isOutputNetStage(*minusNet, stage1) && outputNetDiffPair1.getIdentifier() == plusNet->getIdentifier())
							 || (isOutputNetStage(*plusNet, stage1) && outputNetDiffPair1.getIdentifier() == minusNet->getIdentifier())
							 ||	(isOutputNetStage(*minusNet, stage1) && outputNetDiffPair2.getIdentifier() == plusNet->getIdentifier())
							 || (isOutputNetStage(*plusNet, stage1) && outputNetDiffPair2.getIdentifier() == minusNet->getIdentifier());
 
			 }
			 else
			 {
				 isConnected = ((isOutputNetStage(*minusNet, stage1) && isOutputNetStage(*plusNet, stage2))
						 || (isOutputNetStage(*plusNet, stage1) && isOutputNetStage(*minusNet, stage2)));
			 }
			 if(isConnected)
				 return  isConnected;
			 }
			 return isConnected;
 
		 }
 
		 std::vector<const StructRec::StructureNet*>  PerformanceCalculation::findOutputNetsFirstStage() const
		 {
			 std::vector<const StructRec::StructureNet *> outputNets;
			 if(getPartitioningResult().hasSecondStage())
			 {
				 Partitioning::TransconductancePart & secondStage = getPartitioningResult().getPrimarySecondStage();
				 std::vector<Partitioning::Component *> compsSecondStage = getPartitioningResult().getBelongingComponents(secondStage);
				 for(auto & comp : compsSecondStage)
				 {
					 if(comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Source")).isSupply())
					 {
						 outputNets.push_back(&comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")));
					 }
 
				 }
			 }
			 else
			 {
				 if(getCircuitInformation().getCircuitParameter().isFullyDifferential())
				 {
					 outputNets.push_back(&getStructureRecognitionResult().findStructureCircuit(0).findStructureNet(getCircuitInformation().getCircuitParameter().getOutputPinMinus().getNetId()));
					 outputNets.push_back(&getStructureRecognitionResult().findStructureCircuit(0).findStructureNet(getCircuitInformation().getCircuitParameter().getOutputPinPlus().getNetId()));
				 }
				 else
				 {
					 outputNets.push_back(&getStructureRecognitionResult().findStructureCircuit(0).findStructureNet(getCircuitInformation().getCircuitParameter().getOutputPin().getNetId()));
				 }
			 }
			 assert(!outputNets.empty(), "There should be a first stage with at leas one output net");
 
			 return outputNets;
		 }
 
		 std::vector<const StructRec::StructureNet*>  PerformanceCalculation::findOutputNets(Partitioning::TransconductancePart & stage) const
		 {
			 assert(stage.isFirstStage() || stage.isPrimarySecondStage() || stage.isThirdStage() || stage.isFeedBack(), "Transconductance might only be a helper Structure");
 
			 std::vector<const StructRec::StructureNet *> outputNets;
 
			 if((stage.isPrimarySecondStage() || stage.isThirdStage()) && getPartitioningResult().getBelongingComponents(stage).size() > 1)
			 {
				 std::vector<Partitioning::Component *> compsStage = getPartitioningResult().getBelongingComponents(stage);
				 for(auto & comp : compsStage)
				 {
						 outputNets.push_back(&comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Drain")));
				 }
			 }
			 else if(stage.isFirstStage())
			 {
				 if(getPartitioningResult().hasSecondStage())
				 {
					 Partitioning::TransconductancePart & secondStage = getPartitioningResult().getPrimarySecondStage();
					 std::vector<Partitioning::Component *> compsSecondStage = getPartitioningResult().getBelongingComponents(secondStage);
					 for(auto & comp : compsSecondStage)
					 {
						 if(comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Source")).isSupply())
						 {
							 outputNets.push_back(&comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")));
							 break;
						 }
 
					 }
					 assert(!outputNets.empty(), "There should be at least one transistor in the second stage connected to supply");
				 }
				 else
				 {
 
					 if(getCircuitInformation().getCircuitParameter().isFullyDifferential())
					 {
						 outputNets.push_back(&getStructureRecognitionResult().findStructureCircuit(0).findStructureNet(getCircuitInformation().getCircuitParameter().getOutputPinMinus().getNetId()));
						 outputNets.push_back(&getStructureRecognitionResult().findStructureCircuit(0).findStructureNet(getCircuitInformation().getCircuitParameter().getOutputPinPlus().getNetId()));
					 }
					 else
					 {
						 outputNets.push_back(&getStructureRecognitionResult().findStructureCircuit(0).findStructureNet(getCircuitInformation().getCircuitParameter().getOutputPin().getNetId()));
					 }
				 }
 
			 }
			 else if(stage.isFeedBack())
			 {
				 bool foundNet = false;
				 for(auto &loadPart : stage.getLoadPart())
				 {
					 for(auto & loadComp : getPartitioningResult().getBelongingComponents(*loadPart))
					 {
						 const StructRec::StructureNet & gateNet = loadComp->getArray().findNet(StructRec::StructurePinType(loadComp->getArray().getStructureName(),"Drain"));
						 std::vector<const StructRec::Structure*> gateConnectedStructures = gateNet.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
						 for(auto & gateConnectedStruct : gateConnectedStructures)
						 {
							 if(getPartitioningResult().isLoadPartOfFirstStage(getPartitioningResult().getPart(*gateConnectedStruct)))
							 {
								 outputNets.push_back(&gateNet);
								 foundNet = true;
								 break;
							 }
						 }
						 if(foundNet)
							 break;
					 }
					 if(foundNet)
						 break;
				 }
 
				 assert(foundNet, "There should be a direct net connection between the load of the feedback circuit and the load of the first stage.");
			 }
			 else
			 {
				 Partitioning::Component & compStage = **getPartitioningResult().getBelongingComponents(stage).begin();
				 outputNets.push_back(&compStage.getArray().findNet(StructRec::StructurePinType(compStage.getArray().getStructureName(), "Drain")));
			 }
			 return outputNets;
		 }
 
		 bool PerformanceCalculation::isOutputNetStage(const StructRec::StructureNet & net, Partitioning::TransconductancePart & stage) const
		 {
			 bool isIt = false;
 
			 std::vector<const StructRec::StructureNet* > outputNets = findOutputNets(stage);
			 for(auto & outputNet : outputNets)
			 {
				 if(outputNet->getIdentifier() == net.getIdentifier())
				 {
					 isIt = true;
					 break;
				 }
			 }
			 return isIt;
		 }
 
 
 
	 std::vector<Path> PerformanceCalculation::findShortestPaths(std::vector<Path> paths)
	 {
		 unsigned int shortest = 100000000;
		 std::vector<Path> shortestPaths;
		 for(auto & path : paths)
		 {
			 if(path.getEdges().size() < shortest)
			 {
				 shortest = path.getEdges().size();
				 std::vector<Path> newVector;
				 newVector.push_back(path);
				 shortestPaths = newVector;
			 }
			 else if(path.getEdges().size() == shortest)
			 {
				 shortestPaths.push_back(path);
			 }
		 }
		 return shortestPaths;
	 }
 
	 std::vector<Partitioning::Component * > PerformanceCalculation::computeInputToSupplyRailPathOverBias(Partitioning::TransconductancePart & stage)
	 {
		 assert(stage.isFirstStage() || stage.isFeedBack(), "the Input stage must be either a first stage or a feedback stage");
		 std::vector<Partitioning::Component * > path;
 
		 path.push_back(*getPartitioningResult().getBelongingComponents(stage).begin());
 
		 Partitioning::BiasPart & biasPart = **stage.getBiasPart().begin();
 
		 for(auto & compBias : getPartitioningResult().getBelongingComponents(biasPart))
		 {
			 path.push_back(compBias);
		 }
 
		 return path;
	 }
 
	 std::vector<Partitioning::Component * > PerformanceCalculation::computeInputToSupplyRailPathOverLoad(Partitioning::TransconductancePart & stage)
	 {
		 assert(stage.isFirstStage() || stage.isFeedBack(), "the Input stage must be either a first stage or a feedback stage");
		 std::vector<Partitioning::Component * > path;
 
		 path.push_back(*getPartitioningResult().getBelongingComponents(stage).begin());
 
		 std::vector<Partitioning::LoadPart *> loadParts = stage.getLoadPart();
 
		 if(getPartitioningResult().hasBiasOfFoldedPair())
		 {
			 path.push_back(&getPartitioningResult().getBiasOfFoldedPair());
			 return path;
		 }
		 else if(stage.hasHelperStructure())
		 {
			 path.push_back(*getPartitioningResult().findComponents(stage.getHelperStructure()).begin());
		 }
 
		 for(auto & loadPart : loadParts)
		 {
			 if((**loadPart->getMainStructures().begin()).getTechType() !=
					 (**stage.getMainStructures().begin()).getTechType())
			 {
				 Partitioning::Component * outputComp1 =nullptr;
				 Partitioning::Component * outputComp2 =nullptr;
				 Partitioning::Component * supplyComp1 =nullptr;
				 Partitioning::Component * supplyComp2 =nullptr;
				 for(auto & comp : getPartitioningResult().getBelongingComponents(*loadPart))
				 {
					 if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply()
							 && supplyComp1 == nullptr)
					 {
						 supplyComp1 = comp;
					 }
					 else if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply() )
					 {
						 supplyComp2 = comp;
					 }
					 else if(isOutputNetStage(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Drain")), stage)
							  && outputComp1 == nullptr)
					 {
						 outputComp1 = comp;
					 }
					 else
					 {
						 outputComp2 = comp;
					 }
				 }
				 if(supplyComp1 != nullptr && supplyComp1->getArray().getStructureName().toStr() == "MosfetDiodeArray")
				 {
					 path.push_back(supplyComp1);
				 }
				 else if(supplyComp2 != nullptr && supplyComp2->getArray().getStructureName().toStr() == "MosfetDiodeArray")
				 {
					 path.push_back(supplyComp2);
				 }
				 else if(supplyComp1 != nullptr && supplyComp1->getArray().getStructureName().toStr() == "MosfetNormalArray"
					 &&	( (outputComp1 != nullptr && supplyComp1->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")).getIdentifier()
					 == outputComp1->getArray().findNet(StructRec::StructurePinType(outputComp1->getArray().getStructureName(), "Drain")).getIdentifier())
							 || (outputComp2 != nullptr && supplyComp1->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")).getIdentifier()
					 == outputComp2->getArray().findNet(StructRec::StructurePinType(outputComp2->getArray().getStructureName(), "Drain")).getIdentifier()) ))
				 {
					 path.push_back(supplyComp1);
					 return path;
				 }
				 else if(supplyComp2 != nullptr && supplyComp2->getArray().getStructureName().toStr() == "MosfetNormalArray"
					 &&	( (outputComp1 != nullptr && supplyComp2->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")).getIdentifier()
					 == outputComp1->getArray().findNet(StructRec::StructurePinType(outputComp1->getArray().getStructureName(), "Drain")).getIdentifier())
							 || (outputComp2 != nullptr && supplyComp2->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")).getIdentifier()
					 == outputComp2->getArray().findNet(StructRec::StructurePinType(outputComp2->getArray().getStructureName(), "Drain")).getIdentifier()) ))
				 {
					 path.push_back(supplyComp2);
					 return path;
				 }
				 else
				 {
					 path.push_back(supplyComp1);
				 }
				 if(outputComp1 != nullptr && outputComp1->getArray().getStructureName().toStr() == "MosfetDiodeArray")
				 {
					 path.push_back(outputComp1);
				 }
				 else if(outputComp2 != nullptr && outputComp2->getArray().getStructureName().toStr() == "MosfetDiodeArray")
				 {
					 path.push_back(outputComp2);
 
				 }
				 else if(outputComp1 != nullptr)
				 {
					 path.push_back(outputComp1);
				 }
				 return path;
 
 
			 }
		 }
		 return path;
	 }
 
	 bool PerformanceCalculation::hasCompensationCapacity(const StructRec::StructureNet & net) const
	 {
		 bool hasIt = false;
 
		 if(getPartitioningResult().hasCompensationCapacitance())
		 {
			 for(auto & structure : net.getAllConnectedStructures())
			 {
				 Partitioning::Component & comp =getPartitioningResult().findComponent(structure->getIdentifier());
				 if(comp.getPart().isCapacitancePart() && getPartitioningResult().getCapacitancePart(*structure).isCompensationCapacitance())
				 {
					 hasIt = true;
				 }
			 }
		 }
		 return hasIt;
	 }
 
	 bool PerformanceCalculation::hasComponentInFeedback(const StructRec::StructureNet & net) const
	 {
		 bool hasIt = false;
		 const StructRec::StructureNet & arrayNet = net.findArrayNet();
 
		 for(auto & structure : arrayNet.getAllConnectedStructures())
		 {
			 Partitioning::Component & comp =getPartitioningResult().findComponent(structure->getIdentifier());
			 if(getPartitioningResult().isInFeedback(comp))
			 {
				 hasIt = true;
				 break;
			 }
		 }
		 return hasIt;
	 }
 
	 bool PerformanceCalculation::hasOutputNetWithCapacitor(Partitioning::TransconductancePart & stage) const
	 {
		 const StructRec::StructureNet * outputNet = nullptr;
		 for(auto & net: findOutputNets(stage))
		 {
			 if(!net->findConnectedStructures(StructRec::StructurePinType("CapacitorArray", "Minus")).empty()
				 ||!net->findConnectedStructures(StructRec::StructurePinType("CapacitorArray", "Plus")).empty())
			 {
				 outputNet = net;
				 break;
			 }
		 }
		 return outputNet != nullptr;
	 };
 
	 const StructRec::StructureNet & PerformanceCalculation::findOutputNetWithCapacitor(Partitioning::TransconductancePart & stage)
	 {
		 assert(hasOutputNetWithCapacitor(stage) , "No output net has an capacitor!");
 
		 const StructRec::StructureNet * outputNet = nullptr;
		 for(auto & net: findOutputNets(stage))
		 {
			 if(!net->findConnectedStructures(StructRec::StructurePinType("CapacitorArray", "Minus")).empty()
				 ||!net->findConnectedStructures(StructRec::StructurePinType("CapacitorArray", "Plus")).empty())
			 {
				 outputNet = net;
				 break;
			 }
		 }
		 return *outputNet;
 
 
	 }
 
 
	 float PerformanceCalculation::computeSimulateTransconductance(Partitioning::Component& transistor) 
	 {
		 float mu = getSpace().getScalingFactorMUM();
		 float ids = static_cast<float>(getSpace().createFloatCurrent(getTransistorToCurrentMap().find(transistor)).min());
 
		Edge * drainSourceEdge = NULL;
		Edge * gateSourceEdge = NULL;
		std::vector<Edge*> edges = getGraph().findToComponentBelongingEdges(transistor);
		for(std::vector<Edge*>::const_iterator it = edges.begin(); it != edges.end(); it++)
		{
			Edge* edge = *it;
			if(edge->isDrainSourceEdge())
			{
				drainSourceEdge = edge;
			}
			else if (edge->isGateSourceEdge())
			{
				gateSourceEdge = edge;
			}
		}
		assert(drainSourceEdge != NULL, "No drainSourceEdge found!");
		if(gateSourceEdge == NULL)
		{
			gateSourceEdge = drainSourceEdge;
		}
		float vgs = computeEdgeVoltage(*gateSourceEdge);
		float vds = computeEdgeVoltage(*drainSourceEdge);
 
		 if (getTransistorModel() == "SHM")
		 {
			 const TechnologieSpecificationSHM& techSpec = getCircuitInformation().getTechnologieSpecificationSHM(transistor);
 
			 if (getCircuitInformation().getDesignAttributes().isInWeakInversion(transistor))
			 {
				 float n = techSpec.getSlopeFactor();
				 float Vt = techSpec.getThermalVoltage();
				 float Cox = techSpec.getGateOxideCapacity();
				 float Cj = techSpec.getZeroBiasBulkJunctionCapacitance();
				 float factor = (Cj/Cox +1) * 1/(n*Vt);
 
				 return factor * std::abs(ids);
			 }
			 else
			 {
				 float muCox = techSpec.getMobilityOxideCapacityCoefficient();
				 float width = getTransistorToWidthInputMap().find(transistor) * mu;
				 float multiplier = getTransistorToMultiplierInputMap().find(transistor);
				 float length = getTransistorToLengthInputMap().find(transistor) * mu;
				 float result = std::sqrt(2 * muCox * (width * multiplier / length) * std::abs(ids));
				 return result;
			 }
		 }
		 else
		 {
			 // Change code suited for EKV model
			 const TechnologieSpecificationEKV& techSpec = getCircuitInformation().getTechnologieSpecificationEKV(transistor);

			 float Vt = techSpec.getThermalVoltage();
			 float Cox = techSpec.getGateOxideCapacity();
			 float width = getTransistorToWidthInputMap().find(transistor) * mu;
			 float multiplier = getTransistorToMultiplierInputMap().find(transistor);
			 float length = getTransistorToLengthInputMap().find(transistor) * mu;
			 //logDebug("length: " << length 	<< " width: " << width);

			 float vth;
			 float muCox;
			 float n;
			 float lambda;
			 float theta;
			 float eta;

			if(transistor.getArray().getTechType().isN())
			{
				if (length >= 5.0e-7f) {
					vth = techSpec.getThresholdVoltage_LMIN5000();
					muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN5000();
					n = techSpec.getSlopeFactor_LMIN5000();
					lambda = techSpec.getChannelLengthCoefficient_LMIN5000();
					theta = techSpec.getMobilityReductionCoefficient_LMIN5000();
					eta = techSpec.getDIBLCoefficient_LMIN5000();
				} else if (length >= 2.1e-7f) {
					vth = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
					muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000();
					n = techSpec.getSlopeFactor_LMIN2100_LMAX5000();
					lambda = techSpec.getChannelLengthCoefficient_LMIN2100_LMAX5000();
					theta = techSpec.getMobilityReductionCoefficient_LMIN2100_LMAX5000();
					eta = techSpec.getDIBLCoefficient_LMIN2100_LMAX5000();
				} else if (length >= 1.2e-7f) {
                	vth = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
                	muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100();
                	n = techSpec.getSlopeFactor_LMIN1200_LMAX2100();
                	lambda = techSpec.getChannelLengthCoefficient_LMIN1200_LMAX2100();
                	theta = techSpec.getMobilityReductionCoefficient_LMIN1200_LMAX2100();
                	eta = techSpec.getDIBLCoefficient_LMIN1200_LMAX2100();
            	} else if (length >= 5e-8f) {
                	vth = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
                	muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN500_LMAX1200();
                	n = techSpec.getSlopeFactor_LMIN500_LMAX1200();
                	lambda = techSpec.getChannelLengthCoefficient_LMIN500_LMAX1200();
                	theta = techSpec.getMobilityReductionCoefficient_LMIN500_LMAX1200();
                	eta = techSpec.getDIBLCoefficient_LMIN500_LMAX1200();
           	 	} else {
                	vth = techSpec.getThresholdVoltage_LMAX500();
                	muCox = techSpec.getMobilityOxideCapacityCoefficient_LMAX500();
                	n = techSpec.getSlopeFactor_LMAX500();
                	lambda = techSpec.getChannelLengthCoefficient_LMAX500();
                	theta = techSpec.getMobilityReductionCoefficient_LMAX500();
                	eta = techSpec.getDIBLCoefficient_LMAX500();
            	}


			}
			else
			{
				if (length >= 5.0e-7f) {
					vth = techSpec.getThresholdVoltage_LMIN5000();
					muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN5000();
					n = techSpec.getSlopeFactor_LMIN5000();
					lambda = techSpec.getChannelLengthCoefficient_LMIN5000();
					theta = techSpec.getMobilityReductionCoefficient_LMIN5000();
					eta = techSpec.getDIBLCoefficient_LMIN5000();
				} else if (length >= 2.1e-7f) {
					vth = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
					muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000();
					n = techSpec.getSlopeFactor_LMIN2100_LMAX5000();
					lambda = techSpec.getChannelLengthCoefficient_LMIN2100_LMAX5000();
					theta = techSpec.getMobilityReductionCoefficient_LMIN2100_LMAX5000();
					eta = techSpec.getDIBLCoefficient_LMIN2100_LMAX5000();
				} else if (length >= 1.2e-7f) {
                	vth = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
                	muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100();
                	n = techSpec.getSlopeFactor_LMIN1200_LMAX2100();
                	lambda = techSpec.getChannelLengthCoefficient_LMIN1200_LMAX2100();
               	 	theta = techSpec.getMobilityReductionCoefficient_LMIN1200_LMAX2100();
               		eta = techSpec.getDIBLCoefficient_LMIN1200_LMAX2100();
            	} else if (length >= 5e-8f) {
                	vth = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
                	muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN500_LMAX1200();
                	n = techSpec.getSlopeFactor_LMIN500_LMAX1200();
                	lambda = techSpec.getChannelLengthCoefficient_LMIN500_LMAX1200();
                	theta = techSpec.getMobilityReductionCoefficient_LMIN500_LMAX1200();
                	eta = techSpec.getDIBLCoefficient_LMIN500_LMAX1200();
            	} else {
                	vth = techSpec.getThresholdVoltage_LMAX500();
                	muCox = techSpec.getMobilityOxideCapacityCoefficient_LMAX500();
                	n = techSpec.getSlopeFactor_LMAX500();
                	lambda = techSpec.getChannelLengthCoefficient_LMAX500();
                	theta = techSpec.getMobilityReductionCoefficient_LMAX500();
                	eta = techSpec.getDIBLCoefficient_LMAX500();
            	}
				
			}

			 //logDebug("vth: " << vth << " muCox: " << muCox << " n: " << n << " lambda: " << lambda << " theta: " << theta << " eta: " << eta);


			 if(getEKVVersion() == 1)
			 {
				 float expF = std::exp((abs(vgs) - abs(vth)) * (0.5 / (n * Vt)));
				 float expR = std::exp((abs(vgs) - abs(vth) - n * abs(vds)) * (0.5 / (n * Vt)));
				 float lnF = std::log(1 + expF);
				 float lnR = std::log(1 + expR);
				 float termF = lnF * expF / (1 + expF);
				 float termR = lnR * expR / (1 + expR);

				 float result = 2 * (muCox * (width  * multiplier / length) * Vt * (termF - termR) * (1 + lambda * abs(vds)));
				 return result;
			 }
			 else if(getEKVVersion() == 2)
			 {
				 float vP = (abs(vgs) - abs(vth)) / n;
				 float expF = std::exp((vP) * (0.5 / Vt));
				 float expR = std::exp((vP - abs(vds)) * (0.5 / Vt));
				 float lnF = std::log(1 + expF);
				 float lnR = std::log(1 + expR);
				 float termF = lnF * expF / (1 + expF);
				 float termR = lnR * expR / (1 + expR);
				 float scaleLamda = 1 + lambda * abs(vds);
				 float scaleTheta = 1 + theta * abs(vP);
				 float factor = 2 * (muCox * (width  * multiplier / length) * Vt);

				 float part1 = - factor * Vt * scaleLamda * theta * (pow(lnF, 2) - pow(lnR, 2)) / pow(scaleTheta, 2);
				 float part2 = factor * scaleLamda * (termF - termR) / scaleTheta;
				 float result = part1 + part2;
				 return result;
			 }
			 else
			 {
				 float vth_eff = abs(vth) - eta * abs(vds);
				 float vP = (abs(vgs) - abs(vth_eff)) / n;
				 float expF = std::exp((vP) * (0.5 / Vt));
				 float expR = std::exp((vP - abs(vds)) * (0.5 / Vt));
				 float lnF = std::log(1 + expF);
				 float lnR = std::log(1 + expR);
				 float termF = lnF * expF / (1 + expF);
				 float termR = lnR * expR / (1 + expR);
				 float scaleLamda = 1 + lambda * abs(vds);
				 float scaleTheta = 1 + theta * abs(vP);
				 float factor = 2 * (muCox * (width  * multiplier / length) * Vt);

				 float part1 = - factor * Vt * scaleLamda * theta * (pow(lnF, 2) - pow(lnR, 2)) / pow(scaleTheta, 2);
				 float part2 = factor * scaleLamda * (termF - termR) / scaleTheta;
				 float result = part1 + part2;
				 return result;
			 }
		 }
	 }
 
 
	 float PerformanceCalculation::computeSimulateTransconductance(Partitioning::TransconductancePart & stage)
	 {
		 std::vector<Partitioning::Component*> comps = getPartitioningResult().getBelongingComponents(stage);
		 if(stage.isFirstStage())
		 {
			 if(stage.isComplementary())
			 {
				 assert(comps.size() == 4, "A complementary first stage should have 4 devices!");
				 Partitioning::Component * pmosComp1 = nullptr;
				 Partitioning::Component * pmosComp2 = nullptr;
				 Partitioning::Component * nmosComp1 = nullptr;
				 Partitioning::Component * nmosComp2 = nullptr;
 
				 for(auto & comp : comps)
				 {
					 if(comp->getArray().getTechType().isP() && pmosComp1 == nullptr)
					 {
						 pmosComp1 = comp;
					 }
					 else if(comp->getArray().getTechType().isP())
					 {
						 pmosComp2 = comp;
					 }
					 else if(comp->getArray().getTechType().isN() && nmosComp1 == nullptr)
					 {
						 nmosComp1 = comp;
					 }
					 else
					 {
						 nmosComp2 = comp;
					 }
				 }
 
				 float nmos1Transconductance = computeSimulateTransconductance(*nmosComp1);
				 float nmos2Transconductance = computeSimulateTransconductance(*nmosComp2);
 
				 float pmos1Transconductance = computeSimulateTransconductance(*pmosComp1);
				 float pmos2Transconductance = computeSimulateTransconductance(*pmosComp2);
				 float result = nmos1Transconductance + pmos1Transconductance;
				 return result;
			 }
			 else
			 {
				 float firstTranTransconductance = computeSimulateTransconductance(**comps.begin());
				 float secondTranTransconductance = computeSimulateTransconductance(**std::next(comps.begin()));
				 return firstTranTransconductance;
			 }
 
		 }
		 else
		 {
			 assert(stage.isHigherStage(), "If stage is not complementary or first stage, it needs to be from type higher stage!");
			 Partitioning::Component * supplyComp = nullptr;
 
			 for(auto & comp : getPartitioningResult().getBelongingComponents(stage))
			 {
				 if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply())
				 {
					 supplyComp = comp;
				 }
			 }
 
			 assert(supplyComp != nullptr, "Every higher stage should have a to supply connected component");
			 return computeSimulateTransconductance(*supplyComp);
		 }
 
	 }
 
	 float PerformanceCalculation::computeEdgeVoltage(const Edge& edge)
	 {
 
		 const  Core::NetId startNetId = edge.getStartNodeId();
		  const Core::NetId endNetId = edge.getEndNodeId();
		  float startNetVoltage = static_cast<float>(getSpace().createFloatVoltage(netToVoltageMap_->find(startNetId)).min());
		  float endNetVoltage = static_cast<float>(getSpace().createFloatVoltage(netToVoltageMap_->find(endNetId)).min());
		 logDebug("EdgeVoltage");
		 logDebug(startNetVoltage - endNetVoltage);
		  return startNetVoltage - endNetVoltage;
 
	 }
 
	 float PerformanceCalculation::computeOutputSimulateConductance(Partitioning::Component& transistor)
	 {
		 //logDebug(transistor.toStr());
		 float mu = getSpace().getScalingFactorMUM();
		  float ids = static_cast<float>(getSpace().createFloatCurrent(getTransistorToCurrentMap().find(transistor)).min());
		  if(getTransistorModel() == "SHM")
		  {
			 const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(transistor);
			 if(transistor.getPart().isResistorPart() && getPartitioningResult().getResistorPart(transistor.getArray()).isCompensationResistor())
			 {
				 float muCox = techSpec.getMobilityOxideCapacityCoefficient();
				 float vth = techSpec.getThresholdVoltage();
 
				 float length = getTransistorToLengthInputMap().find(transistor);
				 float width = getTransistorToWidthInputMap().find(transistor);
				 float multiplier = getTransistorToMultiplierInputMap().find(transistor);
 
				 Edge * drainSourceEdge = NULL;
				 Edge * gateSourceEdge = NULL;
				 std::vector<Edge*> edges = getGraph().findToComponentBelongingEdges(transistor);
				 for(std::vector<Edge*>::const_iterator it = edges.begin(); it != edges.end(); it++)
				 {
					 Edge* edge = *it;
					 if(edge->isDrainSourceEdge())
					 {
						 drainSourceEdge = edge;
					 }
					 else if (edge->isGateSourceEdge())
					 {
							 gateSourceEdge = edge;
					 }
				 }
				 assert(drainSourceEdge != NULL, "No drainSourceEdge found!");
				 if(gateSourceEdge == NULL)
				 {
					 gateSourceEdge = drainSourceEdge;
				 }
				 float vgs = computeEdgeVoltage(*gateSourceEdge);
				 float result= muCox * (width * multiplier /length) * abs((vgs -vth));
				 return result;
			 }
			 else
			 {
				  
				float lamda;
				  if(getCircuitInformation().getDesignAttributes().isInWeakInversion(transistor))
				  {
					  lamda = techSpec.getChannelLengthCoefficientWeakInversion();
				  }
				  else
				  {
					  lamda = techSpec.getChannelLengthCoefficientStrongInversion();
				  }
				  if(transistor.getArray().getTechType().isN())
				  {

					  return lamda * ids;
				  }
				  else
				  {

					  return -1* lamda * ids;
				  }
			 }
		  }
		  else
		  { //Change Code suitable for EKV model
			const TechnologieSpecificationEKV & techSpec = getCircuitInformation().getTechnologieSpecificationEKV(transistor);


			float Vt = techSpec.getThermalVoltage();
			float Cox = techSpec.getGateOxideCapacity();
			float length = getTransistorToLengthInputMap().find(transistor) * mu;
			float width = getTransistorToWidthInputMap().find(transistor) * mu;
			float multiplier = getTransistorToMultiplierInputMap().find(transistor);
			logDebug("length: " << length << " width: " << width);

			float vth;
			float muCox;
			float n;
			float lamda;
			float theta;
			float eta;

			if(transistor.getArray().getTechType().isN())
			{
				if (length >= 5.0e-7f) {
					vth = techSpec.getThresholdVoltage_LMIN5000();
					muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN5000();
					n = techSpec.getSlopeFactor_LMIN5000();
					lamda = techSpec.getChannelLengthCoefficient_LMIN5000();
					theta = techSpec.getMobilityReductionCoefficient_LMIN5000();
					eta = techSpec.getDIBLCoefficient_LMIN5000();
				} else if (length >= 2.1e-7f) {
					vth = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
					muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000();
					n = techSpec.getSlopeFactor_LMIN2100_LMAX5000();
					lamda = techSpec.getChannelLengthCoefficient_LMIN2100_LMAX5000();
					theta = techSpec.getMobilityReductionCoefficient_LMIN2100_LMAX5000();
					eta = techSpec.getDIBLCoefficient_LMIN2100_LMAX5000();
				} else if (length >= 1.2e-7f) {
                	vth = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
                	muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100();
                	n = techSpec.getSlopeFactor_LMIN1200_LMAX2100();
                	lamda = techSpec.getChannelLengthCoefficient_LMIN1200_LMAX2100();
                	theta = techSpec.getMobilityReductionCoefficient_LMIN1200_LMAX2100();
                	eta = techSpec.getDIBLCoefficient_LMIN1200_LMAX2100();
            	} else if (length >= 5e-8f) {
                	vth = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
                	muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN500_LMAX1200();
                	n = techSpec.getSlopeFactor_LMIN500_LMAX1200();
                	lamda = techSpec.getChannelLengthCoefficient_LMIN500_LMAX1200();
                	theta = techSpec.getMobilityReductionCoefficient_LMIN500_LMAX1200();
                	eta = techSpec.getDIBLCoefficient_LMIN500_LMAX1200();
           	 	} else {
                	vth = techSpec.getThresholdVoltage_LMAX500();
                	muCox = techSpec.getMobilityOxideCapacityCoefficient_LMAX500();
                	n = techSpec.getSlopeFactor_LMAX500();
                	lamda = techSpec.getChannelLengthCoefficient_LMAX500();
                	theta = techSpec.getMobilityReductionCoefficient_LMAX500();
                	eta = techSpec.getDIBLCoefficient_LMAX500();
            	}
			}
			else
			{
				if (length >= 5.0e-7f) {
					vth = techSpec.getThresholdVoltage_LMIN5000();
					muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN5000();
					n = techSpec.getSlopeFactor_LMIN5000();
					lamda = techSpec.getChannelLengthCoefficient_LMIN5000();
					theta = techSpec.getMobilityReductionCoefficient_LMIN5000();
					eta = techSpec.getDIBLCoefficient_LMIN5000();
				} else if (length >= 2.1e-7f) {
					vth = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
					muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000();
					n = techSpec.getSlopeFactor_LMIN2100_LMAX5000();
					lamda = techSpec.getChannelLengthCoefficient_LMIN2100_LMAX5000();
					theta = techSpec.getMobilityReductionCoefficient_LMIN2100_LMAX5000();
					eta = techSpec.getDIBLCoefficient_LMIN2100_LMAX5000();
				} else if (length >= 1.2e-7f) {
                	vth = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
                	muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100();
                	n = techSpec.getSlopeFactor_LMIN1200_LMAX2100();
                	lamda = techSpec.getChannelLengthCoefficient_LMIN1200_LMAX2100();
               	 	theta = techSpec.getMobilityReductionCoefficient_LMIN1200_LMAX2100();
               		eta = techSpec.getDIBLCoefficient_LMIN1200_LMAX2100();
            	} else if (length >= 5e-8f) {
                	vth = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
                	muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN500_LMAX1200();
                	n = techSpec.getSlopeFactor_LMIN500_LMAX1200();
                	lamda = techSpec.getChannelLengthCoefficient_LMIN500_LMAX1200();
                	theta = techSpec.getMobilityReductionCoefficient_LMIN500_LMAX1200();
                	eta = techSpec.getDIBLCoefficient_LMIN500_LMAX1200();
            	} else {
                	vth = techSpec.getThresholdVoltage_LMAX500();
                	muCox = techSpec.getMobilityOxideCapacityCoefficient_LMAX500();
                	n = techSpec.getSlopeFactor_LMAX500();
                	lamda = techSpec.getChannelLengthCoefficient_LMAX500();
                	theta = techSpec.getMobilityReductionCoefficient_LMAX500();
                	eta = techSpec.getDIBLCoefficient_LMAX500();
            	}
				
				
			}
			

			Edge * drainSourceEdge = NULL;
			Edge * gateSourceEdge = NULL;
			std::vector<Edge*> edges = getGraph().findToComponentBelongingEdges(transistor);
			for(std::vector<Edge*>::const_iterator it = edges.begin(); it != edges.end(); it++)
			{
				Edge* edge = *it;
				if(edge->isDrainSourceEdge())
				{
					drainSourceEdge = edge;
				}
				else if (edge->isGateSourceEdge())
				{
					gateSourceEdge = edge;
				}
			}
			assert(drainSourceEdge != NULL, "No drainSourceEdge found!");
			if(gateSourceEdge == NULL)
			{
				gateSourceEdge = drainSourceEdge;
			}
			float vgs = computeEdgeVoltage(*gateSourceEdge);
			float vds = computeEdgeVoltage(*drainSourceEdge);

			if(transistor.getPart().isResistorPart() && getPartitioningResult().getResistorPart(transistor.getArray()).isCompensationResistor())
			 { 
				 float result= muCox * (width * multiplier /length) * abs((vgs -vth));
				 return result;
			 }
			 else
			 {

				if(getEKVVersion() == 1)
				{
					float expF = std::exp((abs(vgs) - abs(vth)) * (0.5 / (n * Vt)));
					float expR = std::exp((abs(vgs) - abs(vth) - n * abs(vds)) * (0.5 / (n * Vt)));
					float lnF = std::log(1 + expF);
					float lnR = std::log(1 + expR);

					float termF = lnF * expF / (1 + expF);
					float termR = lnR * expR / (1 + expR);

					float result3 = 2 * (muCox * (width  * multiplier / length) * n * Vt * termR) * (1 + lamda * abs(vds)) + lamda * abs(ids);
					return result3;
				}
				else if(getEKVVersion() == 2)
				{
					float vP = (abs(vgs) - abs(vth)) / n;
					float expF = std::exp((vP) * (0.5 / Vt));
					float expR = std::exp((vP - abs(vds)) * (0.5 / Vt));
					float lnF = std::log(1 + expF);
					float lnR = std::log(1 + expR);
					float scaleLamda = 1 + lamda * abs(vds);
					float scaleTheta = 1 + theta * abs(vP);

					float termF = lnF * expF / (1 + expF);
					float termR = lnR * expR / (1 + expR);

					float part1 = lamda * abs(ids);
					float part2 = 2 * (muCox * (width  * multiplier / length) * n * Vt * termR) * scaleLamda / scaleTheta;
					float result = part1 + part2;
					return result;
				}
				else
				{
					float vth_eff = abs(vth) - eta * abs(vds);
					float vP = (abs(vgs) - abs(vth_eff)) / n;
					float expF = std::exp((vP) * (0.5 / Vt));
					float expR = std::exp((vP - abs(vds)) * (0.5 / Vt));
					float lnF = std::log(1 + expF);
					float lnR = std::log(1 + expR);
					float scaleLamda = 1 + lamda * abs(vds);
					float scaleTheta = 1 + theta * abs(vP);

					float termF = lnF * expF / (1 + expF);
					float termR = lnR * expR / (1 + expR);

					float part1 = lamda * abs(ids);
					float part21 = 2 * (muCox * (width  * multiplier / length) * Vt) * scaleLamda * eta * (termF - termR) / scaleTheta;
					float part22 = 2 * (muCox * (width  * multiplier / length) * Vt) * scaleLamda * n * termR / scaleTheta;
					float part3 = 2 * (muCox * (width  * multiplier / length) * Vt * Vt) * theta * eta * scaleLamda / pow(scaleTheta, 2);
					float result = part1 + part21 + part22 + part3;
					return result;
				}
			 }
 
		  }
	 }
 
 
		 float PerformanceCalculation::computeOutputSimulateConductance(Partitioning::Part & part, Partitioning::TransconductancePart & stage)
		 {
			  std::vector<Partitioning::Component * > comps = getPartitioningResult().getBelongingComponents(part);
			  if(part.isTransconductancePart() && stage.isFirstStage())
			  {
				  float gdTran1 = computeOutputSimulateConductance(**comps.begin());
				  float gdTran2 = computeOutputSimulateConductance(**(std::next(comps.begin())));
 
				   return gdTran1;
			  }
			  else
			  {
				 Partitioning::Component * outputComp1 =nullptr;
				 Partitioning::Component * outputComp2 =nullptr;
				 Partitioning::Component * supplyComp1 =nullptr;
				 Partitioning::Component * supplyComp2 =nullptr;
				 Partitioning::Component * supplyComp3 =nullptr;
				 Partitioning::Component * supplyComp4 =nullptr;
 
				 for(auto & comp : comps)
				 {
					 if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply()
							 && supplyComp1 == nullptr)
					 {
						 supplyComp1 = comp;
					 }
					 else if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply()
							 && supplyComp2 == nullptr)
					 {
						 supplyComp2 = comp;
					 }
					 else if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply()
							 && supplyComp3 == nullptr)
					 {
						 supplyComp3 = comp;
					 }
					 else if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply() )
					 {
						 supplyComp4 = comp;
					 }
					 else if(isOutputNetStage(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Drain")), stage) && outputComp1 == nullptr)
					 {
						 outputComp1 = comp;
					 }
					 else
					 {
						 outputComp2 = comp;
					 }
				 }
				 assert(supplyComp1 != nullptr ||(outputComp1 != nullptr && outputComp2!= nullptr), "Part should have at least one component connected to supply or two components connected to the output!" );
 
				 if(supplyComp2 == nullptr && outputComp1 == nullptr && outputComp2 ==nullptr)
				 {
					 return computeOutputSimulateConductance(*supplyComp1);
				 }
				 else if(outputComp1 == nullptr && outputComp2 ==nullptr)
				 {
					  float gdComp1 = computeOutputSimulateConductance(*supplyComp1);
					  float gdComp2 = computeOutputSimulateConductance(*supplyComp2);
 
					  return gdComp1;
				 }
				 else if(supplyComp2 == nullptr && outputComp2 ==nullptr)
				 {
					 float gdSupply = computeOutputSimulateConductance(*supplyComp1);
					 float gdOutput = computeOutputSimulateConductance(*outputComp1);
					 float gmOutput = computeSimulateTransconductance(*outputComp1);
					 float result= gdSupply * gdOutput/gmOutput;
					 return result;
				 }
				 else if(supplyComp1 == nullptr && supplyComp2 == nullptr)
				 {
					 float gdTran = computeOutputSimulateConductance(**getPartitioningResult().getBelongingComponents(stage).begin());
 
					 float gdOutput = computeOutputSimulateConductance(*outputComp1);
					 float gmOutput = computeSimulateTransconductance(*outputComp1);
 
					 float result= gdTran * gdOutput/gmOutput;
					 return result;
				 }
				 else if(outputComp2 ==nullptr)
				 {
					 float gdSupply1 = computeOutputSimulateConductance(*supplyComp1);
					 float gdSupply2 = computeOutputSimulateConductance(*supplyComp2);
					 float gdOutput = computeOutputSimulateConductance(*outputComp1);
					 float gmOutput = computeSimulateTransconductance(*outputComp1);
					 float result=  gdSupply1 * gdOutput/gmOutput;
					 return result;
				 }
				 else
				 {
					 if(part.isLoadPart() && (getPartitioningResult().getLoadPart(**part.getMainStructures().begin()).hasCascodedPair() || stage.isComplementary()))
					 {
						 float gmCascodedPair = computeSimulateTransconductance(*outputComp1);
 
						 float gdCascodedPair = computeOutputSimulateConductance(*outputComp1);
						 float gdBias1 ;
						 if(supplyComp3 == nullptr && supplyComp4 == nullptr)
						 {
							 gdBias1 = computeOutputSimulateConductance(*supplyComp1);
						 }
						 else
						 {
							 for(auto & comp : comps)
							 {
								 const StructRec::StructureNet & gateNetLoad = comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
 
								 if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply()
										 && !hasComponentInFeedback(gateNetLoad))
								 {
									 gdBias1 = computeOutputSimulateConductance(*comp);
								 }
							 }
						 }
 
 
						 Partitioning::Component * compFirstStage = nullptr;
						 if(stage.isComplementary())
						 {
							 for(auto & comp : getPartitioningResult().getBelongingComponents(stage))
							 {
								 if(comp->getArray().getTechType() != outputComp1->getArray().getTechType())
								 {
									 compFirstStage = comp;
									 break;
								 }
							 }
 
							 assert(compFirstStage != nullptr, "There should be a device in the first stage with opposite tech type as the load");
						 }
						 else
						 {
							 compFirstStage = *getPartitioningResult().getBelongingComponents(stage).begin();
						 }
						 float gdFT = computeOutputSimulateConductance(*compFirstStage);
						 float result = gdCascodedPair* (gdFT + gdBias1)/ gmCascodedPair ;
						 return  result;
 
					 }
					 else
					 {
						 float gdSupply1 = computeOutputSimulateConductance(*supplyComp1);
						 float gdSupply2 = computeOutputSimulateConductance(*supplyComp2);
						 float gdOutput = computeOutputSimulateConductance(*outputComp1);
						 //float gmOutput = computeOutputSimulateConductance(*outputComp1);
						 float gmOutput = computeSimulateTransconductance(*outputComp1);
 
						 float result = gdSupply1 * gdOutput/gmOutput;
						 return  result;
					 }
				 }
			  }
		 }
 
 
	  float PerformanceCalculation::computeOutputSimulateResistanceFirstStage()
 
	  {
 
		 Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
		 Partitioning::Component & transPartTransistor = **getPartitioningResult().getBelongingComponents(firstStage).begin();
		 std::vector<Partitioning::LoadPart *> loadParts = firstStage.getLoadPart();
		 float rout;
 
		 float gdTran = computeOutputSimulateConductance(transPartTransistor);
 
 
		 if(getPartitioningResult().hasSecondarySecondStage())
		 {
			  std::vector<Partitioning::Component*> loadComps = getPartitioningResult().getBelongingComponents(**loadParts.begin());
			  Partitioning::Component * outputComp = nullptr;
			  for(auto & loadComp : loadComps)
			  {
				  const StructRec::StructureNet * gateNet = nullptr;
				 if(loadComp->getArray().getStructureName() == StructRec::StructureName("MosfetDiodeArray")
					 && loadComp->getArray().isPartOfCurrentMirror())
				 {
					 gateNet = &loadComp->getArray().findNet(StructRec::StructurePinType("MosfetDiodeArray", "Drain"));
				 }
				 else if(loadComp->getArray().isPartOfCurrentMirror())
				 {
					 gateNet = &loadComp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
				 }
				 if(gateNet != nullptr)
				 {
					 for(auto & outputNet : findOutputNetsFirstStage() )
					 {
						 if(gateNet->getIdentifier() == outputNet->getIdentifier())
						 {
							 outputComp = loadComp;
							 break;
						 }
					 }
				 }
 
				 if(outputComp != nullptr)
					 break;
			  }
 
			  assert(outputComp != nullptr, "There must be one load comp connected to the first stage output");
			  float gmLoad = computeSimulateTransconductance(*outputComp);
			  constexpr float epsilon = std::numeric_limits<float>::epsilon();
			  rout = 1 / gmLoad;
		 }
		 else if(loadParts.size() == 1)
		 {
			 float gdLoad = computeOutputSimulateConductance(**loadParts.begin(), firstStage);
 
			 constexpr float epsilon = std::numeric_limits<float>::epsilon();
			 rout = 1 / (gdLoad + gdTran);
		 }
		 else if(firstStage.hasHelperStructure() || firstStage.isComplementary())
		 {
			 float gdLoad1 = computeOutputSimulateConductance(**loadParts.begin(), firstStage);
			 float gdLoad2 = computeOutputSimulateConductance(**std::next(loadParts.begin()), firstStage);
			 constexpr float epsilon = std::numeric_limits<float>::epsilon();
			 rout = 1 / (gdLoad1 + gdLoad2);
		 }
		 else
		 {
			 float gdLoad1 = computeOutputSimulateConductance(**loadParts.begin(), firstStage);
			 float gdLoad2 = computeOutputSimulateConductance(**std::next(loadParts.begin()), firstStage);
			 constexpr float epsilon = std::numeric_limits<float>::epsilon();
			 rout = 1 / (gdLoad1 + gdLoad2 + gdTran);
		  }
		 return rout;
	  }
 
 
	 float PerformanceCalculation::calculateFirstStageGainFirstStageWithDiodeTransistorLoad()
	 {
 
		 Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
		 Partitioning::Component * compFirstStage = *getPartitioningResult().getBelongingComponents(firstStage).begin();
		 float gmFirstStage = computeSimulateTransconductance(*compFirstStage);
 
		 Partitioning::LoadPart & loadFirstStage = **firstStage.getLoadPart().begin();
 
		 if(loadFirstStage.hasCrossCoupledPair(getPartitioningResult()))
		 {
			 Partitioning::Component * diodeComp = nullptr;
			 Partitioning::Component * normalComp = nullptr;
			 for(auto & comp: getPartitioningResult().getBelongingComponents(loadFirstStage))
			 {
				 if(comp->getArray().getStructureName().toStr() == "MosfetNormalArray")
				 {
					 normalComp = comp;
				 }
				 else if(comp->getArray().getStructureName().toStr() == "MosfetDiodeArray")
				 {
					 diodeComp = comp;
				 }
			 }
			 assert(diodeComp != nullptr && normalComp != nullptr, "There should be a diode connected component and a normal component in the load of the first stage" );
 
			 float gmFL = computeSimulateTransconductance(*diodeComp);
			 float gmFLPF = computeSimulateTransconductance(*normalComp);
 
 
			 return gmFirstStage/(gmFL -gmFLPF);
 
		 }
		 else
		 {
			 Partitioning::Component * compFL = nullptr;
			  for(auto & loadComp : getPartitioningResult().getBelongingComponents(loadFirstStage))
			  {
				  const StructRec::StructureNet * gateNet = nullptr;
				 if(loadComp->getArray().getStructureName() == StructRec::StructureName("MosfetDiodeArray")
					 && loadComp->getArray().isPartOfCurrentMirror())
				 {
					 gateNet = &loadComp->getArray().findNet(StructRec::StructurePinType("MosfetDiodeArray", "Drain"));
				 }
				 else if(loadComp->getArray().isPartOfCurrentMirror())
				 {
					 gateNet = &loadComp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
				 }
				 if(gateNet != nullptr)
				 {
					 for(auto & outputNet : findOutputNetsFirstStage() )
					 {
						 if(gateNet->getIdentifier() == outputNet->getIdentifier())
						 {
							 compFL = loadComp;
							 break;
						 }
					 }
				 }
 
				 if(compFL != nullptr)
					 break;
			  }
			 assert(compFL != nullptr, "There must be one load comp being part of a current mirror connected to the first stage output");
			 float gmLoadFirstStage = computeSimulateTransconductance(*compFL);
			 return gmFirstStage/gmLoadFirstStage;
		 }
 
	 }
 
 
	 float PerformanceCalculation::computeGainSimulateFirstStageErrorFactor()
	 {
		 float errorFactor;
		 constexpr float epsilon = std::numeric_limits<float>::epsilon();
		 if(getPartitioningResult().hasBiasOfFoldedPair())
		 {
			 Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
			 Partitioning::Component & compFT = **getPartitioningResult().getBelongingComponents(firstStage).begin();
			 float gdFT = computeOutputSimulateConductance(compFT);
 
			 const StructRec::Structure & cascodePair = firstStage.getHelperStructure();
			 Partitioning::Component & compFL1C = **getPartitioningResult().findComponents(cascodePair).begin();
			 Partitioning::Component & compFL1B = getPartitioningResult().getBiasOfFoldedPair();
			 float gmFL1C = computeSimulateTransconductance(compFL1C);
			 float gdFL1C = computeOutputSimulateConductance(compFL1C);
			 float gdFL1B = computeOutputSimulateConductance(compFL1B);
 
			 if(getPartitioningResult().hasFirstStageGainEnhancer())
			 {
				 const Partitioning::PositiveFeedbackPart & FPF = getPartitioningResult().getFirstStageGainEnhancer();
				 Partitioning::Component & compFPF = **getPartitioningResult().getBelongingComponents(FPF).begin();
				 float gmFPF = computeSimulateTransconductance(compFPF);
				 float gdFPF = computeOutputSimulateConductance(compFPF);
 
 
				 // Compute errorFactor with gain enhancer
				 float denominator = gdFT + gdFL1B + gmFL1C + gdFL1C + gdFPF + gmFPF;
				 assert(denominator > epsilon && "Division by zero or near-zero in error factor calculation");
				 errorFactor = (gmFL1C + gdFL1C) / denominator;
			 }
			 else
			 {
				 // Compute errorFactor without gain enhancer
				 float denominator = gdFT + gdFL1B + gmFL1C + gdFL1C;
				 assert(denominator > epsilon && "Division by zero or near-zero in error factor calculation");
 
				 errorFactor = (gmFL1C + gdFL1C) / denominator;
			 }
		 }
		 else
		 {
			 errorFactor = 1;
		 }
		 return errorFactor;
	 }
 
 
	 float PerformanceCalculation::calculateGainFirstStage()
	 {
		 float Rout;
		 constexpr float epsilon = std::numeric_limits<float>::epsilon();
		 Partitioning::TransconductancePart & firstStage =  getPartitioningResult().getFirstStage();
		 std::vector<Partitioning::LoadPart*> loadParts = firstStage.getLoadPart();
		 Rout = computeOutputSimulateResistanceFirstStage();
 
		 float firstStageTransconductance = computeSimulateTransconductance(firstStage);
 
 
 
		 if(!getPartitioningResult().hasBiasOfFoldedPair())
		 {
			 if(getPartitioningResult().hasSecondarySecondStage())
			 {
				 return calculateFirstStageGainFirstStageWithDiodeTransistorLoad();
			 }
			 else
			 {
				 return firstStageTransconductance * Rout;
			 }
		 }
		 else if(getPartitioningResult().hasFirstStageGainEnhancer())
		 {
			 float errorFactor = computeGainSimulateFirstStageErrorFactor();
 
 
			 const Partitioning::PositiveFeedbackPart & FPF = getPartitioningResult().getFirstStageGainEnhancer();
			 Partitioning::Component & compFPF = **getPartitioningResult().getBelongingComponents(FPF).begin();
			 float gmFPF = computeSimulateTransconductance(compFPF);
 
 
			 float subtractor = gmFPF * Rout;
			 float k = 1 - subtractor;
 
 
			 float gain;
 
			 assert(std::fabs(k) > epsilon && "Division by zero or near-zero in gain calculation");
 
			 gain = (  /*errorFactor**/  firstStageTransconductance * Rout) / std::fabs(k);
 
			 return gain;
		 }
		 else
		 {
			 float k;
			 float gLoad2;
			 float gLoad1;
			 
			 for(std::vector<Partitioning::LoadPart*>::const_iterator it = loadParts.begin(); it != loadParts.end(); it++)
			 {
				 Partitioning::LoadPart * loadPart = * it;
				  //Only LoadParts Consisting of 4 transistors are supported
				 if(loadPart->hasCascodedPair())
				 {
					 gLoad1 = computeOutputSimulateConductance(*loadPart, firstStage);
 
				  }
				 else
				 {
					 gLoad2 = computeOutputSimulateConductance(*loadPart, firstStage);
				 }
 
			 }

			 return ((2+k)/(2+2*k)) *firstStageTransconductance * Rout;
 
		 }
	 }
 
 
 
	 float PerformanceCalculation::createGainHigherStage(Partitioning::TransconductancePart & higherStage)
	 {
		 assert(higherStage.isPrimarySecondStage() || higherStage.isSecondarySecondStage() || higherStage.isThirdStage(),
				 "Stage needs top be either second or third stage");
 
		 Partitioning::Component * supplyComp = nullptr;
 
		 for(auto & comp : getPartitioningResult().getBelongingComponents(higherStage))
		 {
			 if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply())
			 {
				 supplyComp = comp;
			 }
		 }
 
		 assert(supplyComp != nullptr, "Every higher stage should have a to supply connected component");
		 float gmTrans = computeSimulateTransconductance(*supplyComp);
 
 
		 float Rout = computeOutputResistanceSimulateHigherStage(higherStage);
		 return abs(gmTrans *Rout);
 
 
	 }
 
 
	 float PerformanceCalculation::computeOutputResistanceSimulateHigherStage(Partitioning::TransconductancePart & higherStage)
	 {
 
			 constexpr float epsilon = std::numeric_limits<float>::epsilon();
			 assert(higherStage.isHigherStage(), "The used stage should be of type higher stage!");
 
			 Partitioning::BiasPart & biasPart = **higherStage.getBiasPart().begin();
 
			 float Rout;
 
			 float gST;
			 float gSB;
 
 
			 gST = computeOutputSimulateConductance(higherStage, higherStage);
 
			 gSB = computeOutputSimulateConductance(biasPart, higherStage);
 
			 float denominator = gST + gSB;
 
			 Rout = 1 / denominator;
 
			 return Rout;
 
	 }
 
	 float PerformanceCalculation::calculateGainSecondStage()
	 {
		 float gainSecondStage;
 
		 if(getPartitioningResult().hasSecondStage())
		 {
			 gainSecondStage = createGainHigherStage(getPartitioningResult().getPrimarySecondStage());
		 }
		 else
		 {
			 gainSecondStage = 1;
		 }
		 return gainSecondStage;
	 }
 
	 float PerformanceCalculation::calculateGainThirdStage()
	 {
		 float gainThirdStage;
 
		 if(getPartitioningResult().hasThirdStage())
		 {
			 gainThirdStage = createGainHigherStage(getPartitioningResult().getThirdStage());
		 }
		 else
		 {
			 gainThirdStage = 1;
		 }
		 return gainThirdStage;
	 }
 
 
	 float PerformanceCalculation::calculateGainSimulation()
	 {
		 float gain = 0.0f;
		 
		 logDebug("calculateGain for Simulation"); 
		 
		 float gainFirstStage = calculateGainFirstStage();
		 float gainSecondStage = calculateGainSecondStage();
		 float gainThirdStage = calculateGainThirdStage();
		 
		 gain = gainFirstStage * gainSecondStage * gainThirdStage;
 
		 // Return gain in dB
		 logDebug("GAIN 1 stage");
		 logDebug(gainFirstStage);
		 logDebug("GAIN 2 stage");
		 logDebug(gainSecondStage);
		 logDebug("GAIN 3 stage");
		 logDebug(gainThirdStage);
		 
		 if (gain <= 0) {
		 logDebug("Warning: Gain is non-positive, returning 0");
		 return 0.0f;
		 }
 
		// Logarithmic gain calculation
		float logGain = std::log10(gain);
 
		// Convert logarithmic gain to decibels
		float gainInDecibels = 20.0f * logGain;
 
		// Return gain in decibels
		logDebug("SIMULATED Gain");
		logDebug(gainInDecibels);
		setGain(gainInDecibels);
		return gainInDecibels;
	 }
 
 
	 float PerformanceCalculation::calculateSimulatedPowerConsumption() 
	 {
	 float power = 0.0f;
 
	 // Retrieve supply and ground nodes
	 const Node &supplyNode = getGraph().getSupplyNode();
	 const Node &groundNode = getGraph().getGroundNode();
 
	 // Calculate Vdd value (supply voltage minus ground voltage)
	 float vddValue = getCircuitInformation().getCircuitParameter().getValue(supplyNode.getNetId()) - 
					  getCircuitInformation().getCircuitParameter().getValue(groundNode.getNetId());
 
	 // Vectors to store voltage (Vdd) and current values
	 std::vector<float> vdd;
	 std::vector<float> current;
 
	 // Get incoming and outgoing edges
	 const std::set<Edge*> incomingEdges = groundNode.getIncomingEdges();
	 const std::set<Edge*> outgoingEdges = groundNode.getOutgoingEdges();
 
	 // Initialize Vdd and current values
	 float vddInitial = static_cast<float>(incomingEdges.size() + outgoingEdges.size());
	 float currentInitial = static_cast<float>(incomingEdges.size() + outgoingEdges.size());
 
	 float biasCurrent = getCircuitInformation().getCircuitParameter().getCurrentBiasPin().getValue();
 
	 if (biasCurrent <= 0) {
		 // If no bias current, initialize with initial values
		 vdd.assign(incomingEdges.size() + outgoingEdges.size(), vddInitial);
		 current.assign(incomingEdges.size() + outgoingEdges.size(), currentInitial);
	 } else {
		 // If bias current exists, add an offset
		 float vddAddOn = 1.0f;
		 float currentAddOn = 1.0f;
		 vdd.assign(incomingEdges.size() + outgoingEdges.size(), vddInitial + vddAddOn);
		 current.assign(incomingEdges.size() + outgoingEdges.size(), currentInitial + currentAddOn);
	 }
 
	 int index = 0;
 
	 // Process incoming edges
	 for (Edge *edge : incomingEdges) {
		 if (index >= vdd.size() || index >= current.size()) {
			 std::cerr << "Index out of range during incoming edges processing!" << std::endl;
			 break;
		 }
 
		 current[index] = computeEdgeCurrentSimulation(*edge); // Simulated current for edge
		 vdd[index] = 1 * vddValue; // Negative voltage for incoming edges
		 index++;
	 }
 
	 // Process outgoing edges
	 for (Edge *edge : outgoingEdges) {
		 if (index >= vdd.size() || index >= current.size()) {
			 std::cerr << "Index out of range during outgoing edges processing!" << std::endl;
			 break;
		 }
 
		 current[index] = computeEdgeCurrentSimulation(*edge); // Simulated current for edge
		 vdd[index] = - vddValue; // Positive voltage for outgoing edges
		 index++;
	 }
 
	 // Include bias current if it exists
	 if (biasCurrent > 0 && index < vdd.size() && index < current.size()) {
		 vdd[index] = vddValue; // Voltage associated with bias current
		 current[index] = biasCurrent; // Add bias current
	 }
 
	 // Calculate power as the sum of (voltage * current) for each edge
	 for (size_t i = 0; i < vdd.size(); ++i) {
		 power += vdd[i] * current[i];
		 logDebug("Edge " << i << ": Vdd = " << vdd[i] << ", Current = " << current[i]);
		 logDebug("Power contribution from edge " << i << ": " << vdd[i] * current[i]);
		 logDebug("Power so far: " << power);
	 }
 
	 // Convert power to milliwatts
	 power *=  1000.0f ;
 
	 setPowerConsumption(power); 
 
	 return power; // Return the total power consumption in milliwatts
	}
 
 
 
	 float PerformanceCalculation::computeEdgeCurrentSimulation(Edge & edge)
	  {
		  if(edge.isDrainSourceEdge())
		  {
			  return static_cast<float>(getSpace().createFloatCurrent(getTransistorToCurrentMap().find(edge.getEdgeKey().getStructureId())).min());
 
		  }
		  else
		  {
			  float current = 0.0f;
			  return current;
		  }
	 }
 
 
	 float PerformanceCalculation::calculateSimulatedCMRR()
	{
	 // Retrieve the first stage and its components
	 Partitioning::TransconductancePart &firstStage = getPartitioningResult().getFirstStage();
	 Partitioning::Component *diffStageTran = *getPartitioningResult().getBelongingComponents(firstStage).begin();
	 Partitioning::BiasPart *biasOfFirstStage = *firstStage.getBiasPart().begin();
	 Partitioning::Component *biasTranFirstStage = *getPartitioningResult().getBelongingComponents(*biasOfFirstStage).begin();
 
	 // Calculate the gain and conductance
	 float firstStageGain = calculateGainFirstStage();
	 float gdBiasFirstStage = computeOutputSimulateConductance(*biasTranFirstStage);
 
	 // Retrieve the load part without a cascoded pair
	 std::vector<Partitioning::LoadPart*> loadParts = firstStage.getLoadPart();
	 Partitioning::LoadPart *loadPart = nullptr;
	 for (auto &loadPartIt : loadParts)
	 {
		 if (!loadPartIt->hasCascodedPair())
		 {
			 loadPart = loadPartIt;
			 break;
		 }
	 }
	 assert(loadPart != nullptr && "Every first stage should have a load part without a cascoded pair");
 
	 Core::MosfetDeviceType mosfet;
	 std::vector<const Core::Net*> outputNetsFirstStageTrans = firstStage.getOutputNets();
 
	 Partitioning::Component *loadComp = nullptr;
 
	 // Find the load component connected via the gate to the output
	 for (auto &loadCompIt : getPartitioningResult().getBelongingComponents(*loadPart))
	 {
		 const Core::Device &loadDevice = **loadCompIt->getArray().getDevices().begin();
		 const Core::Net &gateNet = loadDevice.findNet(mosfet.gate());
 
		 for (auto &outputNetFirstStage : outputNetsFirstStageTrans)
		 {
			 if (gateNet.getIdentifier() == outputNetFirstStage->getIdentifier())
			 {
				 loadComp = loadCompIt;
				 break;
			 }
			 if (loadComp != nullptr)
			 	break;
		 }
		 
	 }
 
	 // If no load component was found via the gate, try using the drain connection
	 if (loadComp == nullptr)
	 {
		 for (auto &loadCompIt : getPartitioningResult().getBelongingComponents(*loadPart))
		 {
			 const Core::Device &loadDevice = **loadCompIt->getArray().getDevices().begin();
			 const Core::Net &drainNet = loadDevice.findNet(mosfet.drain());
 
			 for (auto &outputNetFirstStage : outputNetsFirstStageTrans)
			 {
				 if (drainNet.getIdentifier() == outputNetFirstStage->getIdentifier())
				 {
					 loadComp = loadCompIt;
					 break;
				 }
			 }
			 if (loadComp != nullptr)
				 break;
		 }
		 assert(loadComp != nullptr && "There must be a load component connected via its drain to the output of the first stage");
	 }
 
	 // Calculate the transconductance of the load
	 float gmLoad = computeSimulateTransconductance(*loadComp);
	 float CMRR;
 
	 // If there is a secondary second stage, consider it in the calculation
	 if (getPartitioningResult().hasSecondarySecondStage())
	 {
		 float secondStageGain = calculateGainSecondStage();
		 float opAmpGain = firstStageGain * secondStageGain ;
 
		 float inverseCommonModeGain = 2 * gmLoad / gdBiasFirstStage;
 
		 CMRR = opAmpGain * inverseCommonModeGain;
 
		 // Convert to decibels
		 setCMRR(20.0f * std::log10(CMRR));
		 return 20.0f * std::log10(CMRR);
	 }
	 else
	 {
		 // Without a secondary second stage, calculate directly
		 CMRR = 2 * firstStageGain * gmLoad / gdBiasFirstStage ;
		 // Convert to decibels
		 setCMRR(20.0f * std::log10(CMRR));
		 return 20.0f * std::log10(CMRR);
	 }
	 }
	 
 
   
 
	 std::tuple<float, float> PerformanceCalculation::calculateSimulatedCommonModeInputVoltage()
	 {
		 float vss = getCircuitInformation().getCircuitParameter().getGroundVoltage();
		 float vdd = getCircuitInformation().getCircuitParameter().getSupplyVoltage();
	 
		 float vcmMinValue = -10000.0f;  
		 float vcmMaxValue = 100000.0f;  
	 
	 
		 if (!getPartitioningResult().getFirstStage().isComplementary()) 
		 {
			 float u = getSpace().getScalingFactorMUM(); // Scaling factor (not directly needed in the refactored version)
			 
			 // Compute input-to-supply rail path over bias and load
			 std::vector<Partitioning::Component*> inputToSupplyRailPathOverBias = computeInputToSupplyRailPathOverBias(getPartitioningResult().getFirstStage());
			 std::vector<Partitioning::Component*> inputToSupplyRailPathOverLoad = computeInputToSupplyRailPathOverLoad(getPartitioningResult().getFirstStage());
	 
			 // Get the first stage of the partitioning result
			 Partitioning::TransconductancePart& firstStage = getPartitioningResult().getFirstStage();
			 Partitioning::Component* diffStageTran = *getPartitioningResult().getBelongingComponents(firstStage).begin();
	 
			 // Get the threshold voltage for the first stage transistor based on the transistor model
			 float vthFirstStage = 0.0f;
			 if (getTransistorModel() == "SHM") {
				 const TechnologieSpecificationSHM& techSpec = getCircuitInformation().getTechnologieSpecificationSHM(*diffStageTran);
				 vthFirstStage = techSpec.getThresholdVoltage();
			 } else {
				 const TechnologieSpecificationEKV& techSpec = getCircuitInformation().getTechnologieSpecificationEKV(*diffStageTran);
				 float length = getTransistorToLengthInputMap().find(*diffStageTran);
				 logDebug("Length: " << length);
				 if (length >= 5.0f) {
					 vthFirstStage = techSpec.getThresholdVoltage_LMIN5000();
				 } else if (length >= 2.1f) {
					 vthFirstStage = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
				 } else if (length >= 1.2f) {
					 vthFirstStage = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
				 } else if (length >= 0.5f) {
					 vthFirstStage = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
				 } else {
					 vthFirstStage = techSpec.getThresholdVoltage_LMAX500();
				 }
			 }

			 Edge * drainSourceEdge = NULL;
			Edge * gateSourceEdge = NULL;
			std::vector<Edge*> edges = getGraph().findToComponentBelongingEdges(*diffStageTran);
			for(std::vector<Edge*>::const_iterator it = edges.begin(); it != edges.end(); it++)
			{
				Edge* edge = *it;
				if(edge->isDrainSourceEdge())
				{
				 	drainSourceEdge = edge;
				}
				else if (edge->isGateSourceEdge())
				{
				gateSourceEdge = edge;
				}
			}
			assert(drainSourceEdge != NULL, "No drainSourceEdge found!");
			if(gateSourceEdge == NULL)
			{
				gateSourceEdge = drainSourceEdge;
			}
			float vgsFirstStage = computeEdgeVoltage(*gateSourceEdge);
			float vdsFirstStage = computeEdgeVoltage(*drainSourceEdge);

			 const int nLoad = static_cast<int>(inputToSupplyRailPathOverLoad.size());
			 float signInputToSupplyRailPathOverLoad[nLoad];
			 float voltageInputToSupplyRailPathOverLoad[nLoad];
			 int indexInputToSupplyRailPathOverLoad = 0;
			 for (auto& comp : inputToSupplyRailPathOverLoad) {
				 if (!comp->getPart().isTransconductancePart()) {
					Edge * drainSourceEdge = NULL;
					Edge * gateSourceEdge = NULL;

					for(auto& edge: getGraph().findToComponentBelongingEdges(*comp) )
					{
						if(edge->isDrainSourceEdge())
						{
							drainSourceEdge = edge;
						}
						else if(edge->isGateSourceEdge())
						{
							gateSourceEdge = edge;
						}
					}

					if(gateSourceEdge == nullptr){
						gateSourceEdge = drainSourceEdge;
					}
					assert(drainSourceEdge != NULL, "Every transistor should have a drain source Edge");

					float drainSourceVoltage = computeEdgeVoltage(*drainSourceEdge);
					float gateSourceVoltage = computeEdgeVoltage(*gateSourceEdge);

					logDebug("Drain-Source Voltage: " << drainSourceVoltage << ", Gate-Source Voltage: " << gateSourceVoltage);

					 voltageInputToSupplyRailPathOverLoad[indexInputToSupplyRailPathOverLoad] = computeEdgeVoltage(*gateSourceEdge);
				 	if (getPartitioningResult().isPartOfHelperStructureOfDifferentialPair(*comp)) {
					 	signInputToSupplyRailPathOverLoad[indexInputToSupplyRailPathOverLoad] = -1; 
				 	}
				 	else  {
					 	signInputToSupplyRailPathOverLoad[indexInputToSupplyRailPathOverLoad] = 1;
				 	}
					indexInputToSupplyRailPathOverLoad++;
				}
			 }
			 logDebug("nLoad: " << nLoad);
			 logDebug("voltageInputToSupplyRailPathOverLoad[0]: " << voltageInputToSupplyRailPathOverLoad[0] << " signInputToSupplyRailPathOverLoad[0]: " << signInputToSupplyRailPathOverLoad[0]);
			 logDebug("voltageInputToSupplyRailPathOverLoad[1]: " << voltageInputToSupplyRailPathOverLoad[1] << " signInputToSupplyRailPathOverLoad[1]: " << signInputToSupplyRailPathOverLoad[1]);

			 const int nBias = static_cast<int>(inputToSupplyRailPathOverBias.size());
			 float signInputToSupplyRailPathOverBias[nBias];
			 float voltageInputToSupplyRailPathOverBias[nBias];
			 int indexInputToSupplyRailPathOverBias = 0;
			 for (auto& comp : inputToSupplyRailPathOverBias) {
				 if (!comp->getPart().isTransconductancePart()) {
					Edge * gateSourceEdge = NULL;
					Edge * drainSourceEdge = NULL;

					for(auto& edge: getGraph().findToComponentBelongingEdges(*comp) )
					{
						if(edge->isDrainSourceEdge())
						{
							drainSourceEdge = edge;
						}
						else if(edge->isGateSourceEdge())
						{
							gateSourceEdge = edge;
						}
						else if(comp->getArray().getStructureName() == StructRec::StructureName("MosfetDiodeArray")
						&& edge->isDrainSourceEdge())
						{
							gateSourceEdge = edge;
						}
					}

					if(gateSourceEdge == nullptr){
						gateSourceEdge = drainSourceEdge;
					}
					assert(drainSourceEdge != NULL, "Every transistor should have a drain source Edge");

					float drainSourceVoltage = computeEdgeVoltage(*drainSourceEdge);
					float gateSourceVoltage = computeEdgeVoltage(*gateSourceEdge);

					logDebug("Drain-Source Voltage: " << drainSourceVoltage << ", Gate-Source Voltage: " << gateSourceVoltage);

					 voltageInputToSupplyRailPathOverBias[indexInputToSupplyRailPathOverBias] = computeEdgeVoltage(*gateSourceEdge);
					 signInputToSupplyRailPathOverBias[indexInputToSupplyRailPathOverBias] = 1; 
					 indexInputToSupplyRailPathOverBias++;
				}
			 }
			 logDebug("nBias: " << nBias);
			 logDebug("voltageInputToSupplyRailPathOverBias[0]: " << voltageInputToSupplyRailPathOverBias[0] << " signInputToSupplyRailPathOverBias[0]: " << signInputToSupplyRailPathOverBias[0]);
			 logDebug("voltageInputToSupplyRailPathOverBias[1]: " << voltageInputToSupplyRailPathOverBias[1] << " signInputToSupplyRailPathOverBias[1]: " << signInputToSupplyRailPathOverBias[1]);
	 
			 // Variables to hold the total voltage drop in the paths
			 float constantInputToSupplyRailPathOverLoad = 0.0f;
			 float constantInputToSupplyRailPathOverBias = 0.0f;
	 
			 // Calculate the right-hand side expressions for the min and max values
			 if (diffStageTran->getArray().getTechType().isP()) {
				 // For PMOS transistors
				 for (auto& comp : inputToSupplyRailPathOverLoad) {
					float vth = 0.0f;
					if (getTransistorModel() == "SHM") {
						const TechnologieSpecificationSHM& techSpec = getCircuitInformation().getTechnologieSpecificationSHM(*comp);
						vth = techSpec.getThresholdVoltage();
					} else {
						const TechnologieSpecificationEKV& techSpec = getCircuitInformation().getTechnologieSpecificationEKV(*comp);
						float length = getTransistorToLengthInputMap().find(*comp);
				 		logDebug("Length: " << length);
				 		if (length >= 5.0f) {
							vth = techSpec.getThresholdVoltage_LMIN5000();
				 		} else if (length >= 2.1f) {
							vth = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
				 		} else if (length >= 1.2f) {
							vth = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
				 		} else if (length >= 0.5f) {
							vth = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
				 		} else {
							vth = techSpec.getThresholdVoltage_LMAX500();
				 		}
					}
		
					if (comp->getPart().isTransconductancePart()) {
						constantInputToSupplyRailPathOverLoad += vth; // Add voltage for transconductance parts
					} else if (getPartitioningResult().isPartOfHelperStructureOfDifferentialPair(*comp)) {
						constantInputToSupplyRailPathOverLoad += vth; // Add voltage for differential pair parts
					} else if (comp->getArray().getStructureName().toStr() == "MosfetNormalArray") {
						constantInputToSupplyRailPathOverLoad -= vth; // Subtract voltage for normal MOSFET array parts
					}
				 }
				 float sumLoad = 0.0f;
				 for(int i = 0; i < indexInputToSupplyRailPathOverLoad; i++) {
					 sumLoad += signInputToSupplyRailPathOverLoad[i] * voltageInputToSupplyRailPathOverLoad[i];
				 }
				 float sumBias = 0.0f;
				 for(int i = 0; i < indexInputToSupplyRailPathOverBias; i++) {
					 sumBias += signInputToSupplyRailPathOverBias[i] * voltageInputToSupplyRailPathOverBias[i];
				 }

				 logDebug("Sum Load: " << sumLoad << ", Sum Bias: " << sumBias);

				 vcmMinValue = sumLoad + constantInputToSupplyRailPathOverLoad + vss;
				 vcmMaxValue = sumBias + vdd - vthFirstStage;
			 } else {
				 // For NMOS transistors
				 for (auto& comp : inputToSupplyRailPathOverLoad) {
					float vth = 0.0f;
					if (getTransistorModel() == "SHM") {
						const TechnologieSpecificationSHM& techSpec = getCircuitInformation().getTechnologieSpecificationSHM(*comp);
						vth = techSpec.getThresholdVoltage();
					} else {
						const TechnologieSpecificationEKV& techSpec = getCircuitInformation().getTechnologieSpecificationEKV(*comp);
						float length = getTransistorToLengthInputMap().find(*comp);
				 		logDebug("Length: " << length);
				 		if (length >= 5.0f) {
							vth = techSpec.getThresholdVoltage_LMIN5000();
				 		} else if (length >= 2.1f) {
							vth = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
				 		} else if (length >= 1.2f) {
							vth = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
				 		} else if (length >= 0.5f) {
							vth = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
				 		} else {
							vth = techSpec.getThresholdVoltage_LMAX500();
						}
					}

					if (comp->getPart().isTransconductancePart()) {
						constantInputToSupplyRailPathOverLoad += vth;
					} else if (getPartitioningResult().isPartOfHelperStructureOfDifferentialPair(*comp)) {
						constantInputToSupplyRailPathOverLoad += vth;
					} else if (comp->getArray().getStructureName().toStr() == "MosfetNormalArray") {
						constantInputToSupplyRailPathOverLoad -= vth;
					}
				 }
				 for (auto& comp : inputToSupplyRailPathOverBias) {
					float vth = 0.0f;
					if (getTransistorModel() == "SHM") {
						const TechnologieSpecificationSHM& techSpec = getCircuitInformation().getTechnologieSpecificationSHM(*comp);
						vth = techSpec.getThresholdVoltage();
					} else {
						const TechnologieSpecificationEKV& techSpec = getCircuitInformation().getTechnologieSpecificationEKV(*comp);
						float length = getTransistorToLengthInputMap().find(*comp);
				 		logDebug("Length: " << length);
				 		if (length >= 5.0f) {
							vth = techSpec.getThresholdVoltage_LMIN5000();
				 		} else if (length >= 2.1f) {
							vth = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
				 		} else if (length >= 1.2f) {
							vth = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
				 		} else if (length >= 0.5f) {
							vth = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
				 		} else {
							vth = techSpec.getThresholdVoltage_LMAX500();
						}
					}
		
					constantInputToSupplyRailPathOverBias += vth; // Add voltage for biasing components
				}
		
				 float sumLoad = 0.0f;
				 for(int i = 0; i < indexInputToSupplyRailPathOverLoad; i++) {
					 sumLoad += signInputToSupplyRailPathOverLoad[i] * voltageInputToSupplyRailPathOverLoad[i];
				 }
				 float sumBias = 0.0f;
				 for(int i = 0; i < indexInputToSupplyRailPathOverBias; i++) {
					 sumBias += signInputToSupplyRailPathOverBias[i] * voltageInputToSupplyRailPathOverBias[i];
				 }

				 //logDebug("Sum Load: " << sumLoad << ", Sum Bias: " << sumBias);
				 //logDebug("constantInputToSupplyRailPathOverLoad: " << constantInputToSupplyRailPathOverLoad);
				 //logDebug("constantInputToSupplyRailPathOverBias: " << constantInputToSupplyRailPathOverBias);
				 //logDebug("Vth First Stage: " << vthFirstStage);
				 vcmMinValue = sumBias + vss - vthFirstStage + vgsFirstStage;
				 vcmMaxValue = sumLoad + constantInputToSupplyRailPathOverLoad + vdd;
				 logDebug("vcmMinValue: " << vcmMinValue << ", vcmMaxValue: " << vcmMaxValue);

			 }
		 } else {
			 // For complementary configurations, set the min and max values directly
			 vcmMinValue = vss;
			 vcmMaxValue = vdd;
		 }
		 setMaxCommonModeInputVoltage(vcmMaxValue);
		 setMinCommonModeInputVoltage(vcmMinValue);
		 // Return the calculated minimum and maximum common-mode input voltage as a tuple
		 return std::make_tuple(vcmMinValue, vcmMaxValue);
	 }
	 
	 
 
 
 
 std::tuple<float, float> PerformanceCalculation::calculateSimulatedPSRR()
	 {
		 float posPSRR;
 
		 float negPSRR;
		 if(!getPartitioningResult().getFirstStage().hasHelperStructure() && getPartitioningResult().hasSecondStage())
		 {
			 Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
			 Partitioning::Component * diffStageTran = *getPartitioningResult().getBelongingComponents(firstStage).begin();
			 float gmDiffStage = computeSimulateTransconductance(*diffStageTran);
			 float  gdDiffStage = computeOutputSimulateConductance(*diffStageTran);
 
			 Partitioning::BiasPart * biasOfFirstStage = *firstStage.getBiasPart().begin();
			 Partitioning::Component * biasTranFirstStage = *getPartitioningResult().getBelongingComponents(*biasOfFirstStage).begin();
			 float  gdBiasFirstStage= computeOutputSimulateConductance(*biasTranFirstStage);
 
			 Partitioning::TransconductancePart & secondStage = **getPartitioningResult().getSecondStages().begin();
			 Partitioning::Component * secondStageTran = *getPartitioningResult().getBelongingComponents(secondStage).begin();
			 float gmSecondStage = computeSimulateTransconductance(*secondStageTran);
			 float gdSecondStage = computeOutputSimulateConductance(*secondStageTran);
 
			 Partitioning::BiasPart * biasOfSecondStage = *secondStage.getBiasPart().begin();
			 Partitioning::Component * biasTranSecondStage = *getPartitioningResult().getBelongingComponents(*biasOfSecondStage).begin();
			 float gdBiasSecondStage = computeOutputSimulateConductance(*biasTranSecondStage);
 
			 float firstStageGain = calculateGainFirstStage();
			 std::vector<Partitioning::LoadPart*> loadParts = firstStage.getLoadPart();
			 if(loadParts.size() ==1)  //Miller Operational Amplifier
			 {
				 Partitioning::LoadPart * loadOfFirstStage = *firstStage.getLoadPart().begin();
				 Partitioning::Component* loadTranFirstStage = *getPartitioningResult().getBelongingComponents(*loadOfFirstStage).begin();
 
				 float gmLoadFirstStage = computeSimulateTransconductance(*loadTranFirstStage);
				 float  gdLoadFirstStage = computeOutputSimulateConductance(*loadTranFirstStage);
 
 
				 if(getPartitioningResult().getSecondStages().size() == 1)
				 {
 
					 if(diffStageTran->getArray().getTechType().isP())
					 {
 
						 posPSRR = ((2* firstStageGain* gmLoadFirstStage * gmSecondStage)/ ((2* gmLoadFirstStage * gdBiasSecondStage - gmSecondStage * gdBiasFirstStage )))  ;
 
						 negPSRR = ((firstStageGain * gmSecondStage)/ ( gdSecondStage))   ;
 
					 }
					 else
					 {
						 float minuend = 2* gmLoadFirstStage * gdBiasSecondStage ;
 
						 float subtrahend = gmSecondStage * gdBiasFirstStage ;
 
						 float factor = (minuend - subtrahend);
						 
						 float multiplication = (2* firstStageGain * gmLoadFirstStage * gmSecondStage) ;

						 logDebug("gmLoadFirstStage: " << gmLoadFirstStage << " gdBiasSecondStage: " << gdBiasSecondStage << " gmSecondStage: " << gmSecondStage << " gdBiasFirstStage: " << gdBiasFirstStage);
						 logDebug("minuend: " << minuend << " subtrahend: " << subtrahend << " factor: " << factor << " multiplication: " << multiplication);
						 logDebug("firstStageGain: " << firstStageGain << " gmLoadFirstStage: " << gmLoadFirstStage << " gmSecondStage: " << gmSecondStage);
						 logDebug("multiplication: " << multiplication);
						 negPSRR = abs(multiplication / factor)  ;
 
						 posPSRR = ((firstStageGain * gmSecondStage)/ (gdSecondStage))  ;
 
 
					 }
				 }
				 else
				 {
 
					 if(diffStageTran->getArray().getTechType().isP())
 
					 {
 
						 posPSRR = ((2* firstStageGain * gmLoadFirstStage * gmSecondStage)/ ( abs(2* gmLoadFirstStage * gdBiasSecondStage - gmSecondStage * gdBiasFirstStage )))  ;
 
						 negPSRR = ((firstStageGain * gmSecondStage)/ (gdSecondStage)) ;
 
					 }
					 else
					 {
 
						 negPSRR = ((2* firstStageGain * gmLoadFirstStage * gmSecondStage)/ ( abs(2* gmLoadFirstStage * gdBiasSecondStage - gmSecondStage * gdBiasFirstStage ))) ;
 
						 posPSRR = ((firstStageGain * gmSecondStage)/ (gdSecondStage)) ;
 
					 }
 
				 }
				 
				 setPosPSRR(20 * std::log10(posPSRR));
				 setNegPSRR(20 * std::log10(negPSRR));
				 return std::make_tuple(20 * std::log10(negPSRR), 20 * std::log10(posPSRR));
 
			 }
			 else
			 {
				 posPSRR = getCircuitInformation().getCircuitSpecification().getPosPSRR()+10;
				 negPSRR = getCircuitInformation().getCircuitSpecification().getNegPSRR()+10;
				 setPosPSRR(posPSRR);
				 setNegPSRR(negPSRR);
				 return std::make_tuple(negPSRR, posPSRR);
			 }
		 }
		 else
		 {
			 posPSRR = getCircuitInformation().getCircuitSpecification().getPosPSRR()+10;
			 negPSRR = getCircuitInformation().getCircuitSpecification().getNegPSRR()+10;
			 setPosPSRR(posPSRR);
			 setNegPSRR(negPSRR);
			 return std::make_tuple(negPSRR, posPSRR);
		 }
 
	 }
 
 
 
 
 
	 float PerformanceCalculation::calculateSimulatedTransientFrequency()
	{
		Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();

		float gmTran;

		if(!firstStage.isComplementary())
		{
			Partitioning::Component& tranComp = **getPartitioningResult().getBelongingComponents(firstStage).begin();
			gmTran = computeSimulateTransconductance(tranComp);
		}
		else
		{
			Partitioning::Component* pmosComp = nullptr;
			Partitioning::Component * nmosComp = nullptr;
			for(auto& comp: getPartitioningResult().getBelongingComponents(firstStage))
			{
				if(comp->getArray().getTechType().isP() && pmosComp == nullptr)
				{
					pmosComp = comp;
				}
				else if(comp->getArray().getTechType().isN() && nmosComp == nullptr)
				{
					nmosComp = comp;
				}
			}

			assert(pmosComp != nullptr && nmosComp != nullptr, "In a complementary first stage there should be both, nmos and pmos, devices!");

			float gmPmos = computeSimulateTransconductance(*pmosComp);
			float gmNmos = computeSimulateTransconductance(*nmosComp);

			gmTran = gmPmos + gmNmos;
		}

		float outputCapacitance;

		if(getPartitioningResult().getFirstStage().hasLoadPart() == 1
				&& getPartitioningResult().hasSecondarySecondStage())
		{
			float B = computeScalingFactorForSymmetricalOTASim();
			outputCapacitance = calculateSimulatedNetCapacitance(findOutputNetWithCapacitor(getPartitioningResult().getPrimarySecondStage()));

			Partitioning::LoadPart & loadFirstStage = **firstStage.getLoadPart().begin();

			if(loadFirstStage.hasCrossCoupledPair(getPartitioningResult()))
			{
				setTransitFrequency(2 * B * gmTran *1e-6f /(2 * 3.141592653589793 * outputCapacitance));
				return 2 * B * gmTran *1e-6f /(2 * 3.141592653589793 * outputCapacitance); // convert from Hz to MHz   
			}
			else
			{
				setTransitFrequency( B * gmTran *1e-6f  /(2 * 3.141592653589793 * outputCapacitance));
				return B * gmTran *1e-6f  /(2 * 3.141592653589793 * outputCapacitance); // convert from Hz to MHz
			}
		}
		else
		{
			const StructRec::StructureNet& outputNetFirstStage = **findOutputNetsFirstStage().begin();

			if(getPartitioningResult().hasThirdStage() && compensationCapacityConnectedBetween(firstStage, getPartitioningResult().getThirdStage())
					&& !hasCompensationCapacity(outputNetFirstStage))
			{
				float p =  0.0000000000001;
				Partitioning::CapacitancePart & compCap = getPartitioningResult().getCompensationCapacitance();
				Partitioning::Component * compCapComp = *getPartitioningResult().getBelongingComponents(compCap).begin();
				float compCapValue = getTwoPortToValueInputMap().find(*compCapComp);
				outputCapacitance =  compCapValue * p;

			}
			else
			{
				outputCapacitance = calculateSimulatedNetCapacitance(outputNetFirstStage);
			}

			if(getCircuitInformation().getCircuitParameter().isFullyDifferential())
			{
				setTransitFrequency( gmTran *1e-6f  /(2 * 3.141592653589793 * outputCapacitance)); 
				return gmTran *1e-6f  /(2 * 3.141592653589793 * outputCapacitance); // convert from Hz to MHz
			}
			else
			{
				setTransitFrequency( gmTran *1e-6f  /(2 * 3.141592653589793 * outputCapacitance));
				logDebug("gmTran: " << gmTran << " outputCapacitance: " << outputCapacitance << " fT: " << gmTran *1e-6f  /(2 * 3.141592653589793 * outputCapacitance));
				return gmTran *1e-6f  /(2 * 3.141592653589793 * outputCapacitance); // convert from Hz to MHz
			}
		}

	}

   float PerformanceCalculation::computeScalingFactorForSymmetricalOTASim()
	{
		Partitioning::TransconductancePart firstStage = getPartitioningResult().getFirstStage();
		Partitioning::LoadPart & loadFirstStage = **firstStage.getLoadPart().begin();
		Partitioning::Component * compFL = nullptr;
		 for(auto & loadComp : getPartitioningResult().getBelongingComponents(loadFirstStage))
		 {
			 const StructRec::StructureNet * gateNet = nullptr;
			if(loadComp->getArray().getStructureName() == StructRec::StructureName("MosfetDiodeArray")
				&& loadComp->getArray().isPartOfCurrentMirror())
			{
				gateNet = &loadComp->getArray().findNet(StructRec::StructurePinType("MosfetDiodeArray", "Drain"));
			}
			else if(loadComp->getArray().isPartOfCurrentMirror())
			{
				gateNet = &loadComp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
			}
			if(gateNet != nullptr)
			{
				for(auto & outputNet : findOutputNetsFirstStage() )
				{
					if(gateNet->getIdentifier() == outputNet->getIdentifier())
					{
						compFL = loadComp;
						break;
					}
				}
			}

			if(compFL != nullptr)
				break;
		 }
		 assert(compFL != nullptr, "There must be one load comp being part of a current mirror connected to the first stage output");

		float widthLoadFirstStage = getTransistorToWidthInputMap().find(*compFL);
		float lengthLoadFirstStage = getTransistorToLengthInputMap().find(*compFL);
		float multiplierLoadFirstStage = getTransistorToMultiplierInputMap().find(*compFL);


		Partitioning::TransconductancePart & secondStage = getPartitioningResult().getPrimarySecondStage();
		Partitioning::Component * compSecondStage =  nullptr;
		for(auto it : getPartitioningResult().getBelongingComponents(secondStage))
		{
			if(it->getArray().findNet(StructRec::StructurePinType(it->getArray().getStructureName().toStr(), "Source")).isSupply())
			{
					compSecondStage = it;
			}
		}
		assert(compSecondStage != nullptr, "There should be a second stage component connect to a supply net!");

		float widthSecondStage = getTransistorToWidthInputMap().find(*compSecondStage);
		float lengthSecondStage = getTransistorToLengthInputMap().find(*compSecondStage);
		float multiplierSecondStage = getTransistorToMultiplierInputMap().find(*compSecondStage);



		float B = widthSecondStage * multiplierSecondStage * lengthLoadFirstStage /(lengthSecondStage * widthLoadFirstStage * multiplierLoadFirstStage);
		return B;
	}



    float PerformanceCalculation::calculateSimulatedNetCapacitance(const StructRec::StructureNet & net) const
	{
    float netCapacitance = 0.0f;

    const std::vector<StructRec::StructurePin*> pins = net.getConnectedPins();

    for (auto &it_pins : pins)
    {
        StructRec::StructurePin &pin = *it_pins;

        
        Partitioning::Component &component = getPartitioningResult().findComponent(pin.getStructure().getIdentifier());

        
        if (pin.getPinType() == StructRec::StructurePinType("CapacitorArray", "Plus") ||
            pin.getPinType() == StructRec::StructurePinType("CapacitorArray", "Minus"))
        {
            
            float capValue = getTwoPortToValueInputMap().find(component);  
            netCapacitance += capValue;  
        }
        else if (pin.getPinType() == StructRec::StructurePinType("MosfetNormalArray", "Gate"))
        {
            
            if (!(hasCompensationCapacity(net) &&
                component.getPart().isTransconductancePart() &&
                getPartitioningResult().getTransconductancePart(component.getArray()).isHigherStage()))
            {
                float cgs = computeGateSourceCapacitanceSim(component);
                float cgd = computeGateDrainCapacitanceSim(component);
                float cgb = computeGateBulkCapacitanceSim(component);
                netCapacitance += cgs + cgd + cgb; 
            }
        }
        else if (pin.getPinType() == StructRec::StructurePinType("MosfetNormalArray", "Drain"))
        {
            
            float cgd = computeGateDrainCapacitanceSim(component);
            float cdb = computeDrainBulkCapacitanceSim(component);
            netCapacitance += cgd + cdb;  
        }
        else if (pin.getPinType() == StructRec::StructurePinType("MosfetNormalArray", "Source") ||
                 pin.getPinType() == StructRec::StructurePinType("MosfetDiodeArray", "Source"))
        {
            
            float cgs = computeGateSourceCapacitanceSim(component);
            float csb = computeSourceBulkCapacitanceSim(component);
            netCapacitance += cgs + csb;  
        }
        else if (pin.getPinType() == StructRec::StructurePinType("MosfetDiodeArray", "Drain"))
        {
            float cgd = computeGateDrainCapacitanceSim(component);
            float cdb = computeDrainBulkCapacitanceSim(component);
            float cgs = computeGateSourceCapacitanceSim(component);
            float cgb = computeGateBulkCapacitanceSim(component);
            netCapacitance += cgd + cdb + cgs + cgb;  
        }
    }

    return netCapacitance;
}


       float PerformanceCalculation::computeGateSourceCapacitanceSim(Partitioning::Component& component) const
	    {
		 float u = getSpace().getScalingFactorMUM();

		 float overlapCapacity;
		 float gateOxideCapacity;
		 if(getTransistorModel() =="SHM")
		 {
			const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(component);
			overlapCapacity = techSpec.getOverlapCapacity();
			gateOxideCapacity = techSpec.getGateOxideCapacity();
		 }
		 else
		 {
			const TechnologieSpecificationEKV & techSpec = getCircuitInformation().getTechnologieSpecificationEKV(component);
			overlapCapacity = techSpec.getOverlapCapacity();
			gateOxideCapacity = techSpec.getGateOxideCapacity();
		}

		 float width = getTransistorToWidthInputMap().find(component);
		 float length = getTransistorToLengthInputMap().find(component);
		 float multiplier = getTransistorToMultiplierInputMap().find(component);

		 float normedWidth = width * u;


		 return 0.66666667 * length * u* width * u * multiplier * gateOxideCapacity + normedWidth * multiplier *overlapCapacity;

	    }

        float PerformanceCalculation::computeGateDrainCapacitanceSim(Partitioning::Component& component) const
	    {
		float u = getSpace().getScalingFactorMUM();

		 float gateOxideCapacity;
		 float lateralDiffusionLength;
		 if(getTransistorModel() =="SHM")
		 {
			const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(component);
			gateOxideCapacity = techSpec.getGateOxideCapacity();
			lateralDiffusionLength = techSpec.getLateralDiffusionLength();
		 }
		 else
		 {
			const TechnologieSpecificationEKV & techSpec = getCircuitInformation().getTechnologieSpecificationEKV(component);
			gateOxideCapacity = techSpec.getGateOxideCapacity();
			lateralDiffusionLength = techSpec.getLateralDiffusionLength();
		}


		float width = getTransistorToWidthInputMap().find(component);
		float multiplier = getTransistorToMultiplierInputMap().find(component);

		float normedWidth = width * u;


		return gateOxideCapacity* lateralDiffusionLength* normedWidth * multiplier;
	    }


    float PerformanceCalculation::computeGateBulkCapacitanceSim(Partitioning::Component& component) const
	 {
		 float u = getSpace().getScalingFactorMUM();


		 float gateOxideCapacity;
		 float lateralDiffusionLength;
		 if(getTransistorModel() =="SHM")
		 {
			const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(component);
			gateOxideCapacity = techSpec.getGateOxideCapacity();
			lateralDiffusionLength = techSpec.getLateralDiffusionLength();
		 }
		 else
		 {
			const TechnologieSpecificationEKV & techSpec = getCircuitInformation().getTechnologieSpecificationEKV(component);
			gateOxideCapacity = techSpec.getGateOxideCapacity();
			lateralDiffusionLength = techSpec.getLateralDiffusionLength();
		}

	 	 float width = getTransistorToWidthInputMap().find(component);
		 float multiplier = getTransistorToMultiplierInputMap().find(component);

	 	 float normedWidth = width * u;


	 	return normedWidth * multiplier * lateralDiffusionLength * gateOxideCapacity;

	 }


	float PerformanceCalculation::computeSourceBulkCapacitanceSim(Partitioning::Component& component) const
	{
		float u = getSpace().getScalingFactorMUM();

		 float zeroBiasBulkJunctionCapacitance;
		 float lateralDiffusionLength;
		 float zeroBiasSidewallBulkJunctionCapacitance;

		 if(getTransistorModel() =="SHM")
		 {
			const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(component);
			zeroBiasBulkJunctionCapacitance = techSpec.getZeroBiasBulkJunctionCapacitance();
			lateralDiffusionLength = techSpec.getLateralDiffusionLength();
			zeroBiasSidewallBulkJunctionCapacitance = techSpec.getZeroBiasSidewallBulkJunctionCapacitance();

		 }
		 else
		 {
			const TechnologieSpecificationEKV & techSpec = getCircuitInformation().getTechnologieSpecificationEKV(component);
			zeroBiasBulkJunctionCapacitance = techSpec.getZeroBiasBulkJunctionCapacitance();
			lateralDiffusionLength = techSpec.getLateralDiffusionLength();
			zeroBiasSidewallBulkJunctionCapacitance = techSpec.getZeroBiasSidewallBulkJunctionCapacitance();

		}

		float width = getTransistorToWidthInputMap().find(component);
		float multiplier = getTransistorToMultiplierInputMap().find(component);

		float normedWidth = width * u;
		return (zeroBiasBulkJunctionCapacitance* lateralDiffusionLength* normedWidth * multiplier + (2 * lateralDiffusionLength + normedWidth * multiplier) * zeroBiasSidewallBulkJunctionCapacitance);
	}


	float PerformanceCalculation::computeDrainBulkCapacitanceSim(Partitioning::Component& component) const
	{

		float u = getSpace().getScalingFactorMUM();


		Core::NetId drainNet = component.getArray().findNet(StructRec::StructurePinType(component.getArray().getIdentifier().getName().toStr(),"Drain")).getIdentifier();
		Core::NetId bulkNet = component.getArray().findNet(StructRec::StructurePinType(component.getArray().getIdentifier().getName().toStr(),"Bulk")).getIdentifier();

		float drainVoltage = static_cast<float>(getSpace().createFloatVoltage(getNetToVoltageMap().find(drainNet)).min());
		float bulkVoltage = static_cast<float>(getSpace().createFloatVoltage(getNetToVoltageMap().find(bulkNet)).min());
		float vdb = abs(drainVoltage - bulkVoltage);

		 float zeroBiasBulkJunctionCapacitance;
		 float lateralDiffusionLength;
		 float zeroBiasSidewallBulkJunctionCapacitance;
		 float bulkJunctionContactPotential;
		 if(getTransistorModel() =="SHM")
		 {
			const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(component);
			zeroBiasBulkJunctionCapacitance = techSpec.getZeroBiasBulkJunctionCapacitance();
			lateralDiffusionLength = techSpec.getLateralDiffusionLength();
			zeroBiasSidewallBulkJunctionCapacitance = techSpec.getZeroBiasSidewallBulkJunctionCapacitance();
			bulkJunctionContactPotential = techSpec.getBulkJunctionContactPotential();
		 }
		 else
		 {
			const TechnologieSpecificationEKV & techSpec = getCircuitInformation().getTechnologieSpecificationEKV(component);
			zeroBiasBulkJunctionCapacitance = techSpec.getZeroBiasBulkJunctionCapacitance();
			lateralDiffusionLength = techSpec.getLateralDiffusionLength();
			zeroBiasSidewallBulkJunctionCapacitance = techSpec.getZeroBiasSidewallBulkJunctionCapacitance();
			bulkJunctionContactPotential = techSpec.getBulkJunctionContactPotential();
		}

		float width = getTransistorToWidthInputMap().find(component);
		float multiplier = getTransistorToMultiplierInputMap().find(component);
		float normedWidth = width * u;
		float vdbDividedByBulkJunctionPotential = vdb / bulkJunctionContactPotential;

		float squareRootOnePlusVddDividedByBulkJunktionPotential = sqrt(1+ vdbDividedByBulkJunctionPotential);
		float oneDividedBySquareRootOnePlusVddDividedByBulkJunktionPotential = 1/squareRootOnePlusVddDividedByBulkJunktionPotential;

		return (zeroBiasBulkJunctionCapacitance* lateralDiffusionLength* normedWidth * multiplier + (2 * lateralDiffusionLength + normedWidth * multiplier) * zeroBiasSidewallBulkJunctionCapacitance )* oneDividedBySquareRootOnePlusVddDividedByBulkJunktionPotential;
	} 


float PerformanceCalculation::calculateTransientSimulatedFrequencyWithErrorFactor()
{
    Partitioning::TransconductancePart &firstStage = getPartitioningResult().getFirstStage();

    float gmTran = 0.0f;

    if (!firstStage.isComplementary()) {
        Partitioning::Component& tranComp = **getPartitioningResult().getBelongingComponents(firstStage).begin();
        gmTran = computeSimulateTransconductance(tranComp);
    } else {
        Partitioning::Component* pmosComp = nullptr;
        Partitioning::Component* nmosComp = nullptr;
        for (auto& comp : getPartitioningResult().getBelongingComponents(firstStage)) {
            if (comp->getArray().getTechType().isP() && pmosComp == nullptr) {
                pmosComp = comp;
            } else if (comp->getArray().getTechType().isN() && nmosComp == nullptr) {
                nmosComp = comp;
            }
        }
        assert(pmosComp != nullptr && nmosComp != nullptr, "In a complementary first stage there should be both, nmos and pmos, devices!");
        float gmPmos = computeSimulateTransconductance(*pmosComp);
        float gmNmos = computeSimulateTransconductance(*nmosComp);
        gmTran = gmPmos + gmNmos;
    }

    float outputCapacitance = 0.0f;
    float errorFactor = calculateSimulatedErrorFactorTransientFrequency();

    if (errorFactor < 0) {
        errorFactor = 0; 
    }
    float maxErrorFactor = 1000000000000000.0f;
    if (errorFactor > maxErrorFactor) {
        errorFactor = maxErrorFactor; // Capping the value to avoid too large values
    }

    // Calculate the transient frequency with error factor
    if (getPartitioningResult().getFirstStage().hasLoadPart() == 1
        && getPartitioningResult().hasSecondarySecondStage()) {

        Partitioning::LoadPart &loadFirstStage = **firstStage.getLoadPart().begin();
        Partitioning::TransconductancePart &secondStage = getPartitioningResult().getPrimarySecondStage();

        outputCapacitance = calculateSimulatedNetCapacitance(findOutputNetWithCapacitor(secondStage));
        float B = computeScalingFactorForSymmetricalOTASim(); 

        if (loadFirstStage.hasCrossCoupledPair(getPartitioningResult())) {
			setTransitFrequencyWithErrorFactor( 2 * B * gmTran * errorFactor *1e-6f / (2 * 3.141592653589793f * outputCapacitance) );
            return 2 * B * gmTran * errorFactor *1e-6f / (2 * 3.141592653589793f * outputCapacitance); // convert from Hz to MHz
        } else {
			setTransitFrequencyWithErrorFactor( B * gmTran * errorFactor *1e-6f / (2 * 3.141592653589793f * outputCapacitance));
            return B * gmTran * errorFactor *1e-6f / (2 * 3.141592653589793f * outputCapacitance); // convert from Hz to MHz
        }
    } else {
        const StructRec::StructureNet& outputNetFirstStage = **findOutputNetsFirstStage().begin();

        if (getPartitioningResult().hasThirdStage() && compensationCapacityConnectedBetween(firstStage, getPartitioningResult().getThirdStage())
            && !hasCompensationCapacity(outputNetFirstStage)) {

            float p = 0.0000000000001f;
            Partitioning::CapacitancePart &compCap = getPartitioningResult().getCompensationCapacitance();
            Partitioning::Component* compCapComp = *getPartitioningResult().getBelongingComponents(compCap).begin();
            float compCapValue = getTwoPortToValueInputMap().find(*compCapComp);
            outputCapacitance = compCapValue * p;

        } else {
            outputCapacitance = calculateSimulatedNetCapacitance(outputNetFirstStage);
        }
    }

    setTransitFrequencyWithErrorFactor( gmTran * errorFactor *1e-6f / (2 * 3.141592653589793f * outputCapacitance));
    return gmTran * errorFactor *1e-6f / (2 * 3.141592653589793f * outputCapacitance); // convert from Hz to MHz
}



   	float PerformanceCalculation::calculateSimulatedErrorFactorTransientFrequency()
{
    float errorFactor = 0.0f;

    if (getCircuitInformation().getCircuitParameter().isFullyDifferential()) {
		
        errorFactor = 0.5f;
    } 
    else if (!getPartitioningResult().hasCompensationCapacitance()) {
		
        errorFactor = 1.0f;
		
		logDebug(errorFactor);
    } 
    else {
		
        assert(getPartitioningResult().hasSecondStage(), "If the circuit has a compensation capacity, it should have at least a second stage!");

        Partitioning::TransconductancePart &firstStage = getPartitioningResult().getFirstStage();
        Partitioning::TransconductancePart &secondStage = getPartitioningResult().getPrimarySecondStage();

        float outputCapacitanceFirstStage = calculateSimulatedNetCapacitance(**findOutputNets(firstStage).begin());
        float outputCapacitanceSecondStage = calculateSimulatedNetCapacitance(findOutputNetWithCapacitor(secondStage));

        float rFT = computeOutputSimulateResistance(firstStage);
        float rST = computeOutputSimulateResistance(secondStage);
        float gmST = computeSimulateTransconductance(secondStage);


        if (outputCapacitanceFirstStage == 0.0f || outputCapacitanceSecondStage == 0.0f || rFT == 0.0f || rST == 0.0f || gmST == 0.0f) {
            logDebug("Error: Invalid zero value in one of the parameters. Exiting calculation.");
            return 0.0f;
        }

        float transconductanceResistanceFactor = 0.0f;

        if (!getPartitioningResult().hasThirdStage()) {
            float capacitiesDivisor = outputCapacitanceSecondStage / outputCapacitanceFirstStage;

            transconductanceResistanceFactor = 1 / (rFT * gmST);

            float summandDifferentFromOne = transconductanceResistanceFactor * (1 + capacitiesDivisor);

            errorFactor = 1 / (1 + summandDifferentFromOne);
        } else {
            Partitioning::TransconductancePart &thirdStage = getPartitioningResult().getThirdStage();
            float outputCapacitanceThirdStage = calculateSimulatedNetCapacitance(**findOutputNets(thirdStage).begin());
            float gmTT = computeSimulateTransconductance(thirdStage);


            if (compensationCapacityConnectedBetween(secondStage, firstStage)) {
                float capacitiesDivisor = outputCapacitanceSecondStage / outputCapacitanceFirstStage;
                transconductanceResistanceFactor = 1 / (rFT * gmST);

                float summandDifferentFromOne = transconductanceResistanceFactor * (1 + capacitiesDivisor);
                errorFactor = 1 / (1 + summandDifferentFromOne);
            } 
            else if (compensationCapacityConnectedBetween(thirdStage, secondStage)) {
                float capacitiesDivisor = outputCapacitanceThirdStage / outputCapacitanceSecondStage;
                transconductanceResistanceFactor = 1 / (rST * gmTT);

                float summandDifferentFromOne = transconductanceResistanceFactor * (1 + capacitiesDivisor);
                errorFactor = 1 / (1 + summandDifferentFromOne);
            } 
            else {
                float capacitiesDivisor = outputCapacitanceThirdStage / outputCapacitanceFirstStage;
                transconductanceResistanceFactor = 1 / (rFT * rST * gmST * gmTT);

                float summandDifferentFromOne = transconductanceResistanceFactor * (1 + capacitiesDivisor);
                errorFactor = 1 / (1 + summandDifferentFromOne);
            }
        }
    }

    return errorFactor;
}




    float PerformanceCalculation::computeOutputSimulateResistance(Partitioning::TransconductancePart & stage)
		{
			float rout = 0.0f;
			if(stage.isFirstStage() || stage.isFeedBack())
			{

				rout = computeOutputSimulateResistanceFirstStage();
			}
			else
			{
				rout = computeOutputResistanceSimulateHigherStage(stage);
			}
			return rout;
		}


   float PerformanceCalculation::calculateSimulatedSlewRate()
   {
    std::vector<float> slewRates;

    if (hasOutputNetWithCapacitor(getPartitioningResult().getFirstStage())) {
        slewRates.push_back(calculateSlewRateFirstStageSim());
    }

    if (getPartitioningResult().hasSecondStage() && hasOutputNetWithCapacitor(getPartitioningResult().getPrimarySecondStage())) {
        slewRates.push_back(calculateSlewRateSecondStageSim());
    }

    if (getPartitioningResult().hasThirdStage() && hasOutputNetWithCapacitor(getPartitioningResult().getThirdStage())) {
        slewRates.push_back(calculateSlewRateSim(getPartitioningResult().getThirdStage()));
    }

    if (getPartitioningResult().hasThirdStage() &&
        compensationCapacityConnectedBetween(getPartitioningResult().getFirstStage(), getPartitioningResult().getThirdStage()) &&
        !hasCompensationCapacity(**findOutputNets(getPartitioningResult().getFirstStage()).begin())) {
        
        float p = 0.0000000000001f;
        Partitioning::CapacitancePart& compCap = getPartitioningResult().getCompensationCapacitance();
        Partitioning::Component* compCapComp = *getPartitioningResult().getBelongingComponents(compCap).begin();
        float compCapValue = getTwoPortToValueInputMap().find(*compCapComp);
        float cc = compCapValue * p;

        Partitioning::BiasPart& biasFirstStage = **getPartitioningResult().getFirstStage().getBiasPart().begin();
        Partitioning::Component& biasComp = **getPartitioningResult().getBelongingComponents(biasFirstStage).begin();
        float biasCurrent = static_cast<float>(getSpace().createFloatCurrent(getTransistorToCurrentMap().find(biasComp)).min());

        slewRates.push_back(std::abs(biasCurrent / cc));
    }

    float slewRate = std::numeric_limits<float>::max();
    for (float sr : slewRates) {
        if (sr < slewRate) {
            slewRate = sr;
        }
    }
    setSlewRate(slewRate * 1e-6f);
    return slewRate * 1e-6f ; // Convert from V/s to V/mum_s
   }
		
	

    float PerformanceCalculation::calculateSlewRateSim(Partitioning::TransconductancePart & stage )
	{
		Partitioning::BiasPart & biasPart = **stage.getBiasPart().begin();
		Partitioning::Component& biasComp = **getPartitioningResult().getBelongingComponents(biasPart).begin();
		float biasCurrent = static_cast<float>(getSpace().createFloatCurrent(getTransistorToCurrentMap().find(biasComp)).min());

		const StructRec::StructureNet & net = findOutputNetWithCapacitor(stage);

		float cout = calculateSimulatedNetCapacitance(net);

		return abs(biasCurrent/cout);

	}


    float PerformanceCalculation::calculateSlewRateFirstStageSim()
    {
    Partitioning::TransconductancePart& firstStage = getPartitioningResult().getFirstStage();

    // Handle the case where the load part has two components
    if (getPartitioningResult().getFirstStage().getLoadPart().size() == 2) {

        if (!firstStage.isComplementary()) {
            // Non-complementary case
            float current = 0.0f;
            Partitioning::BiasPart& biasPart = **firstStage.getBiasPart().begin();
            Partitioning::Component& biasDiffPairComp = **getPartitioningResult().getBelongingComponents(biasPart).begin();
            float biasDiffPairCurrent = static_cast<float>(getSpace().createFloatCurrent(getTransistorToCurrentMap().find(biasDiffPairComp)).min());

            const StructRec::StructureNet& outputNet = findOutputNetWithCapacitor(firstStage);
            float capacitance = calculateSimulatedNetCapacitance(outputNet);

            if (getPartitioningResult().hasBiasOfFoldedPair()) {
                float outputCurrent = static_cast<float>(getSpace().createFloatCurrent(getTransistorToCurrentMap().find(
                    **(getPartitioningResult().findComponents(firstStage.getHelperStructure()).begin()))).min());

                if (getPartitioningResult().hasSlewRateEnhancer() &&
                    (**getPartitioningResult().getAllSlewRateEnhancer().begin())
                        .hasGateNetsConnectedToBiasOfFoldedPair(getPartitioningResult())) {

                    Partitioning::LoadPart* loadPartWithCascodePair = nullptr;
                    for (auto& loadPart : firstStage.getLoadPart()) {
                        if (loadPart->hasCurrentBiasOfFoldedPair()) {
                            loadPartWithCascodePair = loadPart;
                            break;
                        }
                    }
                    assert(loadPartWithCascodePair != nullptr,
                        "There should be a load part with a cascode pair and a bias of it");

                    Partitioning::Component* biasFL1B = nullptr;
                    Partitioning::Component& FL1B = getPartitioningResult().getBiasOfFoldedPair();

                    for (auto& biasPart : loadPartWithCascodePair->getBiasParts()) {
                        for (auto& comp : getPartitioningResult().getBelongingComponents(*biasPart)) {
                            if (comp->getArray().getStructureName().toStr() == "MosfetDiodeArray" &&
                                comp->getArray()
                                    .findNet(StructRec::StructurePinType("MosfetDiodeArray", "Drain"))
                                    .getIdentifier() ==
                                    FL1B.getArray()
                                        .findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"))
                                        .getIdentifier()) {
                                biasFL1B = comp;
                            }
                        }
                    }

                    assert(biasFL1B != nullptr,
                        "The bias of the folded cascode op amp should have an external bias transistor "
                        "otherwise the new slew rate current cannot be calculated");

                    float ids0FL1B = static_cast<float>(getSpace().createFloatCurrent(getTransistorToCurrentMap().find(FL1B)).min());
                    float ids0BiasFL1B = static_cast<float>(getSpace().createFloatCurrent(getTransistorToCurrentMap().find(*biasFL1B)).min());

                    float a = std::abs(ids0FL1B / ids0BiasFL1B);
                    assert(a >= 10 && a <= 50);

                    float idBiasFL1B = std::abs(biasDiffPairCurrent + ids0BiasFL1B) / (1 + a);
                    assert(idBiasFL1B > 0.000005 + 1.2 * std::abs(ids0BiasFL1B));

                    float idFL1B = a * idBiasFL1B;
                    return std::abs(idFL1B) / capacitance;
                } else {
                    current = std::min(std::abs(biasDiffPairCurrent), std::abs(outputCurrent));
                    return std::abs(current) / capacitance;
                }
            } else {
                return std::abs(biasDiffPairCurrent) / capacitance;
            }
        } else {
            // Complementary case
            float current = 0.0f;
            Partitioning::Component* biasDiffPairCompP = nullptr;
            Partitioning::Component* biasDiffPairCompN = nullptr;
            Partitioning::Component* supplyLoadP = nullptr;
            Partitioning::Component* supplyLoadN = nullptr;

            const StructRec::StructureNet& outputNet = **findOutputNets(firstStage).begin();
            float capacitance = calculateSimulatedNetCapacitance(outputNet);

            Partitioning::BiasPart& biasPart1 = **firstStage.getBiasPart().begin();
            Partitioning::BiasPart& biasPart2 = **std::next(firstStage.getBiasPart().begin());
            Partitioning::Component& biasDiffPairComp1 = **getPartitioningResult().getBelongingComponents(biasPart1).begin();
            Partitioning::Component& biasDiffPairComp2 = **getPartitioningResult().getBelongingComponents(biasPart2).begin();

            assert(biasDiffPairComp1.getArray().getTechType() != biasDiffPairComp2.getArray().getTechType(),
                "The two Bias Parts of the first stage should have different tech types!");

            if (biasDiffPairComp1.getArray().getTechType().isP()) {
                biasDiffPairCompP = &biasDiffPairComp1;
                biasDiffPairCompN = &biasDiffPairComp2;
            } else {
                biasDiffPairCompP = &biasDiffPairComp2;
                biasDiffPairCompN = &biasDiffPairComp1;
            }

            float biasDiffPairCurrentN = static_cast<float>(getSpace().createFloatCurrent(getTransistorToCurrentMap().find(*biasDiffPairCompN)).min());
            float biasDiffPairCurrentP = static_cast<float>(getSpace().createFloatCurrent(getTransistorToCurrentMap().find(*biasDiffPairCompP)).min());

            Partitioning::LoadPart& loadPart1 = **firstStage.getLoadPart().begin();
            Partitioning::LoadPart& loadPart2 = **std::next(firstStage.getLoadPart().begin());

            Partitioning::Component* supplyComp1 = nullptr;
            Partitioning::Component* supplyComp2 = nullptr;

            for (auto& comp1 : getPartitioningResult().getBelongingComponents(loadPart1)) {
                if (comp1->getArray().findNet(StructRec::StructurePinType(comp1->getArray().getStructureName(), "Source")).isSupply()) {
                    supplyComp1 = comp1;
                    break;
                }
            }

            for (auto& comp2 : getPartitioningResult().getBelongingComponents(loadPart2)) {
                if (comp2->getArray().findNet(StructRec::StructurePinType(comp2->getArray().getStructureName(), "Source")).isSupply()) {
                    supplyComp2 = comp2;
                    break;
                }
            }

            assert(supplyComp1->getArray().getTechType() != supplyComp2->getArray().getTechType(),
                "The two loads should have different tech types");

            if (supplyComp1->getArray().getTechType().isP()) {
                supplyLoadP = supplyComp1;
                supplyLoadN = supplyComp2;
            } else {
                supplyLoadP = supplyComp2;
                supplyLoadN = supplyComp1;
            }

            float supplyLoadCurrentN = static_cast<float>(getSpace().createFloatCurrent(getTransistorToCurrentMap().find(*supplyLoadN)).min());
            float supplyLoadCurrentP = static_cast<float>(getSpace().createFloatCurrent(getTransistorToCurrentMap().find(*supplyLoadP)).min());

            float biasDiffPairCurrent = std::abs(biasDiffPairCurrentN) + std::abs(biasDiffPairCurrentP);
            float supplyLoadCurrent = std::abs(supplyLoadCurrentN) + std::abs(supplyLoadCurrentP);
            float mixedCurrentN = std::abs(biasDiffPairCurrentN) + std::abs(supplyLoadCurrentN);
            float mixedCurrentP = std::abs(biasDiffPairCurrentP) + std::abs(supplyLoadCurrentP);

            float current1 = std::min(biasDiffPairCurrent, supplyLoadCurrent);
            float current2 = std::min(mixedCurrentP, mixedCurrentN);
            current = std::min(current1, current2);

            return std::abs(current) / capacitance;
        }
    } else {
        return calculateSlewRateSim(firstStage);
    }
   }


   float PerformanceCalculation::calculateSlewRateSecondStageSim()
	{
		Partitioning::TransconductancePart & secondStage = getPartitioningResult().getPrimarySecondStage();
		if((*getPartitioningResult().getFirstStage().getLoadPart().begin())->hasCrossCoupledPair(getPartitioningResult()))
		{
			Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
			Partitioning::BiasPart & FB = **firstStage.getBiasPart().begin();

			float currentFB = static_cast<float>(getSpace().createFloatCurrent(getTransistorToCurrentMap().find(**getPartitioningResult().getBelongingComponents(FB).begin())).min());

			float B = computeScalingFactorForSymmetricalOTASim();
			const StructRec::StructureNet & outputNet = findOutputNetWithCapacitor(secondStage);
			float capacitance = calculateSimulatedNetCapacitance(outputNet);

			return abs(B * currentFB/(capacitance));
		}
		else if(getPartitioningResult().hasSecondarySecondStage())
		{
			Partitioning::BiasPart & biasPart =**(secondStage.getBiasPart().begin());

			float biasCurrent = static_cast<float>(getSpace().createFloatCurrent(getTransistorToCurrentMap().find(**getPartitioningResult().getBelongingComponents(biasPart).begin())).min());
			const StructRec::StructureNet & outputNet = findOutputNetWithCapacitor(secondStage);
			float capacitance = calculateSimulatedNetCapacitance(outputNet);

			return abs(2*biasCurrent/(capacitance));
		}
		else
		{
			return calculateSlewRateSim(secondStage);
		}
	}


	float PerformanceCalculation::calculateSlewRateThirdStageSim()
	{
		return calculateSlewRateSim(getPartitioningResult().getThirdStage());
	}



/*
std::tuple<float, float> PerformanceCalculation::calculateSimulatedOutputVoltageSwing()
{
    float u = getSpace().getScalingFactorMUM();
    float vdd = getCircuitInformation().getCircuitParameter().getSupplyVoltage();
    float vss = getCircuitInformation().getCircuitParameter().getGroundVoltage();

    std::vector<Path> vddToOutputPaths = getGraph().computeVddToOuputPathsWithOnlyDrainSourceEdges();
    std::vector<Path> outputToGroundPaths = getGraph().computeOutputToGroundPathsWithOnlyDrainSourceEdges();

    std::vector<float> voutMaxValues(vddToOutputPaths.size());
    int vddToOutputPathIndex = 0;

    for (auto &it_vddToOutputPath : vddToOutputPaths)
    {
        std::vector<const Edge *> vddToOutputPath = it_vddToOutputPath.getEdges();
        std::vector<float> voltageVddToOutputPath(vddToOutputPath.size());
        float numTran = 0;

        int indexVddToOutputPath = 0;
        for (auto &edge : vddToOutputPath)
        {
            Partitioning::Component &comp = getPartitioningResult().findComponent(edge->getEdgeKey().getStructureId());
            const Edge *gateSourceEdge = nullptr;

            if (comp.getArray().getStructureName().toStr() == "MosfetDiodeArray")
            {
                gateSourceEdge = edge;
            }
            else
            {
                numTran++;
                for (auto &it : getGraph().findToComponentBelongingEdges(comp))
                {
                    if (it->isGateSourceEdge())
                    {
                        gateSourceEdge = it;
                        break;
                    }
                }
            }
            assert(gateSourceEdge != nullptr && "There must be an edge for every component!");

            voltageVddToOutputPath[indexVddToOutputPath] = computeEdgeVoltage(*gateSourceEdge);
            indexVddToOutputPath++;
        }

        Partitioning::Component &edgeComp = getPartitioningResult().findComponent(
            (**it_vddToOutputPath.getEdges().begin()).getEdgeKey().getStructureId());

        if (edgeComp.getArray().isPartOfCurrentMirror())
        {
            for (auto &parent : edgeComp.getArray().getTopmostParents())
            {
                if (parent->getStructureName().toStr() == "MosfetCascodeCurrentMirror")
                {
                    numTran--;
                    break;
                }
            }
        }

        float vthP = getCircuitInformation().getTechnologieSpecificationSHMPmos().getThresholdVoltage();
        float pathVoltageSum = std::accumulate(voltageVddToOutputPath.begin(), voltageVddToOutputPath.end(), 0.0f);
        voutMaxValues[vddToOutputPathIndex] = vdd - numTran * vthP + pathVoltageSum;
        vddToOutputPathIndex++;
    }

    std::vector<float> voutMinValues(outputToGroundPaths.size());
    int outputToGroundPathIndex = 0;

    for (auto &it_outputToGroundPath : outputToGroundPaths)
    {
        std::vector<const Edge *> outputToGroundPath = it_outputToGroundPath.getEdges();
        std::vector<float> voltageOutputToGroundPath(outputToGroundPath.size());
        float numTran = 0;

        int indexOutputToGroundPath = 0;
        for (auto &edge : outputToGroundPath)
        {
            Partitioning::Component &comp = getPartitioningResult().findComponent(edge->getEdgeKey().getStructureId());
            const Edge *gateSourceEdge = nullptr;

            if (comp.getArray().getStructureName().toStr() == "MosfetDiodeArray")
            {
                gateSourceEdge = edge;
            }
            else
            {
                numTran++;
                for (auto &it : getGraph().findToComponentBelongingEdges(comp))
                {
                    if (it->isGateSourceEdge())
                    {
                        gateSourceEdge = it;
                        break;
                    }
                }
            }
            assert(gateSourceEdge != nullptr && "There must be an edge for every component!");

            voltageOutputToGroundPath[indexOutputToGroundPath] = computeEdgeVoltage(*gateSourceEdge);
            indexOutputToGroundPath++;
        }

        Partitioning::Component &edgeComp = getPartitioningResult().findComponent(
            (**it_outputToGroundPath.getEdges().begin()).getEdgeKey().getStructureId());

        if (edgeComp.getArray().isPartOfCurrentMirror())
        {
            for (auto &parent : edgeComp.getArray().getTopmostParents())
            {
                if (parent->getStructureName().toStr() == "MosfetCascodeCurrentMirror")
                {
                    numTran--;
                    break;
                }
            }
        }

        float vthN = getCircuitInformation().getTechnologieSpecificationSHMNmos().getThresholdVoltage();
        float pathVoltageSum = std::accumulate(voltageOutputToGroundPath.begin(), voltageOutputToGroundPath.end(), 0.0f);
        voutMinValues[outputToGroundPathIndex] = vss + numTran * vthN - pathVoltageSum;
        outputToGroundPathIndex++;
    }

    float voutMaxValue = *std::min_element(voutMaxValues.begin(), voutMaxValues.end());
    float voutMinValue = *std::max_element(voutMinValues.begin(), voutMinValues.end());


    return std::make_tuple(voutMinValue, voutMaxValue);
}

	
*/

std::tuple<float, float> PerformanceCalculation::calculateSimulatedOutputVoltageSwing()
{
    float u = getSpace().getScalingFactorMUM();
    float vdd = getCircuitInformation().getCircuitParameter().getSupplyVoltage();
    float vss = getCircuitInformation().getCircuitParameter().getGroundVoltage();

    std::vector<Path> vddToOutputPaths = getGraph().computeVddToOuputPathsWithOnlyDrainSourceEdges();
    std::vector<Path> outputToGroundPaths = getGraph().computeOutputToGroundPathsWithOnlyDrainSourceEdges();

    float maxVout = -std::numeric_limits<float>::infinity();
    float minVout = std::numeric_limits<float>::infinity();

    // VDD to Output Path Analysis
    for (auto &it_vddToOutputPath : vddToOutputPaths)
    {
        std::vector<const Edge *> vddToOutputPath = it_vddToOutputPath.getEdges();
        float numTran = 0;
        float totalVoltage = 0.0;

        for (auto &edge : vddToOutputPath)
        {
            Partitioning::Component &comp = getPartitioningResult().findComponent(edge->getEdgeKey().getStructureId());
            const Edge *gateSourceEdge = nullptr;

            if (comp.getArray().getStructureName().toStr() == "MosfetDiodeArray")
            {
                gateSourceEdge = edge;
            }
            else
            {
                numTran++;
                for (auto &it : getGraph().findToComponentBelongingEdges(comp))
                {
                    if (it->isGateSourceEdge())
                    {
                        gateSourceEdge = it;
                        break;
                    }
                }
            }
            
            assert(gateSourceEdge != nullptr, "There must be an edge for every component!");

            totalVoltage += computeEdgeVoltage(*gateSourceEdge);
        }

        Partitioning::Component &edgeComp = getPartitioningResult().findComponent((**it_vddToOutputPath.getEdges().begin()).getEdgeKey().getStructureId());
        if (edgeComp.getArray().isPartOfCurrentMirror())
        {
            for (auto &parent : edgeComp.getArray().getTopmostParents())
            {
                if (parent->getStructureName().toStr() == "MosfetCascodeCurrentMirror")
                {
                    numTran -= 1;
                    break;
                }
            }
        }

		float vthP;
		if (getTransistorModel() == "SHM")
		{
			vthP = getCircuitInformation().getTechnologieSpecificationSHMPmos().getThresholdVoltage();
		}
		else
		{
			vthP = getCircuitInformation().getTechnologieSpecificationEKVPmos().getThresholdVoltage_LMAX500();
		}
		
		
        float voutMaxValue = vdd - numTran * vthP + totalVoltage;
        maxVout = std::max(maxVout, voutMaxValue);
    }

    // Output to Ground Path Analysis
    for (auto &it_outputToGroundPath : outputToGroundPaths)
    {
        std::vector<const Edge *> outputToGroundPath = it_outputToGroundPath.getEdges();
        float numTran = 0;
        float totalVoltage = 0.0;

        for (auto &edge : outputToGroundPath)
        {
            Partitioning::Component &comp = getPartitioningResult().findComponent(edge->getEdgeKey().getStructureId());
            const Edge *gateSourceEdge = nullptr;

            if (comp.getArray().getStructureName().toStr() == "MosfetDiodeArray")
            {
                gateSourceEdge = edge;
            }
            else
            {
                numTran++;
                for (auto &it : getGraph().findToComponentBelongingEdges(comp))
                {
                    if (it->isGateSourceEdge())
                    {
                        gateSourceEdge = it;
                        break;
                    }
                }
            }
            
            assert(gateSourceEdge != nullptr, "There must be an edge for every component!");

            totalVoltage += computeEdgeVoltage(*gateSourceEdge);
        }

        Partitioning::Component &edgeComp = getPartitioningResult().findComponent((**it_outputToGroundPath.getEdges().begin()).getEdgeKey().getStructureId());
        if (edgeComp.getArray().isPartOfCurrentMirror())
        {
            for (auto &parent : edgeComp.getArray().getTopmostParents())
            {
                if (parent->getStructureName().toStr() == "MosfetCascodeCurrentMirror")
                {
                    numTran -= 1;
                    break;
                }
            }
        }

		float vthN;
		if (getTransistorModel() == "SHM")
		{
			vthN = getCircuitInformation().getTechnologieSpecificationSHMNmos().getThresholdVoltage();
		}
		else
		{
			vthN = getCircuitInformation().getTechnologieSpecificationEKVNmos().getThresholdVoltage_LMAX500();
		}

        float voutMinValue = vss - numTran * vthN + totalVoltage;
        minVout = std::min(minVout, voutMinValue);
    }

    setMaxOutputVoltage(maxVout);
	setMinOutputVoltage(minVout);
    return std::make_tuple(minVout, maxVout);
}






    void PerformanceCalculation::createPolesAndZeros()
	{
		//logDebug("Get poles and zeros");
		SimulatedPolesAndZeros & polesAndZeros = getSpace().getPolesAndZeros();
		//PolesAndZeros & polesAndZeros;
		//logDebug("Initialize poles ande zeros");
		polesAndZeros.initialize();
		//logDebug("Set poles ande zeros");
		polesAndZeros_ = &polesAndZeros;
		logDebug("Dominant pole");
		float dominantPole = calculateDominantPole();
        logDebug(dominantPole);
		polesAndZeros.setDominantePole(dominantPole);

		if(getPartitioningResult().getFirstStage().isComplementary())
		{
			logDebug("Non dominant pole complementary first stage");
			calculateNonDominantPolesComplementaryFirstStage(polesAndZeros);
		}
		else if(getPartitioningResult().getFirstStage().getLoadPart().size() == 2 ||
			(getPartitioningResult().getBelongingComponents(**getPartitioningResult().getFirstStage().getLoadPart().begin()).size() >1	&&
					(!getPartitioningResult().hasSecondStage() || getPartitioningResult().hasSecondarySecondStage())))
		{
			logDebug("Non dominant first stage ");
			float nonDominantPoleFirstStage = calculateNonDominantPoleFirstStage();
			logDebug(nonDominantPoleFirstStage);			
			polesAndZeros.addImportantNonDominantPole(nonDominantPoleFirstStage);
			//setNonDominantPoleFirstStage(nonDominantPoleFirstStage);
		}
		if(getPartitioningResult().hasCompensationCapacitance())
		{
			logDebug("positive zero compensation capacity");
			float positiveZero = calculatePositiveZeroWithCompensationCapacity();
			logDebug(positiveZero);
			polesAndZeros.setPositiveZero(positiveZero);
		}
		if(!getPartitioningResult().hasSecondStage() && getPartitioningResult().getFirstStage().getLoadPart().size() == 1)
		{
			logDebug("Calculate positive zero first stage");
	
			float positiveZero = calculatePositiveZeroFirstStage();

			logDebug(positiveZero );
			polesAndZeros.addImportantZero(positiveZero);

		}
		if(getPartitioningResult().getFirstStage().getLoadPart().size() == 2)
		{
			logDebug("Calculate unimportant second load poles and zeros");
			calculateFirstStageSecondLoadPolesAndZeros(polesAndZeros);
		}
		if(getPartitioningResult().hasSecondStage())
		{
			logDebug("Calculate non dominat pole second stage");
			float nonDominantPoleSecondStage = calculateNonDominantPoleHigherStages(getPartitioningResult().getPrimarySecondStage(), getPartitioningResult().getFirstStage());
			logDebug(nonDominantPoleSecondStage);
			polesAndZeros.addImportantNonDominantPole(nonDominantPoleSecondStage);
			if(getPartitioningResult().hasCompensationCapacitance()
					&& getPartitioningResult().getBelongingComponents(getPartitioningResult().getPrimarySecondStage()).size() == 2)
			{
				logDebug("Calculate additional non dominant pole second stage");
				float additionalNonDominantPoleSecondStage = calculateAdditionalNonDominantPoleHigherStages(getPartitioningResult().getPrimarySecondStage(), getPartitioningResult().getFirstStage());
				logDebug(additionalNonDominantPoleSecondStage);
				polesAndZeros.addImportantNonDominantPole(additionalNonDominantPoleSecondStage);
			}
			logDebug("Calculate higher stages poles and zeros");
			createBiasHigherStagesNonDominantPolesAndZeros(getPartitioningResult().getPrimarySecondStage(),polesAndZeros);

			if(getPartitioningResult().hasSecondarySecondStage())
			{
				if(getPartitioningResult().getBelongingComponents(getPartitioningResult().getPrimarySecondStage()).size() == 1)
				{
					logDebug("Create secondary second stage positive zero");
					float positiveZeroSecondStage = calculatePositiveZeroSecondStage();
					logDebug(positiveZeroSecondStage);
					polesAndZeros.addImportantZero(positiveZeroSecondStage);
				}
			}

		}
		if(getPartitioningResult().hasThirdStage())
		{
			float nonDominantPoleThirdStage = calculateNonDominantPoleHigherStages(getPartitioningResult().getThirdStage(), getPartitioningResult().getPrimarySecondStage());
			logDebug(nonDominantPoleThirdStage);
			logDebug("Create third stage non dominat pole");
			polesAndZeros.addImportantNonDominantPole(nonDominantPoleThirdStage);
			logDebug("Create third stage bias unimportant poles and zeros");
			createBiasHigherStagesNonDominantPolesAndZeros(getPartitioningResult().getThirdStage(), polesAndZeros);


		}
       
		//getSpace().initializePolesAndZeros();

		logDebug("PolesAndZeros");
		logDebug(polesAndZeros.toStr());
		setDominantPole(polesAndZeros.getDominantPole());
		setPositiveZero(polesAndZeros.getPositiveZero());
		setImportantNonDominantPoles(polesAndZeros.getImportantNonDominantPoles());
		setImportantZeros(polesAndZeros.getImportantZeros());
	}



    float PerformanceCalculation::calculateDominantPole()
	{
		float dominantPole;
		float p =  0.0000000000001;

		Core::NetId outputNet = getCircuitInformation().getCircuitParameter().findOutputPinWithBiggestLoadCapacity(getPartitioningResult()).getNetId();
		float Cout = calculateSimulatedNetCapacitance(getStructureRecognitionResult().findStructureCircuit(0).findStructureNet(outputNet));

		if(getPartitioningResult().hasCompensationCapacitance() )
		{
			Partitioning::CapacitancePart & compCap = getPartitioningResult().getCompensationCapacitance();
			Partitioning::Component * compCapComp = *getPartitioningResult().getBelongingComponents(compCap).begin();
			float compCapValue = getTwoPortToValueInputMap().find(*compCapComp);
			//logDebug("compCapValue: " << compCapValue);
			//float normedCompCapValue = compCapValue * p;
			float normedCompCapValue = compCapValue;

			if(compensationCapacityConnectedBetween(getPartitioningResult().getFirstStage(),getPartitioningResult().getPrimarySecondStage() ))
			{
				float gainSecondStage = calculateGainSecondStage();
				float Rout1 = computeOutputSimulateResistanceFirstStage();

				dominantPole  = 1/(2* 3.14159265 * Rout1 * gainSecondStage * normedCompCapValue);

			}
			else if(compensationCapacityConnectedBetween(getPartitioningResult().getPrimarySecondStage(),getPartitioningResult().getThirdStage() ))
			{
				float gainThirdStage = calculateGainThirdStage();
				float Rout2 = computeOutputResistanceSimulateHigherStage(getPartitioningResult().getPrimarySecondStage());

				dominantPole  = 1/(2* 3.14159265 * Rout2 * gainThirdStage * normedCompCapValue);
			}
			else if(compensationCapacityConnectedBetween(getPartitioningResult().getFirstStage(), getPartitioningResult().getThirdStage()))
			{
				float gainThirdStage = calculateGainThirdStage();
				float gainSecondStage = calculateGainSecondStage();
				float Rout1 = computeOutputSimulateResistanceFirstStage();
				float Rout3 = computeOutputResistanceSimulateHigherStage(getPartitioningResult().getThirdStage());

				float summandWithCc =  Rout1 * gainThirdStage *gainSecondStage * normedCompCapValue ;
				float summandWithCout = Rout3 * Cout;


				dominantPole  = 1/(2* 3.14159265 * (summandWithCc+ summandWithCout));

			}
		}
		else if(getPartitioningResult().hasSecondStage())
		{
			float Rout2 = computeOutputResistanceSimulateHigherStage(getPartitioningResult().getPrimarySecondStage());
			dominantPole = 1/(2* 3.14159265 * Rout2  *Cout);

		}
		else
		{
			float Rout1 = computeOutputSimulateResistanceFirstStage();
			dominantPole  = 1/(2* 3.14159265 * Rout1  *Cout);

		}
		return dominantPole;
	}



    void PerformanceCalculation::calculateNonDominantPolesComplementaryFirstStage(SimulatedPolesAndZeros & polesAndZeros)
	{
		Partitioning::TransconductancePart& firstStage = getPartitioningResult().getFirstStage();
		const StructRec::StructureNet & outputNet = **findOutputNets(firstStage).begin();

		for(auto & connectedStructure : outputNet.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Drain")))
		{
			if(getPartitioningResult().getPart(*connectedStructure).isLoadPart())
			{
				Partitioning::Component & comp = getPartitioningResult().findComponent(connectedStructure->getIdentifier());

				float gm = computeSimulateTransconductance(comp);

				const StructRec::StructureNet & sourceNet = connectedStructure->findNet(StructRec::StructurePinType("MosfetNormalArray" ,"Source"));
				float capacitance = calculateSimulatedNetCapacitance(sourceNet);

				float fndp = gm/(2*3.14159265* capacitance);
				polesAndZeros.addImportantNonDominantPole(fndp);
			}
		}
		assert(polesAndZeros.getImportantNonDominantPoles().size() ==2, "A complementary firstStage should have two non dominant poles");
	}




    float  PerformanceCalculation::calculateNonDominantPoleFirstStage()
	{
		Partitioning::TransconductancePart& firstStage = getPartitioningResult().getFirstStage();
		std::vector<Partitioning::Component*> firstStageComps = getPartitioningResult().getBelongingComponents(firstStage);
		const StructRec::StructureNet *drainNet = nullptr;
		for(auto & it : firstStageComps)
		{
			if(!isOutputNetFirstStage(it->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain")).getIdentifier()))
			{
				 drainNet = &it->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));
				break;
			}
		}
		if(drainNet == nullptr)
		{
			drainNet = &(**firstStageComps.begin()).getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));
		}
		std::vector<Partitioning::LoadPart*> loadParts = firstStage.getLoadPart();
		Partitioning::Component * compLoadPart = nullptr;
		if(loadParts.size() == 1)
		{
			for(auto & it : getPartitioningResult().getBelongingComponents((**loadParts.begin())))
			{
				if(it->getArray().findNet(StructRec::StructurePinType(it->getArray().getStructureName().toStr(), "Drain"))== *drainNet)
				{
					compLoadPart = it;
					break;
				}
			}
			assert(compLoadPart != nullptr, "Should have a connection of load and first stage transconductance via drain net!");
		}
		else if(firstStage.hasHelperStructure())
		{
			for(auto & it_loads :loadParts)
			{
				if(it_loads->hasCascodedPair())
				{
					const StructRec::Structure & cascodedPair =it_loads->getCascodedPair();
					compLoadPart = *getPartitioningResult().findComponents(cascodedPair).begin();
					break;
				}
			}
			assert(compLoadPart != nullptr, "One load part should have cascoded pair!");
		}
		else
		{
			for(auto & it_loads :loadParts)
			{
				for(auto & it : getPartitioningResult().getBelongingComponents(*it_loads))
				{
					if(it->getArray().findNet(StructRec::StructurePinType(it->getArray().getStructureName().toStr(), "Drain"))== *drainNet
							&& it->getArray().getStructureName().toStr() == "MosfetDiodeArray")
					{
						compLoadPart = it;
						break;
					}
				}
				if(compLoadPart != nullptr)
				{
					break;
				}
			}
			if(compLoadPart == nullptr)
			{
				for(auto & it_loads :loadParts)
				{
					for(auto & it : getPartitioningResult().getBelongingComponents(*it_loads))
					{
						if(it->getArray().findNet(StructRec::StructurePinType(it->getArray().getStructureName().toStr(), "Drain"))== *drainNet)
						{
							compLoadPart = it;
							break;
						}
					}
					if(compLoadPart != nullptr)
					{
						break;
					}
				}
			}
		}
		//When input is a three stage op amp with compensation capacity between first and third stage and first stage is a folded variant and compensation capacity connected between doffPair and GCC
		if(getPartitioningResult().hasThirdStage() && getPartitioningResult().hasCompensationCapacitance()
				&& compensationCapacityConnectedBetween(getPartitioningResult().getFirstStage(), getPartitioningResult().getThirdStage()) &&
				!hasCompensationCapacity(**findOutputNets(getPartitioningResult().getFirstStage()).begin()))
		{
			float rout1 = computeOutputSimulateResistance(getPartitioningResult().getFirstStage());
			float cout1 = calculateSimulatedNetCapacitance(**findOutputNets(getPartitioningResult().getFirstStage()).begin());
			float rout2 = computeOutputSimulateResistance(getPartitioningResult().getPrimarySecondStage());
			float cout2 = calculateSimulatedNetCapacitance(**findOutputNets(getPartitioningResult().getPrimarySecondStage()).begin());

			Partitioning::CapacitancePart & compCap = getPartitioningResult().getCompensationCapacitance();
			Partitioning::Component * compCapComp = *getPartitioningResult().getBelongingComponents(compCap).begin();
			float compCapValue = getTwoPortToValueInputMap().find(*compCapComp);
			//logDebug("compCapValue: " << compCapValue);
            float p = 0.0000000000001;
			//float normedCompCapValue = compCapValue*p;
			float normedCompCapValue = compCapValue;

			float gmLoad = computeSimulateTransconductance(*compLoadPart);

			float rc1 = rout1 * cout1;

			float rc2 = rout2 * cout2;

			float summandWithCc = (rc1 + rc2 ) * normedCompCapValue;

			float summandWithGcc =  gmLoad * rout1 * rout2 * cout1 * cout2;

			float denominator = rout1 * rout2 * cout1 * cout2 * normedCompCapValue;

			return (summandWithCc + summandWithGcc)/ (denominator);
		}
		else if(getPartitioningResult().hasSecondarySecondStage()
				&& (*loadParts.begin())->hasCrossCoupledPair(getPartitioningResult()))
		{
			Partitioning::LoadPart & loadFirstStage = **firstStage.getLoadPart().begin();

			Partitioning::Component * diodeComp = nullptr;
			Partitioning::Component * normalComp = nullptr;
			for(auto & comp: getPartitioningResult().getBelongingComponents(loadFirstStage))
			{
				if(comp->getArray().getStructureName().toStr() == "MosfetNormalArray")
				{
					normalComp = comp;
				}
				else if(comp->getArray().getStructureName().toStr() == "MosfetDiodeArray")
				{
					diodeComp = comp;
				}
			}
			assert(diodeComp != nullptr && normalComp != nullptr, "There should be a diode connected component and a normal component in the load of the first stage" );

			float gmFL = computeSimulateTransconductance(*diodeComp);
			float gmFLPF = computeSimulateTransconductance(*normalComp);

			float gmLoad = gmFL - gmFLPF;
			float Cdrain = calculateSimulatedNetCapacitance(*drainNet);
			return gmLoad/(2* 3.14159265 * Cdrain);
		}
		else 
		{
			float gmLoad = computeSimulateTransconductance(*compLoadPart);
			float Cdrain = calculateSimulatedNetCapacitance(*drainNet);
			return gmLoad/(2* 3.14159265 * Cdrain);

		}
	}	



    float PerformanceCalculation::calculatePositiveZeroWithCompensationCapacity()
	{
		float p =  0.0000000000001;
		Partitioning::CapacitancePart & compCap = getPartitioningResult().getCompensationCapacitance();

		Partitioning::Component * compCapComp = *getPartitioningResult().getBelongingComponents(compCap).begin();

		float compCapValue = getTwoPortToValueInputMap().find(*compCapComp);
		//logDebug("compCapValue: " << compCapValue);
		//float normedCapValue = compCapValue * p;
		float normedCapValue = compCapValue;

		if(getPartitioningResult().hasCompensationResistor())
		{
			float fpz = calculatePositiveZeroWithCompensationResistance();
			return fpz;
		}
		else if(getPartitioningResult().hasThirdStage() && compensationCapacityConnectedBetween(getPartitioningResult().getFirstStage(), getPartitioningResult().getThirdStage())
				&& !hasCompensationCapacity(**findOutputNets(getPartitioningResult().getFirstStage()).begin()))
		{
			Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
			assert(firstStage.hasHelperStructure(), "If compensation capacity is not connected to  first stage output the first stage should have a golded pair ");

			Partitioning::Component & gcc = **getPartitioningResult().findComponents(firstStage.getHelperStructure()).begin();
			float gmGcc = computeSimulateTransconductance(gcc);

			return 2* gmGcc/(2* 3.14159265 * normedCapValue);
		}
		else
		{
			Partitioning::TransconductancePart * stage = nullptr;

			if(compensationCapacityConnectedBetween(getPartitioningResult().getFirstStage(), getPartitioningResult().getPrimarySecondStage()))
			{
				stage = &getPartitioningResult().getPrimarySecondStage();
			}
			else if(compensationCapacityConnectedBetween(getPartitioningResult().getPrimarySecondStage(), getPartitioningResult().getThirdStage()))
			{
				stage = &getPartitioningResult().getThirdStage();
			}
			Partitioning::Component * transComp = *getPartitioningResult().getBelongingComponents(*stage).begin();
			float gmTrans = computeSimulateTransconductance(*transComp);

			return gmTrans/(2* 3.14159265 * normedCapValue);

		}
	}	



	float PerformanceCalculation::calculatePositiveZeroWithCompensationResistance()
	   {
		float fpz;
		if(getPartitioningResult().hasSecondStage())
		{
			Partitioning::TransconductancePart & ST = **getPartitioningResult().getSecondStages().begin();
			Partitioning::Component * supplyComp = nullptr;
			for(auto & comp : getPartitioningResult().getBelongingComponents(ST))
			{
				if(comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray","Source")).isSupply())
				{
					supplyComp = comp;
				}
			}
			assert(supplyComp != nullptr, "A second stage should have a to supply connected component");

			float gmST = computeSimulateTransconductance(*supplyComp);

			Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
			Partitioning::Component & compFT = **getPartitioningResult().getBelongingComponents(firstStage).begin();
			float gmFT = computeSimulateTransconductance(compFT);

			Partitioning::CapacitancePart & compCapPart = getPartitioningResult().getCompensationCapacitance();
			Partitioning::Component & compCapComp = **getPartitioningResult().getBelongingComponents(compCapPart).begin();
			float capValue = getTwoPortToValueInputMap().find(compCapComp);
	        float p = 0.0000000000001;
			float normedCapValue = capValue*p;


			Partitioning::ResistorPart & resPart = getPartitioningResult().getCompensationResistor();
			Partitioning::Component & resComp = **getPartitioningResult().getBelongingComponents(resPart).begin();
			
			float gdsRc = computeOutputSimulateConductance(resComp);
            float Rc=1/gdsRc;


			float RgmST = 1/gmST;


			float substraction = abs(RgmST - Rc);

			fpz = 1 /(2 * 3.14159265 * normedCapValue * substraction);
		}

		return fpz;
	}


    float PerformanceCalculation::calculatePositiveZeroFirstStage()
	{
		float fndFirstStage = calculateNonDominantPoleFirstStage();
		return 2* fndFirstStage;
	}



    void PerformanceCalculation::calculateFirstStageSecondLoadPolesAndZeros(SimulatedPolesAndZeros & polesAndZeros)
	{
		std::vector<Partitioning::LoadPart*> loadsFirstStage = getPartitioningResult().getFirstStage().getLoadPart();

		Partitioning::Component * outputConnectedComp = nullptr;
		Partitioning::Component * supplyConnectedComp = nullptr;
		if(loadsFirstStage.size() == 2)
		{
			for(auto & load : loadsFirstStage)
			{
				if(!load->hasCascodedPair() && getPartitioningResult().getBelongingComponents(*load).size() > 2)
				{
					for(auto & comp : getPartitioningResult().getBelongingComponents(*load))
					{
						if(isOutputNetFirstStage(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Drain")).getIdentifier()))
						{
							outputConnectedComp = comp;

							const StructRec::StructureNet & sourceNet = outputConnectedComp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Source"));
							for(auto & sourceConnectedStructure : sourceNet.getAllConnectedStructures())
							{
								if(sourceConnectedStructure->findNet(StructRec::StructurePinType(sourceConnectedStructure->getStructureName().toStr(), "Drain")) == sourceNet)
								{
									for(auto & comp2 : getPartitioningResult().getBelongingComponents(*load))
									{
										if(*sourceConnectedStructure == comp2->getArray())
										{
											supplyConnectedComp = comp2;
											break;
										}
									}
								}
							}
							break;
						}
					}
					break;
				}
			}
			if(outputConnectedComp != nullptr && supplyConnectedComp != nullptr)
			{
				float CsourceNet = calculateSimulatedNetCapacitance(outputConnectedComp->getArray().findNet(StructRec::StructurePinType(outputConnectedComp->getArray().getStructureName().toStr(), "Source")));
				float CgateNet = calculateSimulatedNetCapacitance(outputConnectedComp->getArray().findNet(StructRec::StructurePinType(outputConnectedComp->getArray().getStructureName().toStr(), "Gate")));
				float gmOutputConnectedComp = computeSimulateTransconductance(*outputConnectedComp);
				float gmSupplyConnectedComp = computeSimulateTransconductance(*supplyConnectedComp);
				float fndUpper = gmOutputConnectedComp/(2* 3.14159265 * CsourceNet);
				float fndLower = gmSupplyConnectedComp/(2* 3.14159265 * CgateNet);
				float pzUpper = 2 * fndUpper;
				float pzLower = 2 * fndLower;

				polesAndZeros.addUnimportantNonDominantPole(fndUpper);
				polesAndZeros.addUnimportantNonDominantPole(fndLower);
				polesAndZeros.addUnimportantZero(pzUpper);
				polesAndZeros.addUnimportantZero(pzLower);

			}
		}
	}



    float PerformanceCalculation::calculateNonDominantPoleHigherStages(
			Partitioning::TransconductancePart & currentStage, Partitioning::TransconductancePart & previousStage)
	{
		if(getPartitioningResult().hasCompensationCapacitance() && compensationCapacityConnectedBetween(currentStage, previousStage))
		{
			return calculateNonDominatPoleHigherStagesCompensationCapacitance(currentStage);
		}
		else
		{
			Partitioning::TransconductancePart * stage = nullptr;

			if(getPartitioningResult().getSecondStages().size() == 2)
			{
				stage = &getPartitioningResult().getSecondarySecondStage();
			}
			else
			{
				stage = & currentStage;
			}

			const StructRec::StructureNet * poleNet = nullptr;
			Partitioning::Component * poleComp = nullptr;
			if(getPartitioningResult().getBelongingComponents(*stage).size() == 2)
			{
				for(auto & comp : getPartitioningResult().getBelongingComponents(*stage))
				{
					if(!comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Source")).isSupply())
					{
							poleComp = comp;
							poleNet = & poleComp->getArray().findNet(StructRec::StructurePinType(poleComp->getArray().getStructureName().toStr(), "Source"));
							break;
					}
				}
				assert(poleComp != nullptr, "Second Stage should have an not to supply connected transistor!");
			}
			else
			{

					Partitioning::Component & compST = **getPartitioningResult().getBelongingComponents(*stage).begin();
					const StructRec::StructureNet & drainNet = compST.getArray().findNet(StructRec::StructurePinType(compST.getArray().getStructureName().toStr(), "Drain"));

					for( auto & structure : drainNet.getAllConnectedStructures() )
					{
						if(structure->hasPin(StructRec::StructurePinType(structure->getStructureName().toStr(), "Drain"))
								&& structure->findNet(StructRec::StructurePinType(structure->getStructureName().toStr(), "Drain")) == drainNet
								&& structure->getTechType() != compST.getArray().getTechType())
						{
							for(auto & compSB : getPartitioningResult().getBelongingComponents(**stage->getBiasPart().begin()))
							{
								if(compSB->getArray() == *structure)
								{
									poleComp = compSB;
									poleNet = & drainNet;
									break;
								}
							}
						}
					}
					assert(poleComp != nullptr, "Should have a via drain connected bias component");
			}
			float gm = computeSimulateTransconductance(*poleComp);
			float C = calculateSimulatedNetCapacitance(*poleNet);
			return gm/(C*2*3.14159265);
		}
	}



	float PerformanceCalculation::calculateNonDominatPoleHigherStagesCompensationCapacitance(
			 Partitioning::TransconductancePart & stage)
	 {
		 float tenToTheThree = pow (10, 3);
		 float tenToTheFivetheen = pow(10,15);
		 float fento = pow(10, -15);

		 Partitioning::Component * transComp = nullptr;

		for(auto &compStage :getPartitioningResult().getBelongingComponents(stage))
		{
			if(compStage->getArray().findNet(StructRec::StructurePinType(compStage->getArray().getStructureName(), "Source")).isSupply())
			{
				transComp = compStage;
			}
		}
		assert(transComp != nullptr, "Stage should have a supply connected component");

		float gmTrans = computeSimulateTransconductance(*transComp);

		const StructRec::StructureNet * outputNet = nullptr;
		std::vector<const StructRec::StructureNet*> outputNets = findOutputNets(stage);
		for(auto & net : outputNets)
		{
			if(hasCompensationCapacity(*net))
			{
				outputNet = net;
				break;
			}
		}
		assert(outputNet != nullptr);
		const StructRec::StructureNet & gateNet = transComp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"));

		float cout = calculateSimulatedNetCapacitance(*outputNet);
		float fentoCout = cout * tenToTheFivetheen;

		Partitioning::CapacitancePart & compCap = getPartitioningResult().getCompensationCapacitance();
		Partitioning::Component * compCapComp = *getPartitioningResult().getBelongingComponents(compCap).begin();
		float compCapValue = getTwoPortToValueInputMap().find(*compCapComp);

		float cgSecondStage = calculateSimulatedNetCapacitance(gateNet);

		float fentoCompCapValue = compCapValue * tenToTheThree;

		float fentoCgSecondStage = cgSecondStage * tenToTheFivetheen;

		/*float summand1 = fentoCompCapValue * fentoCgSecondStage;
		float summand2 = fentoCout * fentoCgSecondStage;
		float summand3 = fentoCout * fentoCompCapValue;
		float divisor = 2* 3.1415926535* fento *( summand1 +summand2 +summand3);
		return gmTrans  * fentoCompCapValue /(divisor);*/

		logDebug("compCapValue: " << compCapValue << " cgSecondStage: " << cgSecondStage << " cout: " << cout << " gmTrans: " << gmTrans);
		
		float summand1 = compCapValue * cgSecondStage;
		float summand2 = cout * cgSecondStage * 1e-12;
		float summand3 = cout * compCapValue;
		float divisor = 2* 3.1415926535 *( summand1 +summand2 +summand3);
		logDebug("result: " << gmTrans  * compCapValue /(divisor));
		return gmTrans  * compCapValue /(divisor);
	 }



	float PerformanceCalculation::calculateAdditionalNonDominantPoleHigherStages(
			Partitioning::TransconductancePart & currentStage, Partitioning::TransconductancePart & previousStage)
	{
		assert(getPartitioningResult().getBelongingComponents(currentStage).size() == 2, "There exist only an additional non dominant pol in the second stage, if there is a compensation capacity in the circuit");
		const StructRec::StructureNet * poleNet = nullptr;
		Partitioning::Component * poleComp = nullptr;
		for(auto & comp : getPartitioningResult().getBelongingComponents(currentStage))
		{
			const StructRec::StructureNet & drainNet =comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Drain"));
			if(drainNet.findConnectedStructures(StructRec::StructurePinType("CapacitorArray", "Minus")).empty()
				&& drainNet.findConnectedStructures(StructRec::StructurePinType("CapacitorArray", "Plus")).empty())
			{
				poleNet = & drainNet;
			}
			if(!comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Source")).isSupply())
			{
				poleComp = comp;
			}
		}
		assert(poleComp != nullptr, "Second Stage should have an not to supply connected transistor!");
		float gm = computeSimulateTransconductance(*poleComp);
		float C = calculateSimulatedNetCapacitance(*poleNet);
		return gm/(C*2*3.14159265);

	}


	float PerformanceCalculation::calculatePositiveZeroSecondStage()
	{
		float fndpSecondStage = calculateNonDominantPoleHigherStages(getPartitioningResult().getPrimarySecondStage(), getPartitioningResult().getFirstStage());
		return 2 * fndpSecondStage;

	}



    void PerformanceCalculation::createBiasHigherStagesNonDominantPolesAndZeros(Partitioning::TransconductancePart & stage, SimulatedPolesAndZeros &polesAndZeros)
	{
		Partitioning::BiasPart & biasPart = **stage.getBiasPart().begin();
		if(getPartitioningResult().getBelongingComponents(biasPart).size()==2)
		{
			for(auto &biasComp : getPartitioningResult().getBelongingComponents(biasPart))
			{
				if(!biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source")).isSupply())
				{
					const StructRec::StructureNet& sourceNet = biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source"));
					float gm = computeSimulateTransconductance(*biasComp);
					float C = calculateSimulatedNetCapacitance(sourceNet);
					float fnd = gm/(2* 3.14159265 * C);
					float pz = 2* fnd;
					polesAndZeros.addUnimportantNonDominantPole(fnd);
					polesAndZeros.addUnimportantZero(pz);
				}
				else if(stage.isPrimarySecondStage() && getPartitioningResult().getSecondStages().size() == 2
						&& biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source")).isSupply())
				{
					const StructRec::StructureNet* gateNet = nullptr;
					if(biasComp->getArray().getStructureName() == StructRec::StructureName("MosfetDiodeArray"))
					{
						gateNet = &biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Drain"));
					}
					else
					{
						gateNet = &biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Gate"));
					}

					float gm = computeSimulateTransconductance(*biasComp);
					float C = calculateSimulatedNetCapacitance(*gateNet);
					float fnd = gm/(2* 3.14159265 * C);
					float pz =2* fnd;
					polesAndZeros.addUnimportantNonDominantPole(fnd);
					polesAndZeros.addUnimportantZero(pz);
				}
			}
		}

		if(stage.isPrimarySecondStage() && getPartitioningResult().hasSecondarySecondStage())
		{
			Partitioning::BiasPart & biasPartSecondaryStage = **getPartitioningResult().getSecondarySecondStage().getBiasPart().begin();
			if(getPartitioningResult().getBelongingComponents(biasPartSecondaryStage).size()==2)
			{
				for(auto &biasComp : getPartitioningResult().getBelongingComponents(biasPartSecondaryStage))
				{
					if(!biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source")).isSupply())
					{
						const StructRec::StructureNet& sourceNet = biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source"));
						float gm = computeSimulateTransconductance(*biasComp);
						float C = calculateSimulatedNetCapacitance(sourceNet);
						float fnd = gm/(2* 3.14159265 * C);
						float pz =2* fnd;
						polesAndZeros.addUnimportantNonDominantPole(fnd);
						polesAndZeros.addUnimportantZero(pz);
					}
				}
			}
		}
	}




float PerformanceCalculation::calculateSimulatedPhaseMargin() {
    // Initialize phase margin
    float phaseMargin = 0.0f;

    // Get the first-stage component and compute transconductance
    Partitioning::Component &compFT = **getPartitioningResult().getBelongingComponents(
            getPartitioningResult().getFirstStage()).begin();
    float gmFT = computeSimulateTransconductance(compFT);

    // Calculate transient frequency and dominant pole
    float transientFrequency = calculateSimulatedTransientFrequency()*1e6f ;
    float dominantPole = getPolesAndZeros().getDominantPole();

    if (dominantPole <= 0.0f || transientFrequency <= 0.0f) {
        throw std::runtime_error("Invalid transient frequency or dominant pole");
    }

    // Check conditions for important poles and zeros
    for (const auto &fndp : getPolesAndZeros().getImportantNonDominantPoles()) {
        if (fndp <= 0.0f || fndp * 0.1 <= dominantPole) {
			//logDebug("fndp: " << fndp << "dominantPole: " << dominantPole);
            //throw std::runtime_error("Invalid or out-of-range non-dominant pole value");
        }
    }
    for (const auto &fpz : getPolesAndZeros().getImportantZeros()) {
        if (fpz <= 0.0f || fpz * 0.1 <= dominantPole) {
            throw std::runtime_error("Invalid or out-of-range zero value");
        }
    }

    // Vectors to store contributions
    std::vector<float> sign;
    std::vector<float> arcTanValues;

    // Check for compensation capacitance
    if (getPartitioningResult().hasCompensationCapacitance()) {
        size_t size = getPolesAndZeros().getImportantNonDominantPoles().size() +
                      getPolesAndZeros().getImportantZeros().size() + 1;

        sign.resize(size, 0.0f);
        arcTanValues.resize(size, 0.0f);

        int index = 0;

        // Handle non-dominant poles
        for (const auto &fndp : getPolesAndZeros().getImportantNonDominantPoles()) {
            float arcTanFndp = atan(transientFrequency / fndp );
			logDebug("fndp");
			logDebug(fndp);
            sign[index] = 1.0f; // Positive contribution for poles
            arcTanValues[index] = arcTanFndp;
            index++;
        }

        // Handle zeros
        for (const auto &fz : getPolesAndZeros().getImportantZeros()) {
            float arcTanFz = atan(transientFrequency / fz);
			logDebug("fz");
			logDebug(fz);			
            sign[index] = -1.0f; // Negative contribution for zeros
            arcTanValues[index] = arcTanFz;
            index++;
        }

        // Handle positive zero
        float fpz = getPolesAndZeros().getPositiveZero();
        float arcTanFpz = atan(transientFrequency / fpz);
		logDebug("fpz");
		logDebug(fpz);
        sign[index] = 1.0f; // Positive contribution
        arcTanValues[index] = arcTanFpz;
		index++;

		float piHalfMinusPhaseMargin = 0.0f;
    	for (size_t i = 0; i < sign.size(); ++i) {
        	piHalfMinusPhaseMargin += sign[i] * arcTanValues[i];
    	}
		phaseMargin = 3.1415926536 * 0.5f - piHalfMinusPhaseMargin;

    } else {
        size_t size = getPolesAndZeros().getImportantNonDominantPoles().size() +
                      getPolesAndZeros().getImportantZeros().size();

        sign.resize(size, 0.0f);
        arcTanValues.resize(size, 0.0f);

        int index = 0;

        // Handle non-dominant poles
        for (const auto &fndp : getPolesAndZeros().getImportantNonDominantPoles()) {
            float arcTanFndp = atan(transientFrequency / fndp);
			logDebug("fndp");
			logDebug(fndp);		
            sign[index] = 1.0f; // Positive contribution for poles
            arcTanValues[index] = arcTanFndp;
            index++;
        }

        // Handle zeros
        for (const auto &fz : getPolesAndZeros().getImportantZeros()) {
            float arcTanFz = atan(transientFrequency / fz);
			logDebug("fz");
			logDebug(fz);			
            sign[index] = -1.0f; // Negative contribution for zeros
            arcTanValues[index] = arcTanFz;
            index++;
        }
		float piHalfMinusPhaseMargin = 0.0f;
    	for (size_t i = 0; i < sign.size(); ++i) {
        	piHalfMinusPhaseMargin += sign[i] * arcTanValues[i];
    	}
		phaseMargin = 3.1415926536 * 0.5f - piHalfMinusPhaseMargin;
    }

    // Convert radians to degrees for output if necessary
    phaseMargin = phaseMargin * (180.0f / 3.1415926536);
    setPhaseMargin(phaseMargin);
    return phaseMargin;
}



void PerformanceCalculation::computePerformance()
{
        logDebug("      CreatePolesAndZeros");
		createPolesAndZeros();

		logDebug("      Compute Intermediate Performance");
		computeIntermediatePerformance();

		logDebug("calculate simulated gain");
		float Simulatedgain = calculateGainSimulation();
		logDebug("SIMULATEDGAIN");
		logDebug(Simulatedgain);


		logDebug("Simulate powerconsumption");
		float calculatedpower= calculateSimulatedPowerConsumption();
		logDebug("SIMULATEDPOWER");
		logDebug(calculatedpower);

        logDebug("      Calculate Simulated TransientFrequency ");
		float SimulatedtransitFrequency = calculateSimulatedTransientFrequency();
		logDebug(" ###Simulated TransientFrequency### ");
        logDebug(SimulatedtransitFrequency);

        logDebug("      Calculate Simulated TransientFrequency with error factor ");
		float SimulatedtransitFrequencywitherrorfactor = calculateTransientSimulatedFrequencyWithErrorFactor();
		logDebug(" ###Simulated TransientFrequency with error factor### ");
        logDebug(SimulatedtransitFrequencywitherrorfactor);		

		// change: added area calculate
		int calculatedInputArea= calculateInputArea();
		logDebug("calculatedInputArea: ");
		logDebug(calculatedInputArea);


		logDebug("      Calculate Simulated phasemargin");
		float phaseMarginSim = calculateSimulatedPhaseMargin();
		logDebug("phasemargin");
		logDebug(phaseMarginSim);			


		logDebug(" Calculate Simulated SlewRate ");
 		float simulatedslewRate = calculateSimulatedSlewRate();
		logDebug(" ####Simulated SlewRate#### ");
		logDebug(simulatedslewRate);		


        logDebug("     Calculate simulated CommonModeInputVoltage");
        float vcmMinSim, vcmMaxSim;
        std::tie(vcmMinSim, vcmMaxSim) = calculateSimulatedCommonModeInputVoltage();

        logDebug("     Simulated CommonModeInputVoltage: vcmMin = " + std::to_string(vcmMinSim) + 
         ", vcmMax = " + std::to_string(vcmMaxSim));



		logDebug("     calculateSimulatedCMRR");
		float SIMCMRR = calculateSimulatedCMRR();
		logDebug(" ##SimulatedCMRR##");
		logDebug(SIMCMRR);



        logDebug("     Calculate simulated OutputVoltageSwing");
        float voutMinSim, voutMaxSim;
        std::tie(voutMinSim, voutMaxSim) = calculateSimulatedOutputVoltageSwing();

        logDebug("     Simulated OutputVoltageSwing: voutMinSim = " + std::to_string(voutMinSim) + 
         ", voutMaxSim = " + std::to_string(voutMaxSim));



		logDebug("     Calculate Simulated PSRR ");
		float negPSRRSim, posPSRRSim;
		std::tie(negPSRRSim, posPSRRSim) = calculateSimulatedPSRR();
        logDebug("     Simulated PSRR: negPSRRSim = " + std::to_string(negPSRRSim) + 
         ", posPSRRSim = " + std::to_string(posPSRRSim));		

}


void PerformanceCalculation::computeIntermediatePerformance()
{
	logDebug("CalculateNetCapacitances");
    setNetCapacitances(CalculateNetCapacitances());

    std::unordered_map<std::string, float> uniqueNetCapacitances;

    for (const auto& [netId, capacitance] : getNetcapacitances()) {
       uniqueNetCapacitances[netId.toStr()] = capacitance;
    }

    logDebug("Net Capacitances in p_F:");
    for (const auto& [netIdStr, capacitance] : uniqueNetCapacitances) {
       logDebug("Net ID: " + netIdStr + ", Capacitance: " + std::to_string(capacitance) + " pF");
    }

   logDebug("Calculate Output Resistance in Ω" );
   if(getPartitioningResult().hasFirstStage())
   {
        float rout1 = computeOutputSimulateResistance(getPartitioningResult().getFirstStage());

		setOutputResistanceFirstStage(rout1);

		logDebug("Output Resistance First Stage");
		logDebug(std::to_string(getOutputResistanceFirstStage()) + " Ω");
   }
   if(getPartitioningResult().hasPrimarySecondStage())
	{
        float rout2_1 = computeOutputSimulateResistance(getPartitioningResult().getPrimarySecondStage());

		setOutputResistancePrimarySecondStage(rout2_1);

		logDebug("Output Resistance Primary Second Stage");
		logDebug(std::to_string(getOutputResistancePrimarySecondStage()) + " Ω" );		
	}

	if(getPartitioningResult().hasSecondarySecondStage())
	{
        float rout2_2 = computeOutputSimulateResistance(getPartitioningResult().getSecondarySecondStage());

		setOutputResistanceSecondarySecondStage(rout2_2);

		logDebug("Output Resistance Secondary Second Stage");
		logDebug(std::to_string(getOutputResistanceSecondarySecondStage()) + " Ω" );		
	}

   if(getPartitioningResult().hasThirdStage())
	{
        float rout3 = computeOutputSimulateResistance(getPartitioningResult().getThirdStage());

		setOutputResistanceThirdStage(rout3);

		logDebug("Output Resistance Third Stage");
		logDebug(std::to_string(getOutputResistanceThirdStage()) + " Ω" );		
	}
   
   logDebug("Calculate Stage Gain" );

   float gainFirstStage = calculateGainFirstStage();
   
   setGainFirstStage(gainFirstStage);
   logDebug("Gain First Stage");
   logDebug(getGainFirstStage());
 
   float gainSecondStage = calculateGainSecondStage();
   setGainSecondStage(gainSecondStage);
   logDebug("Gain Second Stage");
   logDebug(getGainSecondStage());

   float gainThirdStage = calculateGainThirdStage();
   setGainThirdStage(gainThirdStage);

   logDebug("Gain Third Stage");
   logDebug(getGainThirdStage());
   

   logDebug("Calculate components and Stages Transconductances in A/V");

   if(getPartitioningResult().hasFirstStage())
   {

	Partitioning::TransconductancePart firstStage = getPartitioningResult().getFirstStage();
	float firstStageTransconductance = computeSimulateTransconductance(firstStage);

	setFirstStageTransconductance(firstStageTransconductance);
	setFirstStageComponentTransconductancesArray(CalculateComponentTransconductancesFirstStage());

	const auto& componentsArray = getFirstStageComponentTransconductancesArray();
    for (const auto& [componentName, transconductance] : componentsArray) {
        logDebug("First Stage Component: " + componentName);
        logDebug("Transconductance : " + std::to_string(transconductance) + " A/V");
    }

	logDebug("First Stage Transconductance");
	logDebug(std::to_string(getFirstStageTransconductance()) + " A/V");


   }
   
   if(getPartitioningResult().hasPrimarySecondStage())
   {

	Partitioning::TransconductancePart PrimarySecondStage = getPartitioningResult().getPrimarySecondStage();
	float PrimarySecondStageTransconductance = computeSimulateTransconductance(PrimarySecondStage);
 
	setPrimarySecondStageTransconductance(PrimarySecondStageTransconductance);	
	setPrimarySecondStageComponentTransconductancesArray(CalculateComponentTransconductancesPrimarySecondStage());

	const auto& componentsArray = getPrimarySecondStageComponentTransconductancesArray();
    for (const auto& [componentName, transconductance] : componentsArray) {
        logDebug("Primary Second Stage Component: " + componentName);
        logDebug("Transconductance: " + std::to_string(transconductance) + " A/V");
    } 

	logDebug("Primary Second Stage Transconductance");
	logDebug(std::to_string(getPrimarySecondStageTransconductance()) + " A/V");



   }


   if(getPartitioningResult().hasSecondarySecondStage())
   {

	Partitioning::TransconductancePart SecondarySecondStage = getPartitioningResult().getSecondarySecondStage();
	float SecondarySecondStageTransconductance = computeSimulateTransconductance(SecondarySecondStage);

	setSecondarySecondStageTransconductance(SecondarySecondStageTransconductance);	
	setSecondarySecondStageComponentTransconductancesArray(CalculateComponentTransconductancesSecondarySecondStage());

	const auto& componentsArray = getSecondarySecondStageComponentTransconductancesArray();
    for (const auto& [componentName, transconductance] : componentsArray) {
        logDebug("Secondary Second Stage Component: " + componentName);
        logDebug("Transconductance: " + std::to_string(transconductance) + " A/V");
    } 

	logDebug("Secondary Second Stage Transconductance");
	logDebug(std::to_string(getSecondarySecondStageTransconductance()) + " A/V");



   }


   if(getPartitioningResult().hasThirdStage())
   {

	Partitioning::TransconductancePart ThirdStage = getPartitioningResult().getThirdStage();
	float ThirdStageTransconductance = computeSimulateTransconductance(ThirdStage);

	setThirdStageTransconductance(ThirdStageTransconductance);
	setThirdStageComponentTransconductancesArray(CalculateComponentTransconductancesThirdStage());

	const auto& componentsArray = getThirdStageComponentTransconductancesArray();
    for (const auto& [componentName, transconductance] : componentsArray) {
        logDebug("Third Stage Component: " + componentName);
        logDebug("Transconductance: " + std::to_string(transconductance) + " A/V");
    }
	logDebug("Third Stage Transconductance");
	logDebug(std::to_string(getThirdStageTransconductance()) + " A/V");


   }

   logDebug("Calculate components and Stages Outputconductances in A/V");

   if(getPartitioningResult().hasFirstStage())
   {

	Partitioning::TransconductancePart firstStage = getPartitioningResult().getFirstStage();
	if (firstStage.hasBiasPart() && firstStage.hasLoadPart())
	{
	std::vector<Partitioning::LoadPart *> loadParts = firstStage.getLoadPart();
	std::vector<Partitioning::BiasPart *> biasParts = firstStage.getBiasPart();

	
	float gdLoad = computeOutputSimulateConductance(**loadParts.begin(), firstStage);
	setOutputConductanceLoadPartFirstStage(gdLoad);
	logDebug ("Output Conductance loadpart first stage");
	logDebug(std::to_string(getOutputConductanceLoadPartFirstStage()) + " A/V");
	

	float gdBias = computeOutputSimulateConductance(**biasParts.begin(), firstStage);
	setOutputConductanceBiasPartFirstStage(gdBias);
	logDebug ("Output Conductance biasPart first stage");
	logDebug(std::to_string(getOutputConductanceBiasPartFirstStage()) + " A/V");


	}	

	setComponentOutputConductancesArrayFirstStage(CalculateComponentOutputConductanceArrayFirstStage());

    const auto& componentsArray = getComponentOutputConductancesArrayFirstStage();
    for (const auto& [componentName, outputconductance] : componentsArray) {
        logDebug("First Stage Component: " + componentName);
        logDebug("Output Conductance: " + std::to_string(outputconductance) + " A/V");
    }

   }
   
   if(getPartitioningResult().hasPrimarySecondStage())
   {

	Partitioning::TransconductancePart PrimarySecondStage = getPartitioningResult().getPrimarySecondStage();
	if (PrimarySecondStage.hasBiasPart() && PrimarySecondStage.hasLoadPart())
	{
	std::vector<Partitioning::LoadPart *> loadParts = PrimarySecondStage.getLoadPart();
	std::vector<Partitioning::BiasPart *> biasParts = PrimarySecondStage.getBiasPart();

	float gdLoad = computeOutputSimulateConductance(**loadParts.begin(), PrimarySecondStage);
	setOutputConductanceLoadPartPrimarySecondStage(gdLoad);
	logDebug ("Output Conductance loadpart Primary Second Stage");
	logDebug(std::to_string(getOutputConductanceLoadPartPrimarySecondStage()) + " A/V");

	float gdBias = computeOutputSimulateConductance(**biasParts.begin(), PrimarySecondStage);
	setOutputConductanceBiasPartPrimarySecondStage(gdBias);
	logDebug ("Output Conductance biasPart Primary Second Stage");
	logDebug(std::to_string(getOutputConductanceBiasPartPrimarySecondStage()) + " A/V");	

    }
    
	setComponentOutputConductancesArrayPrimarySecondStage(CalculateComponentOutputConductanceArrayPrimarySecondStage());

	const auto& componentsArray = getComponentOutputConductancesArrayPrimarySecondStage();
    for (const auto& [componentName, outputconductance] : componentsArray) {
        logDebug("Primary Second Stage Component: " + componentName);
        logDebug("Output Conductance: " + std::to_string(outputconductance) + " A/V");
    }

   }


   if(getPartitioningResult().hasSecondarySecondStage())
   {

	Partitioning::TransconductancePart SecondarySecondStage = getPartitioningResult().getSecondarySecondStage();
	if (SecondarySecondStage.hasBiasPart() && SecondarySecondStage.hasLoadPart())
	{
	std::vector<Partitioning::LoadPart *> loadParts = SecondarySecondStage.getLoadPart();
	std::vector<Partitioning::BiasPart *> biasParts = SecondarySecondStage.getBiasPart();

	float gdLoad = computeOutputSimulateConductance(**loadParts.begin(), SecondarySecondStage);
	setOutputConductanceLoadPartSecondarySecondStage(gdLoad);
	logDebug ("Output Conductance loadpart Secondary Second Stage");
	logDebug(std::to_string(getOutputConductanceLoadPartSecondarySecondStage()) + " A/V");
   

	float gdBias = computeOutputSimulateConductance(**biasParts.begin(), SecondarySecondStage);
	setOutputConductanceBiasPartSecondarySecondStage(gdBias);
	logDebug ("Output Conductance biasPart Secondary Second Stage");
	logDebug(std::to_string(getOutputConductanceBiasPartSecondarySecondStage()) + " A/V");

	 	
    }

	setComponentOutputConductancesArraySecondarySecondStage(CalculateComponentOutputConductanceArraySecondarySecondStage());

	const auto& componentsArray = getComponentOutputConductancesArraySecondarySecondStage();
    for (const auto& [componentName, outputconductance] : componentsArray) {
        logDebug("Secondary Second Stage Component: " + componentName);
        logDebug("Output Conductance: " + std::to_string(outputconductance) + " A/V");
    }	

   }


   if(getPartitioningResult().hasThirdStage())
   {

	Partitioning::TransconductancePart ThirdStage = getPartitioningResult().getThirdStage();
	if (ThirdStage.hasBiasPart() && ThirdStage.hasLoadPart())
	{	
	std::vector<Partitioning::LoadPart *> loadParts = ThirdStage.getLoadPart();
	std::vector<Partitioning::BiasPart *> biasParts = ThirdStage.getBiasPart();

	float gdLoad = computeOutputSimulateConductance(**loadParts.begin(), ThirdStage);
	setOutputConductanceLoadPartThirdStage(gdLoad);
	logDebug ("Output Conductance loadpart Third Stage");
	logDebug(std::to_string(getOutputConductanceLoadPartThirdStage()) + " A/V");


	float gdBias = computeOutputSimulateConductance(**biasParts.begin(), ThirdStage);
	setOutputConductanceBiasPartThirdStage(gdBias);
	logDebug ("Output Conductance biasPart Third Stage");
	logDebug(std::to_string(getOutputConductanceBiasPartThirdStage()) + " A/V");	

    }

	setComponentOutputConductancesArrayThirdStage(CalculateComponentOutputConductanceArrayThirdStage());

	const auto& componentsArray = getComponentOutputConductancesArrayThirdStage();
    for (const auto& [componentName, outputconductance] : componentsArray) {
        logDebug("Third Stage Component: " + componentName);
        logDebug("Output Conductance: " + std::to_string(outputconductance) + " A/V");
    }	

   }


}


std::vector<std::pair<Core::NetId, float>> PerformanceCalculation::CalculateNetCapacitances() {
    std::unordered_map<std::string, float> netCapacitanceCache;
    std::unordered_set<std::string> processedNetIds;
    std::vector<std::pair<Core::NetId, float>> netCapacitances;

    for (auto& node : getGraph().getAllNodes()) {
        Core::NetId netId = node->getNetId();

        for (const auto& [key, circuit] : getStructureRecognitionResult().getCircuitMap()) {
            std::cout << "Processing Circuit Key: " << key << std::endl;

            if (circuit) {
                if (circuit->hasStructureNet(netId)) {
                    StructRec::StructureNet& structureNet = circuit->findStructureNet(netId);
                    std::string netIdentifier = structureNet.toStr();

                    if (processedNetIds.find(netIdentifier) != processedNetIds.end()) {
                        std::cout << "Net " << netIdentifier << " is already processed. Skipping." << std::endl;
                        continue;
                    }

                    if (netCapacitanceCache.find(netIdentifier) == netCapacitanceCache.end()) {
                        float capacitance = PerformanceCalculation::calculateSimulatedNetCapacitance(structureNet);
                        capacitance *= 1e12; // Convert to picofarads (pF)
                        netCapacitanceCache[netIdentifier] = capacitance;

                        std::cout << "Net capacitance calculated for " << netIdentifier << ": " << capacitance << " pF" << std::endl;
                        netCapacitances.emplace_back(netId, capacitance);
                    } else {
                        float cachedCapacitance = netCapacitanceCache[netIdentifier];
                        std::cout << "Net capacitance retrieved from cache for " << netIdentifier << ": " << cachedCapacitance << " pF" << std::endl;
                        netCapacitances.emplace_back(netId, cachedCapacitance);
                    }

                    processedNetIds.insert(netIdentifier);
                } else {
                    std::cout << "Net with ID " << netId << " not found in Circuit " << key << std::endl;
                }
            } else {
                std::cout << "Circuit is nullptr for Circuit key " << key << std::endl;
            }
        }
    }

    return netCapacitances;
}


std::vector<std::pair<std::string, float>> PerformanceCalculation::CalculateComponentTransconductancesFirstStage() {
    std::vector<std::pair<std::string, float>> componentTransconductanceArray;

    if (getPartitioningResult().hasFirstStage()) {
        Partitioning::TransconductancePart firstStage = getPartitioningResult().getFirstStage();
        std::vector<Partitioning::Component*> comps = getPartitioningResult().getBelongingComponents(firstStage);

        for (auto& comp : comps) {
            float compTransconductance = computeSimulateTransconductance(*comp);
            std::string compname = comp->toStr();

            //logDebug("First Stage transconductance of component " + compname);
            //logDebug(compTransconductance);

            componentTransconductanceArray.emplace_back(compname, compTransconductance);
        }
    }

    return componentTransconductanceArray;
}



std::vector<std::pair<std::string, float>> PerformanceCalculation::CalculateComponentTransconductancesPrimarySecondStage() {
    std::vector<std::pair<std::string, float>> componentTransconductanceArray;

    if (getPartitioningResult().hasPrimarySecondStage()) {
        Partitioning::TransconductancePart PrimarySecondStage = getPartitioningResult().getPrimarySecondStage();
        std::vector<Partitioning::Component*> comps = getPartitioningResult().getBelongingComponents(PrimarySecondStage);

        for (auto& comp : comps) {
            float compTransconductance = computeSimulateTransconductance(*comp);
            std::string compname = comp->toStr();

            //logDebug("Primary Second Stage transconductance of component " + compname);
            //logDebug(compTransconductance);

            componentTransconductanceArray.emplace_back(compname, compTransconductance);
        }
    }

    return componentTransconductanceArray;
}



std::vector<std::pair<std::string, float>> PerformanceCalculation::CalculateComponentTransconductancesSecondarySecondStage() {
    std::vector<std::pair<std::string, float>> componentTransconductanceArray;

    if (getPartitioningResult().hasSecondarySecondStage()) {
        Partitioning::TransconductancePart SecondarySecondStage = getPartitioningResult().getSecondarySecondStage();
        std::vector<Partitioning::Component*> comps = getPartitioningResult().getBelongingComponents(SecondarySecondStage);

        for (auto& comp : comps) {
            float compTransconductance = computeSimulateTransconductance(*comp);
            std::string compname = comp->toStr();

            //logDebug("Secondary Second Stage transconductance of component " + compname);
            //logDebug(compTransconductance);

            componentTransconductanceArray.emplace_back(compname, compTransconductance);
        }
    }

    return componentTransconductanceArray;
}

std::vector<std::pair<std::string, float>> PerformanceCalculation::CalculateComponentTransconductancesThirdStage() {
    std::vector<std::pair<std::string, float>> componentTransconductanceArray;

    if (getPartitioningResult().hasThirdStage()) {
        Partitioning::TransconductancePart ThirdStage = getPartitioningResult().getThirdStage();
        std::vector<Partitioning::Component*> comps = getPartitioningResult().getBelongingComponents(ThirdStage);

        for (auto& comp : comps) {
            float compTransconductance = computeSimulateTransconductance(*comp);
            std::string compname = comp->toStr();

            //logDebug("Third Stage transconductance of component " + compname);
            //logDebug(compTransconductance);

            componentTransconductanceArray.emplace_back(compname, compTransconductance);
        }
    }

    return componentTransconductanceArray;
}

std::vector<std::pair<std::string, float>> PerformanceCalculation::CalculateComponentOutputConductanceArrayFirstStage() {
    std::vector<std::pair<std::string, float>> componentOutputConductanceArray;

    if (getPartitioningResult().hasFirstStage()) {
        Partitioning::TransconductancePart firstStage = getPartitioningResult().getFirstStage();
        std::vector<Partitioning::Component*> comps = getPartitioningResult().getBelongingComponents(firstStage);

        for (auto& comp : comps) {
            float compOutputconductance = computeOutputSimulateConductance(*comp);
            std::string compname = comp->toStr();

            //logDebug("First Stage output conductance of component " + compname);
            //logDebug(compOutputconductance);

            componentOutputConductanceArray.emplace_back(compname, compOutputconductance);
        }
    }

    return componentOutputConductanceArray;
}



std::vector<std::pair<std::string, float>> PerformanceCalculation::CalculateComponentOutputConductanceArrayPrimarySecondStage() {
    std::vector<std::pair<std::string, float>> componentOutputConductanceArray;

    if (getPartitioningResult().hasPrimarySecondStage()) {
        Partitioning::TransconductancePart PrimarySecondStage = getPartitioningResult().getPrimarySecondStage();
        std::vector<Partitioning::Component*> comps = getPartitioningResult().getBelongingComponents(PrimarySecondStage);

        for (auto& comp : comps) {
            float compOutputconductance = computeOutputSimulateConductance(*comp);
            std::string compname = comp->toStr();

            //logDebug("Primary Second Stage output conductance of component " + compname);
            //logDebug(compOutputconductance);

            componentOutputConductanceArray.emplace_back(compname, compOutputconductance);
        }
    }

    return componentOutputConductanceArray;
}



std::vector<std::pair<std::string, float>> PerformanceCalculation::CalculateComponentOutputConductanceArraySecondarySecondStage() {
    std::vector<std::pair<std::string, float>> componentOutputConductanceArray;

    if (getPartitioningResult().hasSecondarySecondStage()) {
        Partitioning::TransconductancePart SecondarySecondStage = getPartitioningResult().getSecondarySecondStage();
        std::vector<Partitioning::Component*> comps = getPartitioningResult().getBelongingComponents(SecondarySecondStage);

        for (auto& comp : comps) {
            float compOutputconductance = computeOutputSimulateConductance(*comp);
            std::string compname = comp->toStr();

            //logDebug("Secondary Second Stage output conductance of component " + compname);
            //logDebug(compOutputconductance);

            componentOutputConductanceArray.emplace_back(compname, compOutputconductance);
        }
    }

    return componentOutputConductanceArray;
}

std::vector<std::pair<std::string, float>> PerformanceCalculation::CalculateComponentOutputConductanceArrayThirdStage() {
    std::vector<std::pair<std::string, float>> componentOutputConductanceArray;

    if (getPartitioningResult().hasThirdStage()) {
        Partitioning::TransconductancePart ThirdStage = getPartitioningResult().getThirdStage();
        std::vector<Partitioning::Component*> comps = getPartitioningResult().getBelongingComponents(ThirdStage);

        for (auto& comp : comps) {
            float compOutputconductance = computeOutputSimulateConductance(*comp);
            std::string compname = comp->toStr();

            //logDebug("Third Stage output conductance of component " + compname);
            //logDebug(compOutputconductance);

            componentOutputConductanceArray.emplace_back(compname, compOutputconductance);
        }
    }

    return componentOutputConductanceArray;
}
 }

