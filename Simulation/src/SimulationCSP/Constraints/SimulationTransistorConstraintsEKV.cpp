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


 #include "Simulation/incl/SimulationCSP/Constraints/SimulationTransistorConstraintsEKV.h"

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
 
     SimulationTransistorConstraintsEKV::SimulationTransistorConstraintsEKV():
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
 
     void SimulationTransistorConstraintsEKV::setGraph(
         const Graph& graph)
     {
         graph_ = & graph;
     }
 
     void SimulationTransistorConstraintsEKV::setTransistorToWidthMap(
         ComponentToIntVarInputMap& widthMap)
     {
         transistorToWidthInputMap_ = & widthMap;
     }

     void SimulationTransistorConstraintsEKV::setTransistorToMultiplierMap(
         ComponentToIntVarInputMap& multiplierMap)
     {
         transistorToMultiplierInputMap_ = & multiplierMap;
     }
 
     void SimulationTransistorConstraintsEKV::setTransistorToLengthMap(
         ComponentToIntVarInputMap& lengthMap)
     {
         transistorToLengthInputMap_ = & lengthMap;
     }
 
     void SimulationTransistorConstraintsEKV::setTransistorToCurrentMap(
         ComponentToIntVarMap& currentMap)
     {
         transistorToCurrentMap_ = & currentMap;
     }
 
     void SimulationTransistorConstraintsEKV::setNetToVoltageMap(
         NetToIntVarMap& voltageMap)
     {
         netToVoltageMap_ = & voltageMap;
     }
 
     void SimulationTransistorConstraintsEKV::setSpace(
         SimulationSpace& space)
     {
         space_ = & space;
     }

     void SimulationTransistorConstraintsEKV::setEKVVersion(int version)
     {
         ekvVersion_ = version;
     }

     int SimulationTransistorConstraintsEKV::getEKVVersion() const
     {
         assert(ekvVersion_ == 1 || ekvVersion_ == 2 || ekvVersion_ == 3, "EKV version not supported");
         return ekvVersion_;
     }
 
     void SimulationTransistorConstraintsEKV::createConstraints()
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
 
     const Graph& SimulationTransistorConstraintsEKV::getGraph() const
     {
         assert(graph_ != NULL);
         return * graph_;
     }
 
     ComponentToIntVarInputMap& SimulationTransistorConstraintsEKV::getTransistorToWidthMap()
     {
         assert(transistorToWidthInputMap_ != NULL);
         return *transistorToWidthInputMap_;
     }

     ComponentToIntVarInputMap& SimulationTransistorConstraintsEKV::getTransistorToMultiplierMap()
     {
         assert(transistorToMultiplierInputMap_ != NULL);
         return * transistorToMultiplierInputMap_;
     }
 
     ComponentToIntVarInputMap& SimulationTransistorConstraintsEKV::getTransistorToLengthMap()
     {
         assert(transistorToLengthInputMap_ != NULL);
         return * transistorToLengthInputMap_;
     }
 
     ComponentToIntVarMap& SimulationTransistorConstraintsEKV::getTransistorToCurrentMap()
     {
         assert(transistorToCurrentMap_ != NULL);
         return * transistorToCurrentMap_;
     }
 
     NetToIntVarMap& SimulationTransistorConstraintsEKV::getNetToVoltageMap()
     {
         assert(netToVoltageMap_ != NULL);
         return * netToVoltageMap_;
     }
 
     SimulationSpace& SimulationTransistorConstraintsEKV::getSpace()
     {
         assert(space_ != NULL);
         return *space_;
     }
 

     void SimulationTransistorConstraintsEKV::createCurrentConstraints(Partitioning::Component & component)
     {
         TechnologieSpecificationEKV techSpec;
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
         float lengthValue = getTransistorToLengthMap().find(component) * u;
         float widthValue = getTransistorToWidthMap().find(component) * u;
         float multiplierValue = getTransistorToMultiplierMap().find(component);
         logDebug("Length value: " << lengthValue << ", Width value: " << widthValue << ", Multiplier value: " << multiplierValue);
 
         if(component.getArray().getTechType().isN())
         {
            techSpec = getCircuitInformation().getTechnologieSpecificationEKVNmos();
            Gecode::FloatVal vth;
            Gecode::FloatVal muCox;
            Gecode::FloatVal n;
            Gecode::FloatVal lamda;
            Gecode::FloatVal theta;
            Gecode::FloatVal eta;
		

            if (lengthValue >= 5.0e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN5000();
                muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN5000();
                n = techSpec.getSlopeFactor_LMIN5000();
                lamda = techSpec.getChannelLengthCoefficient_LMIN5000();
                theta = techSpec.getMobilityReductionCoefficient_LMIN5000();
                eta = techSpec.getDIBLCoefficient_LMIN5000();
            } else if (lengthValue >= 2.1e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
                muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000();
                n = techSpec.getSlopeFactor_LMIN2100_LMAX5000();
                lamda = techSpec.getChannelLengthCoefficient_LMIN2100_LMAX5000();
                theta = techSpec.getMobilityReductionCoefficient_LMIN2100_LMAX5000();
                eta = techSpec.getDIBLCoefficient_LMIN2100_LMAX5000();
            } else if (lengthValue >= 1.2e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
                muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100();
                n = techSpec.getSlopeFactor_LMIN1200_LMAX2100();
                lamda = techSpec.getChannelLengthCoefficient_LMIN1200_LMAX2100();
                theta = techSpec.getMobilityReductionCoefficient_LMIN1200_LMAX2100();
                eta = techSpec.getDIBLCoefficient_LMIN1200_LMAX2100();
            } else if (lengthValue >= 5e-8f) {
                vth = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
                muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN500_LMAX1200();
                n = techSpec.getSlopeFactor_LMIN500_LMAX1200();
                lamda = techSpec.getChannelLengthCoefficient_LMIN500_LMAX1200();
                theta = techSpec.getMobilityReductionCoefficient_LMIN500_LMAX1200();
                eta = techSpec.getDIBLCoefficient_LMIN500_LMAX1200();
                } else {
                vth = techSpec.getThresholdVoltage_LMAX500();
                muCox = techSpec.getMobilityOxideCapacityCoefficient_LMAX500();
                n = techSpec.getSlopeFactor_LMAX500();
                lamda = techSpec.getChannelLengthCoefficient_LMAX500();
                theta = techSpec.getMobilityReductionCoefficient_LMAX500();
                eta = techSpec.getDIBLCoefficient_LMAX500();
            }


             Gecode::FloatVal Vt = techSpec.getThermalVoltage();
 
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
                    Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP - vds) * (0.5 / Vt))));
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
             Gecode::FloatVal vth;
             Gecode::FloatVal muCox;
             Gecode::FloatVal n;
             Gecode::FloatVal lamda;
             Gecode::FloatVal theta;
             Gecode::FloatVal eta;

             if (lengthValue >= 5.0e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN5000();
                muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN5000();
                n = techSpec.getSlopeFactor_LMIN5000();
                lamda = techSpec.getChannelLengthCoefficient_LMIN5000();
                theta = techSpec.getMobilityReductionCoefficient_LMIN5000();
                eta = techSpec.getDIBLCoefficient_LMIN5000();
            } else if (lengthValue >= 2.1e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
                muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000();
                n = techSpec.getSlopeFactor_LMIN2100_LMAX5000();
                lamda = techSpec.getChannelLengthCoefficient_LMIN2100_LMAX5000();
                theta = techSpec.getMobilityReductionCoefficient_LMIN2100_LMAX5000();
                eta = techSpec.getDIBLCoefficient_LMIN2100_LMAX5000();
            } else if (lengthValue >= 1.2e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
                muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100();
                n = techSpec.getSlopeFactor_LMIN1200_LMAX2100();
                lamda = techSpec.getChannelLengthCoefficient_LMIN1200_LMAX2100();
                theta = techSpec.getMobilityReductionCoefficient_LMIN1200_LMAX2100();
                eta = techSpec.getDIBLCoefficient_LMIN1200_LMAX2100();
            } else if (lengthValue >= 5e-8f) {
                vth = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
                muCox = techSpec.getMobilityOxideCapacityCoefficient_LMIN500_LMAX1200();
                n = techSpec.getSlopeFactor_LMIN500_LMAX1200();
                lamda = techSpec.getChannelLengthCoefficient_LMIN500_LMAX1200();
                theta = techSpec.getMobilityReductionCoefficient_LMIN500_LMAX1200();
                eta = techSpec.getDIBLCoefficient_LMIN500_LMAX1200();
            } else {
                vth = techSpec.getThresholdVoltage_LMAX500();
                muCox = techSpec.getMobilityOxideCapacityCoefficient_LMAX500();
                n = techSpec.getSlopeFactor_LMAX500();
                lamda = techSpec.getChannelLengthCoefficient_LMAX500();
                theta = techSpec.getMobilityReductionCoefficient_LMAX500();
                eta = techSpec.getDIBLCoefficient_LMAX500();
            }
             

             Gecode::FloatVal Vt = techSpec.getThermalVoltage();
 
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
                    Gecode::FloatVar lnR = expr(getSpace(), log(1 + exp((vP + vds) * (0.5 / Vt))));
                    Gecode::FloatVar iS (getSpace(), 0, 1);
                    Gecode::rel(getSpace(), iS == 2*multiplier * muCox *(width/length) * n * pow(Vt, 2));
                    Gecode::FloatVar scaleLamda (getSpace(), 1, 2);
                    Gecode::rel(getSpace(), scaleLamda == (1 - lamda * vds));
                    Gecode::FloatVar i_ekv (getSpace(), -1, 0);
                    Gecode::rel (getSpace(), i_ekv == - iS * (pow(lnF, 2) - pow(lnR, 2)) * scaleLamda);
                    Gecode::rel(getSpace(), i_ekv / ids <= 1.01);
                    Gecode::rel(getSpace(), i_ekv / ids >= 0.99);
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
 
 
     bool SimulationTransistorConstraintsEKV::isOutputNet(Core::NetId net) const
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
 
     void SimulationTransistorConstraintsEKV::setCircuitInformation(
         const CircuitInformation& information)
     {
         circuitInformation_ = & information;
     }
 
     void SimulationTransistorConstraintsEKV::setPartitioningResult(
         const Partitioning::Result & result)
     {
         partitioningResult_ = & result;
     }
 
     const CircuitInformation& SimulationTransistorConstraintsEKV::getCircuitInformation() const
     {
         assert(circuitInformation_ != NULL);
         return * circuitInformation_;
     }
 
     const Partitioning::Result& SimulationTransistorConstraintsEKV::getPartitioningResult() const
     {
         assert(partitioningResult_ != NULL);
         return * partitioningResult_;
     }
 
     void SimulationTransistorConstraintsEKV::createCompensationResistorConstraint(Partitioning::Component & component)
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
 
 
         rel(getSpace(), ids == 0);
         rel(getSpace(), vds == 0);

         float u = getSpace().getScalingFactorMUM();
         Gecode::FloatVar width(getSpace(), getTransistorToWidthMap().find(component) * u, getTransistorToWidthMap().find(component) * u);
         Gecode::FloatVar length(getSpace(), getTransistorToLengthMap().find(component) * u, getTransistorToLengthMap().find(component) * u);
         Gecode::FloatVar multiplier(getSpace(), getTransistorToMultiplierMap().find(component), getTransistorToMultiplierMap().find(component));
         float lengthValue = getTransistorToLengthMap().find(component) * u;
         //logDebug("Length value: " << lengthValue);
 
 
         if(component.getArray().getTechType().isN())
         {
             techSpec = getCircuitInformation().getTechnologieSpecificationEKVNmos();
             Gecode::FloatVal vth;
             if (lengthValue >= 5.0e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN5000();
             } else if (lengthValue >= 2.1e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
             } else if (lengthValue >= 1.2e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
             } else if (lengthValue >= 5e-8f) {
                vth = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
             } else {
                vth = techSpec.getThresholdVoltage_LMAX500();
             }
             Gecode::rel(getSpace(), vgs - vth > 0);
         }
         else
         {
             techSpec = getCircuitInformation().getTechnologieSpecificationEKVPmos();
             Gecode::FloatVal vth;
             if (lengthValue >= 5.0e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN5000();
             } else if (lengthValue >= 2.1e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
             } else if (lengthValue >= 1.2e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
             } else if (lengthValue >= 5e-8f) {
                vth = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
             } else {
                vth = techSpec.getThresholdVoltage_LMAX500();
             }
             Gecode::rel(getSpace(), vgs - vth < 0);
         }
 
 
     }
 
     void SimulationTransistorConstraintsEKV::createOffConstraint(Partitioning::Component & component)
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

         float u = getSpace().getScalingFactorMUM();
         Gecode::FloatVar width(getSpace(), getTransistorToWidthMap().find(component) * u, getTransistorToWidthMap().find(component) * u);
         Gecode::FloatVar length(getSpace(), getTransistorToLengthMap().find(component) * u, getTransistorToLengthMap().find(component) * u);
         Gecode::FloatVar multiplier(getSpace(), getTransistorToMultiplierMap().find(component), getTransistorToMultiplierMap().find(component));   
         float lengthValue = getTransistorToLengthMap().find(component) * u;
         //logDebug("Length value: " << lengthValue);
 
         if(component.getArray().getTechType().isN())
         {
             techSpec = getCircuitInformation().getTechnologieSpecificationEKVNmos();
             Gecode::FloatVal vth;
             if (lengthValue >= 5.0e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN5000();
             } else if (lengthValue >= 2.1e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
             } else if (lengthValue >= 1.2e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
             } else if (lengthValue >= 5e-8f) {
                vth = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
             } else {
                vth = techSpec.getThresholdVoltage_LMAX500();
             }
             Gecode::rel(getSpace(), vgs - vth < 0);
         }
         else
         {
             techSpec = getCircuitInformation().getTechnologieSpecificationEKVPmos();
             Gecode::FloatVal vth;
             if (lengthValue >= 5.0e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN5000();
             } else if (lengthValue >= 2.1e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN2100_LMAX5000();
             } else if (lengthValue >= 1.2e-7f) {
                vth = techSpec.getThresholdVoltage_LMIN1200_LMAX2100();
             } else if (lengthValue >= 5e-8f) {
                vth = techSpec.getThresholdVoltage_LMIN500_LMAX1200();
             } else {
                vth = techSpec.getThresholdVoltage_LMAX500();
             }
             Gecode::rel(getSpace(), vgs - vth > 0);
         }
     }
 

 
     bool SimulationTransistorConstraintsEKV::isLowerTransistorOf4TransistorCurrentMirror(
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
 
     void SimulationTransistorConstraintsEKV::createSaturationConstraints(Partitioning::Component& component)
     {
        createCurrentConstraints(component);
     }
 
 
     void SimulationTransistorConstraintsEKV::createLinearConstraints(Partitioning::Component& component)
     {
         createCurrentConstraints(component);
     }
 
 
     Gecode::FloatVar SimulationTransistorConstraintsEKV::computeEdgeVoltage(
         const Edge& edge)
     {
         const  Core::NetId startNetId = edge.getStartNodeId();
         const Core::NetId endNetId = edge.getEndNodeId();
 
         Gecode::FloatVar startNetVoltage = getSpace().createFloatVoltage(netToVoltageMap_->find(startNetId));
 
         Gecode::FloatVar endNetVoltage = getSpace().createFloatVoltage(netToVoltageMap_->find(endNetId));
 
         return Gecode::expr(getSpace(), startNetVoltage - endNetVoltage);
     }
 
     void SimulationTransistorConstraintsEKV::createSameRegionConstraintFoldedPair()
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
 
     void SimulationTransistorConstraintsEKV::createSameRegionConstraint(Partitioning::Component& transistor1, Partitioning::Component & transistor2)
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

 
         float u = getSpace().getScalingFactorMUM();
         float length1Value = getTransistorToLengthMap().find(transistor1) * u;
         float length2Value = getTransistorToLengthMap().find(transistor2) * u;
         //logDebug("Length1 value: " << length1Value << " Length2 value: " << length2Value);


         const TechnologieSpecificationEKV & techSpecTran1 =  getCircuitInformation().getTechnologieSpecificationEKV(transistor1);
         Gecode::FloatVar vdsTran1 = computeEdgeVoltage(*drainSourceEdgeTran1);
         Gecode::FloatVar vgsTran1 = computeEdgeVoltage(*gateSourceEdgeTran1);
         Gecode::FloatVal vthTran1;
         if (length1Value >= 5.0e-7f) {
            vthTran1 = techSpecTran1.getThresholdVoltage_LMIN5000();
         } else if (length1Value >= 2.1e-7f) {
            vthTran1 = techSpecTran1.getThresholdVoltage_LMIN2100_LMAX5000();
         } else if (length1Value >= 1.2e-7f) {
            vthTran1 = techSpecTran1.getThresholdVoltage_LMIN1200_LMAX2100();
         } else if (length1Value >= 5e-8f) {
            vthTran1 = techSpecTran1.getThresholdVoltage_LMIN500_LMAX1200();
         } else {
            vthTran1 = techSpecTran1.getThresholdVoltage_LMAX500();
         }
 
         const TechnologieSpecificationEKV & techSpecTran2 = getCircuitInformation().getTechnologieSpecificationEKV(transistor2);
         Gecode::FloatVar vdsTran2 = computeEdgeVoltage(*drainSourceEdgeTran2);
         Gecode::FloatVar vgsTran2 = computeEdgeVoltage(*gateSourceEdgeTran2);
         Gecode::FloatVal vthTran2;
         if (length2Value >= 5.0e-7f) {
            vthTran2 = techSpecTran2.getThresholdVoltage_LMIN5000();
         } else if (length2Value >= 2.1e-7f) {
            vthTran2 = techSpecTran2.getThresholdVoltage_LMIN2100_LMAX5000();
         } else if (length2Value >= 1.2e-7f) {
            vthTran2 = techSpecTran2.getThresholdVoltage_LMIN1200_LMAX2100();
         } else if (length2Value >= 5e-8f) {
            vthTran2 = techSpecTran2.getThresholdVoltage_LMIN500_LMAX1200();
         } else {
            vthTran2 = techSpecTran2.getThresholdVoltage_LMAX500();
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
 
     bool SimulationTransistorConstraintsEKV::isCurrentBias(const StructRec::Structure & structure) const
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
 
     bool SimulationTransistorConstraintsEKV::isCurrentMirror(const StructRec::Structure & structure) const
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
 
 