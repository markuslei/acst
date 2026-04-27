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


 #include "AutomaticSizing/incl/ConstraintProgram/Constraints/TransistorConstraintsEKV.h"

 #include "AutomaticSizing/incl/ConstraintProgram/Graph/Graph.h"
 #include "AutomaticSizing/incl/ConstraintProgram/Graph/Edge.h"
 #include "AutomaticSizing/incl/ConstraintProgram/SearchSpace.h"
 
 #include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"
 #include "AutomaticSizing/incl/CircuitInformation/DesignAttributes.h"
 #include "AutomaticSizing/incl/ConstraintProgram/ComponentToIntVarMap.h"
 #include "AutomaticSizing/incl/CircuitInformation/Pin.h"
 
 #include "Partitioning/incl/Results/Component.h"
 #include "Partitioning/incl/Results/Result.h"
 
 #include "Core/incl/Common/BacktraceAssert.h"
 
 #include<math.h>
 #include <gecode/float.hh>
 
 namespace AutomaticSizing {
 
	 TransistorConstraintsEKV::TransistorConstraintsEKV():
			 graph_(NULL),
			 partitioningResult_(NULL),
			 transistorToWidthMap_(NULL),
			 transistorToMultiplierMap_(NULL),
			 transistorToLengthMap_(NULL),
			 transistorToCurrentMap_(NULL),
			 netToVoltageMap_(NULL),
			 circuitInformation_(NULL),
			 space_(NULL),
			 ekvVersion_(1)
	 {
	 }
 
	 void TransistorConstraintsEKV::setGraph(
		 const Graph& graph)
	 {
		 graph_ = & graph;
	 }
 
	 void TransistorConstraintsEKV::setTransistorToWidthMap(
		 ComponentToIntVarMap& widthMap)
	 {
		 transistorToWidthMap_ = & widthMap;
	 }

	 void TransistorConstraintsEKV::setTransistorToMultiplierMap(
		 ComponentToIntVarMap& multiplierMap)
	 {
		 transistorToMultiplierMap_ = & multiplierMap;
	 }
 
	 void TransistorConstraintsEKV::setTransistorToLengthMap(
		 ComponentToIntVarMap& lengthMap)
	 {
		 transistorToLengthMap_ = & lengthMap;
	 }
 
	 void TransistorConstraintsEKV::setTransistorToCurrentMap(
		 ComponentToIntVarMap& currentMap)
	 {
		 transistorToCurrentMap_ = & currentMap;
	 }
 
	 void TransistorConstraintsEKV::setNetToVoltageMap(
		 NetToIntVarMap& voltageMap)
	 {
		 netToVoltageMap_ = & voltageMap;
	 }
 
	 void TransistorConstraintsEKV::setSpace(
		 SearchSpace& space)
	 {
		 space_ = & space;
	 }

	 void TransistorConstraintsEKV::setEKVVersion(int version)
	 {
		 ekvVersion_ = version;
	 }

	 int TransistorConstraintsEKV::getEKVVersion() const
	 {
		 assert(ekvVersion_ == 1 || ekvVersion_ == 2 || ekvVersion_ == 3, "EKV version not supported");
		 return ekvVersion_;
	 }
 
	 void TransistorConstraintsEKV::createConstraints()
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
 
	 const Graph& TransistorConstraintsEKV::getGraph() const
	 {
		 assert(graph_ != NULL);
		 return * graph_;
	 }
 
	 ComponentToIntVarMap& TransistorConstraintsEKV::getTransistorToWidthMap()
	 {
		 assert(transistorToWidthMap_ != NULL);
		 return *transistorToWidthMap_;
	 }

	 ComponentToIntVarMap& TransistorConstraintsEKV::getTransistorToMultiplierMap()
	 {
		 assert(transistorToMultiplierMap_ != NULL);
		 return * transistorToMultiplierMap_;
	 }
 
	 ComponentToIntVarMap& TransistorConstraintsEKV::getTransistorToLengthMap()
	 {
		 assert(transistorToLengthMap_ != NULL);
		 return * transistorToLengthMap_;
	 }
 
	 ComponentToIntVarMap& TransistorConstraintsEKV::getTransistorToCurrentMap()
	 {
		 assert(transistorToCurrentMap_ != NULL);
		 return * transistorToCurrentMap_;
	 }
 
	 NetToIntVarMap& TransistorConstraintsEKV::getNetToVoltageMap()
	 {
		 assert(netToVoltageMap_ != NULL);
		 return * netToVoltageMap_;
	 }
 
	 SearchSpace& TransistorConstraintsEKV::getSpace()
	 {
		 assert(space_ != NULL);
		 return *space_;
	 }
 
	 void TransistorConstraintsEKV::createSaturationCurrentConstraintsStrongInversion(Partitioning::Component & component)
	 {
 //		if(component.getArray().getIdentifier().toStr() == "MosfetNormalArray[7]")
		 {
		 TechnologieSpecificationEKV techSpec;
		 Gecode::FloatVal u = getSpace().getScalingFactorMUM();
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
 
		 Gecode::FloatVar widthHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		 Gecode::FloatVar multiplierHelperVar(getSpace(), 1, getSpace().getMultiplierUpperBound());
		 Gecode::FloatVar lengthHelperVar(getSpace(), 1, getSpace().getLengthUpperBound());
 
		 Gecode::channel(getSpace(), getTransistorToWidthMap().find(component), widthHelperVar);
		 Gecode::channel(getSpace(), getTransistorToMultiplierMap().find(component), multiplierHelperVar);
		 Gecode::channel(getSpace(), getTransistorToLengthMap().find(component), lengthHelperVar);
 
		 Gecode::FloatVar width = Gecode::expr(getSpace(),widthHelperVar*u);
		 Gecode::FloatVar multiplier = Gecode::expr(getSpace(), multiplierHelperVar);
		 Gecode::FloatVar length = Gecode::expr(getSpace(), lengthHelperVar*u);
 
		 logDebug("Length: " << length << ", Width: " << width << ", Multiplier: " << multiplier);
 
 
		 if(component.getArray().getTechType().isN())
		 {
			 techSpec = getCircuitInformation().getTechnologieSpecificationEKVNmos();
			 Gecode::FloatVal Vt = techSpec.getThermalVoltage();

			 /*Gecode::FloatVal muCox = techSpec.getMobilityOxideCapacityCoefficient();
			 Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			 Gecode::FloatVal n = techSpec.getSlopeFactor();
             Gecode::FloatVal Vt = techSpec.getThermalVoltage();
			 Gecode::FloatVal lamda = techSpec.getChannelLengthCoefficient();
			 Gecode::FloatVal theta = techSpec.getMobilityReductionCoefficient();
			 Gecode::FloatVal eta = techSpec.getDIBLCoefficient();*/

			 Gecode::IntVar idx(getSpace(), 0, 4);
			 Gecode::rel(getSpace(), (length >= 5.0e-6f) >> (idx == 4));
			 Gecode::rel(getSpace(), (length < 5.0e-6f && length >= 2.1e-6f) >> (idx == 3));
			 Gecode::rel(getSpace(), (length < 2.1e-6f && length >= 1.2e-6f) >> (idx == 2));
			 Gecode::rel(getSpace(), (length < 1.2e-6f && length >= 0.5e-6f) >> (idx == 1));
			 Gecode::rel(getSpace(), (length < 0.5e-6f) >> (idx == 0));

			 Gecode::FloatVar vth (getSpace(), -0.5, 0.5);
			 Gecode::FloatVar muCox (getSpace(), 0, 0.01);
			 Gecode::FloatVar n (getSpace(), 0, 2);
			 Gecode::FloatVar lamda (getSpace(), 0, 1);
			 Gecode::FloatVar theta (getSpace(), 0, 1);
			 Gecode::FloatVar eta (getSpace(), 0, 1);
	
			 float muCox_list[5], vth_list[5], n_list[5], lamda_list[5], theta_list[5], eta_list[5];
			 vth_list[0] = techSpec.getThresholdVoltage_LMAX500();
			 vth_list[1] = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
			 vth_list[2] = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
			 vth_list[3] = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
			 vth_list[4] = techSpec.getThresholdVoltage_LMIN5000();
			 muCox_list[0] = techSpec.getMobilityOxideCapacityCoefficient_LMAX500();
			 muCox_list[1] = techSpec.getMobilityOxideCapacityCoefficient_LMIN500_LMAX1200();
			 muCox_list[2] = techSpec.getMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100();
			 muCox_list[3] = techSpec.getMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000();
			 muCox_list[4] = techSpec.getMobilityOxideCapacityCoefficient_LMIN5000();
			 n_list[0] = techSpec.getSlopeFactor_LMAX500();
			 n_list[1] = techSpec.getSlopeFactor_LMIN500_LMAX1200();
			 n_list[2] = techSpec.getSlopeFactor_LMIN1200_LMAX2100();
			 n_list[3] = techSpec.getSlopeFactor_LMIN2100_LMAX5000();
			 n_list[4] = techSpec.getSlopeFactor_LMIN5000();
			 lamda_list[0] = techSpec.getChannelLengthCoefficient_LMAX500();
			 lamda_list[1] = techSpec.getChannelLengthCoefficient_LMIN500_LMAX1200();
			 lamda_list[2] = techSpec.getChannelLengthCoefficient_LMIN1200_LMAX2100();
			 lamda_list[3] = techSpec.getChannelLengthCoefficient_LMIN2100_LMAX5000();
			 lamda_list[4] = techSpec.getChannelLengthCoefficient_LMIN5000();
			 theta_list[0] = techSpec.getMobilityReductionCoefficient_LMAX500();
			 theta_list[1] = techSpec.getMobilityReductionCoefficient_LMIN500_LMAX1200();
			 theta_list[2] = techSpec.getMobilityReductionCoefficient_LMIN1200_LMAX2100();
			 theta_list[3] = techSpec.getMobilityReductionCoefficient_LMIN2100_LMAX5000();
			 theta_list[4] = techSpec.getMobilityReductionCoefficient_LMIN5000();
			 eta_list[0] = techSpec.getDIBLCoefficient_LMAX500();
			 eta_list[1] = techSpec.getDIBLCoefficient_LMIN500_LMAX1200();
			 eta_list[2] = techSpec.getDIBLCoefficient_LMIN1200_LMAX2100();
			 eta_list[3] = techSpec.getDIBLCoefficient_LMIN2100_LMAX5000();
			 eta_list[4] = techSpec.getDIBLCoefficient_LMIN5000();

			 for (int i=0; i<5; i++)
			 {
				 Gecode::rel(getSpace(), (idx == i) >> (vth == vth_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (muCox == muCox_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (n == n_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (lamda == lamda_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (theta == theta_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (eta == eta_list[i]));
			 }

 
			 if(component.getPart().isLoadPart() && getPartitioningResult().getLoadPart(component.getArray()).hasCrossCoupledPair(getPartitioningResult()))
			 {
				if(getEKVVersion() == 1)
				{
					Gecode::FloatVar vP (getSpace(), 0, 1);
					Gecode::rel(getSpace(), vP == (vgs - vth) / n);
					Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
					logDebug("lnF domain: " << lnF);
					Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP - vds) * (0.5 / Vt))));
					logDebug("lnR domain: " << lnR);
					Gecode::FloatVar iS (getSpace(), 0, 1);
					Gecode::rel(getSpace(), iS == 2 * multiplier * muCox *(width /length) * n * pow(Vt, 2));
					Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleLamda == (1 + lamda * vds));
					Gecode::FloatVar i_ekv (getSpace(), 0, 1);
					Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
					Gecode::rel(getSpace(), i_ekv <= 1.1 * ids);
					Gecode::rel(getSpace(), i_ekv >= 0.9 * ids);
				}
				else if(getEKVVersion() == 2)
				{
					Gecode::FloatVar vP (getSpace(), 0, 1);
					Gecode::rel(getSpace(), vP == (vgs - vth) / n);
					Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
					logDebug("lnF domain: " << lnF);
					Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP - vds) * (0.5 / Vt))));
					logDebug("lnR domain: " << lnR);
					Gecode::FloatVar scaleTheta (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleTheta == (1 + theta * vP));
					Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleLamda == (1 + lamda * vds));
					Gecode::FloatVar iS (getSpace(), 0, 1);
					Gecode::rel(getSpace(), iS * scaleTheta == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
					Gecode::FloatVar i_ekv (getSpace(), 0, 1);
					Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
					Gecode::rel(getSpace(), i_ekv <= 1.1 * ids);
					Gecode::rel(getSpace(), i_ekv >= 0.9 * ids);
				}
				else
				{
					Gecode::FloatVar etaTerm (getSpace(), 0, 1);
					Gecode::rel(getSpace(), etaTerm == eta * vds);
					Gecode::FloatVar vth_eff (getSpace(), 0, 1);
					Gecode::rel(getSpace(), vth_eff == vth - etaTerm);
					Gecode::FloatVar vP (getSpace(), 0, 1);
					Gecode::rel(getSpace(), vP == (vgs - vth_eff) / n);
					Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
					logDebug("lnF domain: " << lnF);
					Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP - vds) * (0.5 / Vt))));
					logDebug("lnR domain: " << lnR);
					Gecode::FloatVar scaleTheta (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleTheta == (1 + theta * vP));
					Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleLamda == (1 + lamda * vds));
					Gecode::FloatVar iS (getSpace(), 0, 1);
					Gecode::rel(getSpace(), iS * scaleTheta == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
					Gecode::FloatVar i_ekv (getSpace(), 0, 1);
					Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
					Gecode::rel(getSpace(), i_ekv <= 1.1 * ids);
					Gecode::rel(getSpace(), i_ekv >= 0.9 * ids);
				}
			 }
			 else
			 {
				if(getEKVVersion() == 1)
				{
					Gecode::FloatVar vP (getSpace(), 0, 1);
					Gecode::rel(getSpace(), vP == (vgs - vth) / n);
					Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
					logDebug("lnF domain: " << lnF);
					Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP - vds) * (0.5 / Vt))));
					logDebug("lnR domain: " << lnR);
					Gecode::FloatVar iS (getSpace(), 0, 1);
					Gecode::rel(getSpace(), iS == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
					Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleLamda == (1 + lamda * vds));
					Gecode::FloatVar i_ekv (getSpace(), 0, 1);
					Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
					Gecode::rel(getSpace(), i_ekv <= 1.01 * ids);
					Gecode::rel(getSpace(), i_ekv >= 0.99 * ids);
				}
				else if(getEKVVersion() == 2)
				{
					Gecode::FloatVar vP (getSpace(), 0, 1);
					Gecode::rel(getSpace(), vP == (vgs - vth) / n);
					Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
					logDebug("lnF domain: " << lnF);
					Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP - vds) * (0.5 / Vt))));
					logDebug("lnR domain: " << lnR);
					Gecode::FloatVar scaleTheta (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleTheta == (1 + theta * vP));
					Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleLamda == (1 + lamda * vds));
					Gecode::FloatVar iS (getSpace(), 0, 1);
					Gecode::rel(getSpace(), iS * scaleTheta == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
					Gecode::FloatVar i_ekv (getSpace(), 0, 1);
					Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
					Gecode::rel(getSpace(), i_ekv <= 1.01 * ids);
					Gecode::rel(getSpace(), i_ekv >= 0.99 * ids);
				}
				else
				{
					Gecode::FloatVar etaTerm (getSpace(), 0, 1);
					Gecode::rel(getSpace(), etaTerm == eta * vds);
					Gecode::FloatVar vth_eff (getSpace(), 0, 1);
					Gecode::rel(getSpace(), vth_eff == vth - etaTerm);
					Gecode::FloatVar vP (getSpace(), 0, 1);
					Gecode::rel(getSpace(), vP == (vgs - vth_eff) / n);
					Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
					logDebug("lnF domain: " << lnF);
					Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP - vds) * (0.5 / Vt))));
					logDebug("lnR domain: " << lnR);
					Gecode::FloatVar scaleTheta (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleTheta == (1 + theta * vP));
					Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleLamda == (1 + lamda * vds));
					Gecode::FloatVar iS (getSpace(), 0, 1);
					Gecode::rel(getSpace(), iS * scaleTheta == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
					Gecode::FloatVar i_ekv (getSpace(), 0, 1);
					Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
					Gecode::rel(getSpace(), i_ekv <= 1.01 * ids);
					Gecode::rel(getSpace(), i_ekv >= 0.99 * ids);
				}
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
			 techSpec = getCircuitInformation().getTechnologieSpecificationEKVPmos();
			 Gecode::FloatVal Vt = techSpec.getThermalVoltage();


			 /*Gecode::FloatVal muCox = techSpec.getMobilityOxideCapacityCoefficient();
			 Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			 Gecode::FloatVal n = techSpec.getSlopeFactor();
			 Gecode::FloatVal lamda = techSpec.getChannelLengthCoefficient();
			 Gecode::FloatVal theta = techSpec.getMobilityReductionCoefficient();
			 Gecode::FloatVal eta = techSpec.getDIBLCoefficient();*/

			 Gecode::IntVar idx(getSpace(), 0, 4);
			 Gecode::rel(getSpace(), (length >= 5.0e-6f) >> (idx == 4));
			 Gecode::rel(getSpace(), (length < 5.0e-6f && length >= 2.1e-6f) >> (idx == 3));
			 Gecode::rel(getSpace(), (length < 2.1e-6f && length >= 1.2e-6f) >> (idx == 2));
			 Gecode::rel(getSpace(), (length < 1.2e-6f && length >= 0.5e-6f) >> (idx == 1));
			 Gecode::rel(getSpace(), (length < 0.5e-6f) >> (idx == 0));

			 Gecode::FloatVar vth (getSpace(), -0.5, 0.5);
			 Gecode::FloatVar muCox (getSpace(), 0, 0.01);
			 Gecode::FloatVar n (getSpace(), 0, 2);
			 Gecode::FloatVar lamda (getSpace(), 0, 1);
			 Gecode::FloatVar theta (getSpace(), 0, 1);
			 Gecode::FloatVar eta (getSpace(), 0, 1);
	
			 float muCox_list[5], vth_list[5], n_list[5], lamda_list[5], theta_list[5], eta_list[5];
			 vth_list[0] = techSpec.getThresholdVoltage_LMAX500();
			 vth_list[1] = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
			 vth_list[2] = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
			 vth_list[3] = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
			 vth_list[4] = techSpec.getThresholdVoltage_LMIN5000();
			 muCox_list[0] = techSpec.getMobilityOxideCapacityCoefficient_LMAX500();
			 muCox_list[1] = techSpec.getMobilityOxideCapacityCoefficient_LMIN500_LMAX1200();
			 muCox_list[2] = techSpec.getMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100();
			 muCox_list[3] = techSpec.getMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000();
			 muCox_list[4] = techSpec.getMobilityOxideCapacityCoefficient_LMIN5000();
			 n_list[0] = techSpec.getSlopeFactor_LMAX500();
			 n_list[1] = techSpec.getSlopeFactor_LMIN500_LMAX1200();
			 n_list[2] = techSpec.getSlopeFactor_LMIN1200_LMAX2100();
			 n_list[3] = techSpec.getSlopeFactor_LMIN2100_LMAX5000();
			 n_list[4] = techSpec.getSlopeFactor_LMIN5000();
			 lamda_list[0] = techSpec.getChannelLengthCoefficient_LMAX500();
			 lamda_list[1] = techSpec.getChannelLengthCoefficient_LMIN500_LMAX1200();
			 lamda_list[2] = techSpec.getChannelLengthCoefficient_LMIN1200_LMAX2100();
			 lamda_list[3] = techSpec.getChannelLengthCoefficient_LMIN2100_LMAX5000();
			 lamda_list[4] = techSpec.getChannelLengthCoefficient_LMIN5000();
			 theta_list[0] = techSpec.getMobilityReductionCoefficient_LMAX500();
			 theta_list[1] = techSpec.getMobilityReductionCoefficient_LMIN500_LMAX1200();
			 theta_list[2] = techSpec.getMobilityReductionCoefficient_LMIN1200_LMAX2100();
			 theta_list[3] = techSpec.getMobilityReductionCoefficient_LMIN2100_LMAX5000();
			 theta_list[4] = techSpec.getMobilityReductionCoefficient_LMIN5000();
			 eta_list[0] = techSpec.getDIBLCoefficient_LMAX500();
			 eta_list[1] = techSpec.getDIBLCoefficient_LMIN500_LMAX1200();
			 eta_list[2] = techSpec.getDIBLCoefficient_LMIN1200_LMAX2100();
			 eta_list[3] = techSpec.getDIBLCoefficient_LMIN2100_LMAX5000();
			 eta_list[4] = techSpec.getDIBLCoefficient_LMIN5000();

			 for (int i =0; i<5; i++)
			 {
				 Gecode::rel(getSpace(), (idx == i) >> (vth == vth_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (muCox == muCox_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (n == n_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (lamda == lamda_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (theta == theta_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (eta == eta_list[i]));
			 }
 
			 if(component.getPart().isLoadPart() && getPartitioningResult().getLoadPart(component.getArray()).hasCrossCoupledPair(getPartitioningResult()))
			 {
				if(getEKVVersion() == 1)
				{
					Gecode::FloatVar vP (getSpace(), 0, 1);
					Gecode::rel(getSpace(), vP == (-vgs + vth) / n);
					Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
					logDebug("lnF domain: " << lnF);
					Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP + vds) * (0.5 / Vt))));
					logDebug("lnR domain: " << lnR);
					Gecode::FloatVar iS (getSpace(), 0, 1);
					Gecode::rel(getSpace(), iS == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
					Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleLamda == (1 - lamda * vds));
					Gecode::FloatVar i_ekv (getSpace(), 0, 1);
					Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
					Gecode::rel(getSpace(), - i_ekv / ids <= 1.1);
					Gecode::rel(getSpace(), - i_ekv / ids >= 0.9);
				}
				else if(getEKVVersion() == 2)
				{
					Gecode::FloatVar vP (getSpace(), 0, 1);
					Gecode::rel(getSpace(), vP == (-vgs + vth) / n);
					Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
					logDebug("lnF domain: " << lnF);
					Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP + vds) * (0.5 / Vt))));
					logDebug("lnR domain: " << lnR);
					Gecode::FloatVar scaleTheta (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleTheta == (1 + theta * vP));
					Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleLamda == (1 - lamda * vds));
					Gecode::FloatVar iS (getSpace(), 0, 1);
					Gecode::rel(getSpace(), iS * scaleTheta == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
					Gecode::FloatVar i_ekv (getSpace(), 0, 1);
					Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
					Gecode::rel(getSpace(), - i_ekv / ids <= 1.1);
					Gecode::rel(getSpace(), - i_ekv / ids >= 0.9);
				}
				else
				{
					Gecode::FloatVar etaTerm (getSpace(), -1, 1);
					Gecode::rel(getSpace(), etaTerm == eta * vds);
					Gecode::FloatVar vth_eff (getSpace(), -1, 1);
					Gecode::rel(getSpace(), vth_eff == vth + etaTerm);
					Gecode::FloatVar vP (getSpace(), 0, 1);
					Gecode::rel(getSpace(), vP == (-vgs + vth_eff) / n);
					Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
					logDebug("lnF domain: " << lnF);
					Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP + vds) * (0.5 / Vt))));
					logDebug("lnR domain: " << lnR);
					Gecode::FloatVar scaleTheta (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleTheta == (1 + theta * vP));
					Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleLamda == (1 - lamda * vds));
					Gecode::FloatVar iS (getSpace(), 0, 1);
					Gecode::rel(getSpace(), iS * scaleTheta == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
					Gecode::FloatVar i_ekv (getSpace(), 0, 1);
					Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
					Gecode::rel(getSpace(), - i_ekv / ids <= 1.1);
					Gecode::rel(getSpace(), - i_ekv / ids >= 0.9);
				}
			 }
			 else
			 {
				if(getEKVVersion() == 1)
				{
					Gecode::FloatVar vP (getSpace(), 0, 1);
					Gecode::rel(getSpace(), vP == (-vgs + vth) / n);
					Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
					logDebug("lnF domain: " << lnF);
					Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP + vds) * (0.5 / Vt))));
					logDebug("lnR domain: " << lnR);
					Gecode::FloatVar iS (getSpace(), 0, 1);
					Gecode::rel(getSpace(), iS == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
					Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleLamda == (1 - lamda * vds));
					Gecode::FloatVar i_ekv (getSpace(), 0, 1);
					Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
					Gecode::rel(getSpace(), - i_ekv / ids <= 1.01);
					Gecode::rel(getSpace(), - i_ekv / ids >= 0.99);
				}
				else if(getEKVVersion() == 2)
				{
					Gecode::FloatVar vP (getSpace(), 0, 1);
					Gecode::rel(getSpace(), vP == (-vgs + vth) / n);
					Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
					logDebug("lnF domain: " << lnF);
					Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP + vds) * (0.5 / Vt))));
					logDebug("lnR domain: " << lnR);
					Gecode::FloatVar scaleTheta (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleTheta == (1 + theta * vP));
					Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleLamda == (1 - lamda * vds));
					Gecode::FloatVar iS (getSpace(), 0, 1);
					Gecode::rel(getSpace(), iS * scaleTheta == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
					Gecode::FloatVar i_ekv (getSpace(), 0, 1);
					Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
					Gecode::rel(getSpace(), - i_ekv / ids <= 1.01);
					Gecode::rel(getSpace(), - i_ekv / ids >= 0.99);
				}
				else
				{
					Gecode::FloatVar etaTerm (getSpace(), -1, 1);
					Gecode::rel(getSpace(), etaTerm == eta * vds);
					Gecode::FloatVar vth_eff (getSpace(), -1, 1);
					Gecode::rel(getSpace(), vth_eff == vth + etaTerm);
					Gecode::FloatVar vP (getSpace(), 0, 1);
					Gecode::rel(getSpace(), vP == (-vgs + vth_eff) / n);
					Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
					logDebug("lnF domain: " << lnF);
					Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP + vds) * (0.5 / Vt))));
					logDebug("lnR domain: " << lnR);
					Gecode::FloatVar scaleTheta (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleTheta == (1 + theta * vP));
					Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
					Gecode::rel(getSpace(), scaleLamda == (1 - lamda * vds));
					Gecode::FloatVar iS (getSpace(), 0, 1);
					Gecode::rel(getSpace(), iS * scaleTheta == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
					Gecode::FloatVar i_ekv (getSpace(), 0, 1);
					Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
					Gecode::rel(getSpace(), - i_ekv / ids <= 1.01);
					Gecode::rel(getSpace(), - i_ekv / ids >= 0.99);
				}
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
 
	 void TransistorConstraintsEKV::createSaturationCurrentConstraintsWeakInversion(Partitioning::Component & component)
	 {
		 TechnologieSpecificationEKV techSpec = getCircuitInformation().getTechnologieSpecificationEKV(component);
		 Gecode::FloatVal Vt = techSpec.getThermalVoltage();
		 /*Gecode::FloatVal vth = techSpec.getThresholdVoltage();
		 Gecode::FloatVal n = techSpec.getSlopeFactor();
		 Gecode::FloatVal muCox = techSpec.getMobilityOxideCapacityCoefficient();
		 Gecode::FloatVal lamda = techSpec.getChannelLengthCoefficient();
		 Gecode::FloatVal theta = techSpec.getMobilityReductionCoefficient();
		 Gecode::FloatVal eta = techSpec.getDIBLCoefficient();*/
 
		 Gecode::FloatVal u = getSpace().getScalingFactorMUM();
 
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
		 
		 Gecode::FloatVar widthHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		 Gecode::FloatVar multiplierHelperVar(getSpace(), 1 , getSpace().getMultiplierUpperBound());
		 Gecode::FloatVar lengthHelperVar(getSpace(), 1, getSpace().getLengthUpperBound());
 
		 channel(getSpace(), getTransistorToWidthMap().find(component), widthHelperVar);
		 channel(getSpace(), getTransistorToMultiplierMap().find(component), multiplierHelperVar);
		 channel(getSpace(), getTransistorToLengthMap().find(component), lengthHelperVar);
 
		 Gecode::FloatVar width = Gecode::expr(getSpace(),widthHelperVar*u);
		 Gecode::FloatVar multiplier = Gecode::expr(getSpace(),multiplierHelperVar);
		 Gecode::FloatVar length = Gecode::expr(getSpace(), lengthHelperVar*u);

		Gecode::IntVar idx(getSpace(), 0, 4);
		Gecode::rel(getSpace(), (length >= 5.0e-6f) >> (idx == 4));
		Gecode::rel(getSpace(), (length < 5.0e-6f && length >= 2.1e-6f) >> (idx == 3));
		Gecode::rel(getSpace(), (length < 2.1e-6f && length >= 1.2e-6f) >> (idx == 2));
		Gecode::rel(getSpace(), (length < 1.2e-6f && length >= 0.5e-6f) >> (idx == 1));
		Gecode::rel(getSpace(), (length < 0.5e-6f) >> (idx == 0));

		Gecode::FloatVar vth (getSpace(), -0.5, 0.5);
		Gecode::FloatVar muCox (getSpace(), 0, 0.01);
		Gecode::FloatVar n (getSpace(), 0, 2);
		Gecode::FloatVar lamda (getSpace(), 0, 1);
		Gecode::FloatVar theta (getSpace(), 0, 1);
		Gecode::FloatVar eta (getSpace(), 0, 1);
	
		float muCox_list[5], vth_list[5], n_list[5], lamda_list[5], theta_list[5], eta_list[5];
		vth_list[0] = techSpec.getThresholdVoltage_LMAX500();
		vth_list[1] = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
		vth_list[2] = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
		vth_list[3] = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
		vth_list[4] = techSpec.getThresholdVoltage_LMIN5000();
		muCox_list[0] = techSpec.getMobilityOxideCapacityCoefficient_LMAX500();
		muCox_list[1] = techSpec.getMobilityOxideCapacityCoefficient_LMIN500_LMAX1200();
		muCox_list[2] = techSpec.getMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100();
		muCox_list[3] = techSpec.getMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000();
		muCox_list[4] = techSpec.getMobilityOxideCapacityCoefficient_LMIN5000();
		n_list[0] = techSpec.getSlopeFactor_LMAX500();
		n_list[1] = techSpec.getSlopeFactor_LMIN500_LMAX1200();
		n_list[2] = techSpec.getSlopeFactor_LMIN1200_LMAX2100();
		n_list[3] = techSpec.getSlopeFactor_LMIN2100_LMAX5000();
		n_list[4] = techSpec.getSlopeFactor_LMIN5000();
		lamda_list[0] = techSpec.getChannelLengthCoefficient_LMAX500();
		lamda_list[1] = techSpec.getChannelLengthCoefficient_LMIN500_LMAX1200();
		lamda_list[2] = techSpec.getChannelLengthCoefficient_LMIN1200_LMAX2100();
		lamda_list[3] = techSpec.getChannelLengthCoefficient_LMIN2100_LMAX5000();
		lamda_list[4] = techSpec.getChannelLengthCoefficient_LMIN5000();
		theta_list[0] = techSpec.getMobilityReductionCoefficient_LMAX500();
		theta_list[1] = techSpec.getMobilityReductionCoefficient_LMIN500_LMAX1200();
		theta_list[2] = techSpec.getMobilityReductionCoefficient_LMIN1200_LMAX2100();
		theta_list[3] = techSpec.getMobilityReductionCoefficient_LMIN2100_LMAX5000();
		theta_list[4] = techSpec.getMobilityReductionCoefficient_LMIN5000();
		eta_list[0] = techSpec.getDIBLCoefficient_LMAX500();
		eta_list[1] = techSpec.getDIBLCoefficient_LMIN500_LMAX1200();
		eta_list[2] = techSpec.getDIBLCoefficient_LMIN1200_LMAX2100();
		eta_list[3] = techSpec.getDIBLCoefficient_LMIN2100_LMAX5000();
		eta_list[4] = techSpec.getDIBLCoefficient_LMIN5000();

		for (int i =0; i<5; i++)
		{
			Gecode::rel(getSpace(), (idx == i) >> (vth == vth_list[i]));
			Gecode::rel(getSpace(), (idx == i) >> (muCox == muCox_list[i]));
			Gecode::rel(getSpace(), (idx == i) >> (n == n_list[i]));
			Gecode::rel(getSpace(), (idx == i) >> (lamda == lamda_list[i]));
			Gecode::rel(getSpace(), (idx == i) >> (theta == theta_list[i]));
			Gecode::rel(getSpace(), (idx == i) >> (eta == eta_list[i]));
		}

		
 
		 if(component.getArray().getTechType().isN())
		 {
			if(getEKVVersion() == 1)
			{
				Gecode::FloatVar vP (getSpace(), 0, 1);
				Gecode::rel(getSpace(), vP == (vgs - vth) / n);
				Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
				logDebug("lnF domain: " << lnF);
				Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP - vds) * (0.5 / Vt))));
				logDebug("lnR domain: " << lnR);
				Gecode::FloatVar iS (getSpace(), 0, 1);
				Gecode::rel(getSpace(), iS == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
				Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
				Gecode::rel(getSpace(), scaleLamda == (1 + lamda * vds));
				Gecode::FloatVar i_ekv (getSpace(), 0, 1);
				Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
				Gecode::rel(getSpace(), i_ekv <= 1.01 * ids);
				Gecode::rel(getSpace(), i_ekv >= 0.99 * ids);
			}
			else if(getEKVVersion() == 2)
			{
				Gecode::FloatVar vP (getSpace(), 0, 1);
				Gecode::rel(getSpace(), vP == (vgs - vth) / n);
				Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
				logDebug("lnF domain: " << lnF);
				Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP - vds) * (0.5 / Vt))));
				logDebug("lnR domain: " << lnR);
				Gecode::FloatVar scaleTheta (getSpace(), 1, 2);
				Gecode::rel(getSpace(), scaleTheta == (1 + theta * vP));
				Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
				Gecode::rel(getSpace(), scaleLamda == (1 + lamda * vds));
				Gecode::FloatVar iS (getSpace(), 0, 1);
				Gecode::rel(getSpace(), iS * scaleTheta == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
				Gecode::FloatVar i_ekv (getSpace(), 0, 1);
				Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
				Gecode::rel(getSpace(), i_ekv <= 1.01 * ids);
				Gecode::rel(getSpace(), i_ekv >= 0.99 * ids);
			}
			else
			{
				Gecode::FloatVar vth_eff = expr(getSpace(), vth - eta * vds);
				Gecode::FloatVar vP (getSpace(), 0, 1);
				Gecode::rel(getSpace(), vP == (vgs - vth_eff) / n);
				Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
				logDebug("lnF domain: " << lnF);
				Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP - vds) * (0.5 / Vt))));
				logDebug("lnR domain: " << lnR);
				Gecode::FloatVar scaleTheta (getSpace(), 1, 2);
				Gecode::rel(getSpace(), scaleTheta == (1 + theta * vP));
				Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
				Gecode::rel(getSpace(), scaleLamda == (1 + lamda * vds));
				Gecode::FloatVar iS (getSpace(), 0, 1);
				Gecode::rel(getSpace(), iS * scaleTheta == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
				Gecode::FloatVar i_ekv (getSpace(), 0, 1);
				Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
				Gecode::rel(getSpace(), i_ekv <= 1.01 * ids);
				Gecode::rel(getSpace(), i_ekv >= 0.99 * ids);
			}

			 Gecode::rel(getSpace(), vds > 4* Vt);
			 Gecode::rel(getSpace(), vgs - vth < 0);
			 Gecode::rel(getSpace(), vgs - vth > -0.2);
		 }
		 else
		 {
			if(getEKVVersion() == 1)
			{
				Gecode::FloatVar vP (getSpace(), 0, 1);
				Gecode::rel(getSpace(), vP == (-vgs + vth) / n);
				Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
				logDebug("lnF domain: " << lnF);
				Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP + vds) * (0.5 / Vt))));
				logDebug("lnR domain: " << lnR);
				Gecode::FloatVar iS (getSpace(), 0, 1);
				Gecode::rel(getSpace(), iS == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
				Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
				Gecode::rel(getSpace(), scaleLamda == (1 - lamda * vds));
				Gecode::FloatVar i_ekv (getSpace(), 0, 1);
				Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
				Gecode::rel(getSpace(), - i_ekv / ids <= 1.01);
				Gecode::rel(getSpace(), - i_ekv / ids >= 0.99);
			}
			else if(getEKVVersion() == 2)
			{
				Gecode::FloatVar vP (getSpace(), 0, 1);
				Gecode::rel(getSpace(), vP == (-vgs + vth) / n);
				Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
				logDebug("lnF domain: " << lnF);
				Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP + vds) * (0.5 / Vt))));
				logDebug("lnR domain: " << lnR);
				Gecode::FloatVar scaleTheta (getSpace(), 1, 2);
				Gecode::rel(getSpace(), scaleTheta == (1 + theta * vP));
				Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
				Gecode::rel(getSpace(), scaleLamda == (1 - lamda * vds));
				Gecode::FloatVar iS (getSpace(), 0, 1);
				Gecode::rel(getSpace(), iS * scaleTheta == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
				Gecode::FloatVar i_ekv (getSpace(), 0, 1);
				Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
				Gecode::rel(getSpace(), - i_ekv / ids <= 1.01);
				Gecode::rel(getSpace(), - i_ekv / ids >= 0.99);
			}
			else
			{
				Gecode::FloatVar vth_eff = expr(getSpace(), vth + eta * vds);
				Gecode::FloatVar vP (getSpace(), 0, 1);
				Gecode::rel(getSpace(), vP == (-vgs + vth_eff) / n);
				Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
				logDebug("lnF domain: " << lnF);
				Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP + vds) * (0.5 / Vt))));
				logDebug("lnR domain: " << lnR);
				Gecode::FloatVar scaleTheta (getSpace(), 1, 2);
				Gecode::rel(getSpace(), scaleTheta == (1 + theta * vP));
				Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
				Gecode::rel(getSpace(), scaleLamda == (1 - lamda * vds));
				Gecode::FloatVar iS (getSpace(), 0, 1);
				Gecode::rel(getSpace(), iS * scaleTheta == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
				Gecode::FloatVar i_ekv (getSpace(), 0, 1);
				Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
				Gecode::rel(getSpace(), - i_ekv / ids <= 1.01);
				Gecode::rel(getSpace(), - i_ekv / ids >= 0.99);
			}

			 Gecode::rel(getSpace(), vds < - 4* Vt);
			 Gecode::rel(getSpace(), vgs - vth > 0);
			 Gecode::rel(getSpace(), vgs - vth < 0.2);
		 }
	 }
 
 
	 bool TransistorConstraintsEKV::isOutputNet(Core::NetId net) const
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
 
	 void TransistorConstraintsEKV::setCircuitInformation(
		 const CircuitInformation& information)
	 {
		 circuitInformation_ = & information;
	 }
 
	 void TransistorConstraintsEKV::setPartitioningResult(
		 const Partitioning::Result & result)
	 {
		 partitioningResult_ = & result;
	 }
 
	 const CircuitInformation& TransistorConstraintsEKV::getCircuitInformation() const
	 {
		 assert(circuitInformation_ != NULL);
		 return * circuitInformation_;
	 }
 
	 const Partitioning::Result& TransistorConstraintsEKV::getPartitioningResult() const
	 {
		 assert(partitioningResult_ != NULL);
		 return * partitioningResult_;
	 }
 
	 void TransistorConstraintsEKV::createCompensationResistorConstraint(Partitioning::Component & component)
	 {
		 TechnologieSpecificationEKV techSpec;
		 
 
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

		 Gecode::FloatVal u = getSpace().getScalingFactorMUM();
		 Gecode::FloatVar widthHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		 Gecode::FloatVar multiplierHelperVar(getSpace(), 1 , getSpace().getMultiplierUpperBound());
		 Gecode::FloatVar lengthHelperVar(getSpace(), 1, getSpace().getLengthUpperBound());
 
		 channel(getSpace(), getTransistorToWidthMap().find(component), widthHelperVar);
		 channel(getSpace(), getTransistorToMultiplierMap().find(component), multiplierHelperVar);
		 channel(getSpace(), getTransistorToLengthMap().find(component), lengthHelperVar);
 
		 Gecode::FloatVar width = Gecode::expr(getSpace(),widthHelperVar*u);
		 Gecode::FloatVar multiplier = Gecode::expr(getSpace(),multiplierHelperVar);
		 Gecode::FloatVar length = Gecode::expr(getSpace(), lengthHelperVar*u);
 
 
		 rel(getSpace(), ids == 0);
		 rel(getSpace(), vds == 0);
 
 
		 if(component.getArray().getTechType().isN())
		 {
			 techSpec = getCircuitInformation().getTechnologieSpecificationEKVNmos();
			 
			Gecode::IntVar idx(getSpace(), 0, 4);
			Gecode::rel(getSpace(), (length >= 5.0e-6f) >> (idx == 4));
			Gecode::rel(getSpace(), (length < 5.0e-6f && length >= 2.1e-6f) >> (idx == 3));
			Gecode::rel(getSpace(), (length < 2.1e-6f && length >= 1.2e-6f) >> (idx == 2));
			Gecode::rel(getSpace(), (length < 1.2e-6f && length >= 0.5e-6f) >> (idx == 1));
			Gecode::rel(getSpace(), (length < 0.5e-6f) >> (idx == 0));
	   
			Gecode::FloatVar vth (getSpace(), 0, 0.5);
			float vth_list[5];
			vth_list[0] = techSpec.getThresholdVoltage_LMAX500();
			vth_list[1] = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
			vth_list[2] = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
			vth_list[3] = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
			vth_list[4] = techSpec.getThresholdVoltage_LMIN5000();
	   
			for (int i=0; i<5; i++)
			{
				Gecode::rel(getSpace(), (idx == i) >> (vth == vth_list[i]));
			}
			Gecode::rel(getSpace(), vgs - vth > 0);
		 }
		 else
		 {
			 techSpec = getCircuitInformation().getTechnologieSpecificationEKVPmos();

			 Gecode::IntVar idx(getSpace(), 0, 4);
			 Gecode::rel(getSpace(), (length >= 5.0e-6f) >> (idx == 4));
			 Gecode::rel(getSpace(), (length < 5.0e-6f && length >= 2.1e-6f) >> (idx == 3));
			 Gecode::rel(getSpace(), (length < 2.1e-6f && length >= 1.2e-6f) >> (idx == 2));
			 Gecode::rel(getSpace(), (length < 1.2e-6f && length >= 0.5e-6f) >> (idx == 1));
			 Gecode::rel(getSpace(), (length < 0.5e-6f) >> (idx == 0));
	   
			Gecode::FloatVar vth (getSpace(), -0.5, 0);
			float vth_list[5];
			vth_list[0] = techSpec.getThresholdVoltage_LMAX500();
			vth_list[1] = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
			vth_list[2] = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
			vth_list[3] = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
			vth_list[4] = techSpec.getThresholdVoltage_LMIN5000();
	   
			for (int i =0; i<5; i++)
			{
				Gecode::rel(getSpace(), (idx == i) >> (vth == vth_list[i]));
			}
			 Gecode::rel(getSpace(), vgs - vth < 0);
		 }
 
 
	 }
 
	 void TransistorConstraintsEKV::createOffConstraint(Partitioning::Component & component)
	 {
		 TechnologieSpecificationEKV techSpec;
 
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

		 Gecode::FloatVal u = getSpace().getScalingFactorMUM();
		 Gecode::FloatVar widthHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		 Gecode::FloatVar multiplierHelperVar(getSpace(), 1 , getSpace().getMultiplierUpperBound());
		 Gecode::FloatVar lengthHelperVar(getSpace(), 1, getSpace().getLengthUpperBound());
 
		 channel(getSpace(), getTransistorToWidthMap().find(component), widthHelperVar);
		 channel(getSpace(), getTransistorToMultiplierMap().find(component), multiplierHelperVar);
		 channel(getSpace(), getTransistorToLengthMap().find(component), lengthHelperVar);
 
		 Gecode::FloatVar width = Gecode::expr(getSpace(),widthHelperVar*u);
		 Gecode::FloatVar multiplier = Gecode::expr(getSpace(),multiplierHelperVar);
		 Gecode::FloatVar length = Gecode::expr(getSpace(), lengthHelperVar*u);
 
 
		 if(component.getArray().getTechType().isN())
		 {
			 techSpec = getCircuitInformation().getTechnologieSpecificationEKVNmos();
			 
			 Gecode::IntVar idx(getSpace(), 0, 4);
			 Gecode::rel(getSpace(), (length >= 5.0e-6f) >> (idx == 4));
			 Gecode::rel(getSpace(), (length < 5.0e-6f && length >= 2.1e-6f) >> (idx == 3));
			 Gecode::rel(getSpace(), (length < 2.1e-6f && length >= 1.2e-6f) >> (idx == 2));
			 Gecode::rel(getSpace(), (length < 1.2e-6f && length >= 0.5e-6f) >> (idx == 1));
			 Gecode::rel(getSpace(), (length < 0.5e-6f) >> (idx == 0));
	   
			Gecode::FloatVar vth (getSpace(), 0, 0.5);
			float vth_list[5];
			vth_list[0] = techSpec.getThresholdVoltage_LMAX500();
			vth_list[1] = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
			vth_list[2] = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
			vth_list[3] = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
			vth_list[4] = techSpec.getThresholdVoltage_LMIN5000();
	   
			for (int i=0; i<5; i++)
			{
				Gecode::rel(getSpace(), (idx == i) >> (vth == vth_list[i]));
			}
			Gecode::rel(getSpace(), vgs - vth < 0);
		 }
		 else
		 {
			 techSpec = getCircuitInformation().getTechnologieSpecificationEKVPmos();

			 Gecode::IntVar idx(getSpace(), 0, 4);
			 Gecode::rel(getSpace(), (length >= 5.0e-6f) >> (idx == 4));
			 Gecode::rel(getSpace(), (length < 5.0e-6f && length >= 2.1e-6f) >> (idx == 3));
			 Gecode::rel(getSpace(), (length < 2.1e-6f && length >= 1.2e-6f) >> (idx == 2));
			 Gecode::rel(getSpace(), (length < 1.2e-6f && length >= 0.5e-6f) >> (idx == 1));
			 Gecode::rel(getSpace(), (length < 0.5e-6f) >> (idx == 0));
	   
			Gecode::FloatVar vth (getSpace(), -0.5, 0);
			float vth_list[5];
			vth_list[0] = techSpec.getThresholdVoltage_LMAX500();
			vth_list[1] = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
			vth_list[2] = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
			vth_list[3] = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
			vth_list[4] = techSpec.getThresholdVoltage_LMIN5000();
	   
			for (int i =0; i<5; i++)
			{
				Gecode::rel(getSpace(), (idx == i) >> (vth == vth_list[i]));
			}
			 Gecode::rel(getSpace(), vgs - vth > 0);
		 }
 
	 }
 
	 void TransistorConstraintsEKV::createLinearVoltageConstraints(Partitioning::Component& component)
	 {
 
		 TechnologieSpecificationEKV techSpec;
		 Gecode::FloatVal u = getSpace().getScalingFactorMUM();
 
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
		 Gecode::FloatVar vgs = computeEdgeVoltage(*gateSourceEdge);
		 Gecode::FloatVar vds = computeEdgeVoltage(*drainSourceEdge);
 
		 Gecode::FloatVar ids = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(component));
 
		 Gecode::FloatVar widthHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		 Gecode::FloatVar multiplierHelperVar(getSpace(), 1 , getSpace().getMultiplierUpperBound());
		 Gecode::FloatVar lengthHelperVar(getSpace(), 1, getSpace().getLengthUpperBound());
 
		 channel(getSpace(), getTransistorToWidthMap().find(component), widthHelperVar);
		 channel(getSpace(), getTransistorToMultiplierMap().find(component), multiplierHelperVar);
		 channel(getSpace(), getTransistorToLengthMap().find(component), lengthHelperVar);
 
		 Gecode::FloatVar width = Gecode::expr(getSpace(),widthHelperVar*u);
		 Gecode::FloatVar multiplier = Gecode::expr(getSpace(),multiplierHelperVar);
		 Gecode::FloatVar length = Gecode::expr(getSpace(), lengthHelperVar*u);
 
 
		 if(component.getArray().getTechType().isN())
		 {
			 techSpec = getCircuitInformation().getTechnologieSpecificationEKVNmos();
			 Gecode::FloatVal Vt = techSpec.getThermalVoltage();
			 /*Gecode::FloatVal muCox = techSpec.getMobilityOxideCapacityCoefficient();
			 Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			 Gecode::FloatVal n = techSpec.getSlopeFactor();
             Gecode::FloatVal Vt = techSpec.getThermalVoltage();
			 Gecode::FloatVal lamda = techSpec.getChannelLengthCoefficient();
			 Gecode::FloatVal theta = techSpec.getMobilityReductionCoefficient();
			 Gecode::FloatVal eta = techSpec.getDIBLCoefficient();*/

			Gecode::IntVar idx(getSpace(), 0, 4);
			Gecode::rel(getSpace(), (length >= 5.0e-6f) >> (idx == 4));
			Gecode::rel(getSpace(), (length < 5.0e-6f && length >= 2.1e-6f) >> (idx == 3));
			Gecode::rel(getSpace(), (length < 2.1e-6f && length >= 1.2e-6f) >> (idx == 2));
			Gecode::rel(getSpace(), (length < 1.2e-6f && length >= 0.5e-6f) >> (idx == 1));
			Gecode::rel(getSpace(), (length < 0.5e-6f) >> (idx == 0));

			 Gecode::FloatVar vth (getSpace(), -0.5, 0.5);
			 Gecode::FloatVar muCox (getSpace(), 0, 0.01);
			 Gecode::FloatVar n (getSpace(), 0, 2);
			 Gecode::FloatVar lamda (getSpace(), 0, 1);
			 Gecode::FloatVar theta (getSpace(), 0, 1);
			 Gecode::FloatVar eta (getSpace(), 0, 1);
	
			 float muCox_list[5], vth_list[5], n_list[5], lamda_list[5], theta_list[5], eta_list[5];
			 vth_list[0] = techSpec.getThresholdVoltage_LMAX500();
			 vth_list[1] = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
			 vth_list[2] = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
			 vth_list[3] = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
			 vth_list[4] = techSpec.getThresholdVoltage_LMIN5000();
			 muCox_list[0] = techSpec.getMobilityOxideCapacityCoefficient_LMAX500();
			 muCox_list[1] = techSpec.getMobilityOxideCapacityCoefficient_LMIN500_LMAX1200();
			 muCox_list[2] = techSpec.getMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100();
			 muCox_list[3] = techSpec.getMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000();
			 muCox_list[4] = techSpec.getMobilityOxideCapacityCoefficient_LMIN5000();
			 n_list[0] = techSpec.getSlopeFactor_LMAX500();
			 n_list[1] = techSpec.getSlopeFactor_LMIN500_LMAX1200();
			 n_list[2] = techSpec.getSlopeFactor_LMIN1200_LMAX2100();
			 n_list[3] = techSpec.getSlopeFactor_LMIN2100_LMAX5000();
			 n_list[4] = techSpec.getSlopeFactor_LMIN5000();
			 lamda_list[0] = techSpec.getChannelLengthCoefficient_LMAX500();
			 lamda_list[1] = techSpec.getChannelLengthCoefficient_LMIN500_LMAX1200();
			 lamda_list[2] = techSpec.getChannelLengthCoefficient_LMIN1200_LMAX2100();
			 lamda_list[3] = techSpec.getChannelLengthCoefficient_LMIN2100_LMAX5000();
			 lamda_list[4] = techSpec.getChannelLengthCoefficient_LMIN5000();
			 theta_list[0] = techSpec.getMobilityReductionCoefficient_LMAX500();
			 theta_list[1] = techSpec.getMobilityReductionCoefficient_LMIN500_LMAX1200();
			 theta_list[2] = techSpec.getMobilityReductionCoefficient_LMIN1200_LMAX2100();
			 theta_list[3] = techSpec.getMobilityReductionCoefficient_LMIN2100_LMAX5000();
			 theta_list[4] = techSpec.getMobilityReductionCoefficient_LMIN5000();
			 eta_list[0] = techSpec.getDIBLCoefficient_LMAX500();
			 eta_list[1] = techSpec.getDIBLCoefficient_LMIN500_LMAX1200();
			 eta_list[2] = techSpec.getDIBLCoefficient_LMIN1200_LMAX2100();
			 eta_list[3] = techSpec.getDIBLCoefficient_LMIN2100_LMAX5000();
			 eta_list[4] = techSpec.getDIBLCoefficient_LMIN5000();

			 for (int i =0; i<5; i++)
			 {
				 Gecode::rel(getSpace(), (idx == i) >> (vth == vth_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (muCox == muCox_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (n == n_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (lamda == lamda_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (theta == theta_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (eta == eta_list[i]));
			 }

			 if(getEKVVersion() == 1)
			 {
				 Gecode::FloatVar vP (getSpace(), 0, 1);
				 Gecode::rel(getSpace(), vP == (vgs - vth) / n);
				 Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
				 logDebug("lnF domain: " << lnF);
				 Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP - vds) * (0.5 / Vt))));
				 logDebug("lnR domain: " << lnR);
				 Gecode::FloatVar iS (getSpace(), 0, 1);
				 Gecode::rel(getSpace(), iS == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
				 Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
				 Gecode::rel(getSpace(), scaleLamda == (1 + lamda * vds));
				 Gecode::FloatVar i_ekv (getSpace(), 0, 1);
				 Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
				 Gecode::rel(getSpace(), i_ekv == 1 * ids);
			 }
			 else if(getEKVVersion() == 2)
			 {
				 Gecode::FloatVar vP (getSpace(), 0, 1);
				 Gecode::rel(getSpace(), vP == (vgs - vth) / n);
				 Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
				 logDebug("lnF domain: " << lnF);
				 Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP - vds) * (0.5 / Vt))));
				 logDebug("lnR domain: " << lnR);
				 Gecode::FloatVar scaleTheta (getSpace(), 1, 2);
				 Gecode::rel(getSpace(), scaleTheta == (1 + theta * vP));
				 Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
				 Gecode::rel(getSpace(), scaleLamda == (1 + lamda * vds));
				 Gecode::FloatVar iS (getSpace(), 0, 1);
				 Gecode::rel(getSpace(), iS * scaleTheta == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
				 Gecode::FloatVar i_ekv (getSpace(), 0, 1);
				 Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
				 Gecode::rel(getSpace(), i_ekv == 1 * ids);
			 }
			 else
			 {
				 Gecode::FloatVar vth_eff = expr(getSpace(), vth - eta * vds);
				 Gecode::FloatVar vP (getSpace(), 0, 1);
				 Gecode::rel(getSpace(), vP == (vgs - vth_eff) / n);
				 Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
				 logDebug("lnF domain: " << lnF);
				 Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP - vds) * (0.5 / Vt))));
				 logDebug("lnR domain: " << lnR);
				 Gecode::FloatVar scaleTheta (getSpace(), 1, 2);
				 Gecode::rel(getSpace(), scaleTheta == (1 + theta * vP));
				 Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
				 Gecode::rel(getSpace(), scaleLamda == (1 + lamda * vds));
				 Gecode::FloatVar iS (getSpace(), 0, 1);
				 Gecode::rel(getSpace(), iS * scaleTheta == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
				 Gecode::FloatVar i_ekv (getSpace(), 0, 1);
				 Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
				 Gecode::rel(getSpace(), i_ekv == 1 * ids);
			 }


			 Gecode::rel(getSpace(), vgs - vth > vds);
			 Gecode::rel(getSpace(), vds > 0);
			 Gecode::rel(getSpace(), vgs - vth > 0);
		 }
		 else
		 {
			 techSpec = getCircuitInformation().getTechnologieSpecificationEKVPmos();
			 Gecode::FloatVal Vt = techSpec.getThermalVoltage();
			 /*Gecode::FloatVal muCox = techSpec.getMobilityOxideCapacityCoefficient();
			 Gecode::FloatVal vth = techSpec.getThresholdVoltage();
             Gecode::FloatVal n = techSpec.getSlopeFactor();
             Gecode::FloatVal Vt = techSpec.getThermalVoltage();
			 Gecode::FloatVal lamda = techSpec.getChannelLengthCoefficient();
			 Gecode::FloatVal theta = techSpec.getMobilityReductionCoefficient();
			 Gecode::FloatVal eta = techSpec.getDIBLCoefficient();*/

			 Gecode::IntVar idx(getSpace(), 0, 4);
			 Gecode::rel(getSpace(), (length >= 5.0e-6f) >> (idx == 4));
			 Gecode::rel(getSpace(), (length < 5.0e-6f && length >= 2.1e-6f) >> (idx == 3));
			 Gecode::rel(getSpace(), (length < 2.1e-6f && length >= 1.2e-6f) >> (idx == 2));
			 Gecode::rel(getSpace(), (length < 1.2e-6f && length >= 0.5e-6f) >> (idx == 1));
			 Gecode::rel(getSpace(), (length < 0.5e-6f) >> (idx == 0));

			 Gecode::FloatVar vth (getSpace(), -0.5, 0.5);
			 Gecode::FloatVar muCox (getSpace(), 0, 0.01);
			 Gecode::FloatVar n (getSpace(), 0, 2);
			 Gecode::FloatVar lamda (getSpace(), 0, 1);
			 Gecode::FloatVar theta (getSpace(), 0, 1);
			 Gecode::FloatVar eta (getSpace(), 0, 1);
	
			 float muCox_list[5], vth_list[5], n_list[5], lamda_list[5], theta_list[5], eta_list[5];
			 vth_list[0] = techSpec.getThresholdVoltage_LMAX500();
			 vth_list[1] = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
			 vth_list[2] = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
			 vth_list[3] = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
			 vth_list[4] = techSpec.getThresholdVoltage_LMIN5000();
			 muCox_list[0] = techSpec.getMobilityOxideCapacityCoefficient_LMAX500();
			 muCox_list[1] = techSpec.getMobilityOxideCapacityCoefficient_LMIN500_LMAX1200();
			 muCox_list[2] = techSpec.getMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100();
			 muCox_list[3] = techSpec.getMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000();
			 muCox_list[4] = techSpec.getMobilityOxideCapacityCoefficient_LMIN5000();
			 n_list[0] = techSpec.getSlopeFactor_LMAX500();
			 n_list[1] = techSpec.getSlopeFactor_LMIN500_LMAX1200();
			 n_list[2] = techSpec.getSlopeFactor_LMIN1200_LMAX2100();
			 n_list[3] = techSpec.getSlopeFactor_LMIN2100_LMAX5000();
			 n_list[4] = techSpec.getSlopeFactor_LMIN5000();
			 lamda_list[0] = techSpec.getChannelLengthCoefficient_LMAX500();
			 lamda_list[1] = techSpec.getChannelLengthCoefficient_LMIN500_LMAX1200();
			 lamda_list[2] = techSpec.getChannelLengthCoefficient_LMIN1200_LMAX2100();
			 lamda_list[3] = techSpec.getChannelLengthCoefficient_LMIN2100_LMAX5000();
			 lamda_list[4] = techSpec.getChannelLengthCoefficient_LMIN5000();
			 theta_list[0] = techSpec.getMobilityReductionCoefficient_LMAX500();
			 theta_list[1] = techSpec.getMobilityReductionCoefficient_LMIN500_LMAX1200();
			 theta_list[2] = techSpec.getMobilityReductionCoefficient_LMIN1200_LMAX2100();
			 theta_list[3] = techSpec.getMobilityReductionCoefficient_LMIN2100_LMAX5000();
			 theta_list[4] = techSpec.getMobilityReductionCoefficient_LMIN5000();
			 eta_list[0] = techSpec.getDIBLCoefficient_LMAX500();
			 eta_list[1] = techSpec.getDIBLCoefficient_LMIN500_LMAX1200();
			 eta_list[2] = techSpec.getDIBLCoefficient_LMIN1200_LMAX2100();
			 eta_list[3] = techSpec.getDIBLCoefficient_LMIN2100_LMAX5000();
			 eta_list[4] = techSpec.getDIBLCoefficient_LMIN5000();

			 for (int i =0; i<5; i++)
			 {
				 Gecode::rel(getSpace(), (idx == i) >> (vth == vth_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (muCox == muCox_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (n == n_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (lamda == lamda_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (theta == theta_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (eta == eta_list[i]));
			 }

			 if(getEKVVersion() == 1)
			 {
				 Gecode::FloatVar vP (getSpace(), 0, 1);
				 Gecode::rel(getSpace(), vP == (-vgs + vth) / n);
				 Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
				 logDebug("lnF domain: " << lnF);
				 Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP + vds) * (0.5 / Vt))));
				 logDebug("lnR domain: " << lnR);
				 Gecode::FloatVar iS (getSpace(), 0, 1);
				 Gecode::rel(getSpace(), iS == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
				 Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
				 Gecode::rel(getSpace(), scaleLamda == (1 - lamda * vds));
				 Gecode::FloatVar i_ekv (getSpace(), 0, 1);
				 Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
				 Gecode::rel(getSpace(), - i_ekv == 1 * ids);
			 }
			 else if(getEKVVersion() == 2)
			 {
				 Gecode::FloatVar vP (getSpace(), 0, 1);
				 Gecode::rel(getSpace(), vP == (-vgs + vth) / n);
				 Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
				 logDebug("lnF domain: " << lnF);
				 Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP + vds) * (0.5 / Vt))));
				 logDebug("lnR domain: " << lnR);
				 Gecode::FloatVar scaleTheta (getSpace(), 1, 2);
				 Gecode::rel(getSpace(), scaleTheta == (1 + theta * vP));
				 Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
				 Gecode::rel(getSpace(), scaleLamda == (1 - lamda * vds));
				 Gecode::FloatVar iS (getSpace(), 0, 1);
				 Gecode::rel(getSpace(), iS * scaleTheta == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
				 Gecode::FloatVar i_ekv (getSpace(), 0, 1);
				 Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
				 Gecode::rel(getSpace(), - i_ekv == 1 * ids);
			 }
			 else
			 {
				 Gecode::FloatVar vth_eff = expr(getSpace(), vth + eta * vds);
				 Gecode::FloatVar vP (getSpace(), 0, 1);
				 Gecode::rel(getSpace(), vP == (-vgs + vth_eff) / n);
				 Gecode::FloatVar lnF = expr(getSpace(), log(1 + exp(vP * (0.5 / Vt))));
				 logDebug("lnF domain: " << lnF);
				 Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP + vds) * (0.5 / Vt))));
				 logDebug("lnR domain: " << lnR);
				 Gecode::FloatVar scaleTheta (getSpace(), 1, 2);
				 Gecode::rel(getSpace(), scaleTheta == (1 + theta * vP));
				 Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
				 Gecode::rel(getSpace(), scaleLamda == (1 - lamda * vds));
				 Gecode::FloatVar iS (getSpace(), 0, 1);
				 Gecode::rel(getSpace(), iS * scaleTheta == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
				 Gecode::FloatVar i_ekv (getSpace(), 0, 1);
				 Gecode::rel (getSpace(), i_ekv == iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
				 Gecode::rel(getSpace(), - i_ekv == 1 * ids);
			 }

			 Gecode::rel(getSpace(), vgs - vth <  vds);
			 Gecode::rel(getSpace(), vds < 0);
			 Gecode::rel(getSpace(), vgs - vth < 0);
		 }
	 }
 
 
	 bool TransistorConstraintsEKV::isLowerTransistorOf4TransistorCurrentMirror(
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
 
	 void TransistorConstraintsEKV::createSaturationConstraints(Partitioning::Component& component)
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
 
		 createMinimalAreaConstraint(component);
	 }
 
	 void TransistorConstraintsEKV::createGateOverDriveVoltageConstraint(Partitioning::Component & component)
	 {
			 float u= getSpace().getScalingFactorMUM();
			 float vOverDrive = getCircuitInformation().getCircuitSpecification().getGateOverDriveVoltage();
 
			 Gecode::FloatVar ids = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(component));
 
			 Gecode::FloatVar widthHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
			 Gecode::FloatVar multiplierHelperVar(getSpace(), 1 , getSpace().getMultiplierUpperBound());
			 Gecode::FloatVar lengthHelperVar(getSpace(), 1, getSpace().getLengthUpperBound());
 
			 channel(getSpace(), getTransistorToWidthMap().find(component), widthHelperVar);
			 channel(getSpace(), getTransistorToMultiplierMap().find(component), multiplierHelperVar);
			 channel(getSpace(), getTransistorToLengthMap().find(component), lengthHelperVar);
 
			 Gecode::FloatVar width = Gecode::expr(getSpace(),widthHelperVar* u);
			 Gecode::FloatVar multiplier = Gecode::expr(getSpace(),multiplierHelperVar);
			 Gecode::FloatVar length = Gecode::expr(getSpace(), lengthHelperVar*u);
 
			 TechnologieSpecificationEKV  techSpec;
 
			 if(component.getArray().getTechType().isN())
			 {
				 techSpec = getCircuitInformation().getTechnologieSpecificationEKVNmos();
			 }
			 else
			 {
				 techSpec = getCircuitInformation().getTechnologieSpecificationEKVPmos();
			 }
 
			 /*float muCox = techSpec.getMobilityOxideCapacityCoefficient();
			 float n = techSpec.getSlopeFactor();*/
             float Vt = techSpec.getThermalVoltage();

			 Gecode::IntVar idx(getSpace(), 0, 4);
			Gecode::rel(getSpace(), (length >= 5.0e-6f) >> (idx == 4));
			Gecode::rel(getSpace(), (length < 5.0e-6f && length >= 2.1e-6f) >> (idx == 3));
			Gecode::rel(getSpace(), (length < 2.1e-6f && length >= 1.2e-6f) >> (idx == 2));
			Gecode::rel(getSpace(), (length < 1.2e-6f && length >= 0.5e-6f) >> (idx == 1));
			Gecode::rel(getSpace(), (length < 0.5e-6f) >> (idx == 0));
	 
			 Gecode::FloatVar vth (getSpace(), -0.5, 0.5);
			 Gecode::FloatVar muCox (getSpace(), 0, 0.01);
			 Gecode::FloatVar n (getSpace(), 0, 2);
			 Gecode::FloatVar lamda (getSpace(), 0, 1);
			 Gecode::FloatVar theta (getSpace(), 0, 1);
			 Gecode::FloatVar eta (getSpace(), 0, 1);
		 
			 float muCox_list[5], vth_list[5], n_list[5], lamda_list[5], theta_list[5], eta_list[5];
			 vth_list[0] = techSpec.getThresholdVoltage_LMAX500();
			 vth_list[1] = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
			 vth_list[2] = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
			 vth_list[3] = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
			 vth_list[4] = techSpec.getThresholdVoltage_LMIN5000();
			 muCox_list[0] = techSpec.getMobilityOxideCapacityCoefficient_LMAX500();
			 muCox_list[1] = techSpec.getMobilityOxideCapacityCoefficient_LMIN500_LMAX1200();
			 muCox_list[2] = techSpec.getMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100();
			 muCox_list[3] = techSpec.getMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000();
			 muCox_list[4] = techSpec.getMobilityOxideCapacityCoefficient_LMIN5000();
			 n_list[0] = techSpec.getSlopeFactor_LMAX500();
			 n_list[1] = techSpec.getSlopeFactor_LMIN500_LMAX1200();
			 n_list[2] = techSpec.getSlopeFactor_LMIN1200_LMAX2100();
			 n_list[3] = techSpec.getSlopeFactor_LMIN2100_LMAX5000();
			 n_list[4] = techSpec.getSlopeFactor_LMIN5000();
			 lamda_list[0] = techSpec.getChannelLengthCoefficient_LMAX500();
			 lamda_list[1] = techSpec.getChannelLengthCoefficient_LMIN500_LMAX1200();
			 lamda_list[2] = techSpec.getChannelLengthCoefficient_LMIN1200_LMAX2100();
			 lamda_list[3] = techSpec.getChannelLengthCoefficient_LMIN2100_LMAX5000();
			 lamda_list[4] = techSpec.getChannelLengthCoefficient_LMIN5000();
			 theta_list[0] = techSpec.getMobilityReductionCoefficient_LMAX500();
			 theta_list[1] = techSpec.getMobilityReductionCoefficient_LMIN500_LMAX1200();
			 theta_list[2] = techSpec.getMobilityReductionCoefficient_LMIN1200_LMAX2100();
			 theta_list[3] = techSpec.getMobilityReductionCoefficient_LMIN2100_LMAX5000();
			 theta_list[4] = techSpec.getMobilityReductionCoefficient_LMIN5000();
			 eta_list[0] = techSpec.getDIBLCoefficient_LMAX500();
			 eta_list[1] = techSpec.getDIBLCoefficient_LMIN500_LMAX1200();
			 eta_list[2] = techSpec.getDIBLCoefficient_LMIN1200_LMAX2100();
			 eta_list[3] = techSpec.getDIBLCoefficient_LMIN2100_LMAX5000();
			 eta_list[4] = techSpec.getDIBLCoefficient_LMIN5000();
	 
			 for (int i =0; i<5; i++)
			 {
				 Gecode::rel(getSpace(), (idx == i) >> (vth == vth_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (muCox == muCox_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (n == n_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (lamda == lamda_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (theta == theta_list[i]));
				 Gecode::rel(getSpace(), (idx == i) >> (eta == eta_list[i]));
			 }
 
             /* */
			 Gecode::FloatVar widthLengthRatio = Gecode::expr(getSpace(), length * 2.0f / (muCox * width * multiplier));
             Gecode::rel(getSpace(), abs(ids)* widthLengthRatio >= pow(vOverDrive, 2));

             /* 
			Gecode::FloatVar vP (getSpace(), 0, 10);
			Gecode::rel(getSpace(), vP * n == vOverDrive);
			Gecode::FloatVar expF (getSpace(), 0, 1e6);
	 		Gecode::rel(getSpace(), expF == exp((vP) * (0.5 / Vt)));
			Gecode::FloatVar lnF (getSpace(), 0, 10);
	 		Gecode::rel(getSpace(), lnF == log(1 + expF));

             Gecode::FloatVar iOverDrive = Gecode::expr(getSpace(), 2 * (muCox * width * multiplier / length) * n * pow(Vt, 2) * pow(lnF, 2));
             Gecode::rel(getSpace(), abs(ids) >= iOverDrive);*/
	 }
 
	 void TransistorConstraintsEKV::createLinearConstraints(Partitioning::Component& component)
	 {
		createLinearVoltageConstraints(component);
	 }
 
	 void TransistorConstraintsEKV::createMinimalAreaConstraint(Partitioning::Component& component)
	 {
		 Gecode::FloatVal u = getSpace().getScalingFactorMUM();
		 Gecode::FloatVal p = 0.000000000001;
 
		 Gecode::FloatVar widthHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		 Gecode::FloatVar multiplierHelperVar(getSpace(), 1 , getSpace().getMultiplierUpperBound());
		 Gecode::FloatVar lengthHelperVar(getSpace(), 1, getSpace().getLengthUpperBound());
 
		 channel(getSpace(), getTransistorToWidthMap().find(component), widthHelperVar);
		 channel(getSpace(), getTransistorToMultiplierMap().find(component), multiplierHelperVar);
		 channel(getSpace(), getTransistorToLengthMap().find(component), lengthHelperVar);
 
		 Gecode::FloatVar width = Gecode::expr(getSpace(),widthHelperVar * u);
		 Gecode::FloatVar multiplier = Gecode::expr(getSpace(),multiplierHelperVar);
		 Gecode::FloatVar length = Gecode::expr(getSpace(), lengthHelperVar * u);
 
 
		 Gecode::rel(getSpace(), width * multiplier *  length >= 0.99* p * getCircuitInformation().getTechnologieSpecificationEKV(component).getMinArea());
		 Gecode::rel(getSpace(), length  >= 0.000001* getCircuitInformation().getTechnologieSpecificationEKV(component).getMinLength()-0.00000001);
		 Gecode::rel(getSpace(), width > pow(10,-6)* getCircuitInformation().getTechnologieSpecificationEKV(component).getMinWidth()-0.00000001);
		 Gecode::rel(getSpace(), multiplier > getCircuitInformation().getTechnologieSpecificationEKV(component).getMinMultiplier()-1);
	 }
 
	 Gecode::FloatVar TransistorConstraintsEKV::computeEdgeVoltage(
		 const Edge& edge)
	 {
		 const  Core::NetId startNetId = edge.getStartNodeId();
		 const Core::NetId endNetId = edge.getEndNodeId();
 
		 Gecode::FloatVar startNetVoltage = getSpace().createFloatVoltage(netToVoltageMap_->find(startNetId));
 
		 Gecode::FloatVar endNetVoltage = getSpace().createFloatVoltage(netToVoltageMap_->find(endNetId));
 
		 return Gecode::expr(getSpace(), startNetVoltage - endNetVoltage);
	 }
 
	 void TransistorConstraintsEKV::createSameRegionConstraintFoldedPair()
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
 
	 void TransistorConstraintsEKV::createSameRegionConstraint(Partitioning::Component& transistor1, Partitioning::Component & transistor2)
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

		 Gecode::FloatVal u = getSpace().getScalingFactorMUM();
		 Gecode::FloatVar widthHelperVar1(getSpace(), 1 , getSpace().getWidthUpperBound());
		 Gecode::FloatVar multiplierHelperVar1(getSpace(), 1 , getSpace().getMultiplierUpperBound());
		 Gecode::FloatVar lengthHelperVar1(getSpace(), 1, getSpace().getLengthUpperBound());
 
		 channel(getSpace(), getTransistorToWidthMap().find(transistor1), widthHelperVar1);
		 channel(getSpace(), getTransistorToMultiplierMap().find(transistor1), multiplierHelperVar1);
		 channel(getSpace(), getTransistorToLengthMap().find(transistor1), lengthHelperVar1);
 
		 Gecode::FloatVar width1 = Gecode::expr(getSpace(),widthHelperVar1*u);
		 Gecode::FloatVar multiplier1 = Gecode::expr(getSpace(),multiplierHelperVar1);
		 Gecode::FloatVar length1 = Gecode::expr(getSpace(), lengthHelperVar1*u);

		 Gecode::FloatVar widthHelperVar2(getSpace(), 1 , getSpace().getWidthUpperBound());
		 Gecode::FloatVar multiplierHelperVar2(getSpace(), 1 , getSpace().getMultiplierUpperBound());
		 Gecode::FloatVar lengthHelperVar2(getSpace(), 1, getSpace().getLengthUpperBound());
 
		 channel(getSpace(), getTransistorToWidthMap().find(transistor2), widthHelperVar2);
		 channel(getSpace(), getTransistorToMultiplierMap().find(transistor2), multiplierHelperVar2);
		 channel(getSpace(), getTransistorToLengthMap().find(transistor2), lengthHelperVar2);
 
		 Gecode::FloatVar width2 = Gecode::expr(getSpace(),widthHelperVar2*u);
		 Gecode::FloatVar multiplier2 = Gecode::expr(getSpace(),multiplierHelperVar2);
		 Gecode::FloatVar length2 = Gecode::expr(getSpace(), lengthHelperVar2*u);


		 const TechnologieSpecificationEKV & techSpecTran1 =  getCircuitInformation().getTechnologieSpecificationEKV(transistor1);
		 Gecode::FloatVar vdsTran1 = computeEdgeVoltage(*drainSourceEdgeTran1);
		 Gecode::FloatVar vgsTran1 = computeEdgeVoltage(*gateSourceEdgeTran1);
		 Gecode::FloatVar vthTran1;
		 Gecode::IntVar idx1(getSpace(), 0, 4);
		 Gecode::rel(getSpace(), (length1 >= 5.0e-6f) >> (idx1 == 4));
		 Gecode::rel(getSpace(), (length1 < 5.0e-6f && length1 >= 2.1e-6f) >> (idx1 == 3));
		 Gecode::rel(getSpace(), (length1 < 2.1e-6f && length1 >= 1.2e-6f) >> (idx1 == 2));
		 Gecode::rel(getSpace(), (length1 < 1.2e-6f && length1 >= 0.5e-6f) >> (idx1 == 1));
		 Gecode::rel(getSpace(), (length1 < 0.5e-6f) >> (idx1 == 0));

 
		 float vth_list1[5];
		 vth_list1[0] = techSpecTran1.getThresholdVoltage_LMAX500();
		 vth_list1[1] = techSpecTran1.getThresholdVoltage_LMIN500_LMAX1200();
		 vth_list1[2] = techSpecTran1.getThresholdVoltage_LMIN1200_LMAX2100();
		 vth_list1[3] = techSpecTran1.getThresholdVoltage_LMIN2100_LMAX5000();
		 vth_list1[4] = techSpecTran1.getThresholdVoltage_LMIN5000();
		
		 for (int i=0; i<5; i++)
		 {
			 Gecode::rel(getSpace(), (idx1 == i) >> (vthTran1 == vth_list1[i]));
		 }

 
		 const TechnologieSpecificationEKV & techSpecTran2 = getCircuitInformation().getTechnologieSpecificationEKV(transistor2);
		 Gecode::FloatVar vdsTran2 = computeEdgeVoltage(*drainSourceEdgeTran2);
		 Gecode::FloatVar vgsTran2 = computeEdgeVoltage(*gateSourceEdgeTran2);
		 Gecode::FloatVar vthTran2;
		 Gecode::IntVar idx2(getSpace(), 0, 4);
		 Gecode::rel(getSpace(), (length2 >= 5.0e-6f) >> (idx2 == 4));
		 Gecode::rel(getSpace(), (length2 < 5.0e-6f && length2 >= 2.1e-6f) >> (idx2 == 3));
		 Gecode::rel(getSpace(), (length2 < 2.1e-6f && length2 >= 1.2e-6f) >> (idx2 == 2));
		 Gecode::rel(getSpace(), (length2 < 1.2e-6f && length2 >= 0.5e-6f) >> (idx2 == 1));
		 Gecode::rel(getSpace(), (length2 < 0.5e-6f) >> (idx2 == 0));

		 float vth_list2[5];
		 vth_list2[0] = techSpecTran2.getThresholdVoltage_LMAX500();
		 vth_list2[1] = techSpecTran2.getThresholdVoltage_LMIN500_LMAX1200();
		 vth_list2[2] = techSpecTran2.getThresholdVoltage_LMIN1200_LMAX2100();
		 vth_list2[3] = techSpecTran2.getThresholdVoltage_LMIN2100_LMAX5000();
		 vth_list2[4] = techSpecTran2.getThresholdVoltage_LMIN5000();
	   
		 for (int i=0; i<5; i++)
		 {
			 Gecode::rel(getSpace(), (idx2 == i) >> (vthTran2 == vth_list2[i]));
		 }
 
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
 
	 bool TransistorConstraintsEKV::isCurrentBias(const StructRec::Structure & structure) const
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
 
	 bool TransistorConstraintsEKV::isCurrentMirror(const StructRec::Structure & structure) const
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
 
 