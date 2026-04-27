#include "Partitioning/incl/Results/Result.h"
#include "Simulation/incl/SimulationCSP/NetToIntVarInputMap.h"
#include "Simulation/incl/Results/Result.h"
#include "Log/incl/LogMacros.h"

namespace Simulation {

    NetToIntVarInputMap::NetToIntVarInputMap()
    {
    }

    void NetToIntVarInputMap::add(Core::NetId netId, double intVar)
    {
        assert(!has(netId));
        intVarMap_[netId] = intVar;
    }

    double NetToIntVarInputMap::find(const Core::NetId netId) const
    {
        assert(has(netId));
        return intVarMap_.at(netId);
    }

    bool NetToIntVarInputMap::has(const Core::NetId netId) const
    {
        return intVarMap_.find(netId) != intVarMap_.end();
    }

    void NetToIntVarInputMap::write(Result & result) const
    {
        for(IntVarMap::const_iterator it = intVarMap_.begin(); it != intVarMap_.end(); it++)
        {
            Core::NetId netId = it->first;
            int voltage = it->second;
            float floatVoltage = static_cast<float>(voltage);
            result.addVoltage(netId, floatVoltage / 1000);
        }
    }

    std::string NetToIntVarInputMap::toStr() const
    {
        std::ostringstream oss;
        oss << std::endl;
        for(IntVarMap::const_iterator it = intVarMap_.begin(); it != intVarMap_.end(); it++)
        {
            oss << it->first << " " << it->second << std::endl;
        }
        return oss.str();
    }
/*
    void NetToIntVarInputMap::setVoltages()
    {
        IntVarMap::const_iterator it = intVarMap_.begin();
        if (it != intVarMap_.end()) { it->second = 0; it++; }
        if (it != intVarMap_.end()) { it->second = 0.7; it++; }
        if (it != intVarMap_.end()) { it->second = 1.5; it++; }
        if (it != intVarMap_.end()) { it->second = 1.5; it++; }
        if (it != intVarMap_.end()) { it->second = 1.2; it++; }
        if (it != intVarMap_.end()) { it->second = 1.5; it++; }
        if (it != intVarMap_.end()) { it->second = 2.22; it++; }
        if (it != intVarMap_.end()) { it->second = 1.1; it++; }
        if (it != intVarMap_.end()) { it->second = 2.3; it++; }
        if (it != intVarMap_.end()) { it->second = 2.3; it++; }
        if (it != intVarMap_.end()) { it->second = 5.4; it++; }
        if (it != intVarMap_.end()) { it->second = 5.4; it++; }
        if (it != intVarMap_.end()) { it->second = 1.5; it++; }
        if (it != intVarMap_.end()) { it->second = 3; }
    }
*/
}
