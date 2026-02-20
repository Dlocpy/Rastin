#pragma once

#include <wx/dialog.h>
#include <wx/grid.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <map>

#include "GraphPanel.h"
#include "Spline.h"

class CTWindow : public wxDialog
{
public:
	CTWindow(wxWindow* parent, std::map<wxString, Spline>& limitFactorCurves);
	
	~CTWindow();
private:
	void IsSelectedLine(wxCommandEvent& event);
	void OnButtonAdd(wxCommandEvent& event);
	void OnButtonDelete(wxCommandEvent& event);
	void UpdateAllInWindow(const wxString& str);

	GraphPanel* m_paintPanel;
	wxListBox* m_listCurves;
	wxTextCtrl* m_nameCurve;
	wxButton* m_addCurve;
	wxButton* m_delCurve;
	wxGrid* m_gridOfValue;
	std::map<wxString, Spline>& m_curves;
};

