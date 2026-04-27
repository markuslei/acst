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

#ifndef SIMULATION_INCL_SimulationCSP_CONSTRAINTS_POLESANDZEROSSIMULATION_H_
#define SIMULATION_INCL_SimulationCSP_CONSTRAINTS_POLESANDZEROSSIMULATION_H_

#include "Core/incl/Circuit/Net/NetId/NetId.h"

#include <gecode/float.hh>

namespace Partitioning {

	class Result;
	class TransconductancePart;


}

namespace Simulation {


	class CircuitInformation;
	class Variables;

	class SimulatedPolesAndZeros
	{
	public:
		SimulatedPolesAndZeros(const Partitioning::Result & result, const CircuitInformation & information);

		void initialize();
		void setPolesAndZerosIndexes(int &index, std::map<int, std::string>  & indexToVariableMap, Variables & variables);

		void setDominantePole(float dominantPole);

		void setPositiveZero(float positiveZero);

		
		void addImportantNonDominantPole(float nonDominantPole);
		void addImportantZero(float Zero);
		void addUnimportantNonDominantPole(float nonDominantPole);
		void addUnimportantZero(float Zero);

		float getDominantPole() const;

		int getDominantPoleIndex() const;

		float getPositiveZero() const;

		int getPositiveZeroIndex() const;

		std::vector<float> getImportantNonDominantPoles() const;


		std::vector<int> getImportantNonDominantPolesIndexVector() const;
		std::vector<float> getImportantZeros() const;

		std::vector<int> getImportantZerosIndexVector() const;
		std::vector<float> getUnimportantNonDominantPoles() const;
		std::vector<int> getUnimportantNonDominantPolesIndexVector() const;
		std::vector<float> getUnimportantZeros() const;
		std::vector<int> getUnimportantZerosIndexVector() const;

		int getNumberImportantNonDominantPoles() const;
		int getNumberUnimportantNonDominantPoles() const;
		int getNumberImportantZeros() const;
		int getNumberUnimportantZeros() const;

		std::string toStr() const;

	private:
		void countNumberOfPolesAndZeros(const Partitioning::Result & result, const CircuitInformation & circuitInformation);
		void addBiasHigherStagesNonDominantPolesAndZeros(Partitioning::TransconductancePart & stage, const Partitioning::Result & result);

		bool isOutputNetFirstStage(Core::NetId outputNet, const Partitioning::Result & result, const CircuitInformation & circuitInformation) const;

	private:
		static const int NOT_INITIALIZED_;

		float dominantPole_;

		int dominatPoleIndex_;

		float positiveZero_;
		int positiveZeroIndex_;

		//Differentiations According to Laker/Sansen Design of analog integrated circuits and systems
		std::vector<float> importantNonDominantPoles_;


		std::vector<int> importantNonDominantPolesIndexVector_;
		std::vector<float> importantZeros_;

		std::vector<int> importantZerosIndexVector_;
		std::vector<float> unimportantNonDominantPoles_;
		std::vector<int> unimportantNonDominantPolesIndexVector_;
		std::vector<float> unimportantZeros_;
		std::vector<int> unimportantZerosIndexVector_;

		int numUnimportantZeros_;
		int numImportantZeros_;
		int numUnimportantNonDominantPoles_;
		int numImportantNonDominantPoles_;

	};



}




#endif /* SIMULATION_INCL_SimulationCSP_CONSTRAINTS_POLESANDZEROSSIMULATION_H_ */
