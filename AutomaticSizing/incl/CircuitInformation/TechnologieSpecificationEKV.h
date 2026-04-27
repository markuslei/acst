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


#ifndef AUTOMATICSIZING_INCL_CIRCUITINFORMATION_TECHNOLOGIESPECIFICATIONEKV_H_
#define AUTOMATICSIZING_INCL_CIRCUITINFORMATION_TECHNOLOGIESPECIFICATIONEKV_H_


#include <gecode/float.hh>


namespace AutomaticSizing {

	class TechnologieSpecificationEKV
	{
	public:
		TechnologieSpecificationEKV();

		TechnologieSpecificationEKV & clone() const;

		void setThermalVoltage(float voltage);

		void setThresholdVoltage_LMAX500(float voltage);
		void setThresholdVoltage_LMIN500_LMAX1200(float voltage);
		void setThresholdVoltage_LMIN1200_LMAX2100(float voltage);
		void setThresholdVoltage_LMIN2100_LMAX5000(float voltage);
		void setThresholdVoltage_LMIN5000(float voltage);
		void setMobilityOxideCapacityCoefficient_LMAX500(float muCox);
		void setMobilityOxideCapacityCoefficient_LMIN500_LMAX1200(float muCox);
		void setMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100(float muCox);
		void setMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000(float muCox);
		void setMobilityOxideCapacityCoefficient_LMIN5000(float muCox);
		void setEarlyVoltage(float earlyVoltage);
		void setOverlapCapacity(float overlapCapacity);
		void setGateOxideCapacity(float gateOxideCapacity);
		void setSlopeFactor_LMAX500(float slopeFactor);
		void setSlopeFactor_LMIN500_LMAX1200(float slopeFactor);
		void setSlopeFactor_LMIN1200_LMAX2100(float slopeFactor);
		void setSlopeFactor_LMIN2100_LMAX5000(float slopeFactor);
		void setSlopeFactor_LMIN5000(float slopeFactor);
		void setMinArea(float minArea );
		void setMinLength(float Lmin);
		void setMinWidth(float minWidth);
		void setMinMultiplier(int minMultiplier);
		void setChannelLengthCoefficient_LMAX500(float lamda);
		void setChannelLengthCoefficient_LMIN500_LMAX1200(float lamda);
		void setChannelLengthCoefficient_LMIN1200_LMAX2100(float lamda);
		void setChannelLengthCoefficient_LMIN2100_LMAX5000(float lamda);
		void setChannelLengthCoefficient_LMIN5000(float lamda);
		void setMobilityReductionCoefficient_LMAX500(float theta);
		void setMobilityReductionCoefficient_LMIN500_LMAX1200(float theta);
		void setMobilityReductionCoefficient_LMIN1200_LMAX2100(float theta);
		void setMobilityReductionCoefficient_LMIN2100_LMAX5000(float theta);
		void setMobilityReductionCoefficient_LMIN5000(float theta);
		void setDIBLCoefficient_LMAX500(float eta);
		void setDIBLCoefficient_LMIN500_LMAX1200(float eta);
		void setDIBLCoefficient_LMIN1200_LMAX2100(float eta);
		void setDIBLCoefficient_LMIN2100_LMAX5000(float eta);
		void setDIBLCoefficient_LMIN5000(float eta);

		void setZeroBiasBulkJunctionCapacitance(float Cj);
		void setZeroBiasSidewallBulkJunctionCapacitance(float Cjsw);
		void setBulkJunctionContactPotential(float pb);
		void setLateralDiffusionLength(float Ldiff);

		float getThermalVoltage() const;

		float getOverlapCapacity() const;
		float getThresholdVoltage_LMAX500() const;
		float getThresholdVoltage_LMIN500_LMAX1200() const;
		float getThresholdVoltage_LMIN1200_LMAX2100() const;
		float getThresholdVoltage_LMIN2100_LMAX5000() const;
		float getThresholdVoltage_LMIN5000() const;
		float getMobilityOxideCapacityCoefficient_LMAX500() const;
		float getMobilityOxideCapacityCoefficient_LMIN500_LMAX1200() const;
		float getMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100() const;
		float getMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000() const;
		float getMobilityOxideCapacityCoefficient_LMIN5000() const;
		float getEarlyVoltage() const;
		float getGateOxideCapacity() const;
		float getSlopeFactor_LMAX500() const;
		float getSlopeFactor_LMIN500_LMAX1200() const;
		float getSlopeFactor_LMIN1200_LMAX2100() const;
		float getSlopeFactor_LMIN2100_LMAX5000() const;
		float getSlopeFactor_LMIN5000() const;
		float getChannelLengthCoefficient_LMAX500() const;
		float getChannelLengthCoefficient_LMIN500_LMAX1200() const;
		float getChannelLengthCoefficient_LMIN1200_LMAX2100() const;
		float getChannelLengthCoefficient_LMIN2100_LMAX5000() const;
		float getChannelLengthCoefficient_LMIN5000() const;
		float getMobilityReductionCoefficient_LMAX500() const;
		float getMobilityReductionCoefficient_LMIN500_LMAX1200() const;
		float getMobilityReductionCoefficient_LMIN1200_LMAX2100() const;
		float getMobilityReductionCoefficient_LMIN2100_LMAX5000() const;
		float getMobilityReductionCoefficient_LMIN5000() const;
		float getDIBLCoefficient_LMAX500() const;
		float getDIBLCoefficient_LMIN500_LMAX1200() const;
		float getDIBLCoefficient_LMIN1200_LMAX2100() const;
		float getDIBLCoefficient_LMIN2100_LMAX5000() const;
		float getDIBLCoefficient_LMIN5000() const;
		float getMinArea() const;
		float getMinWidth() const;
		float getMinMultiplier() const;
		float getMinLength() const;

		float getZeroBiasBulkJunctionCapacitance() const;
		float getZeroBiasSidewallBulkJunctionCapacitance() const;
		float getBulkJunctionContactPotential() const;
		float getLateralDiffusionLength() const;


	private:
		static const int NOT_INITIALIZED_;

		bool hasOverlapCapacity() const;
		bool hasThresholdVoltage_LMAX500() const;
		bool hasThresholdVoltage_LMIN500_LMAX1200() const;
		bool hasThresholdVoltage_LMIN1200_LMAX2100() const;
		bool hasThresholdVoltage_LMIN2100_LMAX5000() const;
		bool hasThresholdVoltage_LMIN5000() const;
		bool hasMobilityOxideCapacityCoefficient_LMAX500() const;
		bool hasMobilityOxideCapacityCoefficient_LMIN500_LMAX1200() const;
		bool hasMobilityOxideCapacityCoefficient_LMIN1200_LMAX2100() const;
		bool hasMobilityOxideCapacityCoefficient_LMIN2100_LMAX5000() const;
		bool hasMobilityOxideCapacityCoefficient_LMIN5000() const;
		bool hasEarlyVoltage() const;
		bool hasGateOxideCapacity() const;
		bool hasSlopeFactor_LMAX500() const;
		bool hasSlopeFactor_LMIN500_LMAX1200() const;
		bool hasSlopeFactor_LMIN1200_LMAX2100() const;
		bool hasSlopeFactor_LMIN2100_LMAX5000() const;
		bool hasSlopeFactor_LMIN5000() const;
		bool hasMobilityReductionCoefficient_LMAX500() const;
		bool hasMobilityReductionCoefficient_LMIN500_LMAX1200() const;
		bool hasMobilityReductionCoefficient_LMIN1200_LMAX2100() const;
		bool hasMobilityReductionCoefficient_LMIN2100_LMAX5000() const;
		bool hasMobilityReductionCoefficient_LMIN5000() const;
		bool hasDIBLCoefficient_LMAX500() const;
		bool hasDIBLCoefficient_LMIN500_LMAX1200() const;
		bool hasDIBLCoefficient_LMIN1200_LMAX2100() const;
		bool hasDIBLCoefficient_LMIN2100_LMAX5000() const;
		bool hasDIBLCoefficient_LMIN5000() const;

		bool hasZeroBiasBulkJunctionCapacitance() const;
		bool hasZeroBiasSidewallBulkJunctionCapacitance() const;
		bool hasBulkJunctionContactPotential() const;
		bool hasLateralDiffusionLength() const;

	private:
		float thermalVoltage_;
		
		float thresholdVoltage_LMAX500_;
		float thresholdVoltage_LMIN500_LMAX1200_;
		float thresholdVoltage_LMIN1200_LMAX2100_;
		float thresholdVoltage_LMIN2100_LMAX5000_;
		float thresholdVoltage_LMIN5000_;
		float muCox_LMAX500_;
		float muCox_LMIN500_LMAX1200_;
		float muCox_LMIN1200_LMAX2100_;
		float muCox_LMIN2100_LMAX5000_;
		float muCox_LMIN5000_;
		float earlyVoltage_;
		float overlapCapacity_;
		float gateOxideCapacity_;
		float slopeFactor_LMAX500_;
		float slopeFactor_LMIN500_LMAX1200_;
		float slopeFactor_LMIN1200_LMAX2100_;
		float slopeFactor_LMIN2100_LMAX5000_;
		float slopeFactor_LMIN5000_;
		float Amin_;
		float Lmin_;
		float Wmin_;
		float Mmin_;
		float lambda_LMAX500_;
		float lambda_LMIN500_LMAX1200_;
		float lambda_LMIN1200_LMAX2100_;
		float lambda_LMIN2100_LMAX5000_;
		float lambda_LMIN5000_;
		float theta_LMAX500_;
		float theta_LMIN500_LMAX1200_;
		float theta_LMIN1200_LMAX2100_;
		float theta_LMIN2100_LMAX5000_;
		float theta_LMIN5000_;
		float eta_LMAX500_;
		float eta_LMIN500_LMAX1200_;
		float eta_LMIN1200_LMAX2100_;
		float eta_LMIN2100_LMAX5000_;
		float eta_LMIN5000_;
		float pb_;
		float cj_;
		float cjsw_;
		float ldiff_;

	};


}


#endif /* AUTOMATICSIZING_INCL_CIRCUITINFORMATION_TECHNOLOGIESPECIFICATION_H_ */
