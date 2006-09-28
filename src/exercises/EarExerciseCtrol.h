//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this 
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, 
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------
/*! @file EarExerciseCtrol.h
    @brief Header file for class lmEarExerciseCtrol
    @ingroup html_controls
*/

#ifndef __EAREXERCISECTROL_H__        //to avoid nested includes
#define __EAREXERCISECTROL_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ChordConstrains.h"
#include "../score/score.h"
#include "ScoreAuxCtrol.h"
#include "UrlAuxCtrol.h"
#include "CountersCtrol.h"


//--------------------------------------------------------------------------------
// Abstract class for ear exercises constrains
//--------------------------------------------------------------------------------
class lmEarConstrains
{
public:
    lmEarConstrains(wxString sSection);
    virtual ~lmEarConstrains() {}

    virtual void SaveSettings() {};

    void SetSettingsLink(bool fValue) { m_fSettingsLink = fValue; }
    bool IncludeSettingsLink() { return m_fSettingsLink; }

    void SetTheoryMode(bool fValue) { m_fTheoryMode = fValue; }
    bool IsTheoryMode() { return m_fTheoryMode; }


protected:
    virtual void LoadSettings() {};

    wxString    m_sSection;         // section name to save the constrains
    bool        m_fSettingsLink;    //include settings link

    //The Ctrol could be used both for ear training exercises and for theory exercises.
    //Following variables are used for configuration
    bool        m_fTheoryMode;


};


//--------------------------------------------------------------------------------
// Abstract class for ear exercises controls
//--------------------------------------------------------------------------------


class lmEarExerciseCtrol : public wxWindow    
{
   DECLARE_DYNAMIC_CLASS(lmEarExerciseCtrol)

public:

    // constructor and destructor    
    lmEarExerciseCtrol(wxWindow* parent, wxWindowID id,
               lmEarConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    virtual ~lmEarExerciseCtrol();

    // event handlers
    virtual void OnSize(wxSizeEvent& event);
    virtual void OnRespButton(wxCommandEvent& event);
    virtual void OnPlay(wxCommandEvent& event);
    virtual void OnNewProblem(wxCommandEvent& event);
    virtual void OnDisplaySolution(wxCommandEvent& event);
    virtual void OnSettingsButton(wxCommandEvent& event);

    // event handlers related to debugging
    virtual void OnDebugShowSourceScore(wxCommandEvent& event);
    virtual void OnDebugDumpScore(wxCommandEvent& event);
    virtual void OnDebugShowMidiEvents(wxCommandEvent& event);


protected:
    //virtual pure methods
    virtual void InitializeStrings() {};   
    virtual void CreateAnswerButtons()=0;
    virtual void ReconfigureButtons()=0;
    virtual wxString SetNewProblem()=0;    
    virtual wxDialog* GetSettingsDlg()=0;
    virtual void PrepareAuxScore(int nButton)=0;

    //methods that, normally, it is not necessary to implement
    virtual void SetButtonColor(int i, wxColour& color);
    virtual void EnableButtons(bool fEnable);
    virtual void Play();
    virtual void NewProblem();
    virtual void DisplaySolution();
    virtual void ResetExercise();
    virtual void StopSounds() {};

    void Create();
    void SetButtons(wxButton* pButton[], int nNumButtons, int nIdFirstButton);

        // member variables

    int                 m_nNumButtons;      //num answer buttons
    lmScore*            m_pProblemScore;    //score with the problem (= solution)
    lmScore*            m_pAuxScore;        //score to play user selected buttons 
    lmScoreAuxCtrol*    m_pScoreCtrol;
    lmCountersCtrol*    m_pCounters;
    wxBoxSizer*         m_pMainSizer;
    wxFlexGridSizer*    m_pKeyboardSizer;

    lmEarConstrains*    m_pConstrains;      //constrains for the exercise
    bool                m_fQuestionAsked;   //question asked but not yet answered
    int                 m_nRespIndex;       //index to the button with the right answer
    wxString            m_sAnswer;          //string with the right answer

    lmUrlAuxCtrol*      m_pPlayButton;      // "play" button
    lmUrlAuxCtrol*      m_pShowSolution;    // "show solution" button

    //lmEarExerciseCtrol can used both for ear training exercises and for theory exercises.
    //Following variable is used for configuration
    bool                m_fTheoryMode;

private:
    void DoStopSounds();

    wxButton**      m_pAnswerButtons;   //buttons for the answers
    int             m_nIdFirstButton;   //ID of first button; the others in sequence

    DECLARE_EVENT_TABLE()
};



#endif  // __EAREXERCISECTROL_H__