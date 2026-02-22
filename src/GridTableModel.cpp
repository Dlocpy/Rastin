#include "rastin/GridTableModel.h"
#include <wx/wfstream.h>
#include <wx/msgdlg.h>

GridTableModel::GridTableModel(std::map<wxString, Spline>& limitFactorCurves) : 
	m_curves(limitFactorCurves) {
	initData();
}

void GridTableModel::initData() {
	


    m_colLabels = { _("Iprim"), _("Isec"), _("Knom"), _("Snom"),
                    _("Protection"), _("Sload"), _("Lcable"), _("Section"),
                    _("Rcont"), _("I(3)sc"), _("I(1)sc"), _("K"),
                    _("S(3)perm"), _("S(1)perm") };
#if 1	
	m_dataTable = {
		CTWinding{ _(" "), _(" "), _(" "), _(" "),
				   _(" "), _(" "), _(" "), _(" "),
                   _(" "), _(" "), _(" "), _("<Not selected>"),
				   _(" "), _(" ") }
	};
	for (int row = 0; row < GetNumberRows(); ++row)
		CalcLimitFactorCurvesRow(row);
#endif
}

int GridTableModel::GetNumberRows() {
	
	return static_cast<int>(m_dataTable.size());
}

int GridTableModel::GetNumberCols() {
	return static_cast<int>(m_colLabels.size());
}

bool GridTableModel::IsEmptyCell(int row, int col) {
	if (row < 0 || row >= static_cast<int>(m_dataTable.size()) || col < 0 || col >= static_cast<int>(m_colLabels.size()))
		return true;
	return m_dataTable[row].memberValue(col).IsEmpty();
}

wxString GridTableModel::GetValue(int row, int col) {
	if (row < 0 || row >= static_cast<int>(m_dataTable.size()) || col < 0 || col >= static_cast<int>(m_colLabels.size()))
		return "";
	return m_dataTable[row].memberValue(col);
}

void GridTableModel::SetValue(int row, int col, const wxString& value) { 
	if (row < 0 || row >= static_cast<int>(m_dataTable.size()) || col < 0 || col >= static_cast<int>(m_colLabels.size()))
		return;
	m_dataTable[row].memberValue(col) = value;
    if (col == 0 || col == 9 || col == 10 || col == 11)
		CalcLimitFactorCurvesRow(row);
		
}

wxString GridTableModel::GetColLabelValue(int col) {
	return m_colLabels[col];
}

wxString GridTableModel::GetRowLabelValue(int row) {
	return wxString::Format("N %d", row + 1);
}

bool GridTableModel::AppendRows(size_t numRows) {
	
	m_dataTable.emplace_back(CTWinding{ _(" "), _(" "), _(" "), _(" "),
				   _(" "), _(" "), _(" "), _(" "),
                   _(" "), _(" "), _(" "), _("<Not selected>"),
				   _(" "), _(" ") });
	return true;
}

bool GridTableModel::DeleteRows(size_t pos, size_t numRows)
{
	m_dataTable.pop_back();
	return true;
}

bool GridTableModel::saveToFile(const wxString& filename) {
	wxFileOutputStream outFile(filename);
	if (!outFile.Ok()) return false;
	wxTextOutputStream text(outFile);
	for (const auto& item : m_dataTable) {
		item.serialize(text);
	}
	return true;
}

bool GridTableModel::loadFromFile(const wxString& filename) {
	wxFileInputStream inFile(filename);
	if (!inFile.Ok()) return false;
	wxTextInputStream text(inFile);
	m_dataTable.clear();

	while (!inFile.Eof()) {
		CTWinding item;
		if (item.deserialize(text))
			m_dataTable.push_back(item);
	}
	return true;
}

bool GridTableModel::calcRow(int row) const {
	//из строки в число
	double primaryCT;
	m_dataTable[row].primaryCT.ToDouble(&primaryCT);
	double secondaryCT;
	m_dataTable[row].secondaryCT.ToDouble(&secondaryCT);
	double alf;
	m_dataTable[row].alf.ToDouble(&alf);
	double ratedSecondaryPower;
	m_dataTable[row].ratedSecondaryPower.ToDouble(&ratedSecondaryPower);
	double protectionPower;
	m_dataTable[row].protectionPower.ToDouble(&protectionPower);
	double lengthCable;
	m_dataTable[row].lengthCable.ToDouble(&lengthCable);
	double crossSectionArea;
	m_dataTable[row].crossSectionArea.ToDouble(&crossSectionArea);
	double constactResistance;
	m_dataTable[row].constactResistance.ToDouble(&constactResistance);
	double threePhaseCurrent;
	m_dataTable[row].threePhaseCurrent.ToDouble(&threePhaseCurrent);
	double onePhaseCurrent;
	m_dataTable[row].onePhaseCurrent.ToDouble(&onePhaseCurrent);
	double permissiblePowerThreePhase;
	m_dataTable[row].permissiblePowerThreePhase.ToDouble(&permissiblePowerThreePhase);
	double permissiblePowerOnePhase;
	m_dataTable[row].permissiblePowerOnePhase.ToDouble(&permissiblePowerOnePhase);
	//Расчет
	double rWireThreePhaseCurrent = (lengthCable * 0.0175) / crossSectionArea;
	double rWireOnePhaseCurrent = 2 * (lengthCable * 0.0175) / crossSectionArea;
	double rDevice = protectionPower / (secondaryCT * secondaryCT);
	double alfMaxThreePhaseCurrent = threePhaseCurrent / primaryCT;
	double alfMaxOnePhaseCurrent = onePhaseCurrent / primaryCT;
	double permissibleResistanceThreePhase = permissiblePowerThreePhase / (secondaryCT * secondaryCT);
	double permissibleResistanceOnePhase = permissiblePowerOnePhase / (secondaryCT * secondaryCT);
	double rLoadThreePhaseCurrent = rWireThreePhaseCurrent + rDevice + constactResistance;
	double rLoadOnePhaseCurrent = rWireOnePhaseCurrent + rDevice + constactResistance;
	
	return (permissibleResistanceThreePhase > rLoadThreePhaseCurrent) && (permissibleResistanceOnePhase > rLoadOnePhaseCurrent) ? true : false;
}

void GridTableModel::CalcLimitFactorCurvesRow(int row) {
	double primaryCT;
	double threePhaseCurrent;
	double onePhaseCurrent;
	 
	GetValue(row, 0).ToDouble(&primaryCT);
	GetValue(row, 9).ToDouble(&threePhaseCurrent);
	GetValue(row, 10).ToDouble(&onePhaseCurrent);
	wxString currentCurve = m_dataTable[row].limitFactorCurve;
    if (currentCurve == _("<Not selected>")) {
			SetValue(row, 12, " ");
			SetValue(row, 13, " ");
	}
	else {
		double y;
		y = threePhaseCurrent / primaryCT;
		SetValue(row, 12, wxString::Format("%.3f", (m_curves[currentCurve].interpolateReverse(y))));
		y = onePhaseCurrent / primaryCT;
		SetValue(row, 13, wxString::Format("%.3f", (m_curves[currentCurve].interpolateReverse(y))));
	}
}
