#include <wx/dcclient.h>
#include "rastin/GraphPanel.h"

GraphPanel::GraphPanel(wxWindow* parent) : 
    wxPanel(parent, wxID_ANY, wxDefaultPosition) {
	Bind(wxEVT_PAINT, &GraphPanel::OnPaint, this);
}

void GraphPanel::OnPaint(wxPaintEvent& event) {
    
    wxPaintDC dc(this);

    //очистка фона
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();

    //получение размеров панели пользователя
    int width, height;
    GetClientSize(&width, &height);

    const int margin = 50;
    const double reserve = 15.0 / 100 + 1.0;
    // Оси
    dc.SetPen(wxPen(*wxBLACK, 2));
    dc.DrawLine(margin, height - margin, width - margin, height - margin); // S
    dc.DrawLine(margin, margin, margin, height - margin);                 // Kпр

    // Подписи
    dc.DrawText("S", width - margin + 10, height - margin + 10);
    dc.DrawText("Kпр", margin - 20, margin - 20);

    if (m_currentCurve.has_value()) {
        //кривая предельной кратности
        dc.SetPen(wxPen(*wxBLUE, 5));

        double xMin = m_currentCurve.value().GetXMin();
        double xMax = m_currentCurve.value().GetXMax();
        double yMin = m_currentCurve.value().GetYMin();
        double yMax = m_currentCurve.value().GetYMax();

        double scaleX = (width - 2 * margin) / (xMax * reserve);
        double scaleY = (height - 2 * margin) / (yMax * reserve);

        std::vector<wxPoint> splineArray;


        for (int i = xMin * scaleX; i < xMax * scaleX; ++i) {
            double xValue = i / scaleX;
            double yValue = m_currentCurve.value().interpolate(xValue);

            int screenX = margin + i;
            int screenY = height - margin - static_cast<int>(yValue * scaleY);

            splineArray.emplace_back(wxPoint(screenX, screenY));

        }

        dc.DrawSpline(splineArray.size(), splineArray.data());
    }                 
}

void GraphPanel::SetCurrentCurve(const Spline& curve) {
    m_currentCurve = curve;
}

void GraphPanel::ClearCurve() {
    m_currentCurve.reset();
}

GraphPanel::~GraphPanel() {
}