#include "rastin/ctwinding.h"

wxString& CTWinding::memberValue(int col) {
	switch (col) {
	case 0: return primaryCT;
	case 1: return secondaryCT;
	case 2: return alf;
	case 3: return ratedSecondaryPower;
	case 4: return relayProtection;
	case 5: return protectionPower;
	case 6: return lengthCable;
	case 7: return crossSectionArea;
	case 8: return constactResistance;
	case 9: return threePhaseCurrent;
	case 10: return onePhaseCurrent;
	case 11: return limitFactorCurve;
	case 12: return permissiblePowerThreePhase;
	case 13: return permissiblePowerOnePhase;
		}
}

void CTWinding::serialize(wxTextOutputStream& stream) const {
	stream << primaryCT << wxT("@") << secondaryCT << wxT("@") << alf << wxT("@")
		<< ratedSecondaryPower << wxT("@") << relayProtection << wxT("@") << protectionPower << wxT("@")
		<< lengthCable << wxT("@") << crossSectionArea << wxT("@") << constactResistance << wxT("@")
		<< threePhaseCurrent << wxT("@") << onePhaseCurrent << wxT("@") << limitFactorCurve << wxT("@")
		<< permissiblePowerThreePhase << wxT("@") << permissiblePowerOnePhase << wxT("\n");
}

bool CTWinding::deserialize(wxTextInputStream& stream) {
	wxString line = stream.ReadLine();
	if (line.empty()) return false;
	wxArrayString parts = wxSplit(line, '@', '"');
	primaryCT = parts[0];
	secondaryCT = parts[1];
	alf = parts[2];
	ratedSecondaryPower = parts[3];
	relayProtection = parts[4];
	protectionPower = parts[5];
	lengthCable = parts[6];
	crossSectionArea = parts[7];
	constactResistance = parts[8];
	threePhaseCurrent = parts[9];
	onePhaseCurrent = parts[10];
	limitFactorCurve = parts[11];
	permissiblePowerThreePhase = parts[12];
	permissiblePowerOnePhase = parts[13];
	return true;
}