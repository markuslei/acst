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



 

/**************************************************************************************************
/**************************************************************************************************
/**************************************************************************************************

1 - Fixed parameters (lengths, widths, multiplier and two-port values) are mapped and excluded from the CSP decision process.
2 - Free variables (voltages and currents) are solved subject to transistor, KCL, and voltage constraints.
3 - Branching and domain constraints focus on the variables that need solving, ensuring efficiency.


*************************************************************************************************/



#include "StructRec/incl/StructureCircuit/Structure/Pin/StructurePin.h"

#include "Simulation/incl/SimulationCSP/SimulationSpace.h"

#include <vector>

#include "Partitioning/incl/Results/Component.h"
#include "Simulation/incl/SimulationCSP/Constraints/SimulationKCLConstraints.h"
#include "Simulation/incl/SimulationCSP/Constraints/SimulationTransistorConstraintsEKV.h"
#include "Simulation/incl/SimulationCSP/Constraints/SimulationTransistorConstraintsSHM.h"
#include "Simulation/incl/SimulationCSP/PerformanceCalculation.h"
#include "Simulation/incl/SimulationCSP/Constraints/SimulatedPolesAndZeros.h"
#include "Simulation/incl/SimulationCSP/Graph/Graph.h"
#include "Simulation/incl/SimulationCSP/Graph/Node.h"
#include "Simulation/incl/SimulationCSP/Graph/Edge.h"
#include "Simulation/incl/SimulationCSP/Variables/Transistor.h"
#include "Simulation/incl/SimulationCSP/Variables/Variables.h"
#include "Simulation/incl/SimulationCSP/Variables/Other.h"
#include "Simulation/incl/SimulationCSP/Variables/TwoPort.h"
#include "Simulation/incl/SimulationCSP/Variables/PoleOrZero.h"
#include "Simulation/incl/SimulationCSP/Variables/Specification.h"
#include "Simulation/incl/SimulationCSP/Variables/Voltage.h"
#include "Simulation/incl/CircuitInformation/CircuitInformation.h"
#include "Simulation/incl/CircuitInformation/Pin.h"
#include "Simulation/incl/CircuitInformation/LoadCapacity.h"

#include "Simulation/incl/Results/Result.h"

#include "GecodeExtensions/incl/Brancher/TransistorBrancher.h"
#include "GecodeExtensions/incl/Brancher/IntegerVariableBrancher.h"
#include "GecodeExtensions/incl/Brancher/IntegerVariableBrancherSupportingTransistorDomain.h"
#include "GecodeExtensions/incl/Brancher/TransistorBrancherIntervalSplitting.h"
#include "GecodeExtensions/incl/Brancher/TransistorBrancherOnlyVariableDomain.h"

#include "Partitioning/incl/Results/Result.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

namespace Simulation {

    const float SimulationSpace::NOT_INITIALIZED_ = -1;
	SimulationSpace::~SimulationSpace()
	{

	}

	SimulationSpace::SimulationSpace() :
				result_(nullptr),

				scalingFactorMUM_(NOT_INITIALIZED_),
				circuitInformation_(nullptr),
				graph_(nullptr),
				structRecResult_(nullptr),
				polesAndZeros_(nullptr),
				variablesMap_(nullptr),
				usesHSpiceLibrary_(false)
	{
	}

	SimulationSpace::SimulationSpace(SimulationSpace& other) : Space(other)
	{
		result_ = other.result_;
		circuitInformation_ = other.circuitInformation_;
		graph_ = other.graph_;
		structRecResult_ = other.structRecResult_;
		transistorModel_ = other.transistorModel_;
		ekvVersion_ = other.ekvVersion_;
		definition_ = other.definition_;
		usesHSpiceLibrary_ = other.usesHSpiceLibrary_;
		polesAndZeros_ = other.polesAndZeros_;
		variablesMap_ = other.variablesMap_;

		scalingFactorMUM_ = other.scalingFactorMUM_;
		indexToVariableMap_ = other.indexToVariableMap_;
		pathToCicuitNetlist_ = other.pathToCicuitNetlist_;

		variables_.update(*this, other.variables_);
		helper_.update(*this, other.helper_);

		logDebug("result_ " << result_);
		/*logDebug("circuitInformation_ " << circuitInformation_);
		logDebug("graph_ " << graph_);
		logDebug("structRecResult_ " << structRecResult_);
		logDebug("transistorModel_ " << transistorModel_);
		logDebug("definition_ " << definition_);
		logDebug("usesHSpiceLibrary_ " << usesHSpiceLibrary_);
		logDebug("polesAndZeros_ " << polesAndZeros_);
		logDebug("variablesMap_ " << variablesMap_);
		logDebug("scalingFactorMUM_ " << scalingFactorMUM_);
		logDebug("indexToVariableMap_ " << indexToVariableMap_);
		logDebug("pathToCicuitNetlist_ " << pathToCicuitNetlist_);*/
		logDebug("variables_ " << variables_);
		logDebug("helper_ " << helper_);


        logDebug("Define an objectv of type space");
		createMaps();
	}

    void SimulationSpace::setPolesAndZeros(SimulatedPolesAndZeros & polesAndZeros)
	{
		polesAndZeros_ = &polesAndZeros;
	}

	SimulatedPolesAndZeros & SimulationSpace::getPolesAndZeros()
	{
		//assert(polesAndZeros_ != NULL);
		return *polesAndZeros_;
	}

	void SimulationSpace::initializePolesAndZeros()
	{
		SimulatedPolesAndZeros * polesAndZeros = new SimulatedPolesAndZeros(getPartitioningResult(), getCircuitInformation());
		setPolesAndZeros(*polesAndZeros);

	}



	std::string SimulationSpace::toStr() const
	{
		std::ostringstream oss;
		oss << "Width: " << transistorToWidthInputMap_.toStr() << std::endl;
		oss << "Multiplier: " << transistorToMultiplierInputMap_.toStr() << std::endl;
		oss << "Length: " << transistorToLengthInputMap_.toStr() << std::endl;
		oss << "TwoPorts: " << twoPortToValueInputMap_.toStr() << std::endl;
		oss << "Voltages: " << netToVoltageMap_.toStr() << std::endl;
		oss << "Currents: " << transistorToCurrentMap_.toStr() << std::endl;
		oss << std::endl;



		oss << "Helper: " << helper_ << std::endl;
		return oss.str();
	}


	std::string SimulationSpace::printIndexToVariableMap() const
	{
		std::ostringstream oss;

		for(auto & it : indexToVariableMap_)
		{
			int index = it.first;
			std::string variable = it.second;

			oss << "Index: " << index;
			oss << "        Variable: " << variable << std::endl;
		}

		return oss.str();
	}

	/*
	bool SimulationSpace::master(const Gecode::MetaInfo & mi)
	{
		if(mi.last() != NULL)
		{
			constrain(*mi.last());
		}
		if(mi.solution() == 0)
		{
			mi.nogoods().post(*this);
		}
		return true;
	}

*/

	void SimulationSpace::setResult(const Partitioning::Result& result)
	{
		result_ = & result;
	}




	
	void SimulationSpace::setHelper(Gecode::FloatVar helper)
	{
		helper_ = helper;
	}



	void SimulationSpace::setGraph(const Graph& graph)
	{
		graph_ = & graph;
	}

	void SimulationSpace::setCircuitInformation(
		const CircuitInformation& information)
	{
		circuitInformation_ = & information;
	}

	const Partitioning::Result& SimulationSpace::getPartitioningResult() const
	{
		assert(hasResult());
		return * result_;
	}

	void SimulationSpace::initialize()
	{

		initializeScalingValues();
		logDebug("initialize PolesAndZeros");
        initializePolesAndZeros();
		logDebug("create Variables ");
        createVariables();
        initializeVariableMap();
	
		
		createIndexes();


		logDebug("create Maps ");
		createMaps();
		logDebug("create Constraints ");
		createConstraints();
		initializeBranching();
	}



	void SimulationSpace::print() const
	{

	}

	void SimulationSpace::eraseVariables() const
	{
		if(variablesMap_->isInitialized())
			delete variablesMap_;
	}

	void SimulationSpace::initializeBranching()
	{
		Gecode::Rnd r(1U);
		Gecode::Rnd u(1U);

		logDebug("Starting Branching");

		branchBiasInputTransistor();

        Gecode::branch(*this, variables_, Gecode::INT_VAR_SIZE_MIN(), Gecode::INT_VAL_MED());
		

        logDebug("end Starting Branching");


	}


	void SimulationSpace::branchBiasInputTransistor()
	{
		if(getCircuitInformation().getCircuitParameter().hasCurrentBiasPin())
		{

			const Core::NetId & biasNet = getCircuitInformation().getCircuitParameter().getCurrentBiasPin().getNetId();
			const StructRec::StructureCircuit & arrayCircuit = structRecResult_->findStructureCircuit(0);

			std::vector<const StructRec::Structure*> connectedStructures = arrayCircuit.findConnectedStructures(biasNet);
			std::vector<Partitioning::Component*> connectedComponents;
			for(auto& i : connectedStructures)
			{
				const StructRec::Structure & array = *i;
				if(array.getStructureName() == StructRec::StructureName("MosfetDiodeArray"))
				{

					Partitioning::Component & comp = getPartitioningResult().findComponent(array.getIdentifier());
					branchComponent(comp);
				}
			}
		}

	}

	void SimulationSpace::branchComponent(Partitioning::Component & component)
{
    Gecode::Rnd r(1U); // Random branching for some components
    Gecode::Rnd u(1U);

    // Branch only on current, since lengths and widths are fixed
    Gecode::IntVar current = transistorToCurrentMap_.find(component);
    branch(*this, current, Gecode::INT_VAL_SPLIT_MIN());

    // Check if the component is part of a "MosfetNormalArray"
    if (component.getArray().getIdentifier().getName() == StructRec::StructureName("MosfetNormalArray"))
    {
        Gecode::IntVar sourceVoltage = netToVoltageMap_.find(component.getArray().findPin(StructRec::StructurePinType("MosfetNormalArray", "Source")).getNet().getIdentifier());
        Gecode::IntVar gateVoltage = netToVoltageMap_.find(component.getArray().findPin(StructRec::StructurePinType("MosfetNormalArray", "Gate")).getNet().getIdentifier());

        // Branch on relevant voltages for "MosfetNormalArray"
        branch(*this, gateVoltage, Gecode::INT_VAL_SPLIT_MIN());
        branch(*this, sourceVoltage, Gecode::INT_VAL_SPLIT_MIN());
    }
    else if (component.getArray().getIdentifier().getName() == StructRec::StructureName("MosfetDiodeArray"))
    {
        Gecode::IntVar sourceVoltage = netToVoltageMap_.find(component.getArray().findPin(StructRec::StructurePinType("MosfetDiodeArray", "Source")).getNet().getIdentifier());
        Gecode::IntVar drainVoltage = netToVoltageMap_.find(component.getArray().findPin(StructRec::StructurePinType("MosfetDiodeArray", "Drain")).getNet().getIdentifier());

        // Branch on voltages for "MosfetDiodeArray"
        branch(*this, sourceVoltage, Gecode::INT_VAL_SPLIT_MIN());
        branch(*this, drainVoltage, Gecode::INT_VAL_SPLIT_MIN());
    }
}




	void SimulationSpace::createVariables()
	{
		std::vector<Partitioning::Transistor*> transistors = getPartitioningResult().getAllTransistors();
		std::vector<Partitioning::TwoPort*> twoPort =  getPartitioningResult().getAllTwoPorts();
		const std::vector<Node*> nodes = getGraph().getAllNodes();
		


		int numOfCurrents = transistors.size();
        int numOfVoltages = nodes.size();



		int numOfVariables = numOfCurrents + numOfVoltages ;
		
        variables_ = Gecode::IntVarArray(*this, numOfVariables, -pow(10, 9), pow(10, 9));
		logDebug("Variables: " << variables_);



		helper_ =Gecode::FloatVar(*this, - pow(10,16), pow(10,16));
	}

	void SimulationSpace::createMaps()
	{
		createTransistorVariableMaps();
		createTwoPortValueMap();


	}

	void SimulationSpace::createIndexes()
	{
		int index = 0;
		createTransistorVariableIndexMaps(index);

	}

	void  SimulationSpace::createTransistorVariableMaps()
	{
		createLengthMap();

		createWidthMap();

		createMultiplierMap();

		createCurrentMap();

		createVoltageMap();


	}

	void  SimulationSpace::createTransistorVariableIndexMaps(int & index)
	{

		createCurrentIndexMap(index);

		createVoltageIndexMap(index);
	}

	



	


	void SimulationSpace::initializeVariableMap()
	{
		Variables * variablesMap = new Variables();
		variablesMap_ = variablesMap;
		variablesMap_->initializeRndSeedMap(variables_.size());
		initializeTransistorMap();
	}


	void SimulationSpace::initializeTransistorMap()
	{
		for(auto & transistor : getPartitioningResult().getAllTransistors())
		{
			Transistor * tran = new Transistor;
			tran->setName(transistor->getArray().getIdentifier().toStr());
			if(getPartitioningResult().isInOpAmp(*transistor)){
				tran->setType("opAmp");
			}
			else {
				tran->setType("bias");
			}
			getVariables().add(*tran);
		}
	}



	void SimulationSpace::createWidthMap()
	{
		logDebug("### path to netlist file ###");
		logDebug(pathToCicuitNetlist_);
	
		logDebug("##### EXECUTE createWidthMap #######");
		assert(getVariables().isInitialized(), "Transistors need to be initialized");
	
		// Set the correct definition
		transistorToWidthInputMap_.setDefinition("1mum");
	
		// Map containing fixed transistor widths
		std::map<std::string, float> transistorWidths;
	
		try {
			// Read the netlist file into a string and explicitly close it after reading
			std::ifstream file(pathToCicuitNetlist_);
			if (!file.is_open()) {
				throw std::runtime_error("Cannot open netlist file: " + pathToCicuitNetlist_);
			}
	
			std::string line;
			while (std::getline(file, line)) {
				// Check if the line starts with 'm' (indicating a transistor)
				if (line.empty() || line[0] != 'm') {
					continue;
				}
	
				std::istringstream iss(line);
				std::string transistorName;
				std::string token;
	
				// Extract the transistor name
				iss >> transistorName;
	
				// Find the 'W=' part to extract width
				while (iss >> token) {
					if (token.find("W=") != std::string::npos) {
						double width = std::stod(token.substr(2));  // Extract width after 'W='
						transistorWidths[transistorName] = width;
						break;
					}
				}
			}
	
			// Close the file explicitly
			file.close();
	
		} catch (const std::exception &e) {
			logDebug("Error parsing netlist: " << e.what());
			return;
		}
	
		// Log the transistor widths map
		logDebug("##### Transistor Widths Map #####");
		for (const auto &pair : transistorWidths) {
			logDebug("Transistor: " << pair.first << ", Width: " << pair.second);
		}
		logDebug("##################################");
	
		// Helper function to extract the transistor ID and remove any leading '/'
		auto extractTransistorID = [](const std::string &fullName) -> std::string {
			size_t pos = fullName.find(' ');  // Find the space separating the ID and the type
			std::string id = (pos != std::string::npos) ? fullName.substr(0, pos) : fullName;
			if (!id.empty() && id[0] == '/') {
				id.erase(0, 1);  // Remove leading '/'
			}
			return id;
		};
	
		// Process all transistors and assign fixed widths
		std::vector<Partitioning::Transistor *> transistors = getPartitioningResult().getAllTransistors();
		for (Partitioning::Transistor *tran : transistors) {
			std::string transistorName = tran->toStr();
			std::string transistorID = extractTransistorID(transistorName);
			logDebug("##Transistor ID##");
			logDebug(transistorID);
			
			// Look up the fixed width for the transistor
			if (transistorWidths.find(transistorID) != transistorWidths.end()) {
				double tranWidth = transistorWidths[transistorID];
				logDebug("Width of transistor " << transistorID << ": " << tranWidth);
				transistorToWidthInputMap_.add(*tran, tranWidth);
			} else {
				logDebug("Width for transistor " << transistorID << " not found. Skipping.");
			}
		}
	
		// Finalize the definition for the transistor width map
		logDebug("##### END createWidthMap #######");
		logDebug("##### TransistorToWidthInputMap #######");
		logDebug(transistorToWidthInputMap_.toStr());
	}


	void SimulationSpace::createMultiplierMap()
	{
		logDebug("### path to netlist file ###");
		logDebug(pathToCicuitNetlist_);
	
		logDebug("##### EXECUTE createMultiplierMap #######");
		assert(getVariables().isInitialized(), "Transistors need to be initialized");
	
		// Set the correct definition
		transistorToMultiplierInputMap_.setDefinition("1mum");
	
		// Map containing fixed transistor Multiplier
		std::map<std::string, int> transistorMultiplier;
	
		try {
			// Read the netlist file into a string and explicitly close it after reading
			std::ifstream file(pathToCicuitNetlist_);
			if (!file.is_open()) {
				throw std::runtime_error("Cannot open netlist file: " + pathToCicuitNetlist_);
			}
	
			std::string line;
			while (std::getline(file, line)) {
				// Check if the line starts with 'm' (indicating a transistor)
				if (line.empty() || line[0] != 'm') {
					continue;
				}
	
				std::istringstream iss(line);
				std::string transistorName;
				std::string token;
	
				// Extract the transistor name
				iss >> transistorName;
	
				// Find the 'W=' part to extract Multiplier
				while (iss >> token) {
					if (token.find("M=") != std::string::npos) {
						int multiplier = std::stod(token.substr(2));  // Extract Multiplier after 'M='
						transistorMultiplier[transistorName] = multiplier;
						break;
					}
				}
			}
	
			// Close the file explicitly
			file.close();
	
		} catch (const std::exception &e) {
			logDebug("Error parsing netlist: " << e.what());
			return;
		}
	
		// Log the transistor Multiplier map
		logDebug("##### Transistor Multiplier Map #####");
		for (const auto &pair : transistorMultiplier) {
			logDebug("Transistor: " << pair.first << ", Multiplier: " << pair.second);
		}
		logDebug("##################################");
	
		// Helper function to extract the transistor ID and remove any leading '/'
		auto extractTransistorID = [](const std::string &fullName) -> std::string {
			size_t pos = fullName.find(' ');  // Find the space separating the ID and the type
			std::string id = (pos != std::string::npos) ? fullName.substr(0, pos) : fullName;
			if (!id.empty() && id[0] == '/') {
				id.erase(0, 1);  // Remove leading '/'
			}
			return id;
		};
	
		// Process all transistors and assign fixed Multiplier
		std::vector<Partitioning::Transistor *> transistors = getPartitioningResult().getAllTransistors();
		for (Partitioning::Transistor *tran : transistors) {
			std::string transistorName = tran->toStr();
			std::string transistorID = extractTransistorID(transistorName);
			logDebug("##Transistor ID##");
			logDebug(transistorID);
			
			// Look up the fixed Multiplier for the transistor
			if (transistorMultiplier.find(transistorID) != transistorMultiplier.end()) {
				int tranMultiplier = transistorMultiplier[transistorID];
				logDebug("Multiplier of transistor " << transistorID << ": " << tranMultiplier);
				transistorToMultiplierInputMap_.add(*tran, tranMultiplier);
			} else {
				logDebug("Multiplier for transistor " << transistorID << " not found. Skipping.");
			}
		}
	
		// Finalize the definition for the transistor Multiplier map
		logDebug("##### END createMultiplierMap #######");
		logDebug("##### TransistorToMultiplierInputMap #######");
		logDebug(transistorToMultiplierInputMap_.toStr());
	}
	
	


	void SimulationSpace::createLengthMap()
	{
		logDebug("### path to netlist file ###");
		logDebug(pathToCicuitNetlist_);
	
		logDebug("##### EXECUTE createLengthMap #######");
		assert(getVariables().isInitialized(), "Transistors need to be initialized");
	
		// Set the correct definition
		transistorToLengthInputMap_.setDefinition("1mum");
	
		// Map containing fixed transistor lengths
		std::map<std::string, float> transistorLengths;
	
		try {
			// Read the netlist file into a string and explicitly close it after reading
			std::ifstream file(pathToCicuitNetlist_);
			if (!file.is_open()) {
				throw std::runtime_error("Cannot open netlist file: " + pathToCicuitNetlist_);
			}
	
			std::string line;
			while (std::getline(file, line)) {
				// Check if the line starts with 'm' (indicating a transistor)
				if (line.empty() || line[0] != 'm') {
					continue;
				}
	
				std::istringstream iss(line);
				std::string transistorName;
				std::string token;
	
				// Extract the transistor name
				iss >> transistorName;
	
				// Find the 'L=' part to extract length
				while (iss >> token) {
					if (token.find("L=") != std::string::npos) {
						double length = std::stod(token.substr(2));  // Extract length after 'L='
						transistorLengths[transistorName] = length;
						break;
					}
				}
			}
	
			// Close the file explicitly
			file.close();
	
		} catch (const std::exception &e) {
			logDebug("Error parsing netlist: " << e.what());
			return;
		}
	
		// Log the transistor lengths map
		logDebug("##### Transistor Lengths Map #####");
		for (const auto &pair : transistorLengths) {
			logDebug("Transistor: " << pair.first << ", Length: " << pair.second);
		}
		logDebug("##################################");
	
		// Helper function to extract the transistor ID and remove any leading '/'
		auto extractTransistorID = [](const std::string &fullName) -> std::string {
			size_t pos = fullName.find(' ');  // Find the space separating the ID and the type
			std::string id = (pos != std::string::npos) ? fullName.substr(0, pos) : fullName;
			if (!id.empty() && id[0] == '/') {
				id.erase(0, 1);  // Remove leading '/'
			}
			return id;
		};
	
		// Process all transistors and assign fixed lengths
		std::vector<Partitioning::Transistor *> transistors = getPartitioningResult().getAllTransistors();
		for (Partitioning::Transistor *tran : transistors) {
			std::string transistorName = tran->toStr();
			std::string transistorID = extractTransistorID(transistorName);
	
			// Look up the fixed length for the transistor
			if (transistorLengths.find(transistorID) != transistorLengths.end()) {
				double tranLength = transistorLengths[transistorID];
				logDebug("Length of transistor " << transistorID << ": " << tranLength);
				transistorToLengthInputMap_.add(*tran, tranLength);
			} else {
				logDebug("Length for transistor " << transistorID << " not found. Skipping.");
			}
		}
	
		// Finalize the definition for the transistor length map
		logDebug("##### END createLengthMap #######");
		logDebug("##### TransistorToLengthInputMap #######");
		logDebug(transistorToLengthInputMap_.toStr());
	}
	



   

	

	void SimulationSpace::createTwoPortValueMap()
	{
		logDebug("### path to netlist file ###");
		logDebug(pathToCicuitNetlist_);
	
		logDebug("##### EXECUTE createTwoPortValueMap #######");
		assert(getVariables().isInitialized(), "Variables need to be initialized");
	
		// Map containing capacitor names and their corresponding values (in F)
		std::map<std::string, float> capacitorValues;
	
		try {
			// Read the netlist file into a string and explicitly close it after reading
			std::ifstream file(pathToCicuitNetlist_);
			if (!file.is_open()) {
				throw std::runtime_error("Cannot open netlist file: " + pathToCicuitNetlist_);
			}
	
			std::string line;
			while (std::getline(file, line)) {
				// Check if the line starts with 'c' (indicating a capacitor)
				if (line.empty() || line[0] != 'c') {
					continue;
				}
	
				std::istringstream iss(line);
				std::string capacitorName;
				std::string token;
	
				// Extract the capacitor name
				iss >> capacitorName;
	
				// Find the value (in pF)
				while (iss >> token) {
					if (std::isdigit(token[0])) {
						double value = std::stod(token) * 1e-12;  // Convert from pF to F
						capacitorValues[capacitorName] = value;
						break;
					}
				}
			}
	
			// Close the file explicitly
			file.close();
	
		} catch (const std::exception &e) {
			logDebug("Error parsing netlist: " << e.what());
			return;
		}
	
		// Log the capacitor values map
		logDebug("##### Capacitor Values Map #####");
		for (const auto &pair : capacitorValues) {
			logDebug("Capacitor: " << pair.first << ", Value: " << pair.second);
		}
		logDebug("##################################");
	
		// Helper function to extract the capacitor ID and remove any leading '/'
		auto extractCapacitorID = [](const std::string &fullName) -> std::string {
			if (!fullName.empty() && fullName[0] == '/') {
				return fullName.substr(1);  // Remove leading '/'
			}
			return fullName;
		};
	
		for (auto& it : getPartitioningResult().getAllTwoPorts())
		{
			StructRec::StructureId twoPortId = it->getArray().getIdentifier();
	
			// Extract the name of the capacitor
			std::string twoPortName = extractCapacitorID("/" + it->getDeviceId().getDeviceName());
	
			// Check if the capacitor exists in the map
			if (capacitorValues.find(twoPortName) != capacitorValues.end()) {
				double capacitorValue = capacitorValues[twoPortName]; // Get the value for this capacitor
				
				// Log the value if needed
				logDebug("Capacitor " << twoPortName << " has value: " << capacitorValue);
	
				// Add the capacitor and its value to the twoPortToValueMap
				twoPortToValueInputMap_.add(twoPortId, capacitorValue);
			} else {
				// Log if the capacitor is not found in the map
				logDebug("Capacitor " << twoPortName << " not found in map. Skipping.");
			}
		}
	
		// Finalize the definition for the two port value map
		logDebug("##### END createTwoPortValueMap #######");
		logDebug("##### TwoPortToValueInputMap #######");
		logDebug(twoPortToValueInputMap_.toStr());
	}
	

    

	


	void SimulationSpace::createVoltageMap()
	{
		logDebug("##### EXECUTE VoltageMap #######");
		assert(getVariables().isInitialized(), "NetToVoltageIndexMap needs to be initialized");
		for(auto & node : getGraph().getAllNodes())
		{
			Core::NetId netId =node->getNetId();
			int index = getVariables().findOther(netId.toStr()).getIndex();


			Gecode::IntVar intVar = variables_[index];
			netToVoltageMap_.add(netId, intVar);
			index++;
		}
	}

	void SimulationSpace::createVoltageIndexMap(int & index)
	{
		std::vector<Node*> nodes = getGraph().getAllNodes();
		for(std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++)
		{
			Node * node = *it;

			Voltage * net = new Voltage;
			net->setName(node->getNetId().toStr());
			net->setIndex(index);
			getVariables().add(*net);
			setNetIndexOfTransistors(node->getNetId(), index);

			std::ostringstream mainString;
			mainString << "Voltage: " << node->getNetId().toStr();

			indexToVariableMap_.insert(std::pair<int, std::string>(index, mainString.str()));

			index++;
		}
	}


	void SimulationSpace::setNetIndexOfTransistors(Core::NetId netId, int &index)
	{
		const StructRec::StructureNet & net = getStructureRecognitionResult().findStructureCircuit(0).findStructureNet(netId);

		std::vector<const StructRec::Structure *> gateDTAs = net.findConnectedStructures(StructRec::StructurePinType("MosfetDiodeArray", "Drain"));
		for(auto & gateDTA : gateDTAs)
		{
			getVariables().findTransistor(gateDTA->getIdentifier()).setDrainVoltageIndex(index);
			getVariables().findTransistor(gateDTA->getIdentifier()).setGateVoltageIndex(index);
		}

		std::vector<const StructRec::Structure *> sourceDTAs = net.findConnectedStructures(StructRec::StructurePinType("MosfetDiodeArray", "Source"));
		for(auto & sourceDTA : sourceDTAs)
		{
			getVariables().findTransistor(sourceDTA->getIdentifier()).setSourceVoltageIndex(index);
		}

		std::vector<const StructRec::Structure *> gateNTAs = net.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
		for(auto & gateNTA : gateNTAs)
		{
			getVariables().findTransistor(gateNTA->getIdentifier()).setGateVoltageIndex(index);
		}

		std::vector<const StructRec::Structure *> drainNTAs = net.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Drain"));
		for(auto & drainNTA : drainNTAs)
		{
			getVariables().findTransistor(drainNTA->getIdentifier()).setDrainVoltageIndex(index);
		}

		std::vector<const StructRec::Structure *> sourceNTAs = net.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Source"));
		for(auto & sourceNTA : sourceNTAs)
		{
			getVariables().findTransistor(sourceNTA->getIdentifier()).setSourceVoltageIndex(index);
		}


	}

	void SimulationSpace::createCurrentMap()
	{
		logDebug("##### EXECUTE CurrentMap #######");
		assert(getVariables().isInitialized(), "Transistors need to be initialized");
		std::vector<Partitioning::Transistor*> transistors = getPartitioningResult().getAllTransistors();
		for(std::vector<Partitioning::Transistor*>::const_iterator it = transistors.begin(); it != transistors.end(); it++)
		{
			Partitioning::Transistor * tran = *it;
			int index = getVariables().findTransistor(tran->getArray().getIdentifier()).getCurrentIndex();
			Gecode::IntVar intVar = variables_[index];
			transistorToCurrentMap_.add(*tran, intVar);
			//logDebug("Transistor");
			//logDebug(tran->getDeviceId().getDeviceName());
		}
	}

	void SimulationSpace::createCurrentIndexMap(int & index)
	{
		std::vector<Partitioning::Transistor*> transistors = getPartitioningResult().getAllTransistors();
		for(std::vector<Partitioning::Transistor*>::const_iterator it = transistors.begin(); it != transistors.end(); it++)
		{
			Partitioning::Transistor * tran = *it;
			getVariables().findTransistor(tran->getArray().getIdentifier()).setCurrentIndex(index);

			std::ostringstream mainString;
			mainString << "Current: " << tran->getArray().getIdentifier().toStr();

			indexToVariableMap_.insert(std::pair<int, std::string>(index, mainString.str()));

			index++;
		}
	}





	void SimulationSpace::createConstraints()
	{


		logDebug("Creating Constraints");

		logDebug("Create Domain Constraints");
		createDomainConstraints();
		logDebug("Create Voltage Constraints");
		createVoltageConstraints();

		logDebug("Creating KCL constraints");
		createKCLConstraints();


		logDebug("Creating transistor constraints");
		createTransistorConstraints();
	}

	void SimulationSpace::createDomainConstraints()
	{
		for(auto & tran : getPartitioningResult().getAllTransistors())
		{

			dom(*this, transistorToCurrentMap_.find(*tran), -1000000000, 1000000000);
		}

		int vdd = getCircuitInformation().getCircuitParameter().getSupplyVoltage() * 1000;
		int vss = getCircuitInformation().getCircuitParameter().getGroundVoltage() *1000;
		//logDebug("VDD: " << vdd);
		//logDebug("VSS: " << vss);

		for(auto & net : graph_->getAllNodes())
		{
			dom(*this, netToVoltageMap_.find(net->getNetId()), vss, vdd);
		}


	}





	void SimulationSpace::createVoltageConstraints()
	{

		createVoltageConstraints(getCircuitInformation().getCircuitParameter().getInputPinMinus());
		createVoltageConstraints(getCircuitInformation().getCircuitParameter().getInputPinPlus());
		createVoltageConstraints(getCircuitInformation().getCircuitParameter().getGroundPin());
		createVoltageConstraints(getCircuitInformation().getCircuitParameter().getSupplyVoltagePin());

		if(getCircuitInformation().getCircuitParameter().hasReferenceVoltagePin())
		{
			createVoltageConstraints(getCircuitInformation().getCircuitParameter().getReferenceVoltagePin());
		}

		for(auto & biasVoltagePin : getCircuitInformation().getCircuitParameter().getVoltageBiasPins())
		{
			createVoltageConstraints(*biasVoltagePin);
		}

		createOutputVoltageConstraint();

		if(usesHSpiceLibrary())
		{
			logDebug("Create stage bias voltage pin constraint");
			createStageBiasVoltagePinsConstraints();
		}

	}

	void SimulationSpace::createVoltageConstraints(const Pin & pin)
	{
		Gecode::rel(*this, pin.getValue() * 1000 == netToVoltageMap_.find(pin.getNetId()));

	}

	void SimulationSpace::createOutputVoltageConstraint()
	{
		if(getCircuitInformation().getCircuitParameter().isFullyDifferential())
		{
			const Pin & outputPinMinus = getCircuitInformation().getCircuitParameter().getOutputPinMinus();
			const Pin & inputPinPlus = getCircuitInformation().getCircuitParameter().getInputPinPlus();
			Gecode::rel(*this, netToVoltageMap_.find(outputPinMinus.getNetId()) == 1 * netToVoltageMap_.find(inputPinPlus.getNetId()));

			const Pin & outputPinPlus = getCircuitInformation().getCircuitParameter().getOutputPinPlus();
			const Pin & inputPinMinus = getCircuitInformation().getCircuitParameter().getInputPinMinus();
			Gecode::rel(*this, netToVoltageMap_.find(outputPinPlus.getNetId()) == 1 * netToVoltageMap_.find(inputPinMinus.getNetId()));

		}
		else
		{
			const Pin & outputPin = getCircuitInformation().getCircuitParameter().getOutputPin();
			const Pin & inputPinMinus = getCircuitInformation().getCircuitParameter().getInputPinMinus();
			Gecode::rel(*this, netToVoltageMap_.find(outputPin.getNetId()) == 1 * netToVoltageMap_.find(inputPinMinus.getNetId()));
		}
	}

	void SimulationSpace::createStageBiasVoltagePinsConstraints()
	{
		if(getPartitioningResult().hasPrimarySecondStage() && !getPartitioningResult().hasSecondarySecondStage())
		{
			std::vector<Partitioning::BiasPart*> firstStageBiases = getPartitioningResult().getFirstStage().getBiasPart();
			std::vector<Partitioning::Component*> supplyFirstStageBiasComps;
			for(auto & biasPartFirstStage : firstStageBiases)
			{
				std::vector<Partitioning::Component*> comps = getPartitioningResult().getBelongingComponents(*biasPartFirstStage);
				for(auto & comp: comps)
				{
					if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Source")).isSupply())
					{
						supplyFirstStageBiasComps.push_back(comp);
					}
				}
			}

			std::vector<Partitioning::BiasPart*> secondStageBiases = (**getPartitioningResult().getSecondStages().begin()).getBiasPart();
			std::vector<Partitioning::Component*> supplySecondStageBiasComps;
			for(auto & biasPartSecondStage : secondStageBiases)
			{
				std::vector<Partitioning::Component*> comps = getPartitioningResult().getBelongingComponents(*biasPartSecondStage);
				for(auto & comp: comps)
				{
					if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Source")).isSupply())
					{

						supplySecondStageBiasComps.push_back(comp);
					}
				}
			}

			if((**supplyFirstStageBiasComps.begin()).getArray().getTechType() ==(**supplySecondStageBiasComps.begin()).getArray().getTechType())
			{
				Core::PinType gatePin = Core::PinType(Core::DeviceTypeName("Mosfet"),Core::PinName( "Gate"));
				const StructRec::StructureCircuit & arrayCircuit = getStructureRecognitionResult().findStructureCircuit(0);

				Partitioning::Component & biasCompFirstStage = **supplyFirstStageBiasComps.begin();
				const Core::Device & deviceFirstStageBias = **biasCompFirstStage.getArray().getDevices().begin();
				Core::NetId gateNetBiasCompFirstStage = deviceFirstStageBias.findNet(gatePin).getIdentifier();

				for(auto & biasCompSecondStage : supplySecondStageBiasComps)
				{
					const Core::Device & deviceSecondStageBias = **biasCompSecondStage->getArray().getDevices().begin();
					Core::NetId gateNetBiasCompSecondStage = deviceSecondStageBias.findNet(gatePin).getIdentifier();
					if(arrayCircuit.findStructureNet(gateNetBiasCompSecondStage).getAllConnectedStructures().size() ==1)
					{
						Gecode::rel(*this, netToVoltageMap_.find(gateNetBiasCompSecondStage) == 1 * netToVoltageMap_.find(gateNetBiasCompFirstStage));
					}
				}
			}

		}
	}



	void SimulationSpace::createKCLConstraints()
	{
		SimulationKCLConstraints kclConstraints;
		kclConstraints.setCircuitParameter(getCircuitInformation().getCircuitParameter());
		kclConstraints.setGraph(getGraph());
		kclConstraints.setSpace(*this);
		kclConstraints.setTransistorToCurrentMap(transistorToCurrentMap_);
		kclConstraints.setUsesHspiceLibrary(usesHSpiceLibrary());
		kclConstraints.createConstraints();
	}




	void SimulationSpace::createTransistorConstraints()
	{
		if(transistorModel_ == "SHM")
		{
			SimulationTransistorConstraintsSHM transistorConstraints;
			transistorConstraints.setCircuitInformation(getCircuitInformation());
			transistorConstraints.setGraph(getGraph());
			transistorConstraints.setNetToVoltageMap(netToVoltageMap_);
			transistorConstraints.setPartitioningResult(getPartitioningResult());
			transistorConstraints.setSpace(*this);
			transistorConstraints.setTransistorToCurrentMap(transistorToCurrentMap_);
			transistorConstraints.setTransistorToLengthMap(transistorToLengthInputMap_);
			transistorConstraints.setTransistorToWidthMap(transistorToWidthInputMap_);
			transistorConstraints.setTransistorToMultiplierMap(transistorToMultiplierInputMap_);
			transistorConstraints.createConstraints();
		}
		else
		{
			SimulationTransistorConstraintsEKV transistorConstraints;
			transistorConstraints.setCircuitInformation(getCircuitInformation());
			transistorConstraints.setGraph(getGraph());
			transistorConstraints.setNetToVoltageMap(netToVoltageMap_);
			transistorConstraints.setPartitioningResult(getPartitioningResult());
			transistorConstraints.setSpace(*this);
			transistorConstraints.setTransistorToCurrentMap(transistorToCurrentMap_);
			transistorConstraints.setTransistorToLengthMap(transistorToLengthInputMap_);
			transistorConstraints.setTransistorToWidthMap(transistorToWidthInputMap_);
			transistorConstraints.setTransistorToMultiplierMap(transistorToMultiplierInputMap_);
			transistorConstraints.setEKVVersion(getEKVVersion());
			transistorConstraints.createConstraints();
		}
	}



	bool SimulationSpace::hasCircuitInformation() const
	{
		return circuitInformation_ != NULL;
	}

	bool SimulationSpace::hasGraph() const
	{
		return graph_ != NULL;
	}

	bool SimulationSpace::hasResult() const
	{
		return result_ != NULL;
	}

	const CircuitInformation& SimulationSpace::getCircuitInformation() const
	{
		assert(hasCircuitInformation());
		return * circuitInformation_;
	}

	const Graph& SimulationSpace::getGraph() const
	{
		assert(hasGraph());
		return * graph_;
	}

	void SimulationSpace::setStructureRecognitionResult(
			const StructRec::StructureCircuits& structRecResult)
	{
		structRecResult_ = & structRecResult;
	}

	const StructRec::StructureCircuits& SimulationSpace::getStructureRecognitionResult() const
	{
		assert(structRecResult_ != NULL);
		return * structRecResult_;
	}



	const Result & SimulationSpace::computeResult()
	{ 
		Result * result = new Result;
		result->initializeTransistorMap(getPartitioningResult());
		result->initializeTwoPortMap(getPartitioningResult());
        
		transistorToWidthInputMap_.writeWidthToTransistor(*result);
		transistorToMultiplierInputMap_.writeMultiplierToTransistor(*result);
		transistorToLengthInputMap_.writeLengthToTransistor(*result);
		twoPortToValueInputMap_.writeValueToCapacitance(*result);
        
		netToVoltageMap_.write(*result);
		transistorToCurrentMap_.writeCurrent(*result);
		logDebug("### Found Currents and Voltages Maps ###");
		logDebug("### Voltages Map ###");
		logDebug(netToVoltageMap_.toStr());
		logDebug("### Currents Map ###");
		logDebug(transistorToCurrentMap_.toStr());


		logDebug("#### Performance Calculation ####");
		PerformanceCalculation performanceCalculation;

		performanceCalculation.setTwoPortToValueInputMap(twoPortToValueInputMap_);
		performanceCalculation.setCircuitInformation(getCircuitInformation());
		performanceCalculation.setGraph(getGraph());
		performanceCalculation.setNetToVoltageMap(netToVoltageMap_);
		

		performanceCalculation.setPartitioningResult(getPartitioningResult());
		performanceCalculation.setSpace(*this);
		performanceCalculation.setTransistorToCurrentMap(transistorToCurrentMap_);


		performanceCalculation.setTransistorToLengthInputMap(transistorToLengthInputMap_);
		performanceCalculation.setTransistorToWidthInputMap(transistorToWidthInputMap_);
		performanceCalculation.setTransistorToMultiplierInputMap(transistorToMultiplierInputMap_);
	
		performanceCalculation.setTransistorModel(getTransistorModel());
		performanceCalculation.setEKVVersion(getEKVVersion());
		performanceCalculation.setStructureRecognitionResult(getStructureRecognitionResult());
		performanceCalculation.computePerformance();

		result->setGain(performanceCalculation.getGain());
		logDebug("GAIN");
		logDebug(performanceCalculation.getGain());

		result->setPower(performanceCalculation.getPowerConsumption());
		logDebug("POWER");
		logDebug(performanceCalculation.getPowerConsumption());

		result->setTransitFrequency(performanceCalculation.getTransitFrequency());
		logDebug("TransitFrequency");
		logDebug(performanceCalculation.getTransitFrequency());		
		
		result->setTransitFrequencyWithErrorFactor(performanceCalculation.getTransitFrequencyWithErrorFactor());
		logDebug("TransitFrequencyWithErrorFactor");
		logDebug(performanceCalculation.getTransitFrequencyWithErrorFactor());		
		result->setArea(performanceCalculation.getArea());
		logDebug("AREA");
		logDebug(performanceCalculation.getArea());	

		result->setDominantPole(performanceCalculation.getDominantPole());
		logDebug("DOMINANTPOLE");
        logDebug(performanceCalculation.getDominantPole());	
		
		result->setPositiveZero(performanceCalculation.getPositiveZero());
        logDebug("POSITIVEZERO");
        logDebug(performanceCalculation.getPositiveZero());

        result->setImportantNonDominantPoles(performanceCalculation.getImportantNonDominantPoles());
        logDebug("IMPORTANTNONDOMINANTPOLES");
        for (const auto& pole : performanceCalculation.getImportantNonDominantPoles()) {
           logDebug(pole);
        }

	    result->setImportantZeros(performanceCalculation.getImportantZeros());
        logDebug("IMPORTANTZEROS");
        for (const auto& zero : performanceCalculation.getImportantZeros()) {
           logDebug(zero);
        }
	
		result->setPhaseMargin(performanceCalculation.getPhaseMargin());
		
		logDebug("PHASEMARGIN");
		logDebug(performanceCalculation.getPhaseMargin());			
		result->setSlewRate(performanceCalculation.getSlewRate());
		logDebug("SLEWRATE");
		logDebug(performanceCalculation.getSlewRate());		
		result->setCMRR(performanceCalculation.getCMRR());
		logDebug("CMRR");
		logDebug(performanceCalculation.getCMRR());			
		result->setNegPSRR(performanceCalculation.getNegPSRR());
		result->setPosPSRR(performanceCalculation.getPosPSRR());
		logDebug("NegPSRR");
		logDebug(performanceCalculation.getNegPSRR());
		logDebug("PosPSRR");
		logDebug(performanceCalculation.getPosPSRR());						
		result->setMaxOutputVoltage(performanceCalculation.getMaxOutputVoltage());
		result->setMinOutputVoltage(performanceCalculation.getMinOutputVoltage());
		logDebug("MaxOutputVoltage");
		logDebug(performanceCalculation.getMaxOutputVoltage());
		logDebug("MinOutputVoltage");
		logDebug(performanceCalculation.getMinOutputVoltage());		
		result->setMinCommonModeInputVoltage(performanceCalculation.getMinCommonModeInputVoltage());
		logDebug("MinCommonModeInputVoltage");
		logDebug(performanceCalculation.getMinCommonModeInputVoltage());		
		result->setMaxCommonModeInputVoltage(performanceCalculation.getMaxCommonModeInputVoltage());
		logDebug("MaxCommonModeInputVoltage");
		logDebug(performanceCalculation.getMaxCommonModeInputVoltage());


		// Intermediate performance values
		result->setOutputResistanceFirstStage(performanceCalculation.getOutputResistanceFirstStage());
		logDebug("OutputResistanceFirstStage");
		logDebug(performanceCalculation.getOutputResistanceFirstStage());

		result->setOutputResistancePrimarySecondStage(performanceCalculation.getOutputResistancePrimarySecondStage());
		logDebug("OutputResistancePrimarySecondStage");
		logDebug(performanceCalculation.getOutputResistancePrimarySecondStage());

		result->setOutputResistanceSecondarySecondStage(performanceCalculation.getOutputResistanceSecondarySecondStage());
		logDebug("OutputResistanceSecondarySecondStage");
		logDebug(performanceCalculation.getOutputResistanceSecondarySecondStage());		

		result->setOutputResistanceThirdStage(performanceCalculation.getOutputResistanceThirdStage());
		logDebug("OutputResistanceThirdStage");
		logDebug(performanceCalculation.getOutputResistanceThirdStage());

		result->setGainFirstStage(performanceCalculation.getGainFirstStage());
		logDebug("GainFirstStage");
		logDebug(performanceCalculation.getGainFirstStage());

		result->setGainSecondStage(performanceCalculation.getGainSecondStage());
		logDebug("GainSecondStage");
		logDebug(performanceCalculation.getGainSecondStage());

		result->setGainThirdStage(performanceCalculation.getGainThirdStage());
		logDebug("GainThirdStage");
		logDebug(performanceCalculation.getGainThirdStage());

		result->setFirstStageTransconductance(performanceCalculation.getFirstStageTransconductance());
		logDebug("FirstStageTransconductance");
		logDebug(performanceCalculation.getFirstStageTransconductance());

		result->setPrimarySecondStageTransconductance(performanceCalculation.getPrimarySecondStageTransconductance());
		logDebug("PrimarySecondStageTransconductance");
		logDebug(performanceCalculation.getPrimarySecondStageTransconductance());

		result->setSecondarySecondStageTransconductance(performanceCalculation.getSecondarySecondStageTransconductance());
		logDebug("SecondarySecondStageTransconductance");
		logDebug(performanceCalculation.getSecondarySecondStageTransconductance());

		result->setThirdStageTransconductance(performanceCalculation.getThirdStageTransconductance());
		logDebug("ThirdStageTransconductance");
		logDebug(performanceCalculation.getThirdStageTransconductance());

		result->setOutputConductanceLoadPartFirstStage(performanceCalculation.getOutputConductanceLoadPartFirstStage());
		logDebug("OutputConductanceLoadPartFirstStage");
		logDebug(performanceCalculation.getOutputConductanceLoadPartFirstStage());

		result->setOutputConductanceBiasPartFirstStage(performanceCalculation.getOutputConductanceBiasPartFirstStage());
		logDebug("OutputConductanceBiasPartFirstStage");
		logDebug(performanceCalculation.getOutputConductanceBiasPartFirstStage());

		result->setOutputConductanceLoadPartPrimarySecondStage(performanceCalculation.getOutputConductanceLoadPartPrimarySecondStage());
		logDebug("OutputConductanceLoadPartPrimarySecondStage");
		logDebug(performanceCalculation.getOutputConductanceLoadPartPrimarySecondStage());

		result->setOutputConductanceBiasPartPrimarySecondStage(performanceCalculation.getOutputConductanceBiasPartPrimarySecondStage());
		logDebug("OutputConductanceBiasPartPrimarySecondStage");
		logDebug(performanceCalculation.getOutputConductanceBiasPartPrimarySecondStage());

		result->setOutputConductanceLoadPartSecondarySecondStage(performanceCalculation.getOutputConductanceLoadPartSecondarySecondStage());
		logDebug("OutputConductanceLoadPartSecondarySecondStage");
		logDebug(performanceCalculation.getOutputConductanceLoadPartSecondarySecondStage());

		result->setOutputConductanceBiasPartSecondarySecondStage(performanceCalculation.getOutputConductanceBiasPartSecondarySecondStage());
		logDebug("OutputConductanceBiasPartSecondarySecondStage");
		logDebug(performanceCalculation.getOutputConductanceBiasPartSecondarySecondStage());

		result->setOutputConductanceLoadPartThirdStage(performanceCalculation.getOutputConductanceLoadPartThirdStage());
		logDebug("OutputConductanceLoadPartThirdStage");
		logDebug(performanceCalculation.getOutputConductanceLoadPartThirdStage());

		result->setOutputConductanceBiasPartThirdStage(performanceCalculation.getOutputConductanceBiasPartThirdStage());
		logDebug("OutputConductanceBiasPartThirdStage");
		logDebug(performanceCalculation.getOutputConductanceBiasPartThirdStage());

		result->setNetCapacitances(performanceCalculation.getNetcapacitances());
		logDebug("NetCapacitances");
		for (const auto& cap : performanceCalculation.getNetcapacitances()) {
			logDebug("NetId: " + cap.first.toStr() + ", Value: " + std::to_string(cap.second));
		}

		result->setFirstStageComponentTransconductancesArray(performanceCalculation.getFirstStageComponentTransconductancesArray());
		logDebug("FirstStageComponentTransconductances");
		for (const auto& trans : performanceCalculation.getFirstStageComponentTransconductancesArray()) {
			logDebug("Component: " + trans.first + ", Value: " + std::to_string(trans.second));
		}

		result->setPrimarySecondStageComponentTransconductancesArray(performanceCalculation.getPrimarySecondStageComponentTransconductancesArray());
		logDebug("PrimarySecondStageComponentTransconductances");
		for (const auto& trans : performanceCalculation.getPrimarySecondStageComponentTransconductancesArray()) {
			logDebug("Component: " + trans.first + ", Value: " + std::to_string(trans.second));
		}

		result->setSecondarySecondStageComponentTransconductancesArray(performanceCalculation.getSecondarySecondStageComponentTransconductancesArray());
		logDebug("SecondarySecondStageComponentTransconductances");
		for (const auto& trans : performanceCalculation.getSecondarySecondStageComponentTransconductancesArray()) {
			logDebug("Component: " + trans.first + ", Value: " + std::to_string(trans.second));
		}

		result->setThirdStageComponentTransconductancesArray(performanceCalculation.getThirdStageComponentTransconductancesArray());
		logDebug("ThirdStageComponentTransconductances");
		for (const auto& trans : performanceCalculation.getThirdStageComponentTransconductancesArray()) {
			logDebug("Component: " + trans.first + ", Value: " + std::to_string(trans.second));
		}

		result->setComponentOutputConductancesArrayFirstStage(performanceCalculation.getComponentOutputConductancesArrayFirstStage());
		logDebug("FirstStageComponentOutputConductances");
		for (const auto& cond : performanceCalculation.getComponentOutputConductancesArrayFirstStage()) {
			logDebug("Component: " + cond.first + ", Value: " + std::to_string(cond.second));
		}

		result->setComponentOutputConductancesArrayPrimarySecondStage(performanceCalculation.getComponentOutputConductancesArrayPrimarySecondStage());
		logDebug("PrimarySecondStageComponentOutputConductances");
		for (const auto& cond : performanceCalculation.getComponentOutputConductancesArrayPrimarySecondStage()) {
			logDebug("Component: " + cond.first + ", Value: " + std::to_string(cond.second));
		}

		result->setComponentOutputConductancesArraySecondarySecondStage(performanceCalculation.getComponentOutputConductancesArraySecondarySecondStage());
		logDebug("SecondarySecondStageComponentOutputConductances");
		for (const auto& cond : performanceCalculation.getComponentOutputConductancesArraySecondarySecondStage()) {
			logDebug("Component: " + cond.first + ", Value: " + std::to_string(cond.second));
		}

		result->setComponentOutputConductancesArrayThirdStage(performanceCalculation.getComponentOutputConductancesArrayThirdStage());
		logDebug("ThirdStageComponentOutputConductances");
		for (const auto& cond : performanceCalculation.getComponentOutputConductancesArrayThirdStage()) {
			logDebug("Component: " + cond.first + ", Value: " + std::to_string(cond.second));
		}	
				

		return * result;
	}

       


	void SimulationSpace::setTransistorModel(std::string transistorModel)
	{
		transistorModel_ = transistorModel;
	}

	void SimulationSpace::setEKVVersion(int version)
	{
		ekvVersion_ = version;
	}

	void SimulationSpace::setDefinition(std::string definition)
	{
		definition_ = definition;
	}

	void SimulationSpace::setUsesHSpiceLibrary(bool usesIt)
	{
		usesHSpiceLibrary_ = usesIt;
	}




    void SimulationSpace::setPathToCicuitNetlist(std::string path)
	{
		pathToCicuitNetlist_ = path;
	}

	std::string SimulationSpace::getTransistorModel() const
	{
		assert(transistorModel_ == "SHM" || transistorModel_ == "EKV", "Transistor model not supported");
		return transistorModel_;
	}

	int SimulationSpace::getEKVVersion() const
	{
		assert(ekvVersion_ == 1 || ekvVersion_ == 2 || ekvVersion_ == 3, "EKV version not supported");
		return ekvVersion_;
	}

	Variables & SimulationSpace::getVariables()
	{
		assert(variablesMap_ != nullptr);
		return * variablesMap_;
	}
	const Variables & SimulationSpace::getVariables() const
	{
		assert(variablesMap_ != nullptr);
		return * variablesMap_;
	}



	float SimulationSpace::getScalingFactorMUM() const
	{
		assert(scalingFactorMUM_ != NOT_INITIALIZED_);
		return scalingFactorMUM_;
	}

	void SimulationSpace::initializeScalingValues()
	{
		if(definition_ == "1mum")
		{

			scalingFactorMUM_ = pow(10,-6);
		}
		if(definition_ == "0.1mum")
		{

			scalingFactorMUM_ = pow(10,-7);
		}
	}

	std::string SimulationSpace::getDefinition() const
	{
		assert(definition_ == "1mum" || definition_ == "0.1mum",  "Definition not supported");
		return definition_;
	}

	bool SimulationSpace::isOutputNetStage(const StructRec::StructureNet & net, Partitioning::TransconductancePart & stage) const
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

	std::vector<const StructRec::StructureNet *> SimulationSpace::findOutputNets(Partitioning::TransconductancePart & stage) const
	{
		assert(stage.isFirstStage() || stage.isPrimarySecondStage() || stage.isSecondarySecondStage()|| stage.isThirdStage() , "Transconductance might only be a helper Structure");

		std::vector<const StructRec::StructureNet *> outputNets;

		if((stage.isPrimarySecondStage() || stage.isThirdStage()) && getPartitioningResult().getBelongingComponents(stage).size() > 1)
		{
			std::vector<Partitioning::Component *> compsStage = getPartitioningResult().getBelongingComponents(stage);
			for(auto & comp : compsStage)
			{
				if(!comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply())
				{
					outputNets.push_back(&comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Drain")));
					break;
				}
			}
			assert(!outputNets.empty(), "There should be at least one transistor in the second stage NOT connected to supply");
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
		else
		{
			Partitioning::Component & compStage = **getPartitioningResult().getBelongingComponents(stage).begin();
			outputNets.push_back(&compStage.getArray().findNet(StructRec::StructurePinType(compStage.getArray().getStructureName(), "Drain")));
		}
		return outputNets;
	}




	Gecode::FloatVar SimulationSpace::createFloatCurrent(Gecode::IntVar intCurrent)
	{
		Gecode::FloatVar channeledCurrent = createChanneledFloatVar(intCurrent, -10000000000, 10000000000);
		return expr(*this, channeledCurrent * pow(10,-9));
	}

	Gecode::FloatVar SimulationSpace::createFloatVoltage(Gecode::IntVar intVoltage)
	{
		int vdd = getCircuitInformation().getCircuitParameter().getSupplyVoltage() * 1000;
		int vss = getCircuitInformation().getCircuitParameter().getGroundVoltage() *1000;
		Gecode::FloatVar channeledVoltage = createChanneledFloatVar(intVoltage, vss, vdd);
		return expr(*this, channeledVoltage * pow(10,-3));
	}

	Gecode::FloatVar SimulationSpace::createChanneledFloatVar(Gecode::IntVar integer, long int lb, long int ub)
	{
		Gecode::FloatVar helperVar(*this, lb, ub);
		channel(*this, integer, helperVar);

		return helperVar;

	}

	


	bool SimulationSpace::usesHSpiceLibrary() const
	{
		return usesHSpiceLibrary_;
	}
}
  
