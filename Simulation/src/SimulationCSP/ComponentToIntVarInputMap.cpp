#include "Simulation/incl/SimulationCSP/ComponentToIntVarInputMap.h"
#include "Partitioning/incl/Results/Component.h"
#include "Simulation/incl/Results/Result.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include <sstream>
#include <cmath>

namespace Simulation {

    ComponentToIntVarInputMap::ComponentToIntVarInputMap() 
    { 
    }

    void ComponentToIntVarInputMap::setDefinition(std::string definition)
    {
        definition_ = definition;
    }

    void ComponentToIntVarInputMap::add(
            const StructRec::StructureId structureId,
            float value)
    {
        assert(!has(structureId));
        intVarMap_[structureId] = value;
    }

    void ComponentToIntVarInputMap::add(
            Partitioning::Component & component,
            float value)
    {
        //assert(!has(component));
        intVarMap_[component.getArray().getIdentifier()] = value;
    }

    float ComponentToIntVarInputMap::find(
            const Partitioning::Component& component) const
    {
        assert(has(component));
        return intVarMap_.at(component.getArray().getIdentifier());
    }

    float ComponentToIntVarInputMap::find(
            const StructRec::StructureId& structureId) const
    {
        assert(intVarMap_.find(structureId) != intVarMap_.end(), "Component to intVar Map does not have this component.");
        return intVarMap_.at(structureId);
    }

    bool ComponentToIntVarInputMap::has(
            const Partitioning::Component& component) const
    {
        return intVarMap_.find(component.getArray().getIdentifier()) != intVarMap_.end();
    }

    bool ComponentToIntVarInputMap::has(
            const StructRec::StructureId structureId) const
    {
        return intVarMap_.find(structureId) != intVarMap_.end();
    }

    void ComponentToIntVarInputMap::writeWidthToTransistor(Result & result)
    {
        for (floatVarInputMap::const_iterator it = intVarMap_.begin(); it != intVarMap_.end(); ++it) // Updated to floatVarInputMap
        {
            Partitioning::Component & comp = result.findComponent(it->first);
            if (comp.isTransistor())
            {
                Partitioning::Transistor & tran = static_cast<Partitioning::Transistor & >(comp);
                float value = it->second; // Use float
                if (definition_ == "1mum")
                {
                    tran.setWidth(value);
                }
                else if (definition_ == "0.1mum")
                {
                    tran.setWidth(value / 10.0);
                }
            }
        }
    }
    void ComponentToIntVarInputMap::writeMultiplierToTransistor(Result & result)
    {
        for (floatVarInputMap::const_iterator it = intVarMap_.begin(); it != intVarMap_.end(); ++it) // Updated to floatVarInputMap
        {
            Partitioning::Component & comp = result.findComponent(it->first);
            if (comp.isTransistor())
            {
                Partitioning::Transistor & tran = static_cast<Partitioning::Transistor & >(comp);
                float value = it->second; // Use float
                tran.setMultiplier(static_cast<int>(value));
            }
        }
    }

    void ComponentToIntVarInputMap::writeCurrent(Result & result)
    {
        for (floatVarInputMap::const_iterator it = intVarMap_.begin(); it != intVarMap_.end(); ++it) // Updated to floatVarInputMap
        {
            StructRec::StructureId structureId = it->first;
            float current = it->second; // Use float
            result.addCurrent(result.findComponent(structureId), current * pow(10, -3));
        }
    }

    void ComponentToIntVarInputMap::writeLengthToTransistor(Result & result) {
        for (floatVarInputMap::const_iterator it = intVarMap_.begin(); it != intVarMap_.end(); ++it) // Updated to floatVarInputMap
        {
            Partitioning::Component & comp = result.findComponent(it->first);
            if (comp.isTransistor()) {
                Partitioning::Transistor & tran = static_cast<Partitioning::Transistor & >(comp);
                float value = it->second; // Use float
                if (definition_ == "1mum") {
                    tran.setLength(value);
                } else if (definition_ == "0.1mum") {
                    tran.setLength(value / 10.0);
                }
            }
        }
    }

    void ComponentToIntVarInputMap::writeValueToCapacitance(Result & result)
    {
        for (floatVarInputMap::const_iterator it = intVarMap_.begin(); it != intVarMap_.end(); ++it) // Updated to floatVarInputMap
        {
            Partitioning::Component & comp = result.findComponent(it->first);
            if (comp.isTwoPort())
            {
                Partitioning::TwoPort & twoPort = static_cast<Partitioning::TwoPort & >(comp);
                float value = it->second; 
                /*
                if (twoPort.getArray().getStructureName().toStr() == "CapacitorArray")
                {
                    twoPort.setValue(value / 10.0);
                }
                else
                {
                    twoPort.setValue(value);
                }
                */
                twoPort.setValue(value);
            }
        }
    }

    std::string ComponentToIntVarInputMap::toStr() const {
        std::ostringstream oss;
        oss << std::endl;
        for (floatVarInputMap::const_iterator it = intVarMap_.begin(); it != intVarMap_.end(); ++it) // Updated to floatVarInputMap
        {
            float value = it->second;
            if (definition_ == "1mum") {
                oss << it->first << " " << value << std::endl;
            } else if (definition_ == "0.1mum") {
                oss << it->first << " " << value / 10.0 << std::endl;
            } else {
                StructRec::StructureId id = it->first;
                if (id.getName().toStr() == "CapacitorArray") {
                    oss << it->first << " " << value / 10.0 << std::endl;
                } else {
                    oss << it->first << " " << value << std::endl;
                }
            }
        }
        return oss.str();
    }

}