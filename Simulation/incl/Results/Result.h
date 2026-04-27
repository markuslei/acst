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

#ifndef SIMULATION_INCL_RESULTS_RESULT_H_
#define SIMULATION_INCL_RESULTS_RESULT_H_


#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"

#include "Partitioning/incl/Results/TwoPort.h"
#include "Partitioning/incl/Results/Transistor.h"

#include "StructRec/incl/StructureCircuit/Structure/Structure.h"

#include "Core/incl/Common/RapidXmlUtils.h"
#include "Core/incl/Common/RapidXml.h"
#include "Control/incl/File/OutputFile.h"
#include <rapidxml_print.hpp>

namespace Partitioning {
 class Result;
 class Component;
}

namespace Simulation {

class Result : public Core::Object
{
public:
	Result();
	~Result();

	void initializeTwoPortMap(const Partitioning::Result & partitioningResult);
	void initializeTransistorMap(const Partitioning::Result & partitioningResult);

	void setGain(float gain);
	void setPower(float power);
	void setSlewRate(float slewRate);
	void setPhaseMargin(float phaseMargin);
	void setTransitFrequency(float transitFrequency);
	void setTransitFrequencyWithErrorFactor(float transitFrequency);
	void setArea(float area);
	void setCMRR(float CMRR);
	void setPosPSRR(float posPSRR);
	void setNegPSRR(float negPSRR);
	void setMaxOutputVoltage(float voutMax);
	void setMinOutputVoltage(float voutMin);
	void setMaxCommonModeInputVoltage(float vcmMax);
	void setMinCommonModeInputVoltage(float vcmMin);

	void addCurrent(Partitioning::Component & component, float current);
	void addVoltage(Core::NetId netId, float voltage);

	std::vector<Partitioning::Component*> findComponents(const StructRec::Structure & structure) const;
	Partitioning::Component & findComponent(StructRec::StructureId structureId) const;

	void writeXmlSimulationResult(Control::OutputFile & file) const;
	std::string toStr() const;

public:

	float getGain() const;
	float getPower() const;
	float getSlewRate() const;
	float getPhaseMargin() const;
	float getTransitFrequency() const;
	float getTransitFrequencyWithErrorFactor() const;
	float getArea() const;
	float getCMRR() const;

	float getNegPSRR() const;
	float getPosPSRR() const;
	float getMaxOutputVoltage() const;
	float getMinOutputVoltage() const;
	float getMaxCommonModeInputVoltage() const;
	float getMinCommonModeInputVoltage() const;

	std::vector<Partitioning::TwoPort*> getAllTwoPorts() const;
	std::vector<Partitioning::Transistor*> getAllTransistors() const;


   // get intermediate performance values
    std::vector<std::pair<Core::NetId, float>> getNetcapacitances() const;
	float getOutputResistanceFirstStage() const;
	float getOutputResistancePrimarySecondStage() const;
	float getOutputResistanceSecondarySecondStage() const;
	float getOutputResistanceThirdStage() const;
	float getGainFirstStage() const;
	float getGainSecondStage() const;
    float getGainThirdStage() const;
    float getFirstStageTransconductance() const;
    std::vector<std::pair<std::string, float>> getFirstStageComponentTransconductancesArray() const;
    float getPrimarySecondStageTransconductance() const;
    std::vector<std::pair<std::string, float>> getPrimarySecondStageComponentTransconductancesArray() const;
    float getSecondarySecondStageTransconductance() const;
    std::vector<std::pair<std::string, float>> getSecondarySecondStageComponentTransconductancesArray() const;
    float getThirdStageTransconductance() const;
    std::vector<std::pair<std::string, float>> getThirdStageComponentTransconductancesArray() const;
    float getOutputConductanceLoadPartFirstStage() const;
    float getOutputConductanceBiasPartFirstStage() const;
    std::vector<std::pair<std::string, float>> getComponentOutputConductancesArrayFirstStage() const;
    float getOutputConductanceLoadPartPrimarySecondStage() const;
    float getOutputConductanceBiasPartPrimarySecondStage() const;
    std::vector<std::pair<std::string, float>> getComponentOutputConductancesArrayPrimarySecondStage() const;
    float getOutputConductanceLoadPartSecondarySecondStage() const;
    float getOutputConductanceBiasPartSecondarySecondStage() const;
    std::vector<std::pair<std::string, float>> getComponentOutputConductancesArraySecondarySecondStage() const;
    float getOutputConductanceLoadPartThirdStage() const;
    float getOutputConductanceBiasPartThirdStage() const;
    std::vector<std::pair<std::string, float>> getComponentOutputConductancesArrayThirdStage() const;
	float getDominantPole() const;
	float getPositiveZero() const;
	std::vector<float> getImportantNonDominantPoles() const;
	std::vector<float> getImportantZeros() const;



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




	bool hasArea() const;
	bool hasPower() const;
	bool hasGain() const;
	bool hasSlewRate() const;
	bool hasTransitFrequency() const;
	bool hasTransitFrequencyWithErrorFactor() const;
	bool hasPhaseMargin() const;
	bool hasCMRR() const;
	bool hasNegPSRR() const;
	bool hasPosPSRR() const;
	bool hasMaxCommonModeInputVoltage() const;
	bool hasMinCommonModeInputVoltage() const;
	bool hasMaxOutputVoltage() const;
	bool hasMinOutputVoltage() const;


	std::map<Core::DeviceId, float>  getCurrentMap() const;
	std::map<Core::NetId, float>  getVoltageMap() const;

private:
	static const int NOT_INITIALIZED_;

	void writeXmlSimulatedPerformance(Core::XmlNode & node, Core::XmlDocument & doc) const;
	void writeXmlIntermediateSimulatedPerformance(Core::XmlNode & node, Core::XmlDocument & doc) const;
	void writeXmlCurrents(Core::XmlNode & node, Core::XmlDocument & doc) const;
	void writeXmlVoltages(Core::XmlNode & node, Core::XmlDocument & doc) const;
	void writeXmlComponentDimensions(Core::XmlNode & node, Core::XmlDocument & doc) const;
	void writeXmlCapacitorDimensions(Core::XmlNode & node, Core::XmlDocument & doc) const;
	void writeXmlTransistorDimensions(Core::XmlNode & node, Core::XmlDocument & doc) const;

	void writeSimulatedPerformance(std::ostream& stream) const;
	void writeIntermediateSimulatedPerformance(std::ostream& stream) const;
	void writeVoltages(std::ostream& stream) const;
	void writeCurrents(std::ostream & stream) const;

	bool hasCapacity(Core::DeviceId deviceId) const;

private:
	std::map<StructRec::StructureId,Partitioning::Transistor*> transistors_;
	std::map<StructRec::StructureId,Partitioning::TwoPort*> twoPorts_;


	float gain_;
	float power_;
	float slewRate_;
	float phaseMargin_;
	float transitFrequency_;
	float transitFrequencyWithErrorFactor_;
	float area_;
	float CMRR_;
	float negPSRR_;
	float posPSRR_;
	float voutMax_;
	float voutMin_;
	float vcmMax_;
	float vcmMin_;

	std::map<Core::DeviceId, float> currentMap_;
	std::map<Core::NetId, float> voltageMap_;

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






#endif /* SIMULATION_INCL_SimulationCSP_RESULTS_RESULT_H_ */
