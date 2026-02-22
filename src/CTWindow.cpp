
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/regex.h>
#include <wx/msgdlg.h>
#include <vector>
#include <array>
#include "rastin/CTWindow.h"
#include "rastin/MyGridCellFloatEditor.h"

enum {
	ID_BUTTON_ADD = 1,
	ID_BUTTON_DELETE
};

CTWindow::CTWindow(wxWindow* parent, std::map<wxString, Spline>& limitFactorCurves) : 
    wxDialog(parent, wxID_ANY, _("Accuracy Limit Factor Curves")),
	m_curves(limitFactorCurves) {

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	m_paintPanel = new GraphPanel(this);

	if (!m_curves.empty())
	    m_paintPanel->SetCurrentCurve((*m_curves.begin()).second);
	mainSizer->Add(m_paintPanel, 1, wxEXPAND | wxALL, 10);


	m_listCurves = new wxListBox(
		this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        0, nullptr,
        wxLB_SINGLE | wxLB_NEEDED_SB
	);

	int lineHeight = m_listCurves->GetCharHeight();
    m_listCurves->SetMinSize(wxSize(-1, lineHeight * 5 + 10));
	mainSizer->Add(m_listCurves, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

	wxArrayString sampleItems;
	for (const auto& item : m_curves) sampleItems.Add(item.first);
	m_listCurves->Set(sampleItems);


	m_nameCurve = new wxTextCtrl(this, wxID_ANY);
	mainSizer->Add(m_nameCurve, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

	wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    m_addCurve = new wxButton(this, ID_BUTTON_ADD, _("Add curve"));
    m_delCurve = new wxButton(this, ID_BUTTON_DELETE, _("Delete curve"));

	btnSizer->Add(m_addCurve, 0, wxRIGHT, 5);
	btnSizer->Add(m_delCurve, 0);
	mainSizer->Add(btnSizer, 0, wxALIGN_RIGHT | wxLEFT | wxRIGHT | wxBOTTOM, 5);

	m_gridOfValue = new wxGrid(this, wxID_ANY);
	m_gridOfValue->CreateGrid(2, 7);
	MyGridCellFloatEditor* editor = new MyGridCellFloatEditor;
	for (int row = 0; row < m_gridOfValue->GetNumberRows(); ++row) {
		for (int col = 0; col < m_gridOfValue->GetNumberCols(); ++col) {
			m_gridOfValue->SetCellEditor(row, col, editor);
			editor->IncRef();
		}
	}
	editor->DecRef();

	const std::array<wxString, 7> colLabelValues{ "1", "2", "3", "4", "5", "6", "7" };
	for (int i = 0; i < colLabelValues.size(); ++i) {
		m_gridOfValue->SetColLabelValue(i, colLabelValues[i]);
	}
    m_gridOfValue->SetRowLabelValue(0, _("S"));
    m_gridOfValue->SetRowLabelValue(1, _("K"));

	m_gridOfValue->SetGridLineColour(*wxBLACK);
	mainSizer->Add(m_gridOfValue, 0, wxALIGN_CENTER | wxALL, 10);

	m_paintPanel->SetMinSize(wxSize(10, 300));

	this->SetSizerAndFit(mainSizer);

	Bind(wxEVT_LISTBOX, &CTWindow::IsSelectedLine, this);
	Bind(wxEVT_BUTTON, &CTWindow::OnButtonAdd, this, ID_BUTTON_ADD);
	Bind(wxEVT_BUTTON, &CTWindow::OnButtonDelete, this, ID_BUTTON_DELETE);

	if (!m_curves.empty()) {
		UpdateAllInWindow((*m_curves.begin()).first);
	}
}

CTWindow::~CTWindow() {
	
}

void CTWindow::IsSelectedLine(wxCommandEvent& event) {
	int pos = m_listCurves->GetSelection();
	if (pos != wxNOT_FOUND) {
		wxString selectedText = m_listCurves->GetString(pos);
		UpdateAllInWindow(selectedText);
	}
	
}

void CTWindow::OnButtonAdd(wxCommandEvent& event) {
	wxString newCurveName = m_nameCurve->GetValue();
	wxRegEx regex("^\\s+");
	if (regex.Matches(newCurveName)) {
        wxMessageBox(_("The name of the Accuracy Limit Factor Curve must not begin with spaces!"),
            _("Attention"), wxOK | wxICON_WARNING);
		return;
	}
	if (newCurveName.empty()) {
        wxMessageBox(_("The name of the Accuracy Limit Factor Curve must not be empty!"),
            _("Attention"), wxOK | wxICON_WARNING);
		return;
	}
	std::map<double, double> valuesFromGrid;
	for (int col = 0; col < m_gridOfValue->GetNumberCols(); ++col) {
		wxString valueX = m_gridOfValue->GetCellValue(0, col);
		if (!valueX.empty()) {
			wxString valueY = m_gridOfValue->GetCellValue(1, col);
			if (!valueY.empty()) {
				double xVal, yVal;
				valueX.ToDouble(&xVal);
				valueY.ToDouble(&yVal);
				valuesFromGrid.try_emplace(xVal, yVal);
			}
		}
	}
	if (valuesFromGrid.size() < 3) {
        wxMessageBox(_("The number of points must be at least 3!"),
            _("Attention"), wxOK | wxICON_WARNING);
		return;
	}
	if (m_curves.contains(newCurveName)) {
        int result = wxMessageBox(_("This Accuracy Limit Factor Curve name already exists! Replace the existing curve with the new one?"),
            _("Confirmation"), wxYES_NO | wxICON_QUESTION);
		if (result == wxNO)
			return;
	}
	Spline newSpline;
	std::vector<double> x;
	std::vector<double> y;
	x.reserve(valuesFromGrid.size());
	y.reserve(valuesFromGrid.size());
	for (auto& pair : valuesFromGrid) {
		x.push_back(pair.first);
		y.push_back(pair.second);
	}
	newSpline.init(x, y);
	m_curves[newCurveName] = newSpline;
	wxArrayString sampleItems;
	for (const auto& item : m_curves) sampleItems.Add(item.first);
	m_listCurves->Set(sampleItems);
}

void CTWindow::OnButtonDelete(wxCommandEvent& event) {
	if (m_listCurves->GetCount() > 0) {
        int result = wxMessageBox(_("Delete Accuracy Limit Factor Curve?"),
            _("Confirmation"), wxYES_NO | wxICON_QUESTION);
		if (result == wxNO)
			return;

		int index = m_listCurves->GetSelection();
		if (index != wxNOT_FOUND) {
			const int countItems = static_cast<int>(m_listCurves->GetCount());
			if (countItems == 1) {
				m_listCurves->Delete(index);
				m_listCurves->SetSelection(wxNOT_FOUND);
				m_nameCurve->SetValue("");
				m_gridOfValue->ClearGrid();
				m_paintPanel->ClearCurve();
				m_paintPanel->Refresh();
				m_curves.clear();
				return;
			}
			const bool wasLastIndex = (index == (countItems - 1));
			wxString text = m_listCurves->GetString(index);
			m_listCurves->Delete(index);
			m_curves.erase(text);
			if (wasLastIndex) {
				m_listCurves->SetSelection(static_cast<int>(m_listCurves->GetCount() - 1));
			}
			else {
				m_listCurves->SetSelection(index);
			}
			index = m_listCurves->GetSelection();
			if (index != wxNOT_FOUND) {
				wxString selectedText = m_listCurves->GetString(index);
				m_nameCurve->SetValue(selectedText);
			}
		}

	}
	

}

void CTWindow::UpdateAllInWindow(const wxString& str) {
	m_nameCurve->SetValue(str);
	m_paintPanel->SetCurrentCurve(m_curves[str]);
	m_paintPanel->Refresh();

	for (size_t i = 0; i < m_gridOfValue->GetNumberCols(); ++i) {
		if (i < m_curves[str].GetSizeArray()) {
			m_gridOfValue->SetCellValue(0, i, wxString::Format("%.3f", m_curves[str].GetXArray(i)));
		}
		else m_gridOfValue->SetCellValue(0, i, wxEmptyString);
	}
	for (size_t i = 0; i < m_gridOfValue->GetNumberCols(); ++i) {
		if (i < m_curves[str].GetSizeArray()) {
			m_gridOfValue->SetCellValue(1, i, wxString::Format("%.3f", m_curves[str].GetYArray(i)));
		}
		else m_gridOfValue->SetCellValue(1, i, wxEmptyString);
	}
}
