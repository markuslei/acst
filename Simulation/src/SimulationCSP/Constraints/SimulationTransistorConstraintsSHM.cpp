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


#include "Simulation/incl/SimulationCSP/Constraints/SimulationTransistorConstraintsSHM.h"

#include "Simulation/incl/SimulationCSP/Graph/Graph.h"
#include "Simulation/incl/SimulationCSP/Graph/Edge.h"
#include "Simulation/incl/SimulationCSP/SimulationSpace.h"

#include "Simulation/incl/CircuitInformation/CircuitInformation.h"
#include "Simulation/incl/CircuitInformation/DesignAttributes.h"
#include "Simulation/incl/SimulationCSP/ComponentToIntVarMap.h"
#include "Simulation/incl/SimulationCSP/ComponentToIntVarInputMap.h"
#include "Simulation/incl/CircuitInformation/Pin.h"

#include "Partitioning/incl/Results/Component.h"
#include "Partitioning/incl/Results/Result.h"

#include "Core/incl/Common/BacktraceAssert.h"

#include<math.h>
#include <gecode/float.hh>

namespace Simulation {

	SimulationTransistorConstraintsSHM::SimulationTransistorConstraintsSHM():
			graph_(NULL),
			partitioningResult_(NULL),
			transistorToWidthMap_(NULL),
			transistorToMultiplierMap_(NULL),
			transistorToLengthMap_(NULL),
			transistorToCurrentMap_(NULL),
			netToVoltageMap_(NULL),
			circuitInformation_(NULL),
			space_(NULL)
	{
	}

	void SimulationTransistorConstraintsSHM::setGraph(
		const Graph& graph)
	{
		graph_ = & graph;
	}

	void SimulationTransistorConstraintsSHM::setTransistorToWidthMap(
		ComponentToIntVarInputMap& widthMap)
	{
		transistorToWidthInputMap_ = & widthMap;
	}

	void SimulationTransistorConstraintsSHM::setTransistorToMultiplierMap(
		ComponentToIntVarInputMap& multiplierMap)
	{
		transistorToMultiplierInputMap_ = & multiplierMap;
	}

	void SimulationTransistorConstraintsSHM::setTransistorToLengthMap(
		ComponentToIntVarInputMap& lengthMap)
	{
		transistorToLengthInputMap_ = & lengthMap;
	}

	void SimulationTransistorConstraintsSHM::setTransistorToCurrentMap(
		ComponentToIntVarMap& currentMap)
	{
		transistorToCurrentMap_ = & currentMap;
	}

	void SimulationTransistorConstraintsSHM::setNetToVoltageMap(
		NetToIntVarMap& voltageMap)
	{
		netToVoltageMap_ = & voltageMap;
	}

	void SimulationTransistorConstraintsSHM::setSpace(
		SimulationSpace& space)
	{
		space_ = & space;
	}

	void SimulationTransistorConstraintsSHM::createConstraints()
	{
		std::vector<Partitioning::Transistor*> transistor = getPartitioningResult().getAllTransistors();

		for(std::vector<Partitioning::Transistor*>::const_iterator it =  transistor.begin(); it != transistor.end(); it++)
		{
			Partitioning::Transistor * tran = * it;
			if(isLowerTransistorOf4TransistorCurrentMirror(*tran))
			{
				createLinearConstraints(*tran);
			}
			else if(getPartitioningResult().getPart(tran->getArray()).isResistorPart()
					&& getPartitioningResult().getResistorPart(tran->getArray()).isCompensationResistor())
			{
				createCompensationResistorConstraint(*tran);
			}
			else if(getPartitioningResult().getPart(tran->getArray()).isPositiveFeedbackPart()
					&& getPartitioningResult().getPositiveFeedbackPart(tran->getArray()).hasGateNetsConnectedToBiasOfFoldedPair(getPartitioningResult()))
			{
				createOffConstraint(*tran);
			}
			else
			{
				createSaturationConstraints(*tran);
			}
		}
	}

	const Graph& SimulationTransistorConstraintsSHM::getGraph() const
	{
		assert(graph_ != NULL);
		return * graph_;
	}

	ComponentToIntVarInputMap& SimulationTransistorConstraintsSHM::getTransistorToWidthMap()
	{
		assert(transistorToWidthInputMap_ != NULL);
		return *transistorToWidthInputMap_;
	}

	ComponentToIntVarInputMap& SimulationTransistorConstraintsSHM::getTransistorToMultiplierMap()
	{
		assert(transistorToMultiplierInputMap_ != NULL);
		return * transistorToMultiplierInputMap_;
	}

	ComponentToIntVarInputMap& SimulationTransistorConstraintsSHM::getTransistorToLengthMap()
	{
		assert(transistorToLengthInputMap_ != NULL);
		return * transistorToLengthInputMap_;
	}

	ComponentToIntVarMap& SimulationTransistorConstraintsSHM::getTransistorToCurrentMap()
	{
		assert(transistorToCurrentMap_ != NULL);
		return * transistorToCurrentMap_;
	}

	NetToIntVarMap& SimulationTransistorConstraintsSHM::getNetToVoltageMap()
	{
		assert(netToVoltageMap_ != NULL);
		return * netToVoltageMap_;
	}

	SimulationSpace& SimulationTransistorConstraintsSHM::getSpace()
	{
		assert(space_ != NULL);
		return *space_;
	}

	void SimulationTransistorConstraintsSHM::createSaturationCurrentConstraintsStrongInversion(Partitioning::Component & component)
	{
//		if(component.getArray().getIdentifier().toStr() == "MosfetNormalArray[7]")
		{
		TechnologieSpecificationSHM techSpec;
		float u = getSpace().getScalingFactorMUM();
		Edge * drainSourceEdge = NULL;
		Edge * gateSourceEdge = NULL;
		std::vector<Edge*> edges = getGraph().findToComponentBelongingEdges(component);
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
		Gecode::FloatVar vds = computeEdgeVoltage(*drainSourceEdge);

		Gecode::FloatVar vgs = computeEdgeVoltage(*gateSourceEdge);

		Gecode::FloatVar idsHelperVar = getSpace().createChanneledFloatVar(getTransistorToCurrentMap().find(component), -100000000000, 1000000000);
		Gecode::FloatVar ids = Gecode::expr(getSpace(), idsHelperVar * pow(10,-9));

		//double width = getTransistorToWidthMap().find(component) * u;
        //double length = getTransistorToLengthMap().find(component) * u;

		Gecode::FloatVar width(getSpace(), getTransistorToWidthMap().find(component) * u, getTransistorToWidthMap().find(component) * u);
		Gecode::FloatVar length(getSpace(), getTransistorToLengthMap().find(component) * u, getTransistorToLengthMap().find(component) * u);
		Gecode::FloatVar multiplier(getSpace(), getTransistorToMultiplierMap().find(component), getTransistorToMultiplierMap().find(component));	


		if(component.getArray().getTechType().isN())
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationSHMNmos();
			Gecode::FloatVal muCox = techSpec.getMobilityOxideCapacityCoefficient();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			Gecode::FloatVar lamda(getSpace(), 0.001,0.9);

			Gecode::rel(getSpace(), lamda == techSpec.getChannelLengthCoefficientStrongInversion());

			if(component.getPart().isLoadPart() && getPartitioningResult().getLoadPart(component.getArray()).hasCrossCoupledPair(getPartitioningResult()))
			{
				Gecode::rel(getSpace(), muCox* 0.5 *(width * multiplier /length) * pow(vgs - vth,2)  /* (1+lamda * vds)*/ / ids   <= 1.1);
				Gecode::rel(getSpace(), muCox* 0.5 *(width * multiplier /length) * pow(vgs - vth,2)  /* (1+lamda * vds)*/ / ids   >= 0.9);
			}
			else
			{
				Gecode::rel(getSpace(), muCox* 0.5 *(width * multiplier /length) * pow(vgs - vth,2)  /* (1+lamda * vds)*/ / ids   <= 1.01);
				Gecode::rel(getSpace(), muCox* 0.5 *(width * multiplier /length) * pow(vgs - vth,2)  /* (1+lamda * vds)*/ / ids   >= 0.99);
			}

			if(getPartitioningResult().getFirstStage().hasHelperStructure() && !getPartitioningResult().hasBiasOfFoldedPair())
			{
				Gecode::rel(getSpace(), vgs - vth  < vds);
				rel(getSpace(), abs(vgs-vth) < 0.3);

			}
			else if( !getPartitioningResult().getFirstStage().isComplementary() &&
					((getCircuitInformation().getCircuitParameter().getSupplyVoltage() - getCircuitInformation().getCircuitParameter().getGroundVoltage()) >=3.5))
			{
				if(getPartitioningResult().hasFirstStageGainEnhancer())
				{
					Gecode::rel(getSpace(), vgs - vth  < vds);
				}
				else if(component.getPart().isTransconductancePart() && getPartitioningResult().getTransconductancePart(component.getArray()).isPrimarySecondStage()
					&& isOutputNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Drain")).getIdentifier())
					&& !getCircuitInformation().getCircuitParameter().isVoltageBiasNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Gate")).getIdentifier()))
				{
					Gecode::rel(getSpace(), vgs - vth  +1.4< vds);
				}
				else if(component.getPart().isLoadPart() && getPartitioningResult().getLoadPart(component.getArray()).hasCascodedPair()
					&& isOutputNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Drain")).getIdentifier())
					&& !getCircuitInformation().getCircuitParameter().isVoltageBiasNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Gate")).getIdentifier()))
				{

					Gecode::rel(getSpace(), 1.1 +(vgs - vth) < vds);
				}
				else if(isCurrentBias(component.getArray()) && getPartitioningResult().getFirstStage().hasHelperStructure())
				{
					Gecode::rel(getSpace(), vgs - vth  +0.2< vds);
				}
				else
				{
					Gecode::rel(getSpace(), vgs - vth  < vds);
				}
				rel(getSpace(), abs(vgs-vth) < 0.75);
			}
			else
			{
				Gecode::rel(getSpace(), vgs - vth  < vds);

				if(!getPartitioningResult().getFirstStage().isComplementary())
					rel(getSpace(), abs(vgs-vth) < 0.3);

				if(component.getPart().isLoadPart() && getPartitioningResult().getLoadPart(component.getArray()).hasCascodedPair()
					&& isOutputNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Drain")).getIdentifier() ))
				{
					Gecode::rel(getSpace(), 0.3 +(vgs - vth) > vds);
				}
			}

			Gecode::rel(getSpace(), vds > 0);
			Gecode::rel(getSpace(), vgs - vth > 0);

		}
		else
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationSHMPmos();
			Gecode::FloatVal muCox = techSpec.getMobilityOxideCapacityCoefficient();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();

			Gecode::FloatVar lamda(getSpace(), 0.01,0.9);
			Gecode::rel(getSpace(), lamda == techSpec.getChannelLengthCoefficientStrongInversion());

			if(component.getPart().isLoadPart() && getPartitioningResult().getLoadPart(component.getArray()).hasCrossCoupledPair(getPartitioningResult()))
			{

				Gecode::rel(getSpace(),  -1* muCox* 0.5 *(width * multiplier /length) * pow(vgs - vth,2)  /** (1-lamda * vds) *//ids  <=1.1);
				Gecode::rel(getSpace(),  -1* muCox* 0.5 *(width * multiplier /length) * pow(vgs - vth,2)  /** (1-lamda * vds) *//ids  >= 0.9);
			}
			else
			{
				Gecode::rel(getSpace(),  -1* muCox* 0.5 *(width * multiplier /length) * pow(vgs - vth,2)  /** (1-lamda * vds) *//ids  <=1.01);
				Gecode::rel(getSpace(),  -1* muCox* 0.5 *(width * multiplier /length) * pow(vgs - vth,2)  /** (1-lamda * vds) *//ids  >= 0.99);
			}



			if(getPartitioningResult().getFirstStage().hasHelperStructure() && !getPartitioningResult().hasBiasOfFoldedPair())
			{
				Gecode::rel(getSpace(), (vgs - vth) > vds);
			}
			else if(!getPartitioningResult().getFirstStage().isComplementary() &&
					((getCircuitInformation().getCircuitParameter().getSupplyVoltage() - getCircuitInformation().getCircuitParameter().getGroundVoltage()) >=3.5))
			{
				if(getPartitioningResult().hasFirstStageGainEnhancer())
				{
					Gecode::rel(getSpace(), (vgs - vth) > vds);
				}
				else if(component.getPart().isTransconductancePart() && getPartitioningResult().getTransconductancePart(component.getArray()).isPrimarySecondStage()
					&& isOutputNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Drain")).getIdentifier())
					&& !getCircuitInformation().getCircuitParameter().isVoltageBiasNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Gate")).getIdentifier()))
				{
					Gecode::rel(getSpace(), -1.4 +(vgs - vth) > vds);
				}
				else if(component.getPart().isLoadPart() && getPartitioningResult().getLoadPart(component.getArray()).hasCascodedPair()
					&& isOutputNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Drain")).getIdentifier())
					&& !getCircuitInformation().getCircuitParameter().isVoltageBiasNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Gate")).getIdentifier()))
				{

					Gecode::rel(getSpace(), -1.1 +(vgs - vth) > vds);
				}
				else if(isCurrentBias(component.getArray()) && getPartitioningResult().getFirstStage().hasHelperStructure())
				{
					Gecode::rel(getSpace(), -0.2 +(vgs - vth) > vds);
				}
				else
				{
					Gecode::rel(getSpace(), (vgs - vth) > vds);
				}
				rel(getSpace(), abs(vgs-vth) < 0.75);
			}
			else
			{
				Gecode::rel(getSpace(), (vgs - vth) > vds);

				if(!getPartitioningResult().getFirstStage().isComplementary())
					rel(getSpace(), abs(vgs-vth) < 0.3);
				if(component.getPart().isLoadPart() && getPartitioningResult().getLoadPart(component.getArray()).hasCascodedPair()
					&& isOutputNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Drain")).getIdentifier()))
				{
					if(getCircuitInformation().getDesignAttributes().transconductancesInWeakInversion(getPartitioningResult()))
					{
						Gecode::rel(getSpace(), -0.3 +(vgs - vth) > vds);
					}
					else
					{
						Gecode::rel(getSpace(), -0.3 +(vgs - vth) < vds);
					}
				}
			}
			Gecode::rel(getSpace(), vds < 0);
			Gecode::rel(getSpace(), vgs - vth <= 0);
		}

		}
	}

	void SimulationTransistorConstraintsSHM::createSaturationCurrentConstraintsWeakInversion(Partitioning::Component & component)
	{
		TechnologieSpecificationSHM techSpec = getCircuitInformation().getTechnologieSpecificationSHM(component);
		float Vt = techSpec.getThermalVoltage();
		float vth = techSpec.getThresholdVoltage();
		float n = techSpec.getSlopeFactor();
		float muCox = techSpec.getMobilityOxideCapacityCoefficient();

		float u = getSpace().getScalingFactorMUM();

		Edge * drainSourceEdge = NULL;
		Edge * gateSourceEdge = NULL;
		std::vector<Edge*> edges = getGraph().findToComponentBelongingEdges(component);
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
		Gecode::FloatVar vds = computeEdgeVoltage(*drainSourceEdge);
		Gecode::FloatVar vgs = computeEdgeVoltage(*gateSourceEdge);
		Gecode::FloatVar ids = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(component));
		//velocity

		Gecode::FloatVar velocity(getSpace(), -10000, 1000);
		rel(getSpace(), velocity * 2 * n * Vt == abs(vgs)-fabs(vth));

		//IC
		Gecode::FloatVar inversionCoefficient(getSpace(), 0, 100);
		Gecode::FloatVar exponent(getSpace(), -100, 100);
		rel(getSpace(), exponent == 2* velocity);

		Gecode::exp(getSpace(), exponent, inversionCoefficient);
		rel(getSpace(),inversionCoefficient <= 0.1);
		dom(getSpace(), inversionCoefficient, 0, 100);

		//IdsSpec
		//double width = getTransistorToWidthMap().find(component) * u;
        //double length = getTransistorToLengthMap().find(component) * u;

		Gecode::FloatVar width(getSpace(), getTransistorToWidthMap().find(component) * u, getTransistorToWidthMap().find(component) * u);
		Gecode::FloatVar length(getSpace(), getTransistorToLengthMap().find(component) * u, getTransistorToLengthMap().find(component) * u);
		Gecode::FloatVar multiplier(getSpace(), getTransistorToMultiplierMap().find(component), getTransistorToMultiplierMap().find(component));

		Gecode::FloatVar idsSpec = expr(getSpace(), muCox / (2.0f * n) * (width * multiplier / length) * pow(2.0f * n * Vt, 2.0f));
		dom(getSpace(),idsSpec, 0, 100);


		if(component.getArray().getTechType().isN())
		{

			Gecode::FloatVar idsExact = expr(getSpace(),inversionCoefficient * idsSpec );
			dom(getSpace(), idsExact, 0, 100);

			Gecode::rel(getSpace(), ids <= 1.01 * idsExact);
			Gecode::rel(getSpace(), ids >= 0.99 * idsExact);

			Gecode::rel(getSpace(), vds > 4* Vt);
			Gecode::rel(getSpace(), vgs - vth < 0);
			Gecode::rel(getSpace(), vgs - vth > -0.2);
		}
		else
		{

			Gecode::FloatVar idsExact = expr(getSpace(),inversionCoefficient * idsSpec );
			dom(getSpace(), idsExact, 0, 100);

			Gecode::rel(getSpace(), ids >=  -1* 1.01 * idsExact);
			Gecode::rel(getSpace(), ids <= -1*0.99 *idsExact);

			Gecode::rel(getSpace(), vds < - 4* Vt);
			Gecode::rel(getSpace(), vgs - vth > 0);
			Gecode::rel(getSpace(), vgs - vth < 0.2);
		}
	}


	bool SimulationTransistorConstraintsSHM::isOutputNet(Core::NetId net) const
	{

		if(getCircuitInformation().getCircuitParameter().isFullyDifferential())
		{
			if(net == getCircuitInformation().getCircuitParameter().getOutputPinMinus().getNetId() || net == getCircuitInformation().getCircuitParameter().getOutputPinPlus().getNetId())
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if(net == getCircuitInformation().getCircuitParameter().getOutputPin().getNetId())
			{
				return true;
			}
			else
			{
				return false;
			}
		}

	}

	void SimulationTransistorConstraintsSHM::setCircuitInformation(
		const CircuitInformation& information)
	{
		circuitInformation_ = & information;
	}

	void SimulationTransistorConstraintsSHM::setPartitioningResult(
		const Partitioning::Result & result)
	{
		partitioningResult_ = & result;
	}

	const CircuitInformation& SimulationTransistorConstraintsSHM::getCircuitInformation() const
	{
		assert(circuitInformation_ != NULL);
		return * circuitInformation_;
	}

	const Partitioning::Result& SimulationTransistorConstraintsSHM::getPartitioningResult() const
	{
		assert(partitioningResult_ != NULL);
		return * partitioningResult_;
	}

	void SimulationTransistorConstraintsSHM::createCompensationResistorConstraint(Partitioning::Component & component)
	{
		TechnologieSpecificationSHM techSpec;

		Edge * drainSourceEdge = NULL;
		Edge * gateSourceEdge = NULL;
		std::vector<Edge*> edges = getGraph().findToComponentBelongingEdges(component);
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
		assert(gateSourceEdge != NULL, "No gateSoureEdge found!");
		assert(drainSourceEdge != NULL, "No drainSourceEdge found!");
		Gecode::FloatVar vds = computeEdgeVoltage(*drainSourceEdge);
		Gecode::FloatVar vgs = computeEdgeVoltage(*gateSourceEdge);

		Gecode::FloatVar ids = getSpace().createChanneledFloatVar(getTransistorToCurrentMap().find(component), -1000000000, 1000000000);


		rel(getSpace(), ids == 0);
		rel(getSpace(), vds == 0);


		if(component.getArray().getTechType().isN())
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationSHMNmos();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			Gecode::rel(getSpace(), vgs - vth > 0);
		}
		else
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationSHMPmos();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			Gecode::rel(getSpace(), vgs - vth < 0);
		}


	}

	void SimulationTransistorConstraintsSHM::createOffConstraint(Partitioning::Component & component)
	{
		TechnologieSpecificationSHM techSpec;

		Edge * drainSourceEdge = NULL;
		Edge * gateSourceEdge = NULL;
		std::vector<Edge*> edges = getGraph().findToComponentBelongingEdges(component);
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

		Gecode::FloatVar vgs = computeEdgeVoltage(*gateSourceEdge);
		Gecode::IntVar ids = getTransistorToCurrentMap().find(component);

		rel(getSpace(), ids == 0);

		if(component.getArray().getTechType().isN())
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationSHMNmos();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			Gecode::rel(getSpace(), vgs - vth < 0);
		}
		else
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationSHMPmos();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			Gecode::rel(getSpace(), vgs - vth > 0);
		}
	}

	void SimulationTransistorConstraintsSHM::createLinearCurrentConstraints(Partitioning::Component& component)
	{

		TechnologieSpecificationSHM techSpec;
		float u = getSpace().getScalingFactorMUM();

		Edge * drainSourceEdge = NULL;
		Edge * gateSourceEdge = NULL;
		std::vector<Edge*> edges = getGraph().findToComponentBelongingEdges(component);
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
		assert(gateSourceEdge != NULL, "No gateSoureEdge found!");
		assert(drainSourceEdge != NULL, "No drainSourceEdge found!");
		Gecode::FloatVar vds = computeEdgeVoltage(*drainSourceEdge);
		Gecode::FloatVar vgs = computeEdgeVoltage(*gateSourceEdge);

		Gecode::FloatVar ids = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(component));

		//double width = getTransistorToWidthMap().find(component) * u;
        //double length = getTransistorToLengthMap().find(component) * u;

		Gecode::FloatVar width(getSpace(), getTransistorToWidthMap().find(component) * u, getTransistorToWidthMap().find(component) * u);
		Gecode::FloatVar length(getSpace(), getTransistorToLengthMap().find(component) * u, getTransistorToLengthMap().find(component) * u);
		Gecode::FloatVar multiplier(getSpace(), getTransistorToMultiplierMap().find(component), getTransistorToMultiplierMap().find(component));

		if(component.getArray().getTechType().isN())
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationSHMNmos();
			Gecode::FloatVal muCox = techSpec.getMobilityOxideCapacityCoefficient();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			Gecode::rel(getSpace(), muCox *(width * multiplier /length) * ((vgs - vth)-0.5*vds) * vds  == 1 * ids);
			Gecode::rel(getSpace(), vgs - vth > vds);
			Gecode::rel(getSpace(), vds > 0);
			Gecode::rel(getSpace(), vgs - vth > 0);
		}
		else
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationSHMPmos();
			Gecode::FloatVal muCox = techSpec.getMobilityOxideCapacityCoefficient();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			Gecode::rel(getSpace(), -1* muCox * (width * multiplier /length) * ((vgs - vth) - vds * 0.5) * vds  / ids == 1);
			Gecode::rel(getSpace(), vgs - vth <  vds);
			Gecode::rel(getSpace(), vds < 0);
			Gecode::rel(getSpace(), vgs - vth < 0);
		}
	}


	bool SimulationTransistorConstraintsSHM::isLowerTransistorOf4TransistorCurrentMirror(
			Partitioning::Component& component)
	{
		bool isLowerTransistor = false;
		if(component.getArray().hasParent())
		{
			std::vector<const StructRec::Structure*> topmostParents = component.getArray().getTopmostParents();
			for(auto& it : topmostParents)
			{
				const StructRec::Structure & parent = *it;
				if(parent.getStructureName() == StructRec::StructureName("MosfetFourTransistorCurrentMirror"))
				{
					const StructRec::Pair & fourTransistorCurrentMirror = static_cast<const StructRec::Pair&>(parent);
					const StructRec::Pair & voltageReference1 = static_cast<const StructRec::Pair&>(fourTransistorCurrentMirror.getChild1());
					const StructRec::Pair & currentMirrorLoad = static_cast<const StructRec::Pair&>(fourTransistorCurrentMirror.getChild2());
					const StructRec::Structure & lowerTransistor1 = voltageReference1.getChild2();
					const StructRec::Structure & lowerTransistor2 = currentMirrorLoad.getChild2();
					if(component.getArray() == lowerTransistor1 || component.getArray() == lowerTransistor2)
					{
						isLowerTransistor = true;
					}
				}
			}
		}
		return isLowerTransistor;
	}

	void SimulationTransistorConstraintsSHM::createSaturationConstraints(Partitioning::Component& component)
	{
		if(getCircuitInformation().getDesignAttributes().isInWeakInversion(component))
		{
			createSaturationCurrentConstraintsWeakInversion(component);
		}
		else
		{
			createSaturationCurrentConstraintsStrongInversion(component);
			createGateOverDriveVoltageConstraint(component);
		}

	}

	void SimulationTransistorConstraintsSHM::createGateOverDriveVoltageConstraint(Partitioning::Component & component)
	{
			float u= getSpace().getScalingFactorMUM();
			float vOverDrive = getCircuitInformation().getCircuitSpecification().getGateOverDriveVoltage();

			Gecode::FloatVar ids = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(component));

			//double width = getTransistorToWidthMap().find(component) * u;
            //double length = getTransistorToLengthMap().find(component) * u;

		    Gecode::FloatVar width(getSpace(), getTransistorToWidthMap().find(component) * u, getTransistorToWidthMap().find(component) * u);
		    Gecode::FloatVar length(getSpace(), getTransistorToLengthMap().find(component) * u, getTransistorToLengthMap().find(component) * u);
			Gecode::FloatVar multiplier(getSpace(), getTransistorToMultiplierMap().find(component), getTransistorToMultiplierMap().find(component));	
			TechnologieSpecificationSHM  techSpec;

			if(component.getArray().getTechType().isN())
			{
				techSpec = getCircuitInformation().getTechnologieSpecificationSHMNmos();
			}
			else
			{
				techSpec = getCircuitInformation().getTechnologieSpecificationSHMPmos();
			}

			float muCox = techSpec.getMobilityOxideCapacityCoefficient();

			Gecode::FloatVar widthLengthRatio = Gecode::expr(getSpace(), length * 2.0f / (muCox * width * multiplier));

			Gecode::rel(getSpace(), abs(ids)* widthLengthRatio >= pow(vOverDrive, 2));
	}

	void SimulationTransistorConstraintsSHM::createLinearConstraints(Partitioning::Component& component)
	{
		createLinearCurrentConstraints(component);
	}


	Gecode::FloatVar SimulationTransistorConstraintsSHM::computeEdgeVoltage(
		const Edge& edge)
	{
		const  Core::NetId startNetId = edge.getStartNodeId();
		const Core::NetId endNetId = edge.getEndNodeId();

		Gecode::FloatVar startNetVoltage = getSpace().createFloatVoltage(netToVoltageMap_->find(startNetId));

		Gecode::FloatVar endNetVoltage = getSpace().createFloatVoltage(netToVoltageMap_->find(endNetId));

		return Gecode::expr(getSpace(), startNetVoltage - endNetVoltage);
	}

	void SimulationTransistorConstraintsSHM::createSameRegionConstraintFoldedPair()
	{
		if(getPartitioningResult().hasFirstStage())
		{
			Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
			if(firstStage.hasHelperStructure())
			{
				const StructRec::Structure & foldedPair = firstStage.getHelperStructure();
				std::vector<Partitioning::Component*> compsFoldedPair = getPartitioningResult().findComponents(foldedPair);
				createSameRegionConstraint(**compsFoldedPair.begin(), **compsFoldedPair.rbegin());
			}
		}
	}

	void SimulationTransistorConstraintsSHM::createSameRegionConstraint(Partitioning::Component& transistor1, Partitioning::Component & transistor2)
	{
		Edge * drainSourceEdgeTran1 = NULL;
		Edge * gateSourceEdgeTran1 = NULL;
		std::vector<Edge*> edgesTran1 = getGraph().findToComponentBelongingEdges(transistor1);
		for(std::vector<Edge*>::const_iterator it = edgesTran1.begin(); it != edgesTran1.end(); it++)
		{
			Edge* edge = *it;
			if(edge->isDrainSourceEdge())
			{
				drainSourceEdgeTran1 = edge;
			}
			else if (edge->isGateSourceEdge())
			{
				gateSourceEdgeTran1 = edge;
			}
		}
		assert(drainSourceEdgeTran1 != NULL, "No drainSourceEdge found!");
		if(gateSourceEdgeTran1 == NULL)
		{
			gateSourceEdgeTran1 = drainSourceEdgeTran1;
		}

		Edge * drainSourceEdgeTran2 = NULL;
		Edge * gateSourceEdgeTran2 = NULL;
		std::vector<Edge*> edgesTran2 = getGraph().findToComponentBelongingEdges(transistor2);
		for(std::vector<Edge*>::const_iterator it = edgesTran2.begin(); it != edgesTran2.end(); it++)
		{
			Edge* edge = *it;
			if(edge->isDrainSourceEdge())
			{
				drainSourceEdgeTran2 = edge;
			}
			else if (edge->isGateSourceEdge())
			{
				gateSourceEdgeTran2 = edge;
			}
		}
		assert(drainSourceEdgeTran2 != NULL, "No drainSourceEdge found!");
		if(gateSourceEdgeTran2 == NULL)
		{
			gateSourceEdgeTran2 = drainSourceEdgeTran2;
		}
		const TechnologieSpecificationSHM & techSpecTran1 =  getCircuitInformation().getTechnologieSpecificationSHM(transistor1);
		Gecode::FloatVar vdsTran1 = computeEdgeVoltage(*drainSourceEdgeTran1);
		Gecode::FloatVar vgsTran1 = computeEdgeVoltage(*gateSourceEdgeTran1);
		Gecode::FloatVal vthTran1 = techSpecTran1.getThresholdVoltage();

		const TechnologieSpecificationSHM & techSpecTran2 = getCircuitInformation().getTechnologieSpecificationSHM(transistor2);
		Gecode::FloatVar vdsTran2 = computeEdgeVoltage(*drainSourceEdgeTran2);
		Gecode::FloatVar vgsTran2 = computeEdgeVoltage(*gateSourceEdgeTran2);
		Gecode::FloatVal vthTran2 = techSpecTran2.getThresholdVoltage();

		if(transistor1.getArray().getTechType().isN() && transistor2.getArray().getTechType().isN())
		{
			Gecode::rel(getSpace(), !(vgsTran1 - vthTran1 > vdsTran1  + 0.08) || (vgsTran2 - vthTran2 > vdsTran2  + 0.08));
			Gecode::rel(getSpace(),  !(vgsTran1 - vthTran1 < vdsTran1 - 0.05 ) || (vgsTran2 - vthTran2 < vdsTran2 - 0.05 ));
		}
		else if(transistor1.getArray().getTechType().isP() && transistor2.getArray().getTechType().isP())
		{
			Gecode::rel(getSpace(), !(vgsTran1 - vthTran1 < vdsTran1 - 0.08) || (vgsTran2 - vthTran2 < vdsTran2 - 0.08)  );
			Gecode::rel(getSpace(), !(vgsTran1 - vthTran1 > vdsTran1 + 0.05) || (vgsTran2 - vthTran2 > vdsTran2 + 0.05));
		}
		else if(transistor1.getArray().getTechType().isN() && transistor2.getArray().getTechType().isP())
		{
			Gecode::rel(getSpace(), !(vgsTran1 - vthTran1 > vdsTran1  + 0.08) || (vgsTran2 - vthTran2 < vdsTran2 - 0.08)  );
			Gecode::rel(getSpace(), !(vgsTran1 - vthTran1 < vdsTran1 - 0.05 ) || (vgsTran2 - vthTran2 > vdsTran2 + 0.05));
		}
		else
		{
			Gecode::rel(getSpace(), !(vgsTran1 - vthTran1 < vdsTran1 - 0.08) || (vgsTran2 - vthTran2 > vdsTran2  + 0.08));
			Gecode::rel(getSpace(), !(vgsTran1 - vthTran1 > vdsTran1 + 0.05) || (vgsTran2 - vthTran2 < vdsTran2 - 0.05 ));
		}

	}

	bool SimulationTransistorConstraintsSHM::isCurrentBias(const StructRec::Structure & structure) const
	{
		bool isCurrentBias = false;

		if(structure.hasParent())
		{
			std::vector<const StructRec::Structure*> parents = structure.getTopmostParents();
			for(auto& it_parents: parents)
			{
				if(isCurrentMirror(*it_parents))
				{
					const StructRec::Pair * parentPair =static_cast<const StructRec::Pair*>(it_parents);

					if(parentPair->getChild2().hasCommonDevices(structure))
					{
						isCurrentBias = true;
					}
				}
			}

		}

		return isCurrentBias;
	}

	bool SimulationTransistorConstraintsSHM::isCurrentMirror(const StructRec::Structure & structure) const
	{
	   const StructRec::StructureName& simpleCurrentMirror = StructRec::StructureName("MosfetSimpleCurrentMirror");
	   const StructRec::StructureName& cascodeCurrentMirror = StructRec::StructureName("MosfetCascodeCurrentMirror");
	   const StructRec::StructureName& improvedWilsonCurrentMirror = StructRec::StructureName("MosfetImprovedWilsonCurrentMirror");
	   const StructRec::StructureName& wideSwingCascodeCurrentMirror = StructRec::StructureName("MosfetWideSwingCascodeCurrentMirror");
	   const StructRec::StructureName& wideSwingCurrentMirror = StructRec::StructureName("MosfetWideSwingCurrentMirror");
	   const StructRec::StructureName& fourTransistorCurrentMirror = StructRec::StructureName("MosfetFourTransistorCurrentMirror");
	   const StructRec::StructureName& wilsonCurrentMirror = StructRec::StructureName("MosfetWilsonCurrentMirror");


	    return (structure.getStructureName() == simpleCurrentMirror) ||
	            (structure.getStructureName() == cascodeCurrentMirror) ||
	              (structure.getStructureName() == improvedWilsonCurrentMirror) ||
	                (structure.getStructureName() == wideSwingCascodeCurrentMirror) ||
	                (structure.getStructureName() == fourTransistorCurrentMirror) ||
	                (structure.getStructureName() == wilsonCurrentMirror) ||
	                (structure.getStructureName() == wideSwingCurrentMirror);
	}





}

