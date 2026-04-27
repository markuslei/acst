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


#ifndef SIMULATION_INCL_SimulationCSP_SIMULATIONSPACE_H_
#define SIMULATION_INCL_SimulationCSP_SIMULATIONSPACE_H_

#include "Simulation/incl/SimulationCSP/NetToIntVarMap.h"
#include "Simulation/incl/SimulationCSP/NetToIntVarInputMap.h"

#include "StructRec/incl/Results/StructureCircuits.h"
#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureId.h"
#include "Simulation/incl/SimulationCSP/Constraints/SimulatedPolesAndZeros.h"

#include <gecode/int.hh>
#include <gecode/float.hh>
#include <gecode/minimodel.hh>
#include "ComponentToIntVarMap.h"
#include "ComponentToIntVarInputMap.h"

namespace Partitioning{

	class Result;
	class TransconductancePart;

}

namespace Simulation {

	class Variables;
	class CircuitInformation;
	class Graph;
	class Result;
	class Pin;
	class Node;
	class Edge;
	class SimulatedPolesAndZeros;
	class Transistor;

	class SimulationSpace : public Gecode::Space
	{
	public:
		SimulationSpace();
		~SimulationSpace();
		SimulationSpace(SimulationSpace & other);

		void setResult(const Partitioning::Result & result);
		void setGraph(const Graph & graph);
		void setCircuitInformation(const CircuitInformation & information);
		void setStructureRecognitionResult(const StructRec::StructureCircuits & structRecResult);
		void setTransistorModel(std::string transistorModel);
		void setEKVVersion(int version);
		void setDefinition(std::string definition);
		void setUsesHSpiceLibrary(bool usesIt);
		void setPathToCicuitNetlist(std::string path);

		
		void setHelper(Gecode::FloatVar helper);


		

		const Result &  computeResult();

		void initialize();
		//void initializeTwoPortMap(const Partitioning::Result & partitioningResult);

		virtual SimulationSpace * copy() = 0;

		//virtual void constrain(const Gecode::Space& other) = 0;
		//virtual bool master(const Gecode::MetaInfo & mi);

		std::string toStr() const;
		std::string printIndexToVariableMap() const;

		std::string getDefinition() const;
		float getScalingFactorMUM() const;
        SimulatedPolesAndZeros & getPolesAndZeros();
		void setPolesAndZeros(SimulatedPolesAndZeros & polesAndZeros);

		Gecode::FloatVar createChanneledFloatVar(Gecode::IntVar integer, long int lb, long int ub);

		float createChannelednormalFloatVar(int integer, long int lb, long int ub);
		Gecode::FloatVar createFloatCurrent(Gecode::IntVar intCurrent);
		Gecode::FloatVar createFloatVoltage(Gecode::IntVar intVoltage);

		float createnormalFloatVoltage(int intVoltage);

		void print() const;
		void eraseVariables() const;
		void parseVoltageMap();
		void parseCurrentMap();

		void initializePolesAndZeros();
		

	protected:
		void createMaps();
		
		

		const CircuitInformation & getCircuitInformation() const;
		const Graph & getGraph() const;
		const Partitioning::Result & getPartitioningResult() const;
		const StructRec::StructureCircuits & getStructureRecognitionResult() const;
		std::string getTransistorModel() const;
		int getEKVVersion() const;

		Variables & getVariables();

	private:
		void initializeBranching();
		void initializeScalingValues();
		void initializeTransistorMap();
		void initializeVariableMap();

		void createVariables();

		void createIndexes();
		void createTransistorVariableMaps();
		void createTransistorVariableIndexMaps(int & index);
		void createWidthMap();
		void createMultiplierMap();


		void createLengthMap();


		void createWidthIndexMap(int & index);
		void createMultiplierIndexMap(int & index);
		void createLengthIndexMap(int & index);
		void createTwoPortValueMap();

		void createTwoPortIndexMap(int & index);
		void createResistorToValueMap();
		void createVoltageMap();
		void createCurrentMap();;

		void createCurrentIndexMap(int & index);
		void createVoltageIndexMap(int & index);


		void createConstraints();
		void createDomainConstraints();
		void createVoltageConstraints();
		void createKCLConstraints();
		void createTransistorConstraints();

		void createVoltageConstraints(const Pin & pins);
		void createStageBiasVoltagePinsConstraints();
		void createOutputVoltageConstraint();


		void branchComponent(Partitioning::Component & component);
		void branchBiasInputTransistor();
		bool hasCircuitInformation() const;
		bool hasGraph() const;
		bool hasResult() const;

		const Variables & getVariables() const;


		bool isOutputNetStage(const StructRec::StructureNet & net, Partitioning::TransconductancePart & stage) const;
		std::vector<const StructRec::StructureNet*> findOutputNets(Partitioning::TransconductancePart & stage) const;
		bool usesHSpiceLibrary() const;

		void setNetIndexOfTransistors(Core::NetId netId, int & index);
		



		//only for testing:
		//void setWidthAndLength();


	protected:
    static const float NOT_INITIALIZED_;
	protected:
	    SimulatedPolesAndZeros * polesAndZeros_;
		const Partitioning::Result* result_;
		const StructRec::StructureCircuits* structRecResult_;
		const CircuitInformation * circuitInformation_;
		const Graph * graph_;
		Variables * variablesMap_;

		std::string transistorModel_;
		int ekvVersion_;
		std::string definition_;


		ComponentToIntVarInputMap transistorToWidthInputMap_;
		ComponentToIntVarInputMap transistorToMultiplierInputMap_;

		ComponentToIntVarInputMap transistorToLengthInputMap_;


		ComponentToIntVarInputMap twoPortToValueInputMap_;
		ComponentToIntVarMap transistorToCurrentMap_;


		NetToIntVarMap netToVoltageMap_;
		



		float scalingFactorMUM_;

		bool usesHSpiceLibrary_;
        
		std::string pathToCicuitNetlist_;

		//Order: Lengths, Widths, Multiplier, Currents, Voltages,
		Gecode::IntVarArray variables_;


		Gecode::FloatVar helper_;
		std::map<int, std::string> indexToVariableMap_;


	};

}




#endif /* SIMULATION_INCL_ANALYSIS_GECODESYNTHESISSPACE_H_ */