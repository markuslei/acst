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

#include "../../incl/CircuitInformation/TechnologieSpecificationEKV.h"

namespace Simulation {

const int TechnologieSpecificationEKV::NOT_INITIALIZED_ = -100;

	TechnologieSpecificationEKV::TechnologieSpecificationEKV() :
				thermalVoltage_(NOT_INITIALIZED_),			
				thresholdVoltage_LMAX500_(NOT_INITIALIZED_),
				thresholdVoltage_LMIN500_LMAX1200_(NOT_INITIALIZED_),
				thresholdVoltage_LMIN1200_LMAX2100_(NOT_INITIALIZED_),
				thresholdVoltage_LMIN2100_LMAX5000_(NOT_INITIALIZED_),
				thresholdVoltage_LMIN5000_(NOT_INITIALIZED_),
				muCox_LMAX500_(NOT_INITIALIZED_),
				muCox_LMIN500_LMAX1200_(NOT_INITIALIZED_),
				muCox_LMIN1200_LMAX2100_(NOT_INITIALIZED_),
				muCox_LMIN2100_LMAX5000_(NOT_INITIALIZED_),
				muCox_LMIN5000_(NOT_INITIALIZED_),
				earlyVoltage_(NOT_INITIALIZED_),
				overlapCapacity_(NOT_INITIALIZED_),
				gateOxideCapacity_(NOT_INITIALIZED_),
				slopeFactor_LMAX500_(NOT_INITIALIZED_),
				slopeFactor_LMIN500_LMAX1200_(NOT_INITIALIZED_),
				slopeFactor_LMIN1200_LMAX2100_(NOT_INITIALIZED_),
				slopeFactor_LMIN2100_LMAX5000_(NOT_INITIALIZED_),
				slopeFactor_LMIN5000_(NOT_INITIALIZED_),
				Amin_(NOT_INITIALIZED_),
				Lmin_(NOT_INITIALIZED_),
				Wmin_(NOT_INITIALIZED_),
				Mmin_(NOT_INITIALIZED_),
				lambda_LMAX500_(NOT_INITIALIZED_),
				lambda_LMIN500_LMAX1200_(NOT_INITIALIZED_),
				lambda_LMIN1200_LMAX2100_(NOT_INITIALIZED_),
				lambda_LMIN2100_LMAX5000_(NOT_INITIALIZED_),
				lambda_LMIN5000_(NOT_INITIALIZED_),
				theta_LMAX500_(NOT_INITIALIZED_),
				theta_LMIN500_LMAX1200_(NOT_INITIALIZED_),
				theta_LMIN1200_LMAX2100_(NOT_INITIALIZED_),
				theta_LMIN2100_LMAX5000_(NOT_INITIALIZED_),
				theta_LMIN5000_(NOT_INITIALIZED_),
				eta_LMAX500_(NOT_INITIALIZED_),
				eta_LMIN500_LMAX1200_(NOT_INITIALIZED_),
				eta_LMIN1200_LMAX2100_(NOT_INITIALIZED_),
				eta_LMIN2100_LMAX5000_(NOT_INITIALIZED_),
				eta_LMIN5000_(NOT_INITIALIZED_),
				cj_(NOT_INITIALIZED_),
				cjsw_(NOT_INITIALIZED_),
				pb_(NOT_INITIALIZED_),
				ldiff_(NOT_INITIALIZED_)
	{
	}

	TechnologieSpecificationEKV & TechnologieSpecificationEKV::clone() const
	{
		TechnologieSpecificationEKV * techSpec = new TechnologieSpecificationEKV;

		techSpec->setBulkJunctionContactPotential(getBulkJunctionContactPotential());
		techSpec->setChannelLengthCoefficient_LMAX500(getChannelLengthCoefficient_LMAX500());
		techSpec->setChannelLengthCoefficient_LMIN500_LMAX1200(getChannelLengthCoefficient_LMIN500_LMAX1200());
		techSpec->setChannelLengthCoefficient_LMIN1200_LMAX2100(getChannelLengthCoefficient_LMIN1200_LMAX2100());
		techSpec->setChannelLengthCoefficient_LMIN2100_LMAX5000(getChannelLengthCoefficient_LMIN2100_LMAX5000());
		techSpec->setChannelLengthCoefficient_LMIN5000(getChannelLengthCoefficient_LMIN5000());
		techSpec->setMobilityReductionCoefficient_LMAX500(getMobilityReductionCoefficient_LMAX500());
		techSpec->setMobilityReductionCoefficient_LMIN500_LMAX1200(getMobilityReductionCoefficient_LMIN500_LMAX1200());
		techSpec->setMobilityReductionCoefficient_LMIN1200_LMAX2100(getMobilityReductionCoefficient_LMIN1200_LMAX2100());
		techSpec->setMobilityReductionCoefficient_LMIN2100_LMAX5000(getMobilityReductionCoefficient_LMIN2100_LMAX5000());	
		techSpec->setMobilityReductionCoefficient_LMIN5000(getMobilityReductionCoefficient_LMIN5000());
		techSpec->setDIBLCoefficient_LMAX500(getDIBLCoefficient_LMAX500());
		techSpec->setDIBLCoefficient_LMIN500_LMAX1200(getDIBLCoefficient_LMIN500_LMAX1200());
		techSpec->setDIBLCoefficient_LMIN1200_LMAX2100(getDIBLCoefficient_LMIN1200_LMAX2100());
		techSpec->setDIBLCoefficient_LMIN2100_LMAX5000(getDIBLCoefficient_LMIN2100_LMAX5000());
		techSpec->setDIBLCoefficient_LMIN5000(getDIBLCoefficient_LMIN5000());
		techSpec->setEarlyVoltage(getEarlyVoltage());
		techSpec->setGateOxideCapacity(getGateOxideCapacity());
		techSpec->setLateralDiffusionLength(getLateralDiffusionLength());
		techSpec->setMinArea(getMinArea());
		techSpec->setMinLength(getMinLength());
		techSpec->setMinWidth(getMinWidth());
		techSpec->setMinMultiplier(getMinMultiplier());
		techSpec->setMobilityOxideCapacityCoefficient_LMAX500(getMobilityOxideCapacityCoefficient_LMAX500());
		techSpec->setMobilityOxideCapacityCoefficient_LMIN500_LMAX1200(getMobilityOxideCapacityCoefficient_LMIN500_LMAX1200());
		techSpec->setMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100(getMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100());
		techSpec->setMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000(getMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000());
		techSpec->setMobilityOxideCapacityCoefficient_LMIN5000(getMobilityOxideCapacityCoefficient_LMIN5000());
		techSpec->setOverlapCapacity(getOverlapCapacity());
		techSpec->setSlopeFactor_LMAX500(getSlopeFactor_LMAX500());
		techSpec->setSlopeFactor_LMIN500_LMAX1200(getSlopeFactor_LMIN500_LMAX1200());
		techSpec->setSlopeFactor_LMIN1200_LMAX2100(getSlopeFactor_LMIN1200_LMAX2100());
		techSpec->setSlopeFactor_LMIN2100_LMAX5000(getSlopeFactor_LMIN2100_LMAX5000());
		techSpec->setSlopeFactor_LMIN5000(getSlopeFactor_LMIN5000());
		techSpec->setThresholdVoltage_LMAX500(getThresholdVoltage_LMAX500());
		techSpec->setThresholdVoltage_LMIN500_LMAX1200(getThresholdVoltage_LMIN500_LMAX1200());
		techSpec->setThresholdVoltage_LMIN1200_LMAX2100(getThresholdVoltage_LMIN1200_LMAX2100());
		techSpec->setThresholdVoltage_LMIN2100_LMAX5000(getThresholdVoltage_LMIN2100_LMAX5000());
		techSpec->setThresholdVoltage_LMIN5000(getThresholdVoltage_LMIN5000());
		techSpec->setThermalVoltage(getThermalVoltage());
		techSpec->setZeroBiasBulkJunctionCapacitance(getZeroBiasBulkJunctionCapacitance());
		techSpec->setZeroBiasSidewallBulkJunctionCapacitance(getZeroBiasSidewallBulkJunctionCapacitance());

		return *techSpec;
	}

	void TechnologieSpecificationEKV::setThermalVoltage(
		float voltage)
	{
		thermalVoltage_ = voltage;
	}

	float TechnologieSpecificationEKV::getThermalVoltage() const
	{
		assert(thermalVoltage_ != NOT_INITIALIZED_);
		return thermalVoltage_;
	}

	void TechnologieSpecificationEKV::setThresholdVoltage_LMAX500(
		float voltage)
	{
		thresholdVoltage_LMAX500_ = voltage;
	}

	void TechnologieSpecificationEKV::setThresholdVoltage_LMIN500_LMAX1200(
		float voltage)
	{
		thresholdVoltage_LMIN500_LMAX1200_ = voltage;
	}

	void TechnologieSpecificationEKV::setThresholdVoltage_LMIN1200_LMAX2100(
		float voltage)
	{
		thresholdVoltage_LMIN1200_LMAX2100_ = voltage;
	}

	void TechnologieSpecificationEKV::setThresholdVoltage_LMIN2100_LMAX5000(
		float voltage)
	{
		thresholdVoltage_LMIN2100_LMAX5000_ = voltage;
	}

	void TechnologieSpecificationEKV::setThresholdVoltage_LMIN5000(
		float voltage)
	{
		thresholdVoltage_LMIN5000_ = voltage;
	}

	void TechnologieSpecificationEKV::setMobilityOxideCapacityCoefficient_LMAX500(
		float muCox)
	{
		muCox_LMAX500_ = muCox;
	}

	void TechnologieSpecificationEKV::setMobilityOxideCapacityCoefficient_LMIN500_LMAX1200(
		float muCox)
	{
		muCox_LMIN500_LMAX1200_ = muCox;
	}

	void TechnologieSpecificationEKV::setMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100(
		float muCox)
	{
		muCox_LMIN1200_LMAX2100_ = muCox;
	}

	void TechnologieSpecificationEKV::setMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000(
		float muCox)
	{
		muCox_LMIN2100_LMAX5000_ = muCox;
	}

	void TechnologieSpecificationEKV::setMobilityOxideCapacityCoefficient_LMIN5000(
		float muCox)
	{
		muCox_LMIN5000_ = muCox;
	}

	void TechnologieSpecificationEKV::setEarlyVoltage(
		float earlyVoltage)
	{
		earlyVoltage_ = earlyVoltage;
	}

	float TechnologieSpecificationEKV::getThresholdVoltage_LMAX500() const
	{
		assert(hasThresholdVoltage_LMAX500());
		return thresholdVoltage_LMAX500_;
	}

	float TechnologieSpecificationEKV::getThresholdVoltage_LMIN500_LMAX1200() const
	{
		assert(hasThresholdVoltage_LMIN500_LMAX1200());
		return thresholdVoltage_LMIN500_LMAX1200_;
	}

	float TechnologieSpecificationEKV::getThresholdVoltage_LMIN1200_LMAX2100() const
	{
		assert(hasThresholdVoltage_LMIN1200_LMAX2100());
		return thresholdVoltage_LMIN1200_LMAX2100_;
	}

	float TechnologieSpecificationEKV::getThresholdVoltage_LMIN2100_LMAX5000() const
	{
		assert(hasThresholdVoltage_LMIN2100_LMAX5000());
		return thresholdVoltage_LMIN2100_LMAX5000_;
	}

	float TechnologieSpecificationEKV::getThresholdVoltage_LMIN5000() const
	{
		assert(hasThresholdVoltage_LMIN5000());
		return thresholdVoltage_LMIN5000_;
	}

	float TechnologieSpecificationEKV::getMobilityOxideCapacityCoefficient_LMAX500() const
	{
		assert(hasMobilityOxideCapacityCoefficient_LMAX500());
		return muCox_LMAX500_;
	}

	float TechnologieSpecificationEKV::getMobilityOxideCapacityCoefficient_LMIN500_LMAX1200() const
	{
		assert(hasMobilityOxideCapacityCoefficient_LMIN500_LMAX1200());
		return muCox_LMIN500_LMAX1200_;
	}

	float TechnologieSpecificationEKV::getMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100() const
	{
		assert(hasMobilityOxideCapacityCoefficient_LMAX500());
		return muCox_LMIN1200_LMAX2100_;
	}

	float TechnologieSpecificationEKV::getMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000() const
	{
		assert(hasMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000());
		return muCox_LMIN2100_LMAX5000_;
	}

	float TechnologieSpecificationEKV::getMobilityOxideCapacityCoefficient_LMIN5000() const
	{
		assert(hasMobilityOxideCapacityCoefficient_LMIN5000());
		return muCox_LMIN5000_;
	}

	float TechnologieSpecificationEKV::getEarlyVoltage() const
	{
		assert(hasEarlyVoltage());
		return earlyVoltage_;
	}

	bool TechnologieSpecificationEKV::hasThresholdVoltage_LMAX500() const
	{
		return thresholdVoltage_LMAX500_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasThresholdVoltage_LMIN500_LMAX1200() const
	{
		return thresholdVoltage_LMIN500_LMAX1200_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasThresholdVoltage_LMIN1200_LMAX2100() const
	{
		return thresholdVoltage_LMIN1200_LMAX2100_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasThresholdVoltage_LMIN2100_LMAX5000() const
	{
		return thresholdVoltage_LMIN2100_LMAX5000_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasThresholdVoltage_LMIN5000() const
	{
		return thresholdVoltage_LMIN5000_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasMobilityOxideCapacityCoefficient_LMAX500() const
	{
		return muCox_LMAX500_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasMobilityOxideCapacityCoefficient_LMIN500_LMAX1200() const
	{
		return muCox_LMIN500_LMAX1200_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100() const
	{
		return muCox_LMIN1200_LMAX2100_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000() const
	{
		return muCox_LMIN2100_LMAX5000_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasMobilityOxideCapacityCoefficient_LMIN5000() const
	{
		return muCox_LMIN5000_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasEarlyVoltage() const
	{
		return earlyVoltage_ != NOT_INITIALIZED_;
	}

	void TechnologieSpecificationEKV::setOverlapCapacity(float overlapCapacity)
	{
		overlapCapacity_ = overlapCapacity;
	}

	float TechnologieSpecificationEKV::getOverlapCapacity() const
	{
		assert(hasOverlapCapacity());
		return overlapCapacity_;
	}

	bool TechnologieSpecificationEKV::hasOverlapCapacity() const
	{
		return overlapCapacity_ != NOT_INITIALIZED_;
	}

	void TechnologieSpecificationEKV::setGateOxideCapacity(float gateOxideCapacity)
	{
		gateOxideCapacity_ = gateOxideCapacity;
	}

	float TechnologieSpecificationEKV::getGateOxideCapacity() const
	{
		assert(hasGateOxideCapacity());
		return gateOxideCapacity_;
	}

	void TechnologieSpecificationEKV::setSlopeFactor_LMAX500(float slopeFactor)
	{
		slopeFactor_LMAX500_ = slopeFactor;
	}

	void TechnologieSpecificationEKV::setSlopeFactor_LMIN500_LMAX1200(float slopeFactor)
	{
		slopeFactor_LMIN500_LMAX1200_ = slopeFactor;
	}

	void TechnologieSpecificationEKV::setSlopeFactor_LMIN1200_LMAX2100(float slopeFactor)
	{
		slopeFactor_LMIN1200_LMAX2100_ = slopeFactor;
	}

	void TechnologieSpecificationEKV::setSlopeFactor_LMIN2100_LMAX5000(float slopeFactor)
	{
		slopeFactor_LMIN2100_LMAX5000_ = slopeFactor;
	}

	void TechnologieSpecificationEKV::setSlopeFactor_LMIN5000(float slopeFactor)
	{
		slopeFactor_LMIN5000_ = slopeFactor;
	}

	float TechnologieSpecificationEKV::getSlopeFactor_LMAX500() const
	{
		assert(hasSlopeFactor_LMAX500());
		return slopeFactor_LMAX500_;
	}

	float TechnologieSpecificationEKV::getSlopeFactor_LMIN500_LMAX1200() const
	{
		assert(hasSlopeFactor_LMIN500_LMAX1200());
		return slopeFactor_LMIN500_LMAX1200_;
	}


	float TechnologieSpecificationEKV::getSlopeFactor_LMIN1200_LMAX2100() const
	{
		assert(hasSlopeFactor_LMIN1200_LMAX2100());
		return slopeFactor_LMIN1200_LMAX2100_;
	}

	float TechnologieSpecificationEKV::getSlopeFactor_LMIN2100_LMAX5000() const
	{
		assert(hasSlopeFactor_LMIN2100_LMAX5000());
		return slopeFactor_LMIN2100_LMAX5000_;
	}

	float TechnologieSpecificationEKV::getSlopeFactor_LMIN5000() const
	{
		assert(hasSlopeFactor_LMIN5000());
		return slopeFactor_LMIN5000_;
	}


	bool TechnologieSpecificationEKV::hasGateOxideCapacity() const
	{
		return gateOxideCapacity_ != NOT_INITIALIZED_;
	}

	void TechnologieSpecificationEKV::setChannelLengthCoefficient_LMAX500(float lamda)
	{
		lambda_LMAX500_ = lamda;
	}

	void TechnologieSpecificationEKV::setChannelLengthCoefficient_LMIN500_LMAX1200(float lamda)
	{
		lambda_LMIN500_LMAX1200_ = lamda;
	}

	void TechnologieSpecificationEKV::setChannelLengthCoefficient_LMIN1200_LMAX2100(float lamda)
	{
		lambda_LMIN1200_LMAX2100_ = lamda;
	}

	void TechnologieSpecificationEKV::setChannelLengthCoefficient_LMIN2100_LMAX5000(float lamda)
	{
		lambda_LMIN2100_LMAX5000_ = lamda;
	}

	void TechnologieSpecificationEKV::setChannelLengthCoefficient_LMIN5000(float lamda)
	{
		lambda_LMIN5000_ = lamda;
	}


	float TechnologieSpecificationEKV::getChannelLengthCoefficient_LMAX500() const
	{
		assert(lambda_LMAX500_ != NOT_INITIALIZED_);
		return lambda_LMAX500_;
	}

	float TechnologieSpecificationEKV::getChannelLengthCoefficient_LMIN500_LMAX1200() const
	{
		assert(lambda_LMIN500_LMAX1200_ != NOT_INITIALIZED_);
		return lambda_LMIN500_LMAX1200_;
	}

	float TechnologieSpecificationEKV::getChannelLengthCoefficient_LMIN1200_LMAX2100() const
	{
		assert(lambda_LMIN1200_LMAX2100_ != NOT_INITIALIZED_);
		return lambda_LMIN1200_LMAX2100_;
	}

	float TechnologieSpecificationEKV::getChannelLengthCoefficient_LMIN2100_LMAX5000() const
	{
		assert(lambda_LMIN2100_LMAX5000_ != NOT_INITIALIZED_);
		return lambda_LMIN2100_LMAX5000_;
	}

	float TechnologieSpecificationEKV::getChannelLengthCoefficient_LMIN5000() const
	{
		assert(lambda_LMIN5000_ != NOT_INITIALIZED_);
		return lambda_LMIN5000_;
	}

	bool TechnologieSpecificationEKV::hasSlopeFactor_LMAX500() const
	{
		return slopeFactor_LMAX500_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasSlopeFactor_LMIN500_LMAX1200() const
	{
		return slopeFactor_LMIN500_LMAX1200_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasSlopeFactor_LMIN1200_LMAX2100() const
	{
		return slopeFactor_LMIN1200_LMAX2100_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasSlopeFactor_LMIN2100_LMAX5000() const
	{
		return slopeFactor_LMIN2100_LMAX5000_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasSlopeFactor_LMIN5000() const
	{
		return slopeFactor_LMIN5000_ != NOT_INITIALIZED_;
	}

	void TechnologieSpecificationEKV::setMobilityReductionCoefficient_LMAX500(float theta)
	{
		theta_LMAX500_ = theta;
	}

	void TechnologieSpecificationEKV::setMobilityReductionCoefficient_LMIN500_LMAX1200(float theta)
	{
		theta_LMIN500_LMAX1200_ = theta;
	}

	void TechnologieSpecificationEKV::setMobilityReductionCoefficient_LMIN1200_LMAX2100(float theta)
	{
		theta_LMIN1200_LMAX2100_ = theta;
	}

	void TechnologieSpecificationEKV::setMobilityReductionCoefficient_LMIN2100_LMAX5000(float theta)
	{
		theta_LMIN2100_LMAX5000_ = theta;
	}

	void TechnologieSpecificationEKV::setMobilityReductionCoefficient_LMIN5000(float theta)
	{
		theta_LMIN5000_ = theta;
	}

	float TechnologieSpecificationEKV::getMobilityReductionCoefficient_LMAX500() const
	{
		assert(hasMobilityReductionCoefficient_LMAX500());
		return theta_LMAX500_;
	}

	float TechnologieSpecificationEKV::getMobilityReductionCoefficient_LMIN500_LMAX1200() const
	{
		assert(hasMobilityReductionCoefficient_LMIN500_LMAX1200());
		return theta_LMIN500_LMAX1200_;
	}


	float TechnologieSpecificationEKV::getMobilityReductionCoefficient_LMIN1200_LMAX2100() const
	{
		assert(hasMobilityReductionCoefficient_LMIN1200_LMAX2100());
		return theta_LMIN1200_LMAX2100_;
	}

	float TechnologieSpecificationEKV::getMobilityReductionCoefficient_LMIN2100_LMAX5000() const
	{
		assert(hasMobilityReductionCoefficient_LMIN2100_LMAX5000());
		return theta_LMIN2100_LMAX5000_;
	}

	float TechnologieSpecificationEKV::getMobilityReductionCoefficient_LMIN5000() const
	{
		assert(hasMobilityReductionCoefficient_LMIN5000());
		return theta_LMIN5000_;
	}


	bool TechnologieSpecificationEKV::hasMobilityReductionCoefficient_LMAX500() const
	{
		return theta_LMAX500_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasMobilityReductionCoefficient_LMIN500_LMAX1200() const
	{
		return theta_LMIN500_LMAX1200_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasMobilityReductionCoefficient_LMIN1200_LMAX2100() const
	{
		return theta_LMIN1200_LMAX2100_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasMobilityReductionCoefficient_LMIN2100_LMAX5000() const
	{
		return theta_LMIN2100_LMAX5000_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasMobilityReductionCoefficient_LMIN5000() const
	{
		return theta_LMIN5000_ != NOT_INITIALIZED_;
	}

	void TechnologieSpecificationEKV::setDIBLCoefficient_LMAX500(float eta)
	{
		eta_LMAX500_ = eta;
	}

	void TechnologieSpecificationEKV::setDIBLCoefficient_LMIN500_LMAX1200(float eta)
	{
		eta_LMIN500_LMAX1200_ = eta;
	}

	void TechnologieSpecificationEKV::setDIBLCoefficient_LMIN1200_LMAX2100(float eta)
	{
		eta_LMIN1200_LMAX2100_ = eta;
	}

	void TechnologieSpecificationEKV::setDIBLCoefficient_LMIN2100_LMAX5000(float eta)
	{
		eta_LMIN2100_LMAX5000_ = eta;
	}

	void TechnologieSpecificationEKV::setDIBLCoefficient_LMIN5000(float eta)
	{
		eta_LMIN5000_ = eta;
	}

	float TechnologieSpecificationEKV::getDIBLCoefficient_LMAX500() const
	{
		assert(hasDIBLCoefficient_LMAX500());
		return eta_LMAX500_;
	}

	float TechnologieSpecificationEKV::getDIBLCoefficient_LMIN500_LMAX1200() const
	{
		assert(hasDIBLCoefficient_LMIN500_LMAX1200());
		return eta_LMIN500_LMAX1200_;
	}


	float TechnologieSpecificationEKV::getDIBLCoefficient_LMIN1200_LMAX2100() const
	{
		assert(hasDIBLCoefficient_LMIN1200_LMAX2100());
		return eta_LMIN1200_LMAX2100_;
	}

	float TechnologieSpecificationEKV::getDIBLCoefficient_LMIN2100_LMAX5000() const
	{
		assert(hasDIBLCoefficient_LMIN2100_LMAX5000());
		return eta_LMIN2100_LMAX5000_;
	}

	float TechnologieSpecificationEKV::getDIBLCoefficient_LMIN5000() const
	{
		assert(hasDIBLCoefficient_LMIN5000());
		return eta_LMIN5000_;
	}


	bool TechnologieSpecificationEKV::hasDIBLCoefficient_LMAX500() const
	{
		return eta_LMAX500_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasDIBLCoefficient_LMIN500_LMAX1200() const
	{
		return eta_LMIN500_LMAX1200_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasDIBLCoefficient_LMIN1200_LMAX2100() const
	{
		return eta_LMIN1200_LMAX2100_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasDIBLCoefficient_LMIN2100_LMAX5000() const
	{
		return eta_LMIN2100_LMAX5000_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasDIBLCoefficient_LMIN5000() const
	{
		return eta_LMIN5000_ != NOT_INITIALIZED_;
	}


	void TechnologieSpecificationEKV::setMinArea(float minArea)
	{
		Amin_ =minArea;
	}

	void TechnologieSpecificationEKV::setMinLength(float Lmin)
	{
		Lmin_ = Lmin;
	}

	void TechnologieSpecificationEKV::setMinWidth(float minWidth)
	{
		Wmin_ = minWidth;
	}

	void TechnologieSpecificationEKV::setMinMultiplier(float minMultiplier)
	{
		Mmin_ = minMultiplier;
	}

	float TechnologieSpecificationEKV::getMinArea() const
	{
		assert(Amin_ != NOT_INITIALIZED_);

		return Amin_;
	}

	float TechnologieSpecificationEKV::getMinWidth() const
	{
		assert(Wmin_ != NOT_INITIALIZED_);
		return Wmin_;
	}

	float TechnologieSpecificationEKV::getMinMultiplier() const
	{
		assert(Mmin_ != NOT_INITIALIZED_);
		return Mmin_;
	}

	float TechnologieSpecificationEKV::getMinLength() const
	{
		assert(Lmin_ != NOT_INITIALIZED_);
		return Lmin_;
	}

	void TechnologieSpecificationEKV::setZeroBiasBulkJunctionCapacitance(float Cj)
	{
		cj_ = Cj;
	}

	void TechnologieSpecificationEKV::setZeroBiasSidewallBulkJunctionCapacitance(
		float Cjsw)
	{
		cjsw_ = Cjsw;
	}

	void TechnologieSpecificationEKV::setBulkJunctionContactPotential(float pb)
	{
		pb_ = pb;
	}

	void TechnologieSpecificationEKV::setLateralDiffusionLength(float Ldiff)
	{
		ldiff_ = Ldiff;
	}

	float TechnologieSpecificationEKV::getZeroBiasBulkJunctionCapacitance() const
	{
		assert(hasZeroBiasBulkJunctionCapacitance());
		return cj_;
	}

	float TechnologieSpecificationEKV::getZeroBiasSidewallBulkJunctionCapacitance() const
	{
		assert(hasZeroBiasSidewallBulkJunctionCapacitance());
		return cjsw_;
	}

	float TechnologieSpecificationEKV::getBulkJunctionContactPotential() const
	{
		assert(hasBulkJunctionContactPotential());
		return pb_;
	}


	float TechnologieSpecificationEKV::getLateralDiffusionLength() const
	{
		assert(hasLateralDiffusionLength());
		return ldiff_;
	}

	bool TechnologieSpecificationEKV::hasZeroBiasBulkJunctionCapacitance() const
	{
		return cj_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasZeroBiasSidewallBulkJunctionCapacitance() const
	{
		return cjsw_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasBulkJunctionContactPotential() const
	{
		return pb_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasLateralDiffusionLength() const
	{
		return ldiff_ != NOT_INITIALIZED_;
	}

}

                                                                                                                                                                                                                                                                                                                                                                                                                         
