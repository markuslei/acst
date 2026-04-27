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




 #ifndef SIMULATION_INCL_SimulationCSP_CONSTRAINTS_SIMULATIONTRANSISTORCONSTRAINTSEKV_H_
 #define SIMULATION_INCL_SimulationCSP_CONSTRAINTS_SIMULATIONTRANSISTORCONSTRAINTSEKV_H_
 
 #include "Core/incl/Circuit/Net/NetId/NetId.h"
 #include "Simulation/incl/SimulationCSP/ComponentToIntVarInputMap.h"
 
 #include <gecode/float.hh>
 
 namespace StructRec {
 
     class Structure;
 
 }
 
 namespace Partitioning {
 
     class Result;
     class Component;
 
 }
 
 namespace Simulation
 {
     class Graph;
     class NetToIntVarMap;
     class ComponentToIntVarMap;
     class CircuitInformation;
     class SimulationSpace;
     class Edge;
 
     class SimulationTransistorConstraintsEKV
     {
     public:
         SimulationTransistorConstraintsEKV();
 
         void setGraph(const Graph & graph);
         void setCircuitInformation(const CircuitInformation & information);
         void setPartitioningResult(const Partitioning::Result & result);
         void setTransistorToWidthMap(ComponentToIntVarInputMap & widthMap );
         void setTransistorToMultiplierMap(ComponentToIntVarInputMap & multiplierMap );
         void setTransistorToLengthMap(ComponentToIntVarInputMap & lenghtMap);
         void setTransistorToCurrentMap(ComponentToIntVarMap & currentMap);
         void setNetToVoltageMap(NetToIntVarMap & voltageMap);
         void setSpace(SimulationSpace & space);
         void setEKVVersion(int version);

         void createConstraints();
 
     private:
         const Graph & getGraph() const;
         const CircuitInformation & getCircuitInformation() const;
         const Partitioning::Result & getPartitioningResult() const;
         ComponentToIntVarInputMap & getTransistorToWidthMap();
         ComponentToIntVarInputMap & getTransistorToMultiplierMap();
         ComponentToIntVarInputMap & getTransistorToLengthMap();
         ComponentToIntVarMap & getTransistorToCurrentMap();
         NetToIntVarMap & getNetToVoltageMap();
         SimulationSpace & getSpace();
         int getEKVVersion() const;
 
         void createSaturationConstraints(Partitioning::Component & component);
         void createLinearConstraints(Partitioning::Component & component);
         void createOffConstraint(Partitioning::Component & component);
         void createCompensationResistorConstraint(Partitioning::Component & component);
 
         void createSameRegionConstraintFoldedPair();
         void createSameRegionConstraint(Partitioning::Component& transistor1, Partitioning::Component & transistor2);
 
         void createCurrentConstraints(Partitioning::Component & component);
         //void createLinearCurrentConstraints(Partitioning::Component & component);
         //void createSaturationCurrentConstraintsStrongInversion(Partitioning::Component & component);
         //void createSaturationCurrentConstraintsWeakInversion(Partitioning::Component & component);
         void createDependencyConstraints(Partitioning::Component & component);
 
         void createMinimalAreaConstraint(Partitioning::Component & component);
 
         void createGateOverDriveVoltageConstraint(Partitioning::Component & component);
 
 
         bool isLowerTransistorOf4TransistorCurrentMirror(Partitioning::Component & component);
 
         bool isOutputNet(Core::NetId net) const;
         bool isCurrentBias(const StructRec::Structure & structure) const;
         bool isCurrentMirror(const StructRec::Structure & structure) const;
 
         Gecode::FloatVar computeEdgeVoltage(const Edge & edge);
 
     private:
         const Graph * graph_;
         const CircuitInformation * circuitInformation_;
         const Partitioning::Result * partitioningResult_;
         ComponentToIntVarMap * transistorToWidthMap_;
         ComponentToIntVarMap* transistorToMultiplierMap_;
         ComponentToIntVarMap* transistorToLengthMap_;
         ComponentToIntVarInputMap * transistorToWidthInputMap_;
         ComponentToIntVarInputMap * transistorToMultiplierInputMap_;
         ComponentToIntVarInputMap* transistorToLengthInputMap_;			
         ComponentToIntVarMap * transistorToCurrentMap_;
         NetToIntVarMap * netToVoltageMap_;
         SimulationSpace * space_;
         int ekvVersion_;

 };
 
 }
 
 
 // tHIS LINE SHOULD BE INSPECTED
 #endif /* SIMULATION_INCL_SIMULATION_CONSTRAINTS_TRANSISTORCONSTRAINTS_H_ */
 