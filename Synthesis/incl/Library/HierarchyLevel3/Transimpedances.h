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

#ifndef SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL3_TRANSIMPEDANCES_H_
#define SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL3_TRANSIMPEDANCES_H_

#include "Synthesis/incl/Library/LibraryItem.h"

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceName.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalName.h"

namespace Core
{
	class Circuit;
	class Instance;
}

namespace Synthesis
{
	class StructuralLevel;

	// Composes a transimpedance block from one VoltageBias (input branch)
	// and one CurrentBias (output branch). For v1, only the simple-CM
	// shape is enumerated: diode-connected one-transistor VB on the input,
	// normal one-transistor CB on the output. The same builder will later
	// produce cascoded / Wilson variants by widening the enumeration.
	class Transimpedances : public LibraryItem
	{
		public:
			Transimpedances(const StructuralLevel & structuralLevel);
			~Transimpedances();

			std::vector<const Core::Circuit*> getSimpleCurrentMirrorTransimpedancesPmos() const;
			std::vector<const Core::Circuit*> getSimpleCurrentMirrorTransimpedancesNmos() const;

			std::string toStr() const;

		public:
			static const Core::TerminalName INPUT_TERMINAL_;
			static const Core::TerminalName OUTPUT_TERMINAL_;
			static const Core::TerminalName SOURCE_TERMINAL_;

		private:
			void initializeSimpleCurrentMirrorTransimpedancesPmos(const StructuralLevel & structuralLevel, int & index);
			void initializeSimpleCurrentMirrorTransimpedancesNmos(const StructuralLevel & structuralLevel, int & index);

			const Core::Circuit & createTransimpedance(const Core::Circuit & voltageBias,
			                                            const Core::Circuit & currentBias,
			                                            int & index);

			void connectVoltageBiasInstance(Core::Circuit & transimpedance, Core::Instance & voltageBias) const;
			void connectCurrentBiasInstance(Core::Circuit & transimpedance, Core::Instance & currentBias) const;

			void eraseSimpleCurrentMirrorTransimpedancesPmos();
			void eraseSimpleCurrentMirrorTransimpedancesNmos();

		private:
			static const Core::InstanceName VOLTAGEBIAS_;
			static const Core::InstanceName CURRENTBIAS_;

			static const Core::NetId INPUT_NET_;
			static const Core::NetId OUTPUT_NET_;
			static const Core::NetId SOURCE_NET_;

			std::vector<const Core::Circuit*> simpleCurrentMirrorTransimpedancesPmos_;
			std::vector<const Core::Circuit*> simpleCurrentMirrorTransimpedancesNmos_;
	};
}

#endif /* SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL3_TRANSIMPEDANCES_H_ */
