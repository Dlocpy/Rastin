#include "rastin/MyGridCellFloatEditor.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/utils.h"
#include "wx/dcclient.h"
#include "wx/settings.h"
#include "wx/log.h"
#include "wx/textctrl.h"
#include "wx/checkbox.h"
#include "wx/combobox.h"
#include "wx/intl.h"
#include "wx/math.h"
#include "wx/listbox.h"
#endif

#include "wx/numformatter.h"
#include "wx/valnum.h"
#include "wx/textfile.h"
#include "wx/spinctrl.h"
#include "wx/tokenzr.h"
#include "wx/renderer.h"
#include "wx/datectrl.h"
#include "wx/uilocale.h"

#include "wx/generic/gridsel.h"
#include "wx/generic/grideditors.h"
#include "wx/generic/private/grid.h"

#if defined(__WXGTK__)
#define WXUNUSED_GTK(identifier)    WXUNUSED(identifier)
#else
#define WXUNUSED_GTK(identifier)    identifier
#endif

#ifdef __WXOSX__
#include "wx/osx/private.h"
#endif

// Required for wxIs... functions
#include <ctype.h>


MyGridCellFloatEditor::MyGridCellFloatEditor(const MyGridCellFloatEditor& other)
    : wxGridCellEditor(other),
    m_maxChars(other.m_maxChars),
    m_value(other.m_value)
{
#if wxUSE_VALIDATORS
    if (other.m_validator)
    {
        SetValidator(*other.m_validator);
    }
#endif
}

void MyGridCellFloatEditor::Create(wxWindow* parent,
    wxWindowID id,
    wxEvtHandler* evtHandler)
{
    DoCreate(parent, id, evtHandler);
}

void MyGridCellFloatEditor::DoCreate(wxWindow* parent,
    wxWindowID id,
    wxEvtHandler* evtHandler,
    long style)
{
    style |= wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB | wxNO_BORDER;

    wxTextCtrl* const text = new wxTextCtrl(parent, id, wxEmptyString,
        wxDefaultPosition, wxDefaultSize,
        style);
    text->SetMargins(0, 0);
    m_control = text;

    // set max length allowed in the textctrl, if the parameter was set
    if (m_maxChars != 0)
    {
        Text()->SetMaxLength(m_maxChars);
    }
#if wxUSE_VALIDATORS
    // validate text in textctrl, if validator is set
    if (m_validator)
    {
        Text()->SetValidator(*m_validator);
    }
#endif

    wxGridCellEditor::Create(parent, id, evtHandler);
}

void MyGridCellFloatEditor::SetSize(const wxRect& rectOrig)
{
    wxRect rect(rectOrig);

    // Make the edit control large enough to allow for internal margins
    //
    // TODO: remove this if the text ctrl sizing is improved
    //
#if defined(__WXMSW__)
    rect.x += 2;
    rect.y += 2;

    rect.width -= 2;
    rect.height -= 2;
#elif !defined(__WXGTK__)
    int extra_x = 2;
    int extra_y = 2;

    rect.SetLeft(wxMax(0, rect.x - extra_x));
    rect.SetTop(wxMax(0, rect.y - extra_y));
    rect.SetRight(rect.GetRight() + 2 * extra_x);
    rect.SetBottom(rect.GetBottom() + 2 * extra_y);
#endif

    wxGridCellEditor::SetSize(rect);
}

void MyGridCellFloatEditor::BeginEdit(int row, int col, wxGrid* grid)
{
    wxASSERT_MSG(m_control, wxT("The wxGridCellEditor must be created first!"));

    m_value = grid->GetTable()->GetValue(row, col);

    DoBeginEdit(m_value);
}

void MyGridCellFloatEditor::DoBeginEdit(const wxString& startValue)
{
    Text()->SetValue(startValue);
    Text()->SetInsertionPointEnd();
    Text()->SelectAll();
    Text()->SetFocus();
}

bool MyGridCellFloatEditor::EndEdit(int WXUNUSED(row),
    int WXUNUSED(col),
    const wxGrid* WXUNUSED(grid),
    const wxString& WXUNUSED(oldval),
    wxString* newval)
{
    wxCHECK_MSG(m_control, false,
        "wxGridCellTextEditor must be created first!");

    wxString value = Text()->GetValue();
    if (value == m_value)
        return false;

    double floatValue;
    if (!value.empty()) {
        if (!value.ToDouble(&floatValue))
            return false;
        else {
            wxString temp = wxString::Format("%.3f", floatValue);
            value = temp;
        }
    }

    m_value = value;

    if (newval)
        *newval = m_value;

    return true;
}

void MyGridCellFloatEditor::ApplyEdit(int row, int col, wxGrid* grid)
{
    grid->GetTable()->SetValue(row, col, m_value);
    m_value.clear();
}

void MyGridCellFloatEditor::Reset()
{
    wxASSERT_MSG(m_control, "wxGridCellTextEditor must be created first!");

    DoReset(m_value);
}

void MyGridCellFloatEditor::DoReset(const wxString& startValue)
{
    Text()->SetValue(startValue);
    Text()->SetInsertionPointEnd();
}

bool MyGridCellFloatEditor::IsAcceptedKey(wxKeyEvent& event)
{
    switch (event.GetKeyCode())
    {
    case WXK_DELETE:
    case WXK_BACK:
        return true;

    default:
        return wxGridCellEditor::IsAcceptedKey(event);
    }
}

void MyGridCellFloatEditor::StartingKey(wxKeyEvent& event)
{
    // Since this is now happening in the EVT_CHAR event EmulateKeyPress is no
    // longer an appropriate way to get the character into the text control.
    // Do it ourselves instead.  We know that if we get this far that we have
    // a valid character, so not a whole lot of testing needs to be done.

    wxTextCtrl* tc = Text();
    int ch;

    bool isPrintable;

    ch = event.GetUnicodeKey();
    if (ch != WXK_NONE)
        isPrintable = true;
    else
    {
        ch = event.GetKeyCode();
        isPrintable = ch >= WXK_SPACE && ch < WXK_START;
    }

    switch (ch)
    {
    case WXK_DELETE:
        // Delete the initial character when starting to edit with DELETE.
        tc->Remove(0, 1);
        break;

    case WXK_BACK:
        // Delete the last character when starting to edit with BACKSPACE.
    {
        const long pos = tc->GetLastPosition();
        tc->Remove(pos - 1, pos);
    }
    break;

    default:
        if (isPrintable)
            tc->WriteText(static_cast<wxChar>(ch));
        break;
    }
}

void MyGridCellFloatEditor::HandleReturn(wxKeyEvent&
    WXUNUSED_GTK(event))
{
#if defined(__WXGTK__)
    // wxGTK needs a little extra help...
    size_t pos = (size_t)(Text()->GetInsertionPoint());
    wxString s(Text()->GetValue());
    s = s.Left(pos) + wxT("\n") + s.Mid(pos);
    Text()->SetValue(s);
    Text()->SetInsertionPoint(pos);
#else
    // the other ports can handle a Return key press
    //
    event.Skip();
#endif
}

void MyGridCellFloatEditor::SetParameters(const wxString& params)
{
    if (params.empty())
    {
        // reset to default
        m_maxChars = 0;
    }
    else
    {
        long tmp;
        if (params.ToLong(&tmp))
        {
            m_maxChars = (size_t)tmp;
        }
        else
        {
            wxLogDebug(wxT("Invalid wxGridCellTextEditor parameter string '%s' ignored"), params);
        }
    }
}

#if wxUSE_VALIDATORS
void MyGridCellFloatEditor::SetValidator(const wxValidator& validator)
{
    m_validator.reset(static_cast<wxValidator*>(validator.Clone()));
    if (m_validator && IsCreated())
        Text()->SetValidator(*m_validator);
}
#endif

// return the value in the text control
wxString MyGridCellFloatEditor::GetValue() const
{
    return Text()->GetValue();
}

