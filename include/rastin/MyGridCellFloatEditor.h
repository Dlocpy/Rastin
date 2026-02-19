#pragma once

#include <wx/grid.h>
#include <memory>

// the editor for string/text data
class WXDLLIMPEXP_ADV MyGridCellFloatEditor : public wxGridCellEditor
{
public:
    explicit MyGridCellFloatEditor(size_t maxChars = 0)
        : wxGridCellEditor(),
        m_maxChars(maxChars)
    {
    }

    MyGridCellFloatEditor(const MyGridCellFloatEditor& other);

    virtual void Create(wxWindow* parent,
        wxWindowID id,
        wxEvtHandler* evtHandler) override;
    virtual void SetSize(const wxRect& rect) override;

    virtual bool IsAcceptedKey(wxKeyEvent& event) override;
    virtual void BeginEdit(int row, int col, wxGrid* grid) override;
    virtual bool EndEdit(int row, int col, const wxGrid* grid,
        const wxString& oldval, wxString* newval) override;
    virtual void ApplyEdit(int row, int col, wxGrid* grid) override;

    virtual void Reset() override;
    virtual void StartingKey(wxKeyEvent& event) override;
    virtual void HandleReturn(wxKeyEvent& event) override;

    // parameters string format is "max_width"
    virtual void SetParameters(const wxString& params) override;
#if wxUSE_VALIDATORS
    virtual void SetValidator(const wxValidator& validator);
#endif

    virtual wxGridCellEditor* Clone() const override
    {
        return new MyGridCellFloatEditor(*this);
    }

    // added GetValue so we can get the value which is in the control
    virtual wxString GetValue() const override;

protected:
    wxTextCtrl* Text() const { return (wxTextCtrl*)m_control; }

    // parts of our virtual functions reused by the derived classes
    void DoCreate(wxWindow* parent, wxWindowID id, wxEvtHandler* evtHandler,
        long style = 0);
    void DoBeginEdit(const wxString& startValue);
    void DoReset(const wxString& startValue);

private:
    size_t                   m_maxChars;        // max number of chars allowed
#if wxUSE_VALIDATORS
    std::unique_ptr<wxValidator> m_validator;
#endif
    wxString                 m_value;
};