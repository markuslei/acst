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

#include "Partitioning/incl/Parts/TransimpedancePart.h"
#include "Partitioning/incl/Parts/TransconductancePart.h"
#include "Partitioning/incl/Parts/BiasPart.h"

#include "StructRec/incl/StructureCircuit/Structure/Structure.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/PinType/StructurePinType.h"

#include "Core/incl/Common/BacktraceAssert.h"

namespace Partitioning {

	const std::string TransimpedancePart::SIMPLECURRENTMIRROR_STRING_ = "simpleCurrentMirror";

	TransimpedancePart::TransimpedancePart(int & num) :
			typeEnum_(TYPE_UNINITIALIZED),
			stage_(NULL),
			stageBias_(NULL)
	{
		const PartName & name = PartName("TransimpedancePart");
		PartId id;
		id.setName(name);
		id.setId(num);
		setPartId(id);
		num++;
	}

	TransimpedancePart::~TransimpedancePart()
	{
	}

	void TransimpedancePart::setType(const std::string type)
	{
		typeEnum_ = mapStringToEnum(type);
	}

	void TransimpedancePart::setStage(TransconductancePart & stage)
	{
		stage_ = &stage;
	}

	void TransimpedancePart::setStageBias(BiasPart & stageBias)
	{
		stageBias_ = &stageBias;
	}

	bool TransimpedancePart::isValidString(const std::string & str)
	{
		return getStringToEnumMap().find(str) != getStringToEnumMap().end();
	}

	std::string TransimpedancePart::getType() const
	{
		assert(hasType());
		return mapEnumToString(typeEnum_);
	}

	TransconductancePart & TransimpedancePart::getStage() const
	{
		assert(hasStage());
		return * stage_;
	}

	BiasPart & TransimpedancePart::getStageBias() const
	{
		assert(hasStageBias());
		return * stageBias_;
	}

	const StructRec::StructureNet & TransimpedancePart::getOutputNet() const
	{
		assert(isInitialized());
		const StructRec::Structure & mainStructure = **getMainStructures().begin();
		switch(typeEnum_)
		{
			case TYPE_SIMPLECURRENTMIRROR:
				return mainStructure.findNet(
						StructRec::StructurePinType("MosfetSimpleCurrentMirror", "Output"));
			default:
				assert(false, "Unknown transimpedance type.");
				return mainStructure.findNet(
						StructRec::StructurePinType("MosfetSimpleCurrentMirror", "Output"));
		}
	}

	bool TransimpedancePart::hasType() const
	{
		return typeEnum_ != TYPE_UNINITIALIZED;
	}

	bool TransimpedancePart::hasStage() const
	{
		return stage_ != NULL;
	}

	bool TransimpedancePart::hasStageBias() const
	{
		return stageBias_ != NULL;
	}

	bool TransimpedancePart::isInitialized() const
	{
		return hasType() && hasMainStructures() && hasPartId();
	}

	TransimpedancePart::TypeEnum TransimpedancePart::mapStringToEnum(const std::string & str)
	{
		assert(isValidString(str));
		return getStringToEnumMap().at(str);
	}

	std::string TransimpedancePart::mapEnumToString(const TypeEnum & tt)
	{
		return getEnumToStringMap().at(tt);
	}

	const TransimpedancePart::StringToEnumMap & TransimpedancePart::getStringToEnumMap()
	{
		static StringToEnumMap theMap;

		if(theMap.empty()) {
			theMap[SIMPLECURRENTMIRROR_STRING_] = TYPE_SIMPLECURRENTMIRROR;
		}
		return theMap;
	}

	const TransimpedancePart::EnumToStringMap & TransimpedancePart::getEnumToStringMap()
	{
		static EnumToStringMap theMap;

		if(theMap.empty()) {
			theMap[TYPE_SIMPLECURRENTMIRROR] = SIMPLECURRENTMIRROR_STRING_;
		}
		return theMap;
	}

	void TransimpedancePart::print(std::ostream & stream) const
	{
		stream << " Type: " << getType() << std::endl;
		if(hasStage())
		{
			stream << " Stage: " << stage_->getPartId().toStr() << std::endl;
		}
		if(hasStageBias())
		{
			stream << " StageBias: " << stageBias_->getPartId().toStr() << std::endl;
		}
	}

	void TransimpedancePart::writeXml(Core::XmlNode& xmlNode, Core::XmlDocument& doc) const
	{
		Core::XmlNode & transimpedanceNode = Core::RapidXmlUtils::addNode(xmlNode, doc, "transimpedancePart");
		Core::RapidXmlUtils::addAttr(transimpedanceNode, doc, "type", getType());
		Part::writeXml(transimpedanceNode, doc);
	}

	bool TransimpedancePart::isBiasPart() const
	{
		return false;
	}

	bool TransimpedancePart::isLoadPart() const
	{
		return false;
	}

	bool TransimpedancePart::isTransconductancePart() const
	{
		return false;
	}

	bool TransimpedancePart::isCapacitancePart() const
	{
		return false;
	}

	bool TransimpedancePart::isUndefinedPart() const
	{
		return false;
	}

	bool TransimpedancePart::isResistorPart() const
	{
		return false;
	}

	bool TransimpedancePart::isCommonModeSignalDetectorPart() const
	{
		return false;
	}

	bool TransimpedancePart::isPositiveFeedbackPart() const
	{
		return false;
	}

	bool TransimpedancePart::isTransimpedancePart() const
	{
		return true;
	}

}
