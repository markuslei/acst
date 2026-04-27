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


#include "Simulation/incl/SimulationCSP/Constraints/SimulationKCLConstraints.h"

#include "../../../incl/SimulationCSP/ComponentToIntVarMap.h"
#include "Simulation/incl/SimulationCSP/Graph/Node.h"
#include "Simulation/incl/SimulationCSP/Graph/Graph.h"
#include "Simulation/incl/SimulationCSP/Graph/Edge.h"
#include "Simulation/incl/CircuitInformation/CircuitParameter.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Simulation {

	SimulationKCLConstraints::SimulationKCLConstraints() :
			space_(NULL),
			circuitParameter_(NULL),
			graph_(NULL),
			transistorToCurrentMap_(NULL),
			usesHspiceLibrary_(false)
	{
	}

	void SimulationKCLConstraints::setGraph(const Graph& graph)
	{
		graph_ = & graph;
	}

	void SimulationKCLConstraints::setCircuitParameter(
		const CircuitParameter& parameter)
	{
		circuitParameter_ = & parameter;
	}

	void SimulationKCLConstraints::setSpace(
		Gecode::Space& space)
	{
		space_ = & space;
	}

	void SimulationKCLConstraints::setTransistorToCurrentMap(
		ComponentToIntVarMap& transistorToCurrentMap)
	{
		transistorToCurrentMap_ = & transistorToCurrentMap;
	}

	void SimulationKCLConstraints::setUsesHspiceLibrary(bool usesHspiceLibrary)
	{
		usesHspiceLibrary_ = usesHspiceLibrary;
	}

	void SimulationKCLConstraints::createConstraints()
	{
		const std::vector<Node*> nodes = getGraph().getAllNodes();
		for(std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++)
		{
			Node * node = *it;
			if(getCircuitParameter().isCurrentBiasNet(node->getNetId()))
			{
				createKCLConstraintForBiasNode(*node);
			}
			else if (!getCircuitParameter().isGroundNet(node->getNetId())
					&& ! getCircuitParameter().isSupplyNet(node->getNetId()))
			{
				createKCLConstraint(*node);
			}
		}

	}

	const Graph& SimulationKCLConstraints::getGraph() const
	{
		assert(graph_ != NULL);
		return * graph_;
	}

	const CircuitParameter& SimulationKCLConstraints::getCircuitParameter() const
	{
		assert(circuitParameter_ != NULL);
		return *circuitParameter_;
	}

	Gecode::Space& SimulationKCLConstraints::getSpace()
	{
		assert(space_ != NULL);
		return * space_;
	}

	ComponentToIntVarMap& SimulationKCLConstraints::getTransistorToCurrentMap()
	{
		assert(transistorToCurrentMap_ != NULL);
		return * transistorToCurrentMap_;
	}

	bool SimulationKCLConstraints::usesHspiceLibrary() const
	{
		return usesHspiceLibrary_;
	}

	void SimulationKCLConstraints::createKCLConstraintForBiasNode (
		Node& biasNode)
	{
		const std::set<Edge*> incomingEdges = biasNode.getIncomingEdges();
		const std::set<Edge*> outgoingEdges = biasNode.getOutgoingEdges();
		Gecode::IntVarArgs current(incomingEdges.size() + outgoingEdges.size());
		Gecode::IntArgs sign(incomingEdges.size() + outgoingEdges.size());
		int index = 0;
		if(usesHspiceLibrary())
		{
			for(std::set<Edge*>::const_iterator it_edges = incomingEdges.begin(); it_edges != incomingEdges.end(); it_edges++)
			{
				Edge* edge = *it_edges;
				current[index] = computeEdgeCurrent(*edge);
				sign[index]= -1;
				index++;
			}
			for(std::set<Edge*>::const_iterator it_edges = outgoingEdges.begin(); it_edges != outgoingEdges.end(); it_edges++)
			{
				Edge * edge = *it_edges;
				current[index] = computeEdgeCurrent(*edge);
				sign[index] = 1;
				index++;
			}
			Gecode::linear(getSpace(), sign, current, Gecode::IRT_EQ,  getCircuitParameter().getValue(biasNode.getNetId())* pow(10,9));

		}
		else
		{
			for(std::set<Edge*>::const_iterator it_edges = incomingEdges.begin(); it_edges != incomingEdges.end(); it_edges++)
			{

				Edge* edge = *it_edges;
				current[index] = computeEdgeCurrent(*edge);
				if(edge->getEdgeKey().getComponent().getArray().getTechType().isN())
				{
					sign[index]= -1;
				}
				else
				{
					sign[index]= 1;
				}
				index++;
			}
			for(std::set<Edge*>::const_iterator it_edges = outgoingEdges.begin(); it_edges != outgoingEdges.end(); it_edges++)
			{
				Edge * edge = *it_edges;
				current[index] = computeEdgeCurrent(*edge);
				if(edge->getEdgeKey().getComponent().getArray().getTechType().isN())
				{
					sign[index]= 1;
				}
				else
				{
					sign[index]= -1;
				}
				index++;
			}
			Gecode::linear(getSpace(), sign, current, Gecode::IRT_EQ,  getCircuitParameter().getValue(biasNode.getNetId())* pow(10,9));

		}

	}

	void SimulationKCLConstraints::createKCLConstraint(Node & node)
	{

		const std::set<Edge*> incomingEdges = node.getIncomingEdges();
		const std::set<Edge*> outgoingEdges = node.getOutgoingEdges();
		Gecode::IntVarArgs current(incomingEdges.size() + outgoingEdges.size());
		Gecode::IntArgs sign(incomingEdges.size() + outgoingEdges.size());
		int index = 0;
		for(std::set<Edge*>::const_iterator it_edges = incomingEdges.begin(); it_edges != incomingEdges.end(); it_edges++)
		{
			Edge* edge = *it_edges;
			current[index] = computeEdgeCurrent(*edge);

			sign[index] = -1;
			index++;
		}
		for(std::set<Edge*>::const_iterator it_edges = outgoingEdges.begin(); it_edges != outgoingEdges.end(); it_edges++)
		{
			Edge * edge = *it_edges;

			current[index] = computeEdgeCurrent(*edge);

			sign[index] = 1;
			index++;
		}
//		Gecode::linear(getSpace(), sign, current, Gecode::IRT_EQ, 0.0);
		Gecode::linear(getSpace(), sign, current, Gecode::IRT_GQ, -1);
		Gecode::linear(getSpace(), sign, current, Gecode::IRT_LQ, 1);

	}

	Gecode::IntVar SimulationKCLConstraints::computeEdgeCurrent(Edge & edge)
	{
		if(edge.isDrainSourceEdge())
		{
			return getTransistorToCurrentMap().find(edge.getEdgeKey().getStructureId());
		}
		else
		{
			Gecode::IntVar current(getSpace(), -1,1);
			Gecode::rel(getSpace(), current, Gecode::IRT_EQ, 0);
			return current;
		}
	}


}


