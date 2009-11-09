//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ToolNotes.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/xrc/xmlres.h"
#include "wx/bmpcbox.h"
#include "wx/statline.h"
#include "wx/clrpicker.h"

#include "ToolsBox.h"
#include "ToolNotes.h"
#include "ToolGroup.h"
#include "ToolBoxEvents.h"
#include "../ArtProvider.h"        // to use ArtProvider for managing icons
#include "../../widgets/Button.h"
#include "../TheApp.h"              //to use GetMainFrame()
#include "../MainFrame.h"           //to get active lmScoreCanvas


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
    lmID_BT_Beam_Cut,
    lmID_BT_Beam_Join,
    lmID_BT_Beam_Flatten,
    lmID_BT_Beam_Subgroup,
	lmID_BT_Octave,
	lmID_BT_Voice = lmID_BT_Octave + lm_NUM_OCTAVE_BUTTONS,
    //lmID_BT_MouseMode_Pointer,
    //lmID_BT_MouseMode_DataEntry,
};



IMPLEMENT_ABSTRACT_CLASS(lmToolPageNotes, lmToolPage)


lmToolPageNotes::lmToolPageNotes()
{
}

lmToolPageNotes::lmToolPageNotes(wxWindow* parent)
{
    Create(parent);
}

void lmToolPageNotes::Create(wxWindow* parent)
{
    //base class
    lmToolPage::CreatePage(parent, lmPAGE_NOTES);

    //members initialization
    m_pGrpNoteRest = (lmGrpNoteRest*)NULL;
    m_pGrpNoteDuration = (lmGrpNoteDuration*)NULL;
    m_pGrpNoteAcc = (lmGrpNoteAcc*)NULL;
    m_pGrpNoteDots = (lmGrpNoteDots*)NULL;
    m_pGrpTieTuplet = (lmGrpTieTuplet*)NULL;
    m_pGrpBeams = (lmGrpBeams*)NULL;
	m_pGrpOctave = (lmGrpOctave*)NULL;
	m_pGrpVoice = (lmGrpVoice*)NULL;
	//m_pGrpEntryMode = (lmGrpMouseMode*)NULL;

    //other data initialization
    m_sPageToolTip = _("Edit tools for notes and rests");
    m_sPageBitmapName = _T("tool_notes");
}

lmToolPageNotes::~lmToolPageNotes()
{
}

lmENoteHeads lmToolPageNotes::GetNoteheadType()
{
    return enh_Quarter; //(lmENoteHeads)m_pCboNotehead->GetSelection();
}

wxString lmToolPageNotes::GetToolShortDescription()
{
    //returns a short description of the selected tool. This description is used to
    //be displayed in the status bar

    if (IsNoteSelected())
        return _("Add note");
    else
        return _("Add rest");
}



//--------------------------------------------------------------------------------
// lmGrpNoteDuration implementation
//--------------------------------------------------------------------------------

lmGrpNoteDuration::lmGrpNoteDuration(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmToolButtonsGroup(pParent, lm_eGT_Options, lm_NUM_DUR_BUTTONS,
                             lmTBG_ONE_SELECTED, pMainSizer,
                             lmID_BT_NoteDuration, lmTOOL_NONE, pParent->GetColors())
{
}

void lmGrpNoteDuration::CreateGroupControls(wxBoxSizer* pMainSizer)
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

		m_pButton[iB] = new lmCheckButton(this, lmID_BT_NoteDuration+iB, wxBitmap(24, 24));
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
    SetButtonsBitmaps(true);    //true: bitmaps for notes
	this->Layout();

	SelectButton(3);	//select quarter note
}

lmENoteType lmGrpNoteDuration::GetNoteDuration()
{
    return (lmENoteType)(m_nSelButton+1);
}

void lmGrpNoteDuration::SetButtonsBitmaps(bool fNotes)
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
// lmGrpNoteRest implementation
//--------------------------------------------------------------------------------

lmGrpNoteRest::lmGrpNoteRest(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmToolButtonsGroup(pParent, lm_eGT_ToolSelector, lm_NUM_NR_BUTTONS,
                             lmTBG_ONE_SELECTED, pMainSizer,
                             lmID_BT_NoteRest, lmTOOL_NONE, pParent->GetColors())
{
}

void lmGrpNoteRest::CreateGroupControls(wxBoxSizer* pMainSizer)
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

		m_pButton[iB] = new lmCheckButton(this, lmID_BT_NoteRest+iB, wxBitmap(24, 24));
        m_pButton[iB]->SetBitmapUp(sButtonBmps[iB], _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(sButtonBmps[iB], _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(sButtonBmps[iB], _T("button_over_flat"), btSize);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
	this->Layout();

	SelectButton(0);	//select notes
}

bool lmGrpNoteRest::IsNoteSelected()
{
    return m_nSelButton==0;
}



//--------------------------------------------------------------------------------
// lmGrpOctave implementation
//--------------------------------------------------------------------------------

lmGrpOctave::lmGrpOctave(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmToolButtonsGroup(pParent, lm_eGT_Options, lm_NUM_OCTAVE_BUTTONS,
                             lmTBG_ONE_SELECTED, pMainSizer,
                             lmID_BT_Octave, lmTOOL_NONE, pParent->GetColors())
{
}

void lmGrpOctave::CreateGroupControls(wxBoxSizer* pMainSizer)
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
		m_pButton[iB] = new lmCheckButton(this, lmID_BT_Octave+iB, wxBitmap(16, 16));
        m_pButton[iB]->SetBitmapUp(sBtName, _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(sBtName, _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(sBtName, _T("button_over_flat"), btSize);
		m_pButton[iB]->SetToolTip(sToolTip);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 0) );
	}
	this->Layout();

	SelectButton(4);	//select octave 4
}

void lmGrpOctave::SetOctave(bool fUp)
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
// lmGrpVoice implementation
//--------------------------------------------------------------------------------

lmGrpVoice::lmGrpVoice(lmToolPage* pParent, wxBoxSizer* pMainSizer, int nNumButtons)
        : lmToolButtonsGroup(pParent, lm_eGT_Options, nNumButtons, lmTBG_ONE_SELECTED,
                             pMainSizer, lmID_BT_Voice, lmTOOL_NONE,
                             pParent->GetColors())
{
}

void lmGrpVoice::SetVoice(bool fUp)
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
lmGrpVoiceStd::lmGrpVoiceStd(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmGrpVoice(pParent, pMainSizer, lm_NUM_VOICE_BUTTONS)
{
}

void lmGrpVoiceStd::CreateGroupControls(wxBoxSizer* pMainSizer)
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
			m_pButton[iB] = new lmCheckButton(this, lmID_BT_Voice+iB, wxBitmap(16, 16));
			m_pButton[iB]->SetToolTip(_("Automatic voice assignment"));
		}
		else
		{
			sBtName = wxString::Format(_T("opt_num%1d"), iB);
			m_pButton[iB] = new lmCheckButton(this, lmID_BT_Voice+iB, wxBitmap(16, 16));
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
lmGrpVoiceHarmony::lmGrpVoiceHarmony(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmGrpVoice(pParent, pMainSizer, 4)
{
}

void lmGrpVoiceHarmony::CreateGroupControls(wxBoxSizer* pMainSizer)
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
		m_pButton[iB] = new lmCheckButton(this, lmID_BT_Voice+iB, wxBitmap(16, 16));
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
// lmGrpNoteAcc implementation
//--------------------------------------------------------------------------------

lmGrpNoteAcc::lmGrpNoteAcc(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmToolButtonsGroup(pParent, lm_eGT_Options, lm_NUM_ACC_BUTTONS,
                             lmTBG_ALLOW_NONE, pMainSizer,
                             lmID_BT_NoteAcc, lmTOOL_NONE, pParent->GetColors())
{
}

void lmGrpNoteAcc::CreateGroupControls(wxBoxSizer* pMainSizer)
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
		m_pButton[iB] = new lmCheckButton(this, lmID_BT_NoteAcc+iB, wxBitmap(24,24));
        m_pButton[iB]->SetBitmapUp(sButtonBmps[iB], _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(sButtonBmps[iB], _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(sButtonBmps[iB], _T("button_over_flat"), btSize);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
	this->Layout();

	SelectButton(-1);	//select none
}

lmEAccidentals lmGrpNoteAcc::GetNoteAcc()
{
    return (lmEAccidentals)(m_nSelButton+1);
}


//--------------------------------------------------------------------------------
// lmGrpNoteDots implementation
//--------------------------------------------------------------------------------

lmGrpNoteDots::lmGrpNoteDots(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmToolButtonsGroup(pParent, lm_eGT_Options, lm_NUM_DOT_BUTTONS,
                             lmTBG_ALLOW_NONE, pMainSizer,
                             lmID_BT_NoteDots, lmTOOL_NONE, pParent->GetColors())
{
}

void lmGrpNoteDots::CreateGroupControls(wxBoxSizer* pMainSizer)
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
		m_pButton[iB] = new lmCheckButton(this, lmID_BT_NoteDots+iB, wxBitmap(24,24));
        m_pButton[iB]->SetBitmapUp(sButtonBmps[iB], _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(sButtonBmps[iB], _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(sButtonBmps[iB], _T("button_over_flat"), btSize);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
	this->Layout();

	SelectButton(-1);       //select none
}

int lmGrpNoteDots::GetNoteDots()
{
    return m_nSelButton + 1;
}





//--------------------------------------------------------------------------------
// lmGrpTieTuplet implementation
//--------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmGrpTieTuplet, lmToolGroup)
    EVT_BUTTON  (lmID_BT_Tie, lmGrpTieTuplet::OnTieButton)
    EVT_BUTTON  (lmID_BT_Tuplet, lmGrpTieTuplet::OnTupletButton)
END_EVENT_TABLE()


lmGrpTieTuplet::lmGrpTieTuplet(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmToolGroup(pParent, lm_eGT_Options, pParent->GetColors())
        , m_nSelectedToolID(lmTOOL_NONE)
{
}

void lmGrpTieTuplet::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    SetGroupTitle(_("Modifiers"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    //create the specific controls for this group

    // Tie button
	wxBoxSizer* pRow1Sizer = new wxBoxSizer( wxHORIZONTAL );

    wxSize btSize(24, 24);
	m_pBtnTie = new lmCheckButton(this, lmID_BT_Tie, wxBitmap(24,24));
    m_pBtnTie->SetBitmapUp(_T("tie"), _T(""), btSize);
    m_pBtnTie->SetBitmapDown(_T("tie"), _T("button_selected_flat"), btSize);
    m_pBtnTie->SetBitmapOver(_T("tie"), _T("button_over_flat"), btSize);
    m_pBtnTie->SetBitmapDisabled(_T("tie_dis"), _T(""), btSize);
    m_pBtnTie->SetToolTip(_("Add/remove a tie to/from selected notes"));
	pRow1Sizer->Add( m_pBtnTie, wxSizerFlags(0).Border(wxALL, 2) );

    // Tuplet button
	m_pBtnTuplet = new lmCheckButton(this, lmID_BT_Tuplet, wxBitmap(24,24));
    m_pBtnTuplet->SetBitmapUp(_T("tuplet"), _T(""), btSize);
    m_pBtnTuplet->SetBitmapDown(_T("tuplet"), _T("button_selected_flat"), btSize);
    m_pBtnTuplet->SetBitmapOver(_T("tuplet"), _T("button_over_flat"), btSize);
    m_pBtnTuplet->SetBitmapDisabled(_T("tuplet_dis"), _T(""), btSize);
    m_pBtnTuplet->SetToolTip(_("Add/remove tuplet to/from selected notes"));
	pRow1Sizer->Add( m_pBtnTuplet, wxSizerFlags(0).Border(wxALL, 2) );

	pCtrolsSizer->Add( pRow1Sizer, 0, wxEXPAND, 5 );

	this->Layout();
}

void lmGrpTieTuplet::OnTieButton(wxCommandEvent& event)
{
    m_nSelectedToolID = lmTOOL_NOTE_TIE;
    PostToolBoxEvent(lmTOOL_NOTE_TIE, event.IsChecked());
}

void lmGrpTieTuplet::OnTupletButton(wxCommandEvent& event)
{
    m_nSelectedToolID = lmTOOL_NOTE_TUPLET;
    PostToolBoxEvent(lmTOOL_NOTE_TUPLET, event.IsChecked());
}

//void lmGrpTieTuplet::PostToolBoxEvent(lmEToolID nToolID, bool fSelected)
//{
//    //post tool box event to the active controller
//    wxWindow* pWnd = GetMainFrame()->GetActiveController();
//    if (pWnd)
//    {
//	    lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
//	    wxASSERT(pToolBox);
//        lmToolBoxToolSelectedEvent event(this->GetToolGroupID(), pToolBox->GetCurrentPageID(), nToolID,
//                             fSelected);
//        ::wxPostEvent( pWnd, event );
//    }
//}

void lmGrpTieTuplet::SetToolTie(bool fChecked)
{
    fChecked ? m_pBtnTie->Press() : m_pBtnTie->Release();
}

void lmGrpTieTuplet::SetToolTuplet(bool fChecked)
{
    fChecked ? m_pBtnTuplet->Press() : m_pBtnTuplet->Release();
}

void lmGrpTieTuplet::EnableTool(lmEToolID nToolID, bool fEnabled)
{
    switch (nToolID)
    {
        case lmTOOL_NOTE_TIE:
            m_pBtnTie->Enable(fEnabled);
            break;


        case lmTOOL_NOTE_TUPLET:
            m_pBtnTuplet->Enable(fEnabled);
            break;

        default:
            wxASSERT(false);
    }

    //enable /disable group
    bool fEnableGroup = m_pBtnTie->IsEnabled() || m_pBtnTuplet->IsEnabled();
    EnableGroup(fEnableGroup);
}





//--------------------------------------------------------------------------------
// lmGrpBeams implementation
//--------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmGrpBeams, lmToolGroup)
    EVT_BUTTON  (lmID_BT_Beam_Cut, lmGrpBeams::OnButton)
    EVT_BUTTON  (lmID_BT_Beam_Join, lmGrpBeams::OnButton)
    EVT_BUTTON  (lmID_BT_Beam_Flatten, lmGrpBeams::OnButton)
    EVT_BUTTON  (lmID_BT_Beam_Subgroup, lmGrpBeams::OnButton)
END_EVENT_TABLE()


lmGrpBeams::lmGrpBeams(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmToolGroup(pParent, lm_eGT_Options, pParent->GetColors())
        , m_nSelectedToolID(lmTOOL_NONE)
{
}

void lmGrpBeams::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    SetGroupTitle(_("Beams"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    //create the specific controls for this group

    // cut beam button
	wxBoxSizer* pRow1Sizer = new wxBoxSizer( wxHORIZONTAL );

    wxSize btSize(24, 24);
	m_pBtnBeamCut = new lmBitmapButton(this, lmID_BT_Beam_Cut, wxBitmap(24,24));
    m_pBtnBeamCut->SetBitmapUp(_T("tool_beam_cut"), _T(""), btSize);
    m_pBtnBeamCut->SetBitmapDown(_T("tool_beam_cut"), _T("button_selected_flat"), btSize);
    m_pBtnBeamCut->SetBitmapOver(_T("tool_beam_cut"), _T("button_over_flat"), btSize);
    m_pBtnBeamCut->SetBitmapDisabled(_T("tool_beam_cut_dis"), _T(""), btSize);
    m_pBtnBeamCut->SetToolTip(_("Break beam at current cursor position"));
	pRow1Sizer->Add( m_pBtnBeamCut, wxSizerFlags(0).Border(wxALL, 2) );

    // beam join button
	m_pBtnBeamJoin = new lmBitmapButton(this, lmID_BT_Beam_Join, wxBitmap(24,24));
    m_pBtnBeamJoin->SetBitmapUp(_T("tool_beam_join"), _T(""), btSize);
    m_pBtnBeamJoin->SetBitmapDown(_T("tool_beam_join"), _T("button_selected_flat"), btSize);
    m_pBtnBeamJoin->SetBitmapOver(_T("tool_beam_join"), _T("button_over_flat"), btSize);
    m_pBtnBeamJoin->SetBitmapDisabled(_T("tool_beam_join_dis"), _T(""), btSize);
    m_pBtnBeamJoin->SetToolTip(_("Beam together all selected notes"));
	pRow1Sizer->Add( m_pBtnBeamJoin, wxSizerFlags(0).Border(wxALL, 2) );

    // beam subgroup button
	m_pBtnBeamSubgroup = new lmBitmapButton(this, lmID_BT_Beam_Subgroup, wxBitmap(24,24));
    m_pBtnBeamSubgroup->SetBitmapUp(_T("tool_beam_subgroup"), _T(""), btSize);
    m_pBtnBeamSubgroup->SetBitmapDown(_T("tool_beam_subgroup"), _T("button_selected_flat"), btSize);
    m_pBtnBeamSubgroup->SetBitmapOver(_T("tool_beam_subgroup"), _T("button_over_flat"), btSize);
    m_pBtnBeamSubgroup->SetBitmapDisabled(_T("tool_beam_subgroup_dis"), _T(""), btSize);
    m_pBtnBeamSubgroup->SetToolTip(_("Subdivide beamed group at current cursor position"));
	pRow1Sizer->Add( m_pBtnBeamSubgroup, wxSizerFlags(0).Border(wxALL, 2) );

    // beam flatten button
	m_pBtnBeamFlatten = new lmBitmapButton(this, lmID_BT_Beam_Flatten, wxBitmap(24,24));
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

void lmGrpBeams::OnButton(wxCommandEvent& event)
{
    switch(event.GetId())
    {
        case lmID_BT_Beam_Cut:      m_nSelectedToolID = lmTOOL_BEAMS_CUT;         break;
        case lmID_BT_Beam_Join:     m_nSelectedToolID = lmTOOL_BEAMS_JOIN;        break;
        case lmID_BT_Beam_Flatten:  m_nSelectedToolID = lmTOOL_BEAMS_FLATTEN;     break;
        case lmID_BT_Beam_Subgroup: m_nSelectedToolID = lmTOOL_BEAMS_SUBGROUP;    break;
        default:
            wxASSERT(false);
    }
    PostToolBoxEvent(m_nSelectedToolID, event.IsChecked());
}

void lmGrpBeams::EnableTool(lmEToolID nToolID, bool fEnabled)
{
    switch (nToolID)
    {
        case lmTOOL_BEAMS_CUT:
            m_pBtnBeamCut->Enable(fEnabled);
            break;

        case lmTOOL_BEAMS_JOIN:
            m_pBtnBeamJoin->Enable(fEnabled);
            break;

        case lmTOOL_BEAMS_FLATTEN:
            //m_pBtnBeamFlatten->Enable(fEnabled);
            break;

        case lmTOOL_BEAMS_SUBGROUP:
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
// lmToolPageNotesStd implementation
//-------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(lmToolPageNotesStd, lmToolPageNotes)


lmToolPageNotesStd::lmToolPageNotesStd()
{
}

lmToolPageNotesStd::lmToolPageNotesStd(wxWindow* parent)
{
    Create(parent);
}

void lmToolPageNotesStd::Create(wxWindow* parent)
{
    lmToolPageNotes::Create(parent);
}

lmToolPageNotesStd::~lmToolPageNotesStd()
{
}

void lmToolPageNotesStd::CreateGroups()
{
    //Create the groups for this page

    wxBoxSizer *pMainSizer = GetMainSizer();

	m_pGrpOctave = new lmGrpOctave(this, pMainSizer);
	m_pGrpVoice = new lmGrpVoiceStd(this, pMainSizer);
	m_pGrpNoteRest = new lmGrpNoteRest(this, pMainSizer);
    m_pGrpNoteDuration = new lmGrpNoteDuration(this, pMainSizer);
    m_pGrpNoteAcc = new lmGrpNoteAcc(this, pMainSizer);
    m_pGrpNoteDots = new lmGrpNoteDots(this, pMainSizer);
    m_pGrpTieTuplet = new lmGrpTieTuplet(this, pMainSizer);
    m_pGrpBeams = new lmGrpBeams(this, pMainSizer);
    AddGroup(m_pGrpOctave);
    AddGroup(m_pGrpVoice);
    AddGroup(m_pGrpNoteRest);
    AddGroup(m_pGrpNoteDuration);
    AddGroup(m_pGrpNoteAcc);
    AddGroup(m_pGrpNoteDots);
    AddGroup(m_pGrpTieTuplet);
    AddGroup(m_pGrpBeams);

	CreateLayout();

    //initialize info about selected group/tool
    m_nCurGroupID = lmGRP_NoteRest;
    m_nCurToolID = m_pGrpNoteRest->GetCurrentToolID();

    m_fGroupsCreated = true;
}

//-------------------------------------------------------------------------------------
// lmToolPageNotesHarmony implementation
//-------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(lmToolPageNotesHarmony, lmToolPageNotes)


lmToolPageNotesHarmony::lmToolPageNotesHarmony()
    : m_pMenu((wxMenu*)NULL)
{
}

lmToolPageNotesHarmony::lmToolPageNotesHarmony(wxWindow* parent)
{
    Create(parent);
}

void lmToolPageNotesHarmony::Create(wxWindow* parent)
{
    lmToolPageNotes::Create(parent);
    m_pMenu = (wxMenu*)NULL;
}

lmToolPageNotesHarmony::~lmToolPageNotesHarmony()
{
    if (m_pMenu)
        delete m_pMenu;
}

void lmToolPageNotesHarmony::CreateGroups()
{
    //Create the groups for this page

    wxBoxSizer *pMainSizer = GetMainSizer();

	m_pGrpOctave = new lmGrpOctave(this, pMainSizer);
	m_pGrpVoice = new lmGrpVoiceHarmony(this, pMainSizer);
	m_pGrpNoteRest = new lmGrpNoteRest(this, pMainSizer);
    m_pGrpNoteDuration = new lmGrpNoteDuration(this, pMainSizer);
    m_pGrpNoteAcc = new lmGrpNoteAcc(this, pMainSizer);
    m_pGrpNoteDots = new lmGrpNoteDots(this, pMainSizer);
    m_pGrpTieTuplet = new lmGrpTieTuplet(this, pMainSizer);
    m_pGrpBeams = new lmGrpBeams(this, pMainSizer);
    AddGroup(m_pGrpOctave);
    AddGroup(m_pGrpVoice);
    AddGroup(m_pGrpNoteRest);
    AddGroup(m_pGrpNoteDuration);
    AddGroup(m_pGrpNoteAcc);
    AddGroup(m_pGrpNoteDots);
    AddGroup(m_pGrpTieTuplet);
    AddGroup(m_pGrpBeams);

	CreateLayout();

    //initialize info about selected group/tool
    m_nCurGroupID = lmGRP_NoteRest;
    m_nCurToolID = m_pGrpNoteRest->GetCurrentToolID();

    m_fGroupsCreated = true;
}

wxMenu* lmToolPageNotesHarmony::GetContextualMenuForToolPage()
{
	if (m_pMenu)
        return m_pMenu;

	m_pMenu = new wxMenu();

	m_pMenu->Append(lmTOOL_VOICE_SOPRANO, _("&Soprano"));
	m_pMenu->Append(lmTOOL_VOICE_ALTO, _("&Alto"));
	m_pMenu->Append(lmTOOL_VOICE_TENOR, _("&Tenor"));
	m_pMenu->Append(lmTOOL_VOICE_BASS, _("Bass"));

	return m_pMenu;
}

void lmToolPageNotesHarmony::OnPopUpMenuEvent(wxCommandEvent& event)
{
    int nID = event.GetId();
    if (nID >= lmTOOL_VOICE_SOPRANO && nID <= lmTOOL_VOICE_BASS)
    {
        m_pGrpVoice->SelectButton(nID - lmTOOL_VOICE_SOPRANO);
        event.Skip();
    }
}

