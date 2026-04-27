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


#include "../../../Simulation/incl/CircuitInformation/CircuitInformation.h"
#include "../../../Simulation/incl/CircuitInformation/TechnologieSpecificationEKV.h"
#include "../../incl/CircuitInformationFiles/TechnologieFileEKV.h"
#include "Log/incl/LogMacros.h"
#include "Core/incl/Common/Backtrace.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Simulation {

	const std::string TechnologieFileEKV::GENERAL_NODE_ = "general";
	const std::string TechnologieFileEKV::THERMAL_VOLTAGE_NODE_ = "thermalVoltage";
	const std::string TechnologieFileEKV::THERMAL_VOLTAGE_ATTRIBUTE_ = "Vt";

	const std::string TechnologieFileEKV::PMOS_NODE_ = "pmos";
	const std::string TechnologieFileEKV::NMOS_NODE_ = "nmos";
	const std::string TechnologieFileEKV::THRESHOLD_VOLTAGE_EKV_LMAX500_NODE_ = "thresholdVoltageEKV_LMAX500";
	const std::string TechnologieFileEKV::THRESHOLD_VOLTAGE_EKV_LMIN500_LMAX1200_NODE_ = "thresholdVoltageEKV_LMIN500_LMAX1200";
	const std::string TechnologieFileEKV::THRESHOLD_VOLTAGE_EKV_LMIN1200_LMAX2100_NODE_ = "thresholdVoltageEKV_LMIN1200_LMAX2100";
	const std::string TechnologieFileEKV::THRESHOLD_VOLTAGE_EKV_LMIN2100_LMAX5000_NODE_ = "thresholdVoltageEKV_LMIN2100_LMAX5000";
	const std::string TechnologieFileEKV::THRESHOLD_VOLTAGE_EKV_LMIN5000_NODE_ = "thresholdVoltageEKV_LMIN5000";
	const std::string TechnologieFileEKV::MOBILITY_OXIDE_CAPACITY_EKV_LMAX500_NODE_ = "mobilityOxideCapacityEKV_LMAX500";
	const std::string TechnologieFileEKV::MOBILITY_OXIDE_CAPACITY_EKV_LMIN500_LMAX1200_NODE_ = "mobilityOxideCapacityEKV_LMIN500_LMAX1200";
	const std::string TechnologieFileEKV::MOBILITY_OXIDE_CAPACITY_EKV_LMIN1200_LMAX2100_NODE_ = "mobilityOxideCapacityEKV_LMIN1200_LMAX2100";
	const std::string TechnologieFileEKV::MOBILITY_OXIDE_CAPACITY_EKV_LMIN2100_LMAX5000_NODE_ = "mobilityOxideCapacityEKV_LMIN2100_LMAX5000";
	const std::string TechnologieFileEKV::MOBILITY_OXIDE_CAPACITY_EKV_LMIN5000_NODE_ = "mobilityOxideCapacityEKV_LMIN5000";
	const std::string TechnologieFileEKV::EARLY_VOLTAGE_NODE_ = "earlyVoltage";
	const std::string TechnologieFileEKV::THRESHOLD_VOLTAGE_EKV_ATTRIBUTE_ = "vth";
	const std::string TechnologieFileEKV::MOBILITY_OXIDE_CAPACITY_EKV_ATTRIBUTE_ = "muCox";
	const std::string TechnologieFileEKV::EARLY_VOLTAGE_ATTRIBUTE_ = "earlyVoltage";
	const std::string TechnologieFileEKV::OVERLAP_CAPACITY_NODE_ = "overlapCapacity";
	const std::string TechnologieFileEKV::OVERLAP_CAPACITY_ATTRIBUTE_ = "Cgdov";
	const std::string TechnologieFileEKV::SLOPE_FACTOR_EKV_LMAX500_NODE_ = "slopeFactorEKV_LMAX500";
	const std::string TechnologieFileEKV::SLOPE_FACTOR_EKV_LMIN500_LMAX1200_NODE_ = "slopeFactorEKV_LMIN500_LMAX1200";
	const std::string TechnologieFileEKV::SLOPE_FACTOR_EKV_LMIN1200_LMAX2100_NODE_ = "slopeFactorEKV_LMIN1200_LMAX2100";
	const std::string TechnologieFileEKV::SLOPE_FACTOR_EKV_LMIN2100_LMAX5000_NODE_ = "slopeFactorEKV_LMIN2100_LMAX5000";
	const std::string TechnologieFileEKV::SLOPE_FACTOR_EKV_LMIN5000_NODE_ = "slopeFactorEKV_LMIN5000";
	const std::string TechnologieFileEKV::SLOPE_FACTOR_EKV_ATTRIBUTE_ = "n";
	const std::string TechnologieFileEKV::GATE_OXIDE_CAPACITY_NODE_ = "gateOxideCapacity";
	const std::string TechnologieFileEKV::GATE_OXIDE_CAPACITY_ATTRIBUTE_ = "Cox";
	const std::string TechnologieFileEKV::MIN_AREA_NODE_ = "minArea";
	const std::string TechnologieFileEKV::MIN_AREA_ATTRIBUTE_ = "Amin";
	const std::string TechnologieFileEKV::MIN_LENGTH_NODE_ = "minLength";
	const std::string TechnologieFileEKV::MIN_LENGTH_ATTRIBUTE_ = "Lmin";
	const std::string TechnologieFileEKV::MIN_WIDTH_NODE_ = "minWidth";
	const std::string TechnologieFileEKV::MIN_WIDTH_ATTRIBUTE_ = "Wmin";
	const std::string TechnologieFileEKV::MIN_MULTIPLIER_NODE_ = "minMultiplier";
	const std::string TechnologieFileEKV::MIN_MULTIPLIER_ATTRIBUTE_ = "Mmin";
	const std::string TechnologieFileEKV::CHANNEL_LENGTH_COEFFICIENT_EKV_LMAX500_NODE_ = "channelLengthCoefficientEKV_LMAX500";
	const std::string TechnologieFileEKV::CHANNEL_LENGTH_COEFFICIENT_EKV_LMIN500_LMAX1200_NODE_ = "channelLengthCoefficientEKV_LMIN500_LMAX1200";
	const std::string TechnologieFileEKV::CHANNEL_LENGTH_COEFFICIENT_EKV_LMIN1200_LMAX2100_NODE_ = "channelLengthCoefficientEKV_LMIN1200_LMAX2100";
	const std::string TechnologieFileEKV::CHANNEL_LENGTH_COEFFICIENT_EKV_LMIN2100_LMAX5000_NODE_ = "channelLengthCoefficientEKV_LMIN2100_LMAX5000";
	const std::string TechnologieFileEKV::CHANNEL_LENGTH_COEFFICIENT_EKV_LMIN5000_NODE_ = "channelLengthCoefficientEKV_LMIN5000";
	const std::string TechnologieFileEKV::CHANNEL_LENGTH_COEFFICIENT_EKV_ATTRIBUTE_ = "lamda";
	const std::string TechnologieFileEKV::MOBILITY_REDUCTION_COEFFICIENT_EKV_LMAX500_NODE_ = "mobilityReductionCoefficientEKV_LMAX500";
	const std::string TechnologieFileEKV::MOBILITY_REDUCTION_COEFFICIENT_EKV_LMIN500_LMAX1200_NODE_ = "mobilityReductionCoefficientEKV_LMIN500_LMAX1200";
	const std::string TechnologieFileEKV::MOBILITY_REDUCTION_COEFFICIENT_EKV_LMIN1200_LMAX2100_NODE_ = "mobilityReductionCoefficientEKV_LMIN1200_LMAX2100";
	const std::string TechnologieFileEKV::MOBILITY_REDUCTION_COEFFICIENT_EKV_LMIN2100_LMAX5000_NODE_ = "mobilityReductionCoefficientEKV_LMIN2100_LMAX5000";	
	const std::string TechnologieFileEKV::MOBILITY_REDUCTION_COEFFICIENT_EKV_LMIN5000_NODE_ = "mobilityReductionCoefficientEKV_LMIN5000";	
	const std::string TechnologieFileEKV::MOBILITY_REDUCTION_COEFFICIENT_EKV_ATTRIBUTE_ = "theta";
	const std::string TechnologieFileEKV::DIBL_COEFFICIENT_EKV_LMAX500_NODE_ = "DIBLCoefficientEKV_LMAX500";
	const std::string TechnologieFileEKV::DIBL_COEFFICIENT_EKV_LMIN500_LMAX1200_NODE_ = "DIBLCoefficientEKV_LMIN500_LMAX1200";
	const std::string TechnologieFileEKV::DIBL_COEFFICIENT_EKV_LMIN1200_LMAX2100_NODE_ = "DIBLCoefficientEKV_LMIN1200_LMAX2100";
	const std::string TechnologieFileEKV::DIBL_COEFFICIENT_EKV_LMIN2100_LMAX5000_NODE_ = "DIBLCoefficientEKV_LMIN2100_LMAX5000";
	const std::string TechnologieFileEKV::DIBL_COEFFICIENT_EKV_LMIN5000_NODE_ = "DIBLCoefficientEKV_LMIN5000";
	const std::string TechnologieFileEKV::DIBL_COEFFICIENT_EKV_ATTRIBUTE_ = "eta";
	const std::string TechnologieFileEKV::ZERO_BIAS_BULK_JUNCTION_CAPACITANCE_NODE_ = "zeroBiasBulkJunctionCapacitance";
	const std::string TechnologieFileEKV::ZERO_BIAS_BULK_JUNCTION_CAPACITANCE_ATTRIBUTE_ = "Cj";
	const std::string TechnologieFileEKV::ZERO_BIAS_SIDEWALL_BULK_JUNCTION_CAPACITANCE_NODE_ = "zeroBiasSidewallBulkJunctionCapacitance";
	const std::string TechnologieFileEKV::ZERO_BIAS_SIDEWALL_BULK_JUNCTION_CAPACITANCE_ATTRIBUTE_ = "Cjsw";
	const std::string TechnologieFileEKV::BULK_JUNCTION_CONTACT_POTENTIAL_NODE_ = "bulkJunctionContactPotential";
	const std::string TechnologieFileEKV::BULK_JUNCTION_CONTACT_POTENTIAL_ATTRIBUTE_ = "pb";
	const std::string TechnologieFileEKV::LATERAL_DIFFUSION_LENGTH_NODE_ = "lateralDiffusionLength";
	const std::string TechnologieFileEKV::LATERAL_DIFFUSION_LENGTH_ATTRIBUTE_ = "Ldiff";



	TechnologieFileEKV::TechnologieFileEKV()
	{

	}

	void TechnologieFileEKV::parse(CircuitInformation & circuitInformation)
	{
		try
		{
			Control::XmlInputFile::parse();

			circuitInformation.setTechnologieSpecificationEKVNmos(parseNmosTechnologieSpecificationEKV());
			circuitInformation.setTechnologieSpecificationEKVPmos(parsePmosTechnologieSpecificationEKV());
		}
		catch(const ::Core::BacktraceException& ex)
		{
			logDebug(ex.what());
		}
		catch(...)
		{
			logDebug("Error pasing Technologie file!");
		}

	}

	TechnologieSpecificationEKV & TechnologieFileEKV::parsePmosTechnologieSpecificationEKV()
	{
		rapidxml::xml_node<>* pmosNode = getDocumentNode().first_node(PMOS_NODE_.c_str());
		TechnologieSpecificationEKV * pmosSpecification = new TechnologieSpecificationEKV;
		parseSpecification(*pmosNode, * pmosSpecification);
		parseGeneralSpecification(*pmosSpecification);
		return * pmosSpecification;
	}

	TechnologieSpecificationEKV & TechnologieFileEKV::parseNmosTechnologieSpecificationEKV()
	{
		rapidxml::xml_node<>* nmosNode = getDocumentNode().first_node(NMOS_NODE_.c_str());
		TechnologieSpecificationEKV * nmosSpecification = new TechnologieSpecificationEKV;
		parseSpecification(*nmosNode, * nmosSpecification);
		parseGeneralSpecification(*nmosSpecification);
		return * nmosSpecification;
	}

	void TechnologieFileEKV::parseSpecification(rapidxml::xml_node<>& typeNode, TechnologieSpecificationEKV & technologieSpecification)
	{
		rapidxml::xml_node<>* thresholdVoltage_LMAX500Node = typeNode.first_node(THRESHOLD_VOLTAGE_EKV_LMAX500_NODE_.c_str());
		if(thresholdVoltage_LMAX500Node != NULL)
		{
			rapidxml::xml_attribute<>* voltageAttribute = thresholdVoltage_LMAX500Node->first_attribute(THRESHOLD_VOLTAGE_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setThresholdVoltage_LMAX500(std::atof(voltageAttribute->value()));
		}
		else
		{
			assert(false, "ThresholdVoltage_LMAX500 needs to be specified.");
		}
		rapidxml::xml_node<>* thresholdVoltage_LMIN500_LMAX1200Node = typeNode.first_node(THRESHOLD_VOLTAGE_EKV_LMIN500_LMAX1200_NODE_.c_str());
		if(thresholdVoltage_LMIN500_LMAX1200Node != NULL)
		{
			rapidxml::xml_attribute<>* voltageAttribute = thresholdVoltage_LMIN500_LMAX1200Node->first_attribute(THRESHOLD_VOLTAGE_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setThresholdVoltage_LMIN500_LMAX1200(std::atof(voltageAttribute->value()));
		}
		else
		{
			assert(false, "ThresholdVoltage_LMIN500_LMAX1200 needs to be specified.");
		}
		rapidxml::xml_node<>* thresholdVoltage_LMIN1200_LMAX2100Node = typeNode.first_node(THRESHOLD_VOLTAGE_EKV_LMIN1200_LMAX2100_NODE_.c_str());
		if(thresholdVoltage_LMIN1200_LMAX2100Node != NULL)
		{
			rapidxml::xml_attribute<>* voltageAttribute = thresholdVoltage_LMIN1200_LMAX2100Node->first_attribute(THRESHOLD_VOLTAGE_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setThresholdVoltage_LMIN1200_LMAX2100(std::atof(voltageAttribute->value()));
		}
		else
		{
			assert(false, "ThresholdVoltage_LMIN1200_LMAX2100 needs to be specified.");
		}
		rapidxml::xml_node<>* thresholdVoltage_LMIN2100_LMAX5000Node = typeNode.first_node(THRESHOLD_VOLTAGE_EKV_LMIN2100_LMAX5000_NODE_.c_str());
		if(thresholdVoltage_LMIN2100_LMAX5000Node != NULL)
		{
			rapidxml::xml_attribute<>* voltageAttribute = thresholdVoltage_LMIN2100_LMAX5000Node->first_attribute(THRESHOLD_VOLTAGE_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setThresholdVoltage_LMIN2100_LMAX5000(std::atof(voltageAttribute->value()));
		}
		else
		{
			assert(false, "ThresholdVoltage_LMIN2100_LMAX5000 needs to be specified.");
		}
		rapidxml::xml_node<>* thresholdVoltage_LMIN5000Node = typeNode.first_node(THRESHOLD_VOLTAGE_EKV_LMIN5000_NODE_.c_str());
		if(thresholdVoltage_LMIN5000Node != NULL)
		{
			rapidxml::xml_attribute<>* voltageAttribute = thresholdVoltage_LMIN5000Node->first_attribute(THRESHOLD_VOLTAGE_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setThresholdVoltage_LMIN5000(std::atof(voltageAttribute->value()));
		}
		else
		{
			assert(false, "ThresholdVoltage_LMIN5000 needs to be specified.");
		}

		rapidxml::xml_node<>* mobilityOxideCapacity_LMAX500Node = typeNode.first_node(MOBILITY_OXIDE_CAPACITY_EKV_LMAX500_NODE_.c_str());
		if(mobilityOxideCapacity_LMAX500Node != NULL)
		{
			rapidxml::xml_attribute<>* muCoxAttribute = mobilityOxideCapacity_LMAX500Node->first_attribute(MOBILITY_OXIDE_CAPACITY_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setMobilityOxideCapacityCoefficient_LMAX500(std::atof(muCoxAttribute->value()));
		}
		else
		{
			assert(false, "Mobility times oxide capacity_LMAX500 needs to be specified.");
		}
		rapidxml::xml_node<>* mobilityOxideCapacity_LMIN500_LMAX1200Node = typeNode.first_node(MOBILITY_OXIDE_CAPACITY_EKV_LMIN500_LMAX1200_NODE_.c_str());
		if(mobilityOxideCapacity_LMIN500_LMAX1200Node != NULL)
		{
			rapidxml::xml_attribute<>* muCoxAttribute = mobilityOxideCapacity_LMIN500_LMAX1200Node->first_attribute(MOBILITY_OXIDE_CAPACITY_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setMobilityOxideCapacityCoefficient_LMIN500_LMAX1200(std::atof(muCoxAttribute->value()));
		}
		else
		{
			assert(false, "Mobility times oxide capacity_LMIN500_LMAX1200 needs to be specified.");
		}
		rapidxml::xml_node<>* mobilityOxideCapacity_LMIN1200_LMAX2100Node = typeNode.first_node(MOBILITY_OXIDE_CAPACITY_EKV_LMIN1200_LMAX2100_NODE_.c_str());
		if(mobilityOxideCapacity_LMIN1200_LMAX2100Node != NULL)
		{
			rapidxml::xml_attribute<>* muCoxAttribute = mobilityOxideCapacity_LMIN1200_LMAX2100Node->first_attribute(MOBILITY_OXIDE_CAPACITY_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100(std::atof(muCoxAttribute->value()));
		}
		else
		{
			assert(false, "Mobility times oxide capacity_LMIN1200_LMAX2100 needs to be specified.");
		}
		rapidxml::xml_node<>* mobilityOxideCapacity_LMIN2100_LMAX5000Node = typeNode.first_node(MOBILITY_OXIDE_CAPACITY_EKV_LMIN2100_LMAX5000_NODE_.c_str());
		if(mobilityOxideCapacity_LMIN2100_LMAX5000Node != NULL)
		{
			rapidxml::xml_attribute<>* muCoxAttribute = mobilityOxideCapacity_LMIN2100_LMAX5000Node->first_attribute(MOBILITY_OXIDE_CAPACITY_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000(std::atof(muCoxAttribute->value()));
		}
		else
		{
			assert(false, "Mobility times oxide capacity_LMIN2100_LMAX5000 needs to be specified.");
		}
		rapidxml::xml_node<>* mobilityOxideCapacity_LMIN5000Node = typeNode.first_node(MOBILITY_OXIDE_CAPACITY_EKV_LMIN5000_NODE_.c_str());
		if(mobilityOxideCapacity_LMIN5000Node != NULL)
		{
			rapidxml::xml_attribute<>* muCoxAttribute = mobilityOxideCapacity_LMIN5000Node->first_attribute(MOBILITY_OXIDE_CAPACITY_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setMobilityOxideCapacityCoefficient_LMIN5000(std::atof(muCoxAttribute->value()));
		}
		else
		{
			assert(false, "Mobility times oxide capacity_LMIN5000 needs to be specified.");
		}

		rapidxml::xml_node<>* earlyVoltageNode = typeNode.first_node(EARLY_VOLTAGE_NODE_.c_str());
		if(earlyVoltageNode != NULL)
		{
			rapidxml::xml_attribute<>* earlyVoltageAttribute = earlyVoltageNode->first_attribute(EARLY_VOLTAGE_ATTRIBUTE_.c_str());
			technologieSpecification.setEarlyVoltage(std::atof(earlyVoltageAttribute->value()));
		}
		else
		{
			assert(false, "Early voltage needs to be specified.");
		}
		rapidxml::xml_node<>* overlapCapacityNode = typeNode.first_node(OVERLAP_CAPACITY_NODE_.c_str());
		if(overlapCapacityNode != NULL)
		{
			rapidxml::xml_attribute<>* overlabCapacityAttribute = overlapCapacityNode->first_attribute(OVERLAP_CAPACITY_ATTRIBUTE_.c_str());
			technologieSpecification.setOverlapCapacity(std::atof(overlabCapacityAttribute->value()));
		}
		else
		{
			assert(false, "Overlap capacity needs to be specified.");
		}
		rapidxml::xml_node<>* slopeFactor_LMAX500Node = typeNode.first_node(SLOPE_FACTOR_EKV_LMAX500_NODE_.c_str());
		if(slopeFactor_LMAX500Node != NULL)
		{
			rapidxml::xml_attribute<>* slopeFactorAttribute = slopeFactor_LMAX500Node->first_attribute(SLOPE_FACTOR_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setSlopeFactor_LMAX500(std::atof(slopeFactorAttribute->value()));
		}
		else
		{
			assert(false, "SlopeFactor_LMAX500 needs to be specified.");
		}
		rapidxml::xml_node<>* slopeFactor_LMIN500_LMAX1200Node = typeNode.first_node(SLOPE_FACTOR_EKV_LMIN500_LMAX1200_NODE_.c_str());
		if(slopeFactor_LMIN500_LMAX1200Node != NULL)
		{
			rapidxml::xml_attribute<>* slopeFactorAttribute = slopeFactor_LMIN500_LMAX1200Node->first_attribute(SLOPE_FACTOR_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setSlopeFactor_LMIN500_LMAX1200(std::atof(slopeFactorAttribute->value()));
		}
		else
		{
			assert(false, "SlopeFactor_LMIN500_LMAX1200 needs to be specified.");
		}
		rapidxml::xml_node<>* slopeFactor_LMIN1200_LMAX2100Node = typeNode.first_node(SLOPE_FACTOR_EKV_LMIN1200_LMAX2100_NODE_.c_str());
		if(slopeFactor_LMIN1200_LMAX2100Node != NULL)
		{
			rapidxml::xml_attribute<>* slopeFactorAttribute = slopeFactor_LMIN1200_LMAX2100Node->first_attribute(SLOPE_FACTOR_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setSlopeFactor_LMIN1200_LMAX2100(std::atof(slopeFactorAttribute->value()));
		}
		else
		{
			assert(false, "SlopeFactor_LMIN1200_LMAX2100 needs to be specified.");
		}
		rapidxml::xml_node<>* slopeFactor_LMIN2100_LMAX5000Node = typeNode.first_node(SLOPE_FACTOR_EKV_LMIN2100_LMAX5000_NODE_.c_str());
		if(slopeFactor_LMIN2100_LMAX5000Node != NULL)
		{
			rapidxml::xml_attribute<>* slopeFactorAttribute = slopeFactor_LMIN2100_LMAX5000Node->first_attribute(SLOPE_FACTOR_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setSlopeFactor_LMIN2100_LMAX5000(std::atof(slopeFactorAttribute->value()));
		}
		else
		{
			assert(false, "SlopeFactor_LMIN2100_LMAX5000 needs to be specified.");
		}
		rapidxml::xml_node<>* slopeFactor_LMIN5000Node = typeNode.first_node(SLOPE_FACTOR_EKV_LMIN5000_NODE_.c_str());
		if(slopeFactor_LMIN5000Node != NULL)
		{
			rapidxml::xml_attribute<>* slopeFactorAttribute = slopeFactor_LMIN5000Node->first_attribute(SLOPE_FACTOR_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setSlopeFactor_LMIN5000(std::atof(slopeFactorAttribute->value()));
		}
		else
		{
			assert(false, "SlopeFactor_LMIN5000 needs to be specified.");
		}

		rapidxml::xml_node<>* gateOxideCapacityNode = typeNode.first_node(GATE_OXIDE_CAPACITY_NODE_.c_str());
		if(gateOxideCapacityNode != NULL)
		{
			rapidxml::xml_attribute<>* gateOxideCapacityAttribute = gateOxideCapacityNode->first_attribute(GATE_OXIDE_CAPACITY_ATTRIBUTE_.c_str());
			technologieSpecification.setGateOxideCapacity(std::atof(gateOxideCapacityAttribute->value()));
		}
		else
		{
			assert(false, "Gate oxide capacity needs to be specified.");
		}
		rapidxml::xml_node<>* minAreaNode = typeNode.first_node(MIN_AREA_NODE_.c_str());
		if(minAreaNode != NULL)
		{
			rapidxml::xml_attribute<>* minAreaAttribute = minAreaNode->first_attribute(MIN_AREA_ATTRIBUTE_.c_str());
			technologieSpecification.setMinArea(std::atof(minAreaAttribute->value()));
		}
		else
		{
			assert(false, "Minimal area needs to be specified.");
		}
		rapidxml::xml_node<>* minLengthNode = typeNode.first_node(MIN_LENGTH_NODE_.c_str());
		if(minLengthNode != NULL)
		{
			rapidxml::xml_attribute<>* minLengthAttribute = minLengthNode->first_attribute(MIN_LENGTH_ATTRIBUTE_.c_str());
			technologieSpecification.setMinLength(std::atof(minLengthAttribute->value()));
		}
		else
		{
			assert(false, "Minimal length needs to be specified.");
		}
		rapidxml::xml_node<>* minWidthNode = typeNode.first_node(MIN_WIDTH_NODE_.c_str());
		if(minWidthNode != NULL)
		{
			rapidxml::xml_attribute<>* minWidthAttribute = minWidthNode->first_attribute(MIN_WIDTH_ATTRIBUTE_.c_str());
			technologieSpecification.setMinWidth(std::atof(minWidthAttribute->value()));
		}
		else
		{
			assert(false, "Minimal Width needs to be specified.");
		}
		rapidxml::xml_node<>* minMultiplierNode = typeNode.first_node(MIN_MULTIPLIER_NODE_.c_str());
		if(minMultiplierNode != NULL)
		{
			rapidxml::xml_attribute<>* minMultiplierAttribute = minMultiplierNode->first_attribute(MIN_MULTIPLIER_ATTRIBUTE_.c_str());
			technologieSpecification.setMinMultiplier(std::atof(minMultiplierAttribute->value()));
		}
		else
		{
			assert(false, "Minimal multiplier needs to be specified.");
		}
		rapidxml::xml_node<>* channelLengthCoefficient_LMAX500Node = typeNode.first_node(CHANNEL_LENGTH_COEFFICIENT_EKV_LMAX500_NODE_.c_str());
		if(channelLengthCoefficient_LMAX500Node != NULL)
		{
			rapidxml::xml_attribute<>* channelLengthCoefficientAttribute = channelLengthCoefficient_LMAX500Node->first_attribute(CHANNEL_LENGTH_COEFFICIENT_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setChannelLengthCoefficient_LMAX500(std::atof(channelLengthCoefficientAttribute->value()));
		}
		else
		{
			assert(false, "Channel length coefficient_LMAX500 needs to be specified.");
		}
		rapidxml::xml_node<>* channelLengthCoefficient_LMIN500_LMAX1200Node = typeNode.first_node(CHANNEL_LENGTH_COEFFICIENT_EKV_LMIN500_LMAX1200_NODE_.c_str());
		if(channelLengthCoefficient_LMIN500_LMAX1200Node != NULL)
		{
			rapidxml::xml_attribute<>* channelLengthCoefficientAttribute = channelLengthCoefficient_LMIN500_LMAX1200Node->first_attribute(CHANNEL_LENGTH_COEFFICIENT_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setChannelLengthCoefficient_LMIN500_LMAX1200(std::atof(channelLengthCoefficientAttribute->value()));
		}
		else
		{
			assert(false, "Channel length coefficient_LMIN500_LMAX1200 needs to be specified.");
		}
		rapidxml::xml_node<>* channelLengthCoefficient_LMIN1200_LMAX2100Node = typeNode.first_node(CHANNEL_LENGTH_COEFFICIENT_EKV_LMIN1200_LMAX2100_NODE_.c_str());
		if(channelLengthCoefficient_LMIN1200_LMAX2100Node != NULL)
		{
			rapidxml::xml_attribute<>* channelLengthCoefficientAttribute = channelLengthCoefficient_LMIN1200_LMAX2100Node->first_attribute(CHANNEL_LENGTH_COEFFICIENT_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setChannelLengthCoefficient_LMIN1200_LMAX2100(std::atof(channelLengthCoefficientAttribute->value()));
		}
		else
		{
			assert(false, "Channel length coefficient_LMIN1200_LMAX2100 needs to be specified.");
		}
		rapidxml::xml_node<>* channelLengthCoefficient_LMIN2100_LMAX5000Node = typeNode.first_node(CHANNEL_LENGTH_COEFFICIENT_EKV_LMIN2100_LMAX5000_NODE_.c_str());
		if(channelLengthCoefficient_LMIN2100_LMAX5000Node != NULL)
		{
			rapidxml::xml_attribute<>* channelLengthCoefficientAttribute = channelLengthCoefficient_LMIN2100_LMAX5000Node->first_attribute(CHANNEL_LENGTH_COEFFICIENT_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setChannelLengthCoefficient_LMIN2100_LMAX5000(std::atof(channelLengthCoefficientAttribute->value()));
		}
		else
		{
			assert(false, "Channel length coefficient_LMIN2100_LMAX5000 needs to be specified.");
		}
		rapidxml::xml_node<>* channelLengthCoefficient_LMIN5000Node = typeNode.first_node(CHANNEL_LENGTH_COEFFICIENT_EKV_LMIN5000_NODE_.c_str());
		if(channelLengthCoefficient_LMIN5000Node != NULL)
		{
			rapidxml::xml_attribute<>* channelLengthCoefficientAttribute = channelLengthCoefficient_LMIN5000Node->first_attribute(CHANNEL_LENGTH_COEFFICIENT_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setChannelLengthCoefficient_LMIN5000(std::atof(channelLengthCoefficientAttribute->value()));
		}
		else
		{
			assert(false, "Channel length coefficient_LMIN5000 needs to be specified.");
		}

		rapidxml::xml_node<>* mobilityReductionCoefficient_LMAX500Node = typeNode.first_node(MOBILITY_REDUCTION_COEFFICIENT_EKV_LMAX500_NODE_.c_str());
		if(mobilityReductionCoefficient_LMAX500Node != NULL)
		{
			rapidxml::xml_attribute<>* mobilityReductionCoefficientAttribute = mobilityReductionCoefficient_LMAX500Node->first_attribute(MOBILITY_REDUCTION_COEFFICIENT_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setMobilityReductionCoefficient_LMAX500(std::atof(mobilityReductionCoefficientAttribute->value()));
		}
		else
		{
			assert(false, "Mobility reduction coefficient_LMAX500 needs to be specified.");
		}
		rapidxml::xml_node<>* mobilityReductionCoefficient_LMIN500_LMAX1200Node = typeNode.first_node(MOBILITY_REDUCTION_COEFFICIENT_EKV_LMIN500_LMAX1200_NODE_.c_str());
		if(mobilityReductionCoefficient_LMIN500_LMAX1200Node != NULL)
		{
			rapidxml::xml_attribute<>* mobilityReductionCoefficientAttribute = mobilityReductionCoefficient_LMIN500_LMAX1200Node->first_attribute(MOBILITY_REDUCTION_COEFFICIENT_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setMobilityReductionCoefficient_LMIN500_LMAX1200(std::atof(mobilityReductionCoefficientAttribute->value()));
		}
		else
		{
			assert(false, "Mobility reduction coefficient_LMIN500_LMAX1200 needs to be specified.");
		}
		rapidxml::xml_node<>* mobilityReductionCoefficient_LMIN1200_LMAX2100Node = typeNode.first_node(MOBILITY_REDUCTION_COEFFICIENT_EKV_LMIN1200_LMAX2100_NODE_.c_str());
		if(mobilityReductionCoefficient_LMIN1200_LMAX2100Node != NULL)
		{
			rapidxml::xml_attribute<>* mobilityReductionCoefficientAttribute = mobilityReductionCoefficient_LMIN1200_LMAX2100Node->first_attribute(MOBILITY_REDUCTION_COEFFICIENT_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setMobilityReductionCoefficient_LMIN1200_LMAX2100(std::atof(mobilityReductionCoefficientAttribute->value()));
		}
		else
		{
			assert(false, "Mobility reduction coefficient_LMIN1200_LMAX2100 needs to be specified.");
		}
		rapidxml::xml_node<>* mobilityReductionCoefficient_LMIN2100_LMAX5000Node = typeNode.first_node(MOBILITY_REDUCTION_COEFFICIENT_EKV_LMIN2100_LMAX5000_NODE_.c_str());
		if(mobilityReductionCoefficient_LMIN2100_LMAX5000Node != NULL)
		{
			rapidxml::xml_attribute<>* mobilityReductionCoefficientAttribute = mobilityReductionCoefficient_LMIN2100_LMAX5000Node->first_attribute(MOBILITY_REDUCTION_COEFFICIENT_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setMobilityReductionCoefficient_LMIN2100_LMAX5000(std::atof(mobilityReductionCoefficientAttribute->value()));
		}
		else
		{
			assert(false, "Mobility reduction coefficient_LMIN2100_LMAX5000 needs to be specified.");
		}
		rapidxml::xml_node<>* mobilityReductionCoefficient_LMIN5000Node = typeNode.first_node(MOBILITY_REDUCTION_COEFFICIENT_EKV_LMIN5000_NODE_.c_str());
		if(mobilityReductionCoefficient_LMIN5000Node != NULL)
		{
			rapidxml::xml_attribute<>* mobilityReductionCoefficientAttribute = mobilityReductionCoefficient_LMIN5000Node->first_attribute(MOBILITY_REDUCTION_COEFFICIENT_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setMobilityReductionCoefficient_LMIN5000(std::atof(mobilityReductionCoefficientAttribute->value()));
		}
		else
		{
			assert(false, "Mobility reduction coefficient_LMIN5000 needs to be specified.");
		}

		rapidxml::xml_node<>* DIBLCoefficient_LMAX500Node = typeNode.first_node(DIBL_COEFFICIENT_EKV_LMAX500_NODE_.c_str());
		if(DIBLCoefficient_LMAX500Node != NULL)
		{
			rapidxml::xml_attribute<>* DIBLCoefficientAttribute = DIBLCoefficient_LMAX500Node->first_attribute(DIBL_COEFFICIENT_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setDIBLCoefficient_LMAX500(std::atof(DIBLCoefficientAttribute->value()));
		}
		else
		{
			assert(false, "DIBL coefficient_LMAX500 needs to be specified.");
		}
		rapidxml::xml_node<>* DIBLCoefficient_LMIN500_LMAX1200Node = typeNode.first_node(DIBL_COEFFICIENT_EKV_LMIN500_LMAX1200_NODE_.c_str());
		if(DIBLCoefficient_LMIN500_LMAX1200Node != NULL)
		{
			rapidxml::xml_attribute<>* DIBLCoefficientAttribute = DIBLCoefficient_LMIN500_LMAX1200Node->first_attribute(DIBL_COEFFICIENT_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setDIBLCoefficient_LMIN500_LMAX1200(std::atof(DIBLCoefficientAttribute->value()));
		}
		else
		{
			assert(false, "DIBL coefficient_LMIN500_LMAX1200 needs to be specified.");
		}
		rapidxml::xml_node<>* DIBLCoefficient_LMIN1200_LMAX2100Node = typeNode.first_node(DIBL_COEFFICIENT_EKV_LMIN1200_LMAX2100_NODE_.c_str());
		if(DIBLCoefficient_LMIN1200_LMAX2100Node != NULL)
		{
			rapidxml::xml_attribute<>* DIBLCoefficientAttribute = DIBLCoefficient_LMIN1200_LMAX2100Node->first_attribute(DIBL_COEFFICIENT_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setDIBLCoefficient_LMIN1200_LMAX2100(std::atof(DIBLCoefficientAttribute->value()));
		}
		else
		{
			assert(false, "DIBL coefficient_LMIN1200_LMAX2100 needs to be specified.");
		}
		rapidxml::xml_node<>* DIBLCoefficient_LMIN2100_LMAX5000Node = typeNode.first_node(DIBL_COEFFICIENT_EKV_LMIN2100_LMAX5000_NODE_.c_str());
		if(DIBLCoefficient_LMIN2100_LMAX5000Node != NULL)
		{
			rapidxml::xml_attribute<>* DIBLCoefficientAttribute = DIBLCoefficient_LMIN2100_LMAX5000Node->first_attribute(DIBL_COEFFICIENT_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setDIBLCoefficient_LMIN2100_LMAX5000(std::atof(DIBLCoefficientAttribute->value()));
		}
		else
		{
			assert(false, "DIBL coefficient_LMIN2100_LMAX5000 needs to be specified.");
		}
		rapidxml::xml_node<>* DIBLCoefficient_LMIN5000Node = typeNode.first_node(DIBL_COEFFICIENT_EKV_LMIN5000_NODE_.c_str());
		if(DIBLCoefficient_LMIN5000Node != NULL)
		{
			rapidxml::xml_attribute<>* DIBLCoefficientAttribute = DIBLCoefficient_LMIN5000Node->first_attribute(DIBL_COEFFICIENT_EKV_ATTRIBUTE_.c_str());
			technologieSpecification.setDIBLCoefficient_LMIN5000(std::atof(DIBLCoefficientAttribute->value()));
		}
		else
		{
			assert(false, "DIBL coefficient_LMIN5000 needs to be specified.");
		}

		rapidxml::xml_node<>* zeroBiasBulkJunctionCapacitanceNode = typeNode.first_node(ZERO_BIAS_BULK_JUNCTION_CAPACITANCE_NODE_.c_str());
		if(zeroBiasBulkJunctionCapacitanceNode != NULL)
		{
			rapidxml::xml_attribute<>* zeroBiasBulkJunctionCapacitanceAttribute = zeroBiasBulkJunctionCapacitanceNode->first_attribute(ZERO_BIAS_BULK_JUNCTION_CAPACITANCE_ATTRIBUTE_.c_str());
			technologieSpecification.setZeroBiasBulkJunctionCapacitance(std::atof(zeroBiasBulkJunctionCapacitanceAttribute->value()));
		}
		else
		{
			assert(false, "Zero-bias bulk junction capacitance needs to be specified.");
		}
		rapidxml::xml_node<>* zeroBiasSidewallBulkJunctionCapacitanceNode = typeNode.first_node(ZERO_BIAS_SIDEWALL_BULK_JUNCTION_CAPACITANCE_NODE_.c_str());
		if(zeroBiasSidewallBulkJunctionCapacitanceNode != NULL)
		{
			rapidxml::xml_attribute<>* zeroBiasSidewallBulkJunctionCapacitanceAttribute = zeroBiasSidewallBulkJunctionCapacitanceNode->first_attribute(ZERO_BIAS_SIDEWALL_BULK_JUNCTION_CAPACITANCE_ATTRIBUTE_.c_str());
			technologieSpecification.setZeroBiasSidewallBulkJunctionCapacitance(std::atof(zeroBiasSidewallBulkJunctionCapacitanceAttribute->value()));
		}
		else
		{
			assert(false, "Zero-bias sidewall  bulk junction capacitance needs to be specified.");
		}
		rapidxml::xml_node<>* bulkJunctionContactPotentialNode = typeNode.first_node(BULK_JUNCTION_CONTACT_POTENTIAL_NODE_.c_str());
		if(bulkJunctionContactPotentialNode != NULL)
		{
			rapidxml::xml_attribute<>* bulkJunctionContactPotentialAttribute = bulkJunctionContactPotentialNode->first_attribute(BULK_JUNCTION_CONTACT_POTENTIAL_ATTRIBUTE_.c_str());
			technologieSpecification.setBulkJunctionContactPotential(std::atof(bulkJunctionContactPotentialAttribute->value()));
		}
		else
		{
			assert(false, "Bulk junction contact potential needs to be specified.");
		}
		rapidxml::xml_node<>* lateralDiffusionLengthNode = typeNode.first_node(LATERAL_DIFFUSION_LENGTH_NODE_.c_str());
		if(lateralDiffusionLengthNode != NULL)
		{
			rapidxml::xml_attribute<>* lateralDiffusionLengthAttribute =lateralDiffusionLengthNode->first_attribute(LATERAL_DIFFUSION_LENGTH_ATTRIBUTE_.c_str());
			float lateralDiffusionLengthTemp = std::atof(lateralDiffusionLengthAttribute->value());
			technologieSpecification.setLateralDiffusionLength(lateralDiffusionLengthTemp/1000000);
		}
		else
		{
			assert(false, "Channel length coefficient needs to be specified.");
		}

	}

	void TechnologieFileEKV::parseGeneralSpecification(TechnologieSpecificationEKV & specification)
	{
		rapidxml::xml_node<>* generalNode = getDocumentNode().first_node(GENERAL_NODE_.c_str());

		rapidxml::xml_node<>* thermalVoltageNode = generalNode->first_node(THERMAL_VOLTAGE_NODE_.c_str());
		assert(thermalVoltageNode != NULL, "Thermal voltage needs to be specified.");
		rapidxml::xml_attribute<>* thermalVoltageAttribute = thermalVoltageNode->first_attribute(THERMAL_VOLTAGE_ATTRIBUTE_.c_str());
		specification.setThermalVoltage(std::atof(thermalVoltageAttribute->value()));
	}
}
