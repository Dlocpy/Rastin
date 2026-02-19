#pragma once
#include <wx/panel.h>
#include <optional>
#include "Spline.h"

class GraphPanel : public wxPanel
{
public:
	GraphPanel(wxWindow* parent);
	void SetCurrentCurve(const Spline& curve);
	void ClearCurve();
	~GraphPanel();
private:
	void OnPaint(wxPaintEvent& event);
	

	std::optional<Spline> m_currentCurve;
};

