#ifndef SIMULATION_INCL_SimulationCSP_COMPONENTTOINTVARINPUTMAP_H_
#define SIMULATION_INCL_SimulationCSP_COMPONENTTOINTVARINPUTMAP_H_

#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureId.h"
#include <map>
#include <string>

namespace Partitioning {
    class Component;
}

namespace Simulation {

    typedef std::map<const StructRec::StructureId, float> floatVarInputMap; 

    class Result;

    class ComponentToIntVarInputMap
    {
    public:
        ComponentToIntVarInputMap();

        void setDefinition(std::string definition);

        void add(const StructRec::StructureId, float value);
        void add(Partitioning::Component & component, float value);

        float find(const Partitioning::Component & component) const;
        float find(const StructRec::StructureId& structureId) const;

        void writeWidthToTransistor(Result & result);
        void writeMultiplierToTransistor(Result & result);
        void writeLengthToTransistor(Result & result);
        void writeValueToCapacitance(Result & result);
        void writeCurrent(Result & result);

        std::string toStr() const;

    private:
        bool has(const Partitioning::Component& component) const;
        bool has(const StructRec::StructureId structRecId) const;

        floatVarInputMap intVarMap_; 
        std::string definition_;
    };

}

#endif /* SIMULATION_INCL_SimulationCSP_COMPONENTTOINTVARINPUTMAP_H_ */