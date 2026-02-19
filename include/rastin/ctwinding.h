#pragma once
#include <wx/string.h>
#include <wx/txtstrm.h>

struct CTWinding {
	wxString primaryCT;							//первичный ток ТТ										целое число
	wxString secondaryCT;						//вторичный ток ТТ										1 или 5
	wxString alf;								//номинальная кратность ТТ								целое число 1 до 100
	wxString ratedSecondaryPower;				//номинальная мощность вторичной обмотки ТТ				целое число от 3 до 100
	wxString relayProtection;					//подключенная к обмотке защита/ы						текст
	wxString protectionPower;					//мощность подключенной защиты							вещественное число
	wxString lengthCable;						//длина кабеля											вещественное число
	wxString crossSectionArea;					//сечение жилы кабеля									1.5, 2.5, 4.0, 6.0
	wxString constactResistance;				//сопротивление контактов								вещественное число
	wxString threePhaseCurrent;					//ток трехфазного короткого замыкания					вещественное число
	wxString onePhaseCurrent;					//ток однофазного короткого замыкания					вещественное число
	wxString limitFactorCurve;					//Выбранная кривая предельной кратности	
	wxString permissiblePowerThreePhase;		//допустимая мощность по кривой предельной кратности	вещественное число
	wxString permissiblePowerOnePhase;

	void serialize(wxTextOutputStream& stream) const;
	bool deserialize(wxTextInputStream& stream);

	wxString& memberValue(int col);

};

