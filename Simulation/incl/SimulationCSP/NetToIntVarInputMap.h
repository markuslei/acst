#ifndef SIMULATION_INCL_SimulationCSP_NETTOINTVARINPUTMAP_H_
#define SIMULATION_INCL_SimulationCSP_NETTOINTVARINPUTMAP_H_

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include <map>
#include <string>
#include <sstream>

namespace Simulation {

    class Result;

    class NetToIntVarInputMap
    {
    private:
        typedef std::map<const Core::NetId, int> IntVarMap;
    public:
        NetToIntVarInputMap();

        void add(Core::NetId netId, double intVar);
        double find(const Core::NetId netId) const;

        void write(Result & result) const;

        //For Debugging
        //void setVoltages();

        std::string toStr() const;

    private:
        bool has(const Core::NetId netId) const;

        IntVarMap intVarMap_;
    };
}

#endif /* SIMULATION_INCL_SimulationCSP_NETTOINTVARINPUTMAP_H_ */
