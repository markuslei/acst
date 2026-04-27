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

#ifndef SIMULATION_INCL_SimulationCSP_SIMULATION_H_
#define SIMULATION_INCL_SimulationCSP_SIMULATION_H_

#include "Partitioning/incl/Partitioning.h"
#include "Partitioning/incl/Results/Result.h"
#include "Simulation/incl/CircuitInformation/CircuitInformation.h"
#include "Simulation/incl/SimulationCSP/Graph/Graph.h"

#include "StructRec/incl/Results/StructureCircuits.h"

#include <sstream>

#include "SimpleSimulationSearchSpace.h"

namespace Simulation {

	class Simulation {
	public:
		Simulation();
		~Simulation();

		void setPartitioningResult(const Partitioning::Result& result);
		void setStructureRecognitionResult(const StructRec::StructureCircuits & structRecResult);
		void setCircuitInformation(CircuitInformation & circuitInformation);
		void setTransistorModel(std::string model);
		void setEKVVersion(int version);
		void setDefinition(std::string definition);
		void setRuntime(int runtime);

		void setPathToCicuitNetlist(std::string path);

		const Result & Simulate();

	private:
		Graph & computeGraph();


		void initializeSearchSpace();
		SimpleSimulationSearchSpace &computeGecodeSolution();

	private:

		static const int NOT_INITIALIZED_;

		const Partitioning::Result & getPartitioningResult() const;
		Graph & getGraph();
		const StructRec::StructureCircuits & getStructureRecognitionResult() const;
		CircuitInformation & getCircuitInformation();

		std::string getTransistorModel() const;
		int getEKVVersion() const;
		std::string getDefinition() const;

		int getRuntime() const;

		const Partitioning::Result * partitioningResult_;
		Graph * graph_;
		CircuitInformation* circuitInformation_;
		const StructRec::StructureCircuits* structRecResult_;
		SimpleSimulationSearchSpace  searchSpace_;
		std::string transistorModel_;
		int ekvVersion_;
		std::string definition_;
		int runtime_;
		std::string pathToCicuitNetlist_;


	};



}



#endif /* SIMULATION_INCL_SIMULATION_SIMULATION_H_ */
