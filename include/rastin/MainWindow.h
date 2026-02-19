#pragma once

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/wxprec.h>
#include <wx/grid.h>
#include <wx/filedlg.h>
#include <wx/wfstream.h>
#include <wx/numformatter.h>
#include <wx/button.h>


#include "GridTableModel.h"
#include "Spline.h"
#include <cereal/archives/binary.hpp>
#include <cereal/types/map.hpp>
#include "wxstring.h"

#include <map>


struct CTWinding;

class MainWindow : public wxFrame
{
    
public:
    MainWindow();
    ~MainWindow();

private:
    void OnOpen(wxCommandEvent& event);
    void OnClose(wxCommandEvent& event);
    void OnCreate(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);

    void OnWindowCT(wxCommandEvent& event);

    void OnAddRow(wxCommandEvent& event);
    void OnDeleteRow(wxCommandEvent& event);

    void OnAbout(wxCommandEvent& event);


    void saveCurves(const std::string& filename);
    void loadCurves(const std::string& filename);

    void Calc10Persent(wxCommandEvent& event);

    void InitTableValidation();
    void InitLimitFactorCurves();

    GridTableModel* model;
    wxGrid* grid;
    wxButton* calcButton;
    std::map<wxString, Spline> limitFactorCurves;
    wxString currentFilePath;

public:
    template <class Archive>
    void serialize(Archive& ar) {
        ar(limitFactorCurves);
    }

};

