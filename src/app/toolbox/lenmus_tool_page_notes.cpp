//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2013 LenMus project
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//---------------------------------------------------------------------------------------

//lenmus
#include "lenmus_tool_box.h"
#include "lenmus_tool_page_notes.h"
#include "lenmus_tool_group.h"
#include "lenmus_tool_box_events.h"
#include "lenmus_button.h"
//#include "../ArtProvider.h"        // to use ArtProvider for managing icons
//#include "../TheApp.h"              //to use GetMainFrame()
//#include "../MainFrame.h"           //to get active lmScoreCanvas

//wxWidgets
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/xrc/xmlres.h>
    #include <wx/bmpcbox.h>
    #include <wx/statline.h>
    #include <wx/clrpicker.h>
#endif



namespace lenmus
{


#define lmSPACING 5

//event IDs
#define lm_NUM_NR_BUTTONS   2       //note or rest
#define lm_NUM_DUR_BUTTONS  10
#define lm_NUM_ACC_BUTTONS  8
#define lm_NUM_DOT_BUTTONS  3
#define lm_NUM_OCTAVE_BUTTONS 10
#define lm_NUM_VOICE_BUTTONS 9
//#define lm_NUM_MOUSE_MODE_BUTTONS 2

enum {
	lmID_BT_NoteRest = 2600,
	lmID_BT_NoteDuration = lmID_BT_NoteRest + lm_NUM_NR_BUTTONS,
    lmID_BT_NoteAcc = lmID_BT_NoteDuration + lm_NUM_DUR_BUTTONS,
    lmID_BT_NoteDots = lmID_BT_NoteAcc + lm_NUM_ACC_BUTTONS,
    lmID_BT_Tie = lmID_BT_NoteDots + lm_NUM_DOT_BUTTONS,
    lmID_BT_Tuplet,
    lmID_BT_ToggleStem,
    lmID_BT_Beam_Cut,
    lmID_BT_Beam_Join,
    lmID_BT_Beam_Flatten,
    lmID_BT_Beam_Subgroup,
	lmID_BT_Octave,
	lmID_BT_Voice = lmID_BT_Octave + lm_NUM_OCTAVE_BUTTONS,
    //lmID_BT_MouseMode_Pointer,
    //lmID_BT_MouseMode_DataEntry,
};



IMPLEMENT_ABSTRACT_CLASS(ToolPageNotes, ToolPage)


//---------------------------------------------------------------------------------------
ToolPageNotes::ToolPageNotes()
{
}

//---------------------------------------------------------------------------------------
ToolPageNotes::ToolPageNotes(wxWindow* parent)
{
    Create(parent);
}

//---------------------------------------------------------------------------------------
void ToolPageNotes::Create(wxWindow* parent)
{
    //base class
    ToolPage::CreatePage(parent, k_page_notes);

    //members initialization
    m_pGrpNoteRest = (GrpNoteRest*)NULL;
    m_pGrpNoteDuration = (GrpNoteDuration*)NULL;
    m_pGrpNoteAcc = (GrpNoteAcc*)NULL;
    m_pGrpNoteDots = (GrpNoteDots*)NULL;
    m_pGrpModifiers = (GrpNoteModifiers*)NULL;
    m_pGrpBeams = (GrpBeams*)NULL;
	m_pGrpOctave = (GrpOctave*)NULL;
	m_pGrpVoice = (GrpVoice*)NULL;
	//m_pGrpEntryMode = (GrpMouseMode*)NULL;

    //other data initialization
    m_sPageToolTip = _("Edit tools for notes and rests");
    m_sPageBitmapName = _T("tool_notes");
}

//---------------------------------------------------------------------------------------
ToolPageNotes::~ToolPageNotes()
{
}

//---------------------------------------------------------------------------------------
ENoteHeads ToolPageNotes::GetNoteheadType()
{
    return k_notehead_quarter; //(ENoteHeads)m_pCboNotehead->GetSelection();
}

//---------------------------------------------------------------------------------------
wxString ToolPageNotes::GetToolShortDescription()
{
    //returns a short description of the selected tool. This description is used to
    //be displayed in the status bar

    if (IsNoteSelected())
        return _("Add note");
    else
        return _("Add rest");
}



//--------------------------------------------------------------------------------
// GrpNoteDuration implementation
//--------------------------------------------------------------------------------

GrpNoteDuration::GrpNoteDuration(ToolPage* pParent, wxBoxSizer* pMainSizer)
        : ToolButtonsGroup(pParent, k_group_type_options, lm_NUM_DUR_BUTTONS,
                             lmTBG_ONE_SELECTED, pMainSizer,
                             lmID_BT_NoteDuration, k_tool_none, pParent->GetColors())
{
}

//---------------------------------------------------------------------------------------
void GrpNoteDuration::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    SetGroupTitle(_("Duration"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    //create the specific controls for this group
    wxBoxSizer* pButtonsSizer;
	for (int iB=0; iB < lm_NUM_DUR_BUTTONS; iB++)
	{
		if (iB % 5 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}

		m_pButton[iB] = new CheckButton(this, lmID_BT_NoteDuration+iB, wxBitmap(24, 24));
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
    SetButtonsBitmaps(true);    //true: bitmaps for notes
	this->Layout();

	SelectButton(3);	//select quarter note
}

//---------------------------------------------------------------------------------------
ENoteType GrpNoteDuration::GetNoteDuration()
{
    return (ENoteType)(m_nSelButton+1);
}

//---------------------------------------------------------------------------------------
void GrpNoteDuration::SetButtonsBitmaps(bool fNotes)
{
    //Set buttons bitmap for rest or for notes, depending on flag fNotes

    const wxString sNoteBmps[lm_NUM_DUR_BUTTONS] = {
        _T("note_0"),
        _T("note_1"),
        _T("note_2"),
        _T("note_4"),
        _T("note_8"),
        _T("note_16"),
        _T("note_32"),
        _T("note_64"),
        _T("note_128"),
        _T("note_256"),
    };
    const wxString sRestBmps[lm_NUM_DUR_BUTTONS] = {
        _T("rest_0"),
        _T("rest_1"),
        _T("rest_2"),
        _T("rest_4"),
        _T("rest_8"),
        _T("rest_16"),
        _T("rest_32"),
        _T("rest_64"),
        _T("rest_128"),
        _T("rest_256"),
    };

    wxSize btSize(24, 24);
    if (fNotes)
	    for (int iB=0; iB < lm_NUM_DUR_BUTTONS; iB++)
	    {
            m_pButton[iB]->SetBitmapUp(sNoteBmps[iB], _T(""), btSize);
            m_pButton[iB]->SetBitmapDown(sNoteBmps[iB], _T("button_selected_flat"), btSize);
            m_pButton[iB]->SetBitmapOver(sNoteBmps[iB], _T("button_over_flat"), btSize);
	    }
    else
	    for (int iB=0; iB < lm_NUM_DUR_BUTTONS; iB++)
	    {
            m_pButton[iB]->SetBitmapUp(sRestBmps[iB], _T(""), btSize);
            m_pButton[iB]->SetBitmapDown(sRestBmps[iB], _T("button_selected_flat"), btSize);
            m_pButton[iB]->SetBitmapOver(sRestBmps[iB], _T("button_over_flat"), btSize);
	    }
}


//--------------------------------------------------------------------------------
// GrpNoteRest implementation
//--------------------------------------------------------------------------------

wxString m_sGrpNoteRestToolTips[lm_NUM_NR_BUTTONS];
bool m_fGrpNoteRestStringsInitialized = false;

GrpNoteRest::GrpNoteRest(ToolPage* pParent, wxBoxSizer* pMainSizer)
        : ToolButtonsGroup(pParent, k_group_type_tool_selector, lm_NUM_NR_BUTTONS,
                             lmTBG_ONE_SELECTED, pMainSizer,
                             lmID_BT_NoteRest, k_tool_none, pParent->GetColors())
{
    //load language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    if (!m_fGrpNoteRestStringsInitialized)
    {
        m_sGrpNoteRestToolTips[0] = _("Add notes");
        m_sGrpNoteRestToolTips[1] = _("Add rests");
        m_fGrpNoteRestStringsInitialized = true;
    }
}

//---------------------------------------------------------------------------------------
void GrpNoteRest::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    SetGroupTitle(_("Notes or rests"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    //create the specific controls for this group
    const wxString sButtonBmps[lm_NUM_NR_BUTTONS] = {
        _T("select_note"),
        _T("select_rest"),
    };


    wxBoxSizer* pButtonsSizer;
    wxSize btSize(24, 24);
	for (int iB=0; iB < lm_NUM_NR_BUTTONS; iB++)
	{
		if (iB % 5 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}

		m_pButton[iB] = new CheckButton(this, lmID_BT_NoteRest+iB, wxBitmap(24, 24));
        m_pButton[iB]->SetBitmapUp(sButtonBmps[iB], _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(sButtonBmps[iB], _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(sButtonBmps[iB], _T("button_over_flat"), btSize);
		m_pButton[iB]->SetToolTip( m_sGrpNoteRestToolTips[iB] );
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
	this->Layout();

	SelectButton(0);	//select notes
}

//---------------------------------------------------------------------------------------
bool GrpNoteRest::IsNoteSelected()
{
    return m_nSelButton==0;
}



//--------------------------------------------------------------------------------
// GrpOctave implementation
//--------------------------------------------------------------------------------

GrpOctave::GrpOctave(ToolPage* pParent, wxBoxSizer* pMainSizer)
        : ToolButtonsGroup(pParent, k_group_type_options, lm_NUM_OCTAVE_BUTTONS,
                             lmTBG_ONE_SELECTED, pMainSizer,
                             lmID_BT_Octave, k_tool_none, pParent->GetColors())
{
}

//---------------------------------------------------------------------------------------
void GrpOctave::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    wxString sTitle = _("Octave");
    SetGroupTitle(sTitle + _T(" (Ctrl)"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    wxBoxSizer* pButtonsSizer;
    wxSize btSize(16, 16);
	for (int iB=0; iB < lm_NUM_OCTAVE_BUTTONS; iB++)
	{
		if (iB % 9 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}

		wxString sBtName = wxString::Format(_T("opt_num%1d"), iB);
		wxString sToolTip = wxString::Format(_("Select octave %d"), iB);
        sToolTip += _T(". (Ctrl + num/+/-)");
		m_pButton[iB] = new CheckButton(this, lmID_BT_Octave+iB, wxBitmap(16, 16));
        m_pButton[iB]->SetBitmapUp(sBtName, _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(sBtName, _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(sBtName, _T("button_over_flat"), btSize);
		m_pButton[iB]->SetToolTip(sToolTip);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 0) );
	}
	this->Layout();

	SelectButton(4);	//select octave 4
}

//---------------------------------------------------------------------------------------
void GrpOctave::SetOctave(bool fUp)
{
    if (fUp)
    {
        if (m_nSelButton < 9)
            SelectButton(++m_nSelButton);
    }
    else
    {
        if (m_nSelButton > 0)
            SelectButton(--m_nSelButton);
    }
}


//--------------------------------------------------------------------------------
// GrpVoice implementation
//--------------------------------------------------------------------------------

GrpVoice::GrpVoice(ToolPage* pParent, wxBoxSizer* pMainSizer, int nNumButtons)
        : ToolButtonsGroup(pParent, k_group_type_options, nNumButtons, lmTBG_ONE_SELECTED,
                             pMainSizer, lmID_BT_Voice, k_tool_none,
                             pParent->GetColors())
{
}

//---------------------------------------------------------------------------------------
void GrpVoice::SetVoice(bool fUp)
{
    if (fUp)
    {
        if (m_nSelButton < 8)
            SelectButton(++m_nSelButton);
    }
    else
    {
        if (m_nSelButton > 0)
            SelectButton(--m_nSelButton);
    }
}


//--------------------------------------------------------------------------------
// Group for voice number: standard group
//--------------------------------------------------------------------------------
GrpVoiceStd::GrpVoiceStd(ToolPage* pParent, wxBoxSizer* pMainSizer)
        : GrpVoice(pParent, pMainSizer, lm_NUM_VOICE_BUTTONS)
{
}

//---------------------------------------------------------------------------------------
void GrpVoiceStd::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    wxString sTitle = _("Voice");
    SetGroupTitle(sTitle + _T(" (Alt)"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    wxBoxSizer* pButtonsSizer;
    wxSize btSize(16, 16);
	for (int iB=0; iB < lm_NUM_VOICE_BUTTONS; iB++)
	{
		if (iB % 9 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}

		wxString sBtName;
		if (iB == 0)
		{
			//button 0: AutoVoice
			sBtName = _T("opt_auto");
			m_pButton[iB] = new CheckButton(this, lmID_BT_Voice+iB, wxBitmap(16, 16));
			m_pButton[iB]->SetToolTip(_("Automatic voice assignment"));
		}
		else
		{
			sBtName = wxString::Format(_T("opt_num%1d"), iB);
			m_pButton[iB] = new CheckButton(this, lmID_BT_Voice+iB, wxBitmap(16, 16));
            wxString sTip = wxString::Format(_("Select voice %d"), iB);
            sTip += _T(". (Alt + num/+/-)");
			m_pButton[iB]->SetToolTip(sTip);
		}
        m_pButton[iB]->SetBitmapUp(sBtName, _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(sBtName, _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(sBtName, _T("button_over_flat"), btSize);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 0) );
	}
	this->Layout();

	SelectButton(0);	//select voice auto
}



//--------------------------------------------------------------------------------
// Group for voice number: for harmony exercises
//--------------------------------------------------------------------------------
GrpVoiceHarmony::GrpVoiceHarmony(ToolPage* pParent, wxBoxSizer* pMainSizer)
        : GrpVoice(pParent, pMainSizer, 4)
{
}

//---------------------------------------------------------------------------------------
void GrpVoiceHarmony::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //voice names
    static const wxString sBtName[4] = { _T("opt_voice_S"), _T("opt_voice_A"),
                                         _T("opt_voice_T"), _T("opt_voice_B") };
    const wxString sTipStr[4] = { _("Select voice Soprano"), _("Select voice Alto"),
                                  _("Select voice Tenor"), _("Select voice Bass") };

    //create the common controls for a group
    SetGroupTitle(_("Voice (Alt)"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    wxBoxSizer* pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
    pCtrolsSizer->Add(pButtonsSizer);

    wxSize btSize(16, 16);
	for (int iB=0; iB < 4; iB++)
	{
		m_pButton[iB] = new CheckButton(this, lmID_BT_Voice+iB, wxBitmap(16, 16));
        wxString sTip = sTipStr[iB] + _T(". (Alt + num/+/-)");
		m_pButton[iB]->SetToolTip(sTip);

        m_pButton[iB]->SetBitmapUp(sBtName[iB], _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(sBtName[iB], _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(sBtName[iB], _T("button_over_flat"), btSize);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 0) );
	}
	this->Layout();

	SelectButton(0);	//select voice Soprano
}



//--------------------------------------------------------------------------------
// GrpNoteAcc implementation
//--------------------------------------------------------------------------------

GrpNoteAcc::GrpNoteAcc(ToolPage* pParent, wxBoxSizer* pMainSizer)
        : ToolButtonsGroup(pParent, k_group_type_options, lm_NUM_ACC_BUTTONS,
                             lmTBG_ALLOW_NONE, pMainSizer,
                             lmID_BT_NoteAcc, k_tool_none, pParent->GetColors())
{
}

//---------------------------------------------------------------------------------------
void GrpNoteAcc::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    SetGroupTitle(_("Accidentals"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    //create the specific controls for this group
    const wxString sButtonBmps[lm_NUM_ACC_BUTTONS] = {
	    _T("acc_natural"),
	    _T("acc_flat"),
	    _T("acc_sharp"),
	    _T("acc_flat_flat"),
	    _T("acc_double_sharp"),
	    _T("acc_sharp_sharp"),
	    _T("acc_natural_flat"),
	    _T("acc_natural_sharp"),
    };

    wxBoxSizer* pButtonsSizer;
    wxSize btSize(24, 24);
	for (int iB=0; iB < lm_NUM_ACC_BUTTONS; iB++)
	{
		if (iB % 5 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}
		m_pButton[iB] = new CheckButton(this, lmID_BT_NoteAcc+iB, wxBitmap(24,24));
        m_pButton[iB]->SetBitmapUp(sButtonBmps[iB], _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(sButtonBmps[iB], _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(sButtonBmps[iB], _T("button_over_flat"), btSize);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
	this->Layout();

	SelectButton(-1);	//select none
}

//---------------------------------------------------------------------------------------
EAccidentals GrpNoteAcc::GetNoteAcc()
{
    return (EAccidentals)(m_nSelButton+1);
}


//--------------------------------------------------------------------------------
// GrpNoteDots implementation
//--------------------------------------------------------------------------------

GrpNoteDots::GrpNoteDots(ToolPage* pParent, wxBoxSizer* pMainSizer)
        : ToolButtonsGroup(pParent, k_group_type_options, lm_NUM_DOT_BUTTONS,
                             lmTBG_ALLOW_NONE, pMainSizer,
                             lmID_BT_NoteDots, k_tool_none, pParent->GetColors())
{
}

//---------------------------------------------------------------------------------------
void GrpNoteDots::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    SetGroupTitle(_("Dots"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    //create the specific controls for this group
    const wxString sButtonBmps[lm_NUM_DOT_BUTTONS] = {
	    _T("dot_1"),
	    _T("dot_2"),
	    _T("dot_3"),
    };

    wxBoxSizer* pButtonsSizer;
    wxSize btSize(24, 24);
	for (int iB=0; iB < lm_NUM_DOT_BUTTONS; iB++)
	{
		if (iB % 5 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}
		m_pButton[iB] = new CheckButton(this, lmID_BT_NoteDots+iB, wxBitmap(24,24));
        m_pButton[iB]->SetBitmapUp(sButtonBmps[iB], _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(sButtonBmps[iB], _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(sButtonBmps[iB], _T("button_over_flat"), btSize);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
	this->Layout();

	SelectButton(-1);       //select none
}

//---------------------------------------------------------------------------------------
int GrpNoteDots::GetNoteDots()
{
    return m_nSelButton + 1;
}





//--------------------------------------------------------------------------------
// GrpNoteModifiers implementation
//--------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GrpNoteModifiers, ToolGroup)
    EVT_BUTTON  (lmID_BT_Tie, GrpNoteModifiers::OnTieButton)
    EVT_BUTTON  (lmID_BT_Tuplet, GrpNoteModifiers::OnTupletButton)
    EVT_BUTTON  (lmID_BT_ToggleStem, GrpNoteModifiers::OnToggleStemButton)
END_EVENT_TABLE()


//---------------------------------------------------------------------------------------
GrpNoteModifiers::GrpNoteModifiers(ToolPage* pParent, wxBoxSizer* pMainSizer)
        : ToolGroup(pParent, k_group_type_options, pParent->GetColors())
        , m_nSelectedToolID(k_tool_none)
{
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    SetGroupTitle(_("Modifiers"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    //create the specific controls for this group

    // Tie button
	wxBoxSizer* pRow1Sizer = new wxBoxSizer( wxHORIZONTAL );

    wxSize btSize(24, 24);
	m_pBtnTie = new CheckButton(this, lmID_BT_Tie, wxBitmap(24,24));
    m_pBtnTie->SetBitmapUp(_T("tie"), _T(""), btSize);
    m_pBtnTie->SetBitmapDown(_T("tie"), _T("button_selected_flat"), btSize);
    m_pBtnTie->SetBitmapOver(_T("tie"), _T("button_over_flat"), btSize);
    m_pBtnTie->SetBitmapDisabled(_T("tie_dis"), _T(""), btSize);
    m_pBtnTie->SetToolTip(_("Add/remove a tie to/from selected notes"));
	pRow1Sizer->Add( m_pBtnTie, wxSizerFlags(0).Border(wxALL, 2) );

    // Tuplet button
	m_pBtnTuplet = new CheckButton(this, lmID_BT_Tuplet, wxBitmap(24,24));
    m_pBtnTuplet->SetBitmapUp(_T("tuplet"), _T(""), btSize);
    m_pBtnTuplet->SetBitmapDown(_T("tuplet"), _T("button_selected_flat"), btSize);
    m_pBtnTuplet->SetBitmapOver(_T("tuplet"), _T("button_over_flat"), btSize);
    m_pBtnTuplet->SetBitmapDisabled(_T("tuplet_dis"), _T(""), btSize);
    m_pBtnTuplet->SetToolTip(_("Add/remove tuplet to/from selected notes"));
	pRow1Sizer->Add( m_pBtnTuplet, wxSizerFlags(0).Border(wxALL, 2) );

    // Toggle stem button
	m_pBtnToggleStem = new CheckButton(this, lmID_BT_ToggleStem, wxBitmap(24,24));
    m_pBtnToggleStem->SetBitmapUp(_T("toggle_stem"), _T(""), btSize);
    m_pBtnToggleStem->SetBitmapDown(_T("toggle_stem"), _T("button_selected_flat"), btSize);
    m_pBtnToggleStem->SetBitmapOver(_T("toggle_stem"), _T("button_over_flat"), btSize);
    m_pBtnToggleStem->SetBitmapDisabled(_T("toggle_stem_dis"), _T(""), btSize);
    m_pBtnToggleStem->SetToolTip(_("Toggle stem in selected notes"));
	pRow1Sizer->Add( m_pBtnToggleStem, wxSizerFlags(0).Border(wxALL, 2) );

    pCtrolsSizer->Add( pRow1Sizer, 0, wxEXPAND, 5 );

	this->Layout();
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::OnTieButton(wxCommandEvent& event)
{
    m_nSelectedToolID = k_tool_note_tie;
    PostToolBoxEvent(k_tool_note_tie, event.IsChecked());
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::OnTupletButton(wxCommandEvent& event)
{
    m_nSelectedToolID = k_tool_note_tuplet;
    PostToolBoxEvent(k_tool_note_tuplet, event.IsChecked());
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::OnToggleStemButton(wxCommandEvent& event)
{
    m_nSelectedToolID = k_tool_note_toggle_stem;
    PostToolBoxEvent(k_tool_note_toggle_stem, event.IsChecked());
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::SetToolTie(bool fChecked)
{
    fChecked ? m_pBtnTie->Press() : m_pBtnTie->Release();
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::SetToolTuplet(bool fChecked)
{
    fChecked ? m_pBtnTuplet->Press() : m_pBtnTuplet->Release();
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::SetToolToggleStem(bool fChecked)
{
    fChecked ? m_pBtnToggleStem->Press() : m_pBtnToggleStem->Release();
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::EnableTool(EToolID nToolID, bool fEnabled)
{
    switch (nToolID)
    {
        case k_tool_note_tie:
            m_pBtnTie->Enable(fEnabled);
            break;

        case k_tool_note_tuplet:
            m_pBtnTuplet->Enable(fEnabled);
            break;

        case k_tool_note_toggle_stem:
            m_pBtnToggleStem->Enable(fEnabled);
            break;

        default:
            wxASSERT(false);
    }

    //enable /disable group
    bool fEnableGroup = m_pBtnTie->IsEnabled() || m_pBtnTuplet->IsEnabled()
                        || m_pBtnToggleStem->IsEnabled();
    EnableGroup(fEnableGroup);
}





//--------------------------------------------------------------------------------
// GrpBeams implementation
//--------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GrpBeams, ToolGroup)
    EVT_BUTTON  (lmID_BT_Beam_Cut, GrpBeams::OnButton)
    EVT_BUTTON  (lmID_BT_Beam_Join, GrpBeams::OnButton)
    EVT_BUTTON  (lmID_BT_Beam_Flatten, GrpBeams::OnButton)
    EVT_BUTTON  (lmID_BT_Beam_Subgroup, GrpBeams::OnButton)
END_EVENT_TABLE()


//---------------------------------------------------------------------------------------
GrpBeams::GrpBeams(ToolPage* pParent, wxBoxSizer* pMainSizer)
        : ToolGroup(pParent, k_group_type_options, pParent->GetColors())
        , m_nSelectedToolID(k_tool_none)
{
}

//---------------------------------------------------------------------------------------
void GrpBeams::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    SetGroupTitle(_("Beams"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    //create the specific controls for this group

    // cut beam button
	wxBoxSizer* pRow1Sizer = new wxBoxSizer( wxHORIZONTAL );

    wxSize btSize(24, 24);
	m_pBtnBeamCut = new BitmapButton(this, lmID_BT_Beam_Cut, wxBitmap(24,24));
    m_pBtnBeamCut->SetBitmapUp(_T("tool_beam_cut"), _T(""), btSize);
    m_pBtnBeamCut->SetBitmapDown(_T("tool_beam_cut"), _T("button_selected_flat"), btSize);
    m_pBtnBeamCut->SetBitmapOver(_T("tool_beam_cut"), _T("button_over_flat"), btSize);
    m_pBtnBeamCut->SetBitmapDisabled(_T("tool_beam_cut_dis"), _T(""), btSize);
    m_pBtnBeamCut->SetToolTip(_("Break beam at current cursor position"));
	pRow1Sizer->Add( m_pBtnBeamCut, wxSizerFlags(0).Border(wxALL, 2) );

    // beam join button
	m_pBtnBeamJoin = new BitmapButton(this, lmID_BT_Beam_Join, wxBitmap(24,24));
    m_pBtnBeamJoin->SetBitmapUp(_T("tool_beam_join"), _T(""), btSize);
    m_pBtnBeamJoin->SetBitmapDown(_T("tool_beam_join"), _T("button_selected_flat"), btSize);
    m_pBtnBeamJoin->SetBitmapOver(_T("tool_beam_join"), _T("button_over_flat"), btSize);
    m_pBtnBeamJoin->SetBitmapDisabled(_T("tool_beam_join_dis"), _T(""), btSize);
    m_pBtnBeamJoin->SetToolTip(_("Beam together all selected notes"));
	pRow1Sizer->Add( m_pBtnBeamJoin, wxSizerFlags(0).Border(wxALL, 2) );

    // beam subgroup button
	m_pBtnBeamSubgroup = new BitmapButton(this, lmID_BT_Beam_Subgroup, wxBitmap(24,24));
    m_pBtnBeamSubgroup->SetBitmapUp(_T("tool_beam_subgroup"), _T(""), btSize);
    m_pBtnBeamSubgroup->SetBitmapDown(_T("tool_beam_subgroup"), _T("button_selected_flat"), btSize);
    m_pBtnBeamSubgroup->SetBitmapOver(_T("tool_beam_subgroup"), _T("button_over_flat"), btSize);
    m_pBtnBeamSubgroup->SetBitmapDisabled(_T("tool_beam_subgroup_dis"), _T(""), btSize);
    m_pBtnBeamSubgroup->SetToolTip(_("Subdivide beamed group at current cursor position"));
	pRow1Sizer->Add( m_pBtnBeamSubgroup, wxSizerFlags(0).Border(wxALL, 2) );

    // beam flatten button
	m_pBtnBeamFlatten = new BitmapButton(this, lmID_BT_Beam_Flatten, wxBitmap(24,24));
    m_pBtnBeamFlatten->SetBitmapUp(_T("tool_beam_flatten"), _T(""), btSize);
    m_pBtnBeamFlatten->SetBitmapDown(_T("tool_beam_flatten"), _T("button_selected_flat"), btSize);
    m_pBtnBeamFlatten->SetBitmapOver(_T("tool_beam_flatten"), _T("button_over_flat"), btSize);
    m_pBtnBeamFlatten->SetBitmapDisabled(_T("tool_beam_flatten_dis"), _T(""), btSize);
    m_pBtnBeamFlatten->SetToolTip(_("Adjust selected beam to draw it horizontal"));
	pRow1Sizer->Add( m_pBtnBeamFlatten, wxSizerFlags(0).Border(wxALL, 2) );


	pCtrolsSizer->Add( pRow1Sizer, 0, wxEXPAND, 5 );
	this->Layout();

    //disable buttons not yet used
    m_pBtnBeamFlatten->Enable(false);
    m_pBtnBeamSubgroup->Enable(false);
}

//---------------------------------------------------------------------------------------
void GrpBeams::OnButton(wxCommandEvent& event)
{
    switch(event.GetId())
    {
        case lmID_BT_Beam_Cut:      m_nSelectedToolID = k_tool_beams_cut;         break;
        case lmID_BT_Beam_Join:     m_nSelectedToolID = k_tool_beams_join;        break;
        case lmID_BT_Beam_Flatten:  m_nSelectedToolID = k_tool_beams_flatten;     break;
        case lmID_BT_Beam_Subgroup: m_nSelectedToolID = k_tool_beams_subgroup;    break;
        default:
            wxASSERT(false);
    }
    PostToolBoxEvent(m_nSelectedToolID, event.IsChecked());
}

//---------------------------------------------------------------------------------------
void GrpBeams::EnableTool(EToolID nToolID, bool fEnabled)
{
    switch (nToolID)
    {
        case k_tool_beams_cut:
            m_pBtnBeamCut->Enable(fEnabled);
            break;

        case k_tool_beams_join:
            m_pBtnBeamJoin->Enable(fEnabled);
            break;

        case k_tool_beams_flatten:
            //m_pBtnBeamFlatten->Enable(fEnabled);
            break;

        case k_tool_beams_subgroup:
            //m_pBtnBeamSubgroup->Enable(fEnabled);
            break;

        default:
            wxASSERT(false);
    }

    //enable /disable group
    bool fEnableGroup = m_pBtnBeamCut->IsEnabled() || m_pBtnBeamJoin->IsEnabled() ||
                        m_pBtnBeamFlatten->IsEnabled() || m_pBtnBeamSubgroup->IsEnabled();
    EnableGroup(fEnableGroup);
    //disable buttons not yet used
    m_pBtnBeamFlatten->Enable(false);
    m_pBtnBeamSubgroup->Enable(false);
}



//-------------------------------------------------------------------------------------
// ToolPageNotesStd implementation
//-------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(ToolPageNotesStd, ToolPageNotes)


//---------------------------------------------------------------------------------------
ToolPageNotesStd::ToolPageNotesStd()
{
}

//---------------------------------------------------------------------------------------
ToolPageNotesStd::ToolPageNotesStd(wxWindow* parent)
{
    Create(parent);
}

//---------------------------------------------------------------------------------------
void ToolPageNotesStd::Create(wxWindow* parent)
{
    ToolPageNotes::Create(parent);
}

//---------------------------------------------------------------------------------------
ToolPageNotesStd::~ToolPageNotesStd()
{
}

//---------------------------------------------------------------------------------------
void ToolPageNotesStd::CreateGroups()
{
    //Create the groups for this page

    wxBoxSizer *pMainSizer = GetMainSizer();

	m_pGrpOctave = new GrpOctave(this, pMainSizer);
	m_pGrpVoice = new GrpVoiceStd(this, pMainSizer);
	m_pGrpNoteRest = new GrpNoteRest(this, pMainSizer);
    m_pGrpNoteDuration = new GrpNoteDuration(this, pMainSizer);
    m_pGrpNoteAcc = new GrpNoteAcc(this, pMainSizer);
    m_pGrpNoteDots = new GrpNoteDots(this, pMainSizer);
    m_pGrpModifiers = new GrpNoteModifiers(this, pMainSizer);
    m_pGrpBeams = new GrpBeams(this, pMainSizer);
    AddGroup(m_pGrpOctave);
    AddGroup(m_pGrpVoice);
    AddGroup(m_pGrpNoteRest);
    AddGroup(m_pGrpNoteDuration);
    AddGroup(m_pGrpNoteAcc);
    AddGroup(m_pGrpNoteDots);
    AddGroup(m_pGrpModifiers);
    AddGroup(m_pGrpBeams);

	CreateLayout();

    //initialize info about selected group/tool
    m_nCurGroupID = k_grp_NoteRest;
    m_nCurToolID = m_pGrpNoteRest->GetCurrentToolID();

    m_fGroupsCreated = true;
}

//-------------------------------------------------------------------------------------
// ToolPageNotesHarmony implementation
//-------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(ToolPageNotesHarmony, ToolPageNotes)


//---------------------------------------------------------------------------------------
ToolPageNotesHarmony::ToolPageNotesHarmony()
    : m_pMenu((wxMenu*)NULL)
{
}

//---------------------------------------------------------------------------------------
ToolPageNotesHarmony::ToolPageNotesHarmony(wxWindow* parent)
{
    Create(parent);
}

//---------------------------------------------------------------------------------------
void ToolPageNotesHarmony::Create(wxWindow* parent)
{
    ToolPageNotes::Create(parent);
    m_pMenu = (wxMenu*)NULL;
}

//---------------------------------------------------------------------------------------
ToolPageNotesHarmony::~ToolPageNotesHarmony()
{
    if (m_pMenu)
        delete m_pMenu;
}

//---------------------------------------------------------------------------------------
void ToolPageNotesHarmony::CreateGroups()
{
    //Create the groups for this page

    wxBoxSizer *pMainSizer = GetMainSizer();

	m_pGrpOctave = new GrpOctave(this, pMainSizer);
	m_pGrpVoice = new GrpVoiceHarmony(this, pMainSizer);
	m_pGrpNoteRest = new GrpNoteRest(this, pMainSizer);
    m_pGrpNoteDuration = new GrpNoteDuration(this, pMainSizer);
    m_pGrpNoteAcc = new GrpNoteAcc(this, pMainSizer);
    m_pGrpNoteDots = new GrpNoteDots(this, pMainSizer);
    m_pGrpModifiers = new GrpNoteModifiers(this, pMainSizer);
    m_pGrpBeams = new GrpBeams(this, pMainSizer);
    AddGroup(m_pGrpOctave);
    AddGroup(m_pGrpVoice);
    AddGroup(m_pGrpNoteRest);
    AddGroup(m_pGrpNoteDuration);
    AddGroup(m_pGrpNoteAcc);
    AddGroup(m_pGrpNoteDots);
    AddGroup(m_pGrpModifiers);
    AddGroup(m_pGrpBeams);

	CreateLayout();

    //initialize info about selected group/tool
    m_nCurGroupID = k_grp_NoteRest;
    m_nCurToolID = m_pGrpNoteRest->GetCurrentToolID();

    m_fGroupsCreated = true;
}

//---------------------------------------------------------------------------------------
wxMenu* ToolPageNotesHarmony::GetContextualMenuForToolPage()
{
	if (m_pMenu)
        return m_pMenu;

	m_pMenu = new wxMenu();

//TODO TB
//	m_pMenu->Append(lmTOOL_VOICE_SOPRANO, _("&Soprano"));
//	m_pMenu->Append(lmTOOL_VOICE_ALTO, _("&Alto"));
//	m_pMenu->Append(lmTOOL_VOICE_TENOR, _("&Tenor"));
//	m_pMenu->Append(lmTOOL_VOICE_BASS, _("Bass"));

	return m_pMenu;
}

//---------------------------------------------------------------------------------------
void ToolPageNotesHarmony::OnPopUpMenuEvent(wxCommandEvent& event)
{
//TODO TB
//    int nID = event.GetId();
//    if (nID >= lmTOOL_VOICE_SOPRANO && nID <= lmTOOL_VOICE_BASS)
//    {
//        m_pGrpVoice->SelectButton(nID - lmTOOL_VOICE_SOPRANO);
//        event.Skip();
//    }
}


}   //namespace lenmus
