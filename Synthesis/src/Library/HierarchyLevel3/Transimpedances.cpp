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

#include "Synthesis/incl/Library/HierarchyLevel3/Transimpedances.h"
#include "Synthesis/incl/Library/HierarchyLevel2/StructuralLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel2/VoltageBiases.h"
#include "Synthesis/incl/Library/HierarchyLevel2/CurrentBiases.h"

#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/CircuitId/CircuitIds.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Device/TechType.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"

#include "Core/incl/FlatCircuitRecursion/FlatCircuitRecursion.h"

#include <sstream>


namespace Synthesis {

	const Core::TerminalName Transimpedances::INPUT_TERMINAL_  = Core::TerminalName("Input");
	const Core::TerminalName Transimpedances::OUTPUT_TERMINAL_ = Core::TerminalName("Output");
	const Core::TerminalName Transimpedances::SOURCE_TERMINAL_ = Core::TerminalName("Source");

	const Core::InstanceName Transimpedances::VOLTAGEBIAS_ = Core::InstanceName("VoltageBias");
	const Core::InstanceName Transimpedances::CURRENTBIAS_ = Core::InstanceName("CurrentBias");

	const Core::NetId Transimpedances::INPUT_NET_  = Core::NetName("Input").createRootIdentifier();
	const Core::NetId Transimpedances::OUTPUT_NET_ = Core::NetName("Output").createRootIdentifier();
	const Core::NetId Transimpedances::SOURCE_NET_ = Core::NetName("Source").createRootIdentifier();


	Transimpedances::Transimpedances(const StructuralLevel & structuralLevel)
	{
		int index = 1;
		initializeSimpleCurrentMirrorTransimpedancesPmos(structuralLevel, index);
		initializeSimpleCurrentMirrorTransimpedancesNmos(structuralLevel, index);
	}

	Transimpedances::~Transimpedances()
	{
		eraseSimpleCurrentMirrorTransimpedancesPmos();
		eraseSimpleCurrentMirrorTransimpedancesNmos();
	}

	std::vector<const Core::Circuit*> Transimpedances::getSimpleCurrentMirrorTransimpedancesPmos() const
	{
		assert(!simpleCurrentMirrorTransimpedancesPmos_.empty());
		return simpleCurrentMirrorTransimpedancesPmos_;
	}

	std::vector<const Core::Circuit*> Transimpedances::getSimpleCurrentMirrorTransimpedancesNmos() const
	{
		assert(!simpleCurrentMirrorTransimpedancesNmos_.empty());
		return simpleCurrentMirrorTransimpedancesNmos_;
	}

	std::string Transimpedances::toStr() const
	{
		std::ostringstream oss;
		oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<< Transimpedances: " << std::endl;

		oss << "Pmos simple current mirrors:" << std::endl;
		for(auto & transimpedance : simpleCurrentMirrorTransimpedancesPmos_)
		{
			oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << transimpedance->getCircuitIdentifier().toStr() << std::endl;
			oss << transimpedance->toStr() << std::endl;

			Core::FlatCircuitRecursion flatCircuitRecursion;
			Core::Circuit * flatCircuit = flatCircuitRecursion.createNewFlatCopy(*transimpedance);
			oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
			oss << flatCircuit->toStr() << std::endl;
			delete flatCircuit;
		}

		oss << "Nmos simple current mirrors:" << std::endl;
		for(auto & transimpedance : simpleCurrentMirrorTransimpedancesNmos_)
		{
			oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << transimpedance->getCircuitIdentifier().toStr() << std::endl;
			oss << transimpedance->toStr() << std::endl;

			Core::FlatCircuitRecursion flatCircuitRecursion;
			Core::Circuit * flatCircuit = flatCircuitRecursion.createNewFlatCopy(*transimpedance);
			oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
			oss << flatCircuit->toStr() << std::endl;
			delete flatCircuit;
		}

		return oss.str();
	}

	void Transimpedances::initializeSimpleCurrentMirrorTransimpedancesPmos(const StructuralLevel & structuralLevel, int & index)
	{
		const Core::Circuit & voltageBias = structuralLevel.getVoltageBiases().getDiodeTransistorVoltageBiasPmos();
		const Core::Circuit & currentBias = structuralLevel.getCurrentBiases().getNormalTransistorCurrentBias(Core::TechType::p());
		simpleCurrentMirrorTransimpedancesPmos_.push_back(&createTransimpedance(voltageBias, currentBias, index));
	}

	void Transimpedances::initializeSimpleCurrentMirrorTransimpedancesNmos(const StructuralLevel & structuralLevel, int & index)
	{
		const Core::Circuit & voltageBias = structuralLevel.getVoltageBiases().getDiodeTransistorVoltageBiasNmos();
		const Core::Circuit & currentBias = structuralLevel.getCurrentBiases().getNormalTransistorCurrentBias(Core::TechType::n());
		simpleCurrentMirrorTransimpedancesNmos_.push_back(&createTransimpedance(voltageBias, currentBias, index));
	}

	const Core::Circuit & Transimpedances::createTransimpedance(const Core::Circuit & voltageBias,
	                                                             const Core::Circuit & currentBias,
	                                                             int & index)
	{
		Core::Circuit * transimpedance = new Core::Circuit;

		Core::Instance & voltageBiasInstance = createInstance(voltageBias, VOLTAGEBIAS_);
		Core::Instance & currentBiasInstance = createInstance(currentBias, CURRENTBIAS_);

		Core::CircuitIds circuitIds;
		Core::CircuitId transimpedanceId = circuitIds.transimpedance(index);
		transimpedanceId.setTechType(voltageBias.getCircuitIdentifier().getTechType());
		transimpedance->setCircuitIdentifier(transimpedanceId);

		std::vector<Core::NetId> netNames;
		netNames.push_back(INPUT_NET_);
		netNames.push_back(OUTPUT_NET_);
		netNames.push_back(SOURCE_NET_);

		std::map<Core::TerminalName, Core::NetId> terminalToNetMap;
		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INPUT_TERMINAL_, INPUT_NET_));
		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTPUT_TERMINAL_, OUTPUT_NET_));
		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE_TERMINAL_, SOURCE_NET_));

		addNetsToCircuit(*transimpedance, netNames);
		addTerminalsToCircuit(*transimpedance, terminalToNetMap);

		transimpedance->addInstance(voltageBiasInstance);
		transimpedance->addInstance(currentBiasInstance);

		connectVoltageBiasInstance(*transimpedance, voltageBiasInstance);
		connectCurrentBiasInstance(*transimpedance, currentBiasInstance);

		voltageBiasInstance.setCircuit(*transimpedance);
		currentBiasInstance.setCircuit(*transimpedance);

		index++;
		return *transimpedance;
	}

	void Transimpedances::connectVoltageBiasInstance(Core::Circuit & transimpedance, Core::Instance & voltageBias) const
	{
		// Diode-connected input branch: VB.IN (drain) and VB.OUT (gate) tied
		// to the same parent net = mirror gate = transimpedance INPUT.
		connectInstanceTerminal(transimpedance, voltageBias, VoltageBiases::IN_TERMINAL_,     INPUT_NET_);
		connectInstanceTerminal(transimpedance, voltageBias, VoltageBiases::OUT_TERMINAL_,    INPUT_NET_);
		connectInstanceTerminal(transimpedance, voltageBias, VoltageBiases::SOURCE_TERMINAL_, SOURCE_NET_);
	}

	void Transimpedances::connectCurrentBiasInstance(Core::Circuit & transimpedance, Core::Instance & currentBias) const
	{
		// Mirror output branch: CB.IN (gate) shares the mirror-gate net with VB,
		// CB.OUT (drain) is the transimpedance output.
		connectInstanceTerminal(transimpedance, currentBias, CurrentBiases::IN_TERMINAL_,     INPUT_NET_);
		connectInstanceTerminal(transimpedance, currentBias, CurrentBiases::OUT_TERMINAL_,    OUTPUT_NET_);
		connectInstanceTerminal(transimpedance, currentBias, CurrentBiases::SOURCE_TERMINAL_, SOURCE_NET_);
	}

	void Transimpedances::eraseSimpleCurrentMirrorTransimpedancesPmos()
	{
		for(auto & transimpedance : simpleCurrentMirrorTransimpedancesPmos_)
		{
			delete transimpedance;
		}
	}

	void Transimpedances::eraseSimpleCurrentMirrorTransimpedancesNmos()
	{
		for(auto & transimpedance : simpleCurrentMirrorTransimpedancesNmos_)
		{
			delete transimpedance;
		}
	}

}
