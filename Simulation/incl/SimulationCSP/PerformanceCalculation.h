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


#ifndef SIMULATION_INCL_SimulationCSP_CONSTRAINTS_PerformanceCalculation_H_
#define SIMULATION_INCL_SimulationCSP_CONSTRAINTS_PerformanceCalculation_H_

#include <gecode/float.hh>

#include "Partitioning/incl/Results/Component.h"
#include "StructRec/incl/StructureCircuit/Structure/Structure.h"
#include "Simulation/incl/SimulationCSP/Constraints/SimulatedPolesAndZeros.h"

namespace StructRec {

	 class StructureCircuits;
}


namespace Partitioning {

	class Result;
	class LoadPart;
	class BiasPart;
	class TransconductancePart;

}

namespace Simulation {


	class CircuitInformation;
	class TechnologieSpecification;
	class Graph;
	class NetToIntVarMap;
	class ComponentToIntVarMap;
	class SimulationSpace;
	class Path;
	class Edge;
	class SimulatedPolesAndZeros;

	class PerformanceCalculation
	{
	public:
		PerformanceCalculation();

		void setPartitioningResult(const Partitioning::Result & result);
		void setStructureRecognitionResult(const StructRec::StructureCircuits & circuits);
		void setGraph(const Graph & graph);
		void setCircuitInformation(const CircuitInformation & information);

		void setTwoPortToValueInputMap(ComponentToIntVarInputMap & valueMap );

		void setTransistorToWidthInputMap(ComponentToIntVarInputMap & widthMap );
		void setTransistorToMultiplierInputMap(ComponentToIntVarInputMap & multiplierMap );
		void setTransistorToLengthInputMap(ComponentToIntVarInputMap & lenghtMap);	
	
		void setTransistorToCurrentMap(ComponentToIntVarMap & currentMap);
		void setNetToVoltageMap(NetToIntVarMap & voltageMap);

		void setSpace(SimulationSpace & space);
		void setTransistorModel(std::string model);
		void setEKVVersion(int version);


		void computePerformance();
		void computeIntermediatePerformance();
		std::vector<std::pair<Core::NetId, float>> CalculateNetCapacitances();
		std::vector<std::pair<std::string, float>> CalculateComponentTransconductancesFirstStage();
		std::vector<std::pair<std::string, float>> CalculateComponentTransconductancesPrimarySecondStage();
		std::vector<std::pair<std::string, float>> CalculateComponentTransconductancesSecondarySecondStage();
		std::vector<std::pair<std::string, float>> CalculateComponentTransconductancesThirdStage();
		std::vector<std::pair<std::string, float>> CalculateComponentOutputConductanceArrayFirstStage();
		std::vector<std::pair<std::string, float>> CalculateComponentOutputConductanceArrayPrimarySecondStage();
		std::vector<std::pair<std::string, float>> CalculateComponentOutputConductanceArraySecondarySecondStage();
		std::vector<std::pair<std::string, float>> CalculateComponentOutputConductanceArrayThirdStage();

        // get Performance values
		float getGain();
		float getPowerConsumption();
		float getTransitFrequency();
		float getTransitFrequencyWithErrorFactor();
		float getArea();
		float getPhaseMargin();
		float getSlewRate();
		float getCMRR();
		float getNegPSRR();
		float getPosPSRR();
		float getMaxOutputVoltage();
		float getMinOutputVoltage();
		float getMinCommonModeInputVoltage();
		float getMaxCommonModeInputVoltage();

        // set Performance values
		void setGain(float gain);
		void setPowerConsumption(float power);
		void setTransitFrequency(float TransitFrequency);
		void setTransitFrequencyWithErrorFactor(float TransitFrequencyWithErrorFactor);
		void setArea(float area);
		void setPhaseMargin(float phaseMargin);
		void setSlewRate(float slewRate);
		void setCMRR(float CMRR);
		void setNegPSRR(float negPSRR);
		void setPosPSRR(float posPSRR);
		void setMaxOutputVoltage(float voutMax);
		void setMinOutputVoltage(float voutMin);
		void setMinCommonModeInputVoltage(float vcmMax);
		void setMaxCommonModeInputVoltage(float vcmMin);

		// get intermediate performance values
        std::vector<std::pair<Core::NetId, float>> getNetcapacitances();
		float getOutputResistanceFirstStage();
		float getOutputResistancePrimarySecondStage();
		float getOutputResistanceSecondarySecondStage();
		float getOutputResistanceThirdStage();
		float getGainFirstStage();
		float getGainSecondStage();
        float getGainThirdStage();
        float getFirstStageTransconductance();
        std::vector<std::pair<std::string, float>> getFirstStageComponentTransconductancesArray();
        float getPrimarySecondStageTransconductance();
        std::vector<std::pair<std::string, float>> getPrimarySecondStageComponentTransconductancesArray();
        float getSecondarySecondStageTransconductance();
        std::vector<std::pair<std::string, float>> getSecondarySecondStageComponentTransconductancesArray();
        float getThirdStageTransconductance();
        std::vector<std::pair<std::string, float>> getThirdStageComponentTransconductancesArray();
        float getOutputConductanceLoadPartFirstStage();
        float getOutputConductanceBiasPartFirstStage();
        std::vector<std::pair<std::string, float>> getComponentOutputConductancesArrayFirstStage();
        float getOutputConductanceLoadPartPrimarySecondStage();
        float getOutputConductanceBiasPartPrimarySecondStage();
        std::vector<std::pair<std::string, float>> getComponentOutputConductancesArrayPrimarySecondStage();
        float getOutputConductanceLoadPartSecondarySecondStage();
        float getOutputConductanceBiasPartSecondarySecondStage();
        std::vector<std::pair<std::string, float>> getComponentOutputConductancesArraySecondarySecondStage();
        float getOutputConductanceLoadPartThirdStage();
        float getOutputConductanceBiasPartThirdStage();
        std::vector<std::pair<std::string, float>> getComponentOutputConductancesArrayThirdStage();
		float getDominantPole();
		float getPositiveZero();
		std::vector<float> getImportantNonDominantPoles();
		std::vector<float> getImportantZeros();


        // Set intermediate performance values
        void setNetCapacitances(const std::vector<std::pair<Core::NetId, float>>& netCapacitances);
        void setOutputResistanceFirstStage(float outputResistanceFirstStage);
        void setOutputResistancePrimarySecondStage(float outputResistancePrimarySecondStage);
		void setOutputResistanceSecondarySecondStage(float outputResistanceSecondarySecondStage);
        void setOutputResistanceThirdStage(float outputResistanceThirdStage);
        void setGainFirstStage(float gainFirstStage);
        void setGainSecondStage(float gainSecondStage);
        void setGainThirdStage(float gainThirdStage);
        void setFirstStageTransconductance(float firstStageTransconductance);
        void setFirstStageComponentTransconductancesArray(const std::vector<std::pair<std::string, float>>& firstStageComponentTransconductancesArray);
        void setPrimarySecondStageTransconductance(float primarySecondStageTransconductance);
        void setPrimarySecondStageComponentTransconductancesArray(const std::vector<std::pair<std::string, float>>& primarySecondStageComponentTransconductancesArray);
        void setSecondarySecondStageTransconductance(float secondarySecondStageTransconductance);
        void setSecondarySecondStageComponentTransconductancesArray(const std::vector<std::pair<std::string, float>>& secondarySecondStageComponentTransconductancesArray);
        void setThirdStageTransconductance(float thirdStageTransconductance);
        void setThirdStageComponentTransconductancesArray(const std::vector<std::pair<std::string, float>>& thirdStageComponentTransconductancesArray);
        void setOutputConductanceLoadPartFirstStage(float outputConductanceLoadPartFirstStage);
        void setOutputConductanceBiasPartFirstStage(float outputConductanceBiasPartFirstStage);
        void setComponentOutputConductancesArrayFirstStage(const std::vector<std::pair<std::string, float>>& componentOutputConductancesArrayFirstStage);
        void setOutputConductanceLoadPartPrimarySecondStage(float outputConductanceLoadPartPrimarySecondStage);
        void setOutputConductanceBiasPartPrimarySecondStage(float outputConductanceBiasPartPrimarySecondStage);
        void setComponentOutputConductancesArrayPrimarySecondStage(const std::vector<std::pair<std::string, float>>& componentOutputConductancesArrayPrimarySecondStage);
        void setOutputConductanceLoadPartSecondarySecondStage(float outputConductanceLoadPartSecondarySecondStage);
        void setOutputConductanceBiasPartSecondarySecondStage(float outputConductanceBiasPartSecondarySecondStage);
        void setComponentOutputConductancesArraySecondarySecondStage(const std::vector<std::pair<std::string, float>>& componentOutputConductancesArraySecondarySecondStage);
        void setOutputConductanceLoadPartThirdStage(float outputConductanceLoadPartThirdStage);
        void setOutputConductanceBiasPartThirdStage(float outputConductanceBiasPartThirdStage);
        void setComponentOutputConductancesArrayThirdStage(const std::vector<std::pair<std::string, float>>& componentOutputConductancesArrayThirdStage);
		void setDominantPole(float dominantPole);
		void setPositiveZero(float positivezero);
		void setImportantNonDominantPoles(std::vector<float> ImportantNonDominantPoles);
		void setImportantZeros(std::vector<float> ImportantZeros);
		








	private:
		const Partitioning::Result & getPartitioningResult() const;
		const Graph & getGraph() const;
		const CircuitInformation & getCircuitInformation() const;

		const ComponentToIntVarInputMap & getTransistorToWidthInputMap() const;
		const ComponentToIntVarInputMap & getTransistorToMultiplierInputMap() const;
		const ComponentToIntVarInputMap & getTransistorToLengthInputMap() const;


		const ComponentToIntVarMap & getTransistorToCurrentMap() const;


		const ComponentToIntVarInputMap & getTwoPortToValueInputMap() const;
		const NetToIntVarMap & getNetToVoltageMap() const;

		SimulationSpace & getSpace() const;
		std::string getTransistorModel() const;
		int getEKVVersion() const;
		const SimulatedPolesAndZeros& getPolesAndZeros() const;
		const StructRec::StructureCircuits&  getStructureRecognitionResult() const;


		float calculateGainSimulation ();

		float calculateSimulatedPowerConsumption();

		float calculateSimulatedSlewRate();

		float calculateSimulatedTransientFrequency();

		float calculateTransientSimulatedFrequencyWithErrorFactor();

		std::tuple<float, float> calculateSimulatedOutputVoltageSwing();

		std::tuple<float, float> calculateSimulatedCommonModeInputVoltage();

		std::tuple<float, float> calculateSimulatedPSRR();

		float calculateSimulatedCMRR();

		float calculateInputArea();

		float calculateSimulatedPhaseMargin();

		float computeScalingFactorForSymmetricalOTASim();

		void createPolesAndZeros();


        float calculateSlewRateFirstStageSim();

		float calculateSlewRateSecondStageSim();

		float calculateSlewRateThirdStageSim();

		float calculateSlewRateSim(Partitioning::TransconductancePart & stage );

		float calculateNonDominatPoleHigherStagesCompensationCapacitance(
				Partitioning::TransconductancePart & stage);

		float calculateDominantPole();
		float calculateNonDominantPoleFirstStage();
		float calculatePositiveZeroFirstStage();
		float calculateNonDominantPoleHigherStages(
				Partitioning::TransconductancePart & currentStage, Partitioning::TransconductancePart & previousStage);
		float calculateAdditionalNonDominantPoleHigherStages(
			Partitioning::TransconductancePart & currentStage, Partitioning::TransconductancePart & previousStage);
		float calculatePositiveZeroWithCompensationCapacity();
		float calculatePositiveZeroSecondStage();
		float calculatePositiveZeroWithCompensationResistance();

		void calculateNonDominantPolesComplementaryFirstStage(SimulatedPolesAndZeros & polesAndZeros);
		void calculateFirstStageSecondLoadPolesAndZeros(SimulatedPolesAndZeros & polesAndZeros);

		void createBiasHigherStagesNonDominantPolesAndZeros(
				Partitioning::TransconductancePart & stage, SimulatedPolesAndZeros &polesAndZeros);



		float calculateSimulatedErrorFactorTransientFrequency();


		float computeGateSourceCapacitanceSim(Partitioning::Component & component) const; 

		float computeDrainBulkCapacitanceSim(Partitioning::Component & component) const;  

		float computeSourceBulkCapacitanceSim(Partitioning::Component & component) const;

		float computeGateDrainCapacitanceSim(Partitioning::Component & component) const; 

		float computeGateBulkCapacitanceSim(Partitioning::Component & component) const;

		float calculateSimulatedNetCapacitance(const StructRec::StructureNet & net) const;


		float calculateGainFirstStage();

		float calculateGainSecondStage();

		float createGainHigherStage(Partitioning::TransconductancePart & higherStage);

		float calculateGainThirdStage();

		float calculateFirstStageGainFirstStageWithDiodeTransistorLoad();

		float computeGainSimulateFirstStageErrorFactor();


		float computeSimulateTransconductance(Partitioning::Component& transistor) ;


		float computeSimulateTransconductance(Partitioning::TransconductancePart & stage);
		

		float computeOutputSimulateConductance(Partitioning::Component & transistor);
		

		float computeOutputSimulateConductance(Partitioning::Part & part, Partitioning::TransconductancePart & stage);


		float computeOutputSimulateResistanceFirstStage();
      

		float computeOutputResistanceSimulateHigherStage(Partitioning::TransconductancePart & higherStage);

		float computeOutputSimulateResistance(Partitioning::TransconductancePart & stage);



		
		float computeEdgeCurrentSimulation(Edge & edge);

		float computeSign(const Edge & edge, const Path & path) const;

		float computeEdgeVoltage(const Edge & edge);

		Gecode::FloatVar getOutputCurrent();

		bool isOutputNetFirstStage(Core::NetId outputNet) const;
		bool isOutputNetStage(const StructRec::StructureNet & net, Partitioning::TransconductancePart & stage) const;
		bool compensationCapacityConnectedBetween(Partitioning::TransconductancePart &stage2, Partitioning::TransconductancePart &stage1) const;
		bool hasCompensationCapacity(const StructRec::StructureNet & net) const;
		bool hasComponentInFeedback(const StructRec::StructureNet & net) const;
		bool hasOutputNetWithCapacitor(Partitioning::TransconductancePart & stage) const;

		const StructRec::StructureNet & findOutputNetWithCapacitor(Partitioning::TransconductancePart & stage);

		std::vector<const StructRec::StructureNet*>  findOutputNetsFirstStage() const;
		std::vector<const StructRec::StructureNet*>  findOutputNets(Partitioning::TransconductancePart & stage) const;

		std::vector<Path> findShortestPaths(std::vector<Path> paths);
		std::vector<Partitioning::Component * > computeInputToSupplyRailPathOverBias(Partitioning::TransconductancePart & stage);
		std::vector<Partitioning::Component * > computeInputToSupplyRailPathOverLoad(Partitioning::TransconductancePart & stage);

	private:
		const Partitioning::Result * partitioningResult_;
		const StructRec::StructureCircuits* structRecResult_;
		const SimulatedPolesAndZeros * polesAndZeros_;
		const Graph * graph_;
		const CircuitInformation * circuitInformation_;

		ComponentToIntVarInputMap * transistorToLengthInputMap_;

		ComponentToIntVarInputMap * transistorToWidthInputMap_;	
		ComponentToIntVarInputMap * transistorToMultiplierInputMap_;	
		ComponentToIntVarMap * transistorToCurrentMap_;


		ComponentToIntVarInputMap * twoPortToValueInputMap_;
		
		NetToIntVarMap * netToVoltageMap_;

		SimulationSpace * space_;
		std::string transistorModel_;
		int ekvVersion_;

		// Performace variables

		float GAIN_;
		float POWER_;
		float TRANSITFREQUENCY_;
		float TRANSIT_FREQUENCY_WITH_ERROR_FACTOR_;
		float AREA_;
		float PHASE_MARGIN_;
		float SLEW_RATE_;
		float CMRR_;
		float POS_PSRR_;
		float NEG_PSRR_;
		float VOUT_MAX_;
		float VOUT_MIN_;
		float VCM_MAX_;
		float VCM_MIN_;

		// Intermediate performance variables

		std::vector<std::pair<Core::NetId, float>> netCapacitances_;

		float Output_Resistance_FirstStage_;

		float Output_Resistance_PrimarySecondStage_;

		float Output_Resistance_SecondarySecondStage_;

		float Output_Resistance_ThirdStage_;

		float gainFirstStage_;

		float gainSecondStage_;

		float gainThirdStage_;

		float firstStageTransconductance_;

		std::vector<std::pair<std::string, float>> FirstStagecomponentTransconductancesArray_;

		float PrimarySecondStageTransconductance_;

		std::vector<std::pair<std::string, float>> PrimarySecondStagecomponentTransconductancesArray_;

		float SecondarySecondStageTransconductance_;

		std::vector<std::pair<std::string, float>> SecondarySecondStagecomponentTransconductancesArray_;

		float ThirdStageTransconductance_;

		std::vector<std::pair<std::string, float>> ThirdStagecomponentTransconductancesArray_;

		float Output_Conductance_loadpart_first_stage_;

		float Output_Conductance_biasPart_first_stage_;
		
		std::vector<std::pair<std::string, float>> ComponentOutputConductancesArrayFirstStage_;

		float Output_Conductance_loadpart_Primary_Second_Stage_;
		
		float Output_Conductance_biasPart_Primary_Second_Stage_;

		std::vector<std::pair<std::string, float>> ComponentOutputConductancesArrayPrimarySecondStage_;
		
		float Output_Conductance_loadpart_Secondary_Second_Stage_;
		
		float Output_Conductance_biasPart_Secondary_Second_Stage_;
		
		std::vector<std::pair<std::string, float>> ComponentOutputConductancesArraySecondarySecondStage_;

		float Output_Conductance_loadpart_Third_Stage_;
		
		float Output_Conductance_biasPart_Third_Stage_;

		std::vector<std::pair<std::string, float>> ComponentOutputConductancesArrayThirdStage_;
		
		float DOMINANTPOLE_;
		float POSITIVEZERO_;
		std::vector<float> ImportantNonDominantPoles_;
		std::vector<float> ImportantZeros_;





	};



}



#endif /* SIMULATION_INCL_SIMULATION_CONSTRAINTS_PERFORMACECALCULATION_H_ */


