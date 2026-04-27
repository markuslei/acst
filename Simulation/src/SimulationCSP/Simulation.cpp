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


#include "Simulation/incl/SimulationCSP/Simulation.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include "Log/incl/LogMacros.h"
#include <sstream>
#include <gecode/search.hh>



namespace Simulation {

	const int Simulation::NOT_INITIALIZED_ = -100;

	Simulation::Simulation() :
		partitioningResult_(NULL) ,
		circuitInformation_(NULL),
		graph_(NULL),
		structRecResult_(NULL),
		ekvVersion_(NOT_INITIALIZED_),
		runtime_(NOT_INITIALIZED_)
	{
	}

	Simulation::~Simulation()
	{
		delete graph_;
	}

	void Simulation::setPartitioningResult(
			const Partitioning::Result& result)
	{
		partitioningResult_ = & result;
	}

	/*
	const Result & Simulation::compute()
	{
		graph_ = &computeGraph();
		const Result & result = computeSearch();
		return result;
		
	}


	const Result & Simulation::computeSearch()
	{
		
		initializeSearchSpace();
		
		const Result & result = computeGecodeSolution();
		
		return result;
	}

*/
	Graph& Simulation::computeGraph()
	{
		Graph * graph = new Graph;
		graph->setCircuitParameter(getCircuitInformation().getCircuitParameter());
		std::vector<Partitioning::Component*> components = getPartitioningResult().getAllComponents();
		for(std::vector<Partitioning::Component*>::const_iterator it = components.begin(); it != components.end(); it++)
		{
			Partitioning::Component * comp = *it;
			graph->addEdges(*comp);
		}
		graph->equalizeParallelEdges();

		return * graph;
	}


	Graph& Simulation::getGraph()
	{
		assert(graph_ != NULL);
		return *graph_;
	}


	void Simulation::setCircuitInformation(CircuitInformation & circuitInformation)
	{
		circuitInformation_ = & circuitInformation;
	}


	CircuitInformation& Simulation::getCircuitInformation()
	{
		assert(circuitInformation_ != NULL);
		return *circuitInformation_;
	}



	void Simulation::initializeSearchSpace()
	{
	
		searchSpace_.setResult(getPartitioningResult());
		searchSpace_.setCircuitInformation(getCircuitInformation());
		searchSpace_.setGraph(getGraph());
		searchSpace_.setStructureRecognitionResult(getStructureRecognitionResult());
		searchSpace_.setTransistorModel(getTransistorModel());
		searchSpace_.setEKVVersion(getEKVVersion());
		searchSpace_.setDefinition(getDefinition());
		searchSpace_.setPathToCicuitNetlist(pathToCicuitNetlist_);
		searchSpace_.initialize();
	}
   
    
    SimpleSimulationSearchSpace& Simulation::computeGecodeSolution() {
    // Create a stopping condition for the search
    Gecode::Search::Stop* stop = Gecode::Search::Stop::time(getRuntime() * 1000);
    Gecode::Search::Options options;
    options.stop = stop; // Apply the stopping condition

    // Create a search engine with the specified options
    Gecode::DFS<SimpleSimulationSearchSpace> search(&searchSpace_, options);

    int numSolution = 0;

    SimpleSimulationSearchSpace* solution = nullptr;

    while (SimpleSimulationSearchSpace* space = search.next()) {
        logDebug(space->toStr());
        logDebug("Found a solution");
        numSolution++;

        if (solution == nullptr) {
            solution = space; 
			break;
        } else {
            delete space; 
        }
    }

    logDebug("Number of solutions found: " + std::to_string(numSolution));

    assert(solution != nullptr && "No solution found!");

	return *solution;

    }

    
	
	const Result & Simulation::Simulate()

	{
      
		graph_ = &computeGraph();
		initializeSearchSpace();
		logDebug("Compute Currents and Voltages");
		SimpleSimulationSearchSpace & solution = computeGecodeSolution();
		logDebug("Compute Simulation Results");
		const Result& result = solution.computeResult();
		logDebug("Simulation Results");
		return(result);
		//logDebug(result.toStr());
		
		
	}



	void Simulation::setStructureRecognitionResult(
			const StructRec::StructureCircuits& structRecResult)
	{
		structRecResult_ = & structRecResult;
	}


	void Simulation::setPathToCicuitNetlist(std::string path)
	{
		pathToCicuitNetlist_ = path;
	}

	const StructRec::StructureCircuits& Simulation::getStructureRecognitionResult() const
	{
		assert(structRecResult_ != NULL);
		return * structRecResult_;
	}

	const Partitioning::Result & Simulation::getPartitioningResult() const
	{
		assert(partitioningResult_ != NULL);
		return * partitioningResult_;
	}

	void Simulation::setTransistorModel(std::string model)
	{
		transistorModel_ = model;
	}

	std::string Simulation::getTransistorModel() const
	{
		assert(transistorModel_ == "SHM" || transistorModel_ == "EKV", "Transistor model not supported");
		return transistorModel_;
	}

	void Simulation::setEKVVersion(int version)
	{
		ekvVersion_ = version;
	}

	int Simulation::getEKVVersion() const
	{
		assert(ekvVersion_ == 1 || ekvVersion_ == 2 || ekvVersion_ == 3, "EKV version not supported");
		return ekvVersion_;
	}

	void Simulation::setDefinition(std::string definition)
	{
		definition_ = definition;
	}

	std::string Simulation::getDefinition() const
	{
		assert(definition_ == "1mum" || definition_ == "0.1mum", "Definition not supported");
		return definition_;
	}

	void Simulation::setRuntime(int runtime)
	{
		runtime_ = runtime;
	}

	int Simulation::getRuntime() const
	{
		assert(runtime_ != NOT_INITIALIZED_);
		return runtime_;
	}

}


