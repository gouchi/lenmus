//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
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

#ifndef __LENMUS_IDFY_CADENCES_CTROL_H__        //to avoid nested includes
#define __LENMUS_IDFY_CADENCES_CTROL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_ctrol.h"
#include "lenmus_cadence.h"

//lomse
#include <lomse_pitch.h>
using namespace lomse;


namespace lenmus
{
class CadencesConstrains;


//---------------------------------------------------------------------------------------
class IdfyCadencesCtrol : public OneScoreCtrol
{
public:

    // constructor and destructor
    IdfyCadencesCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);

    ~IdfyCadencesCtrol();

    //implementation of virtual pure in parent EBookCtrol
    void get_ctrol_options_from_params();
    void set_problem_space();

    //implementation of virtual methods
    void initialize_strings();
    void initialize_ctrol();
    void create_answer_buttons(LUnits height, LUnits spacing);
    ImoScore* prepare_aux_score(int nButton);
    wxString set_new_problem();
    wxDialog* get_settings_dialog();
    void on_settings_changed();
    string get_initial_msge();
    bool are_answer_buttons_allowed_for_playing() { return false; }

private:
    wxString prepare_score(EClef nClef, ECadenceType nType, ImoScore** pProblemScore,
                          ImoScore** pSolutionScore = NULL );
    int DisplayButton(int iBt, ECadenceType iStartC, ECadenceType iEndC, wxString sButtonLabel);

        // member variables

    enum {
        k_num_cols = 5,
        k_num_rows = 1,
        k_num_buttons = 5,     // NUM_COLS * NUM_ROWS;
    };

    CadencesConstrains* m_pConstrains;       //constraints for the exercise

    //problem asked
    EKeySignature  m_nKey;

    //answer
    ButtonCtrl*   m_pAnswerButton[k_num_buttons];   //buttons for the answers
    //cadence that corresponds to each valid button
    ECadenceType  m_nStartCadence[k_num_buttons];
    ECadenceType  m_nEndCadence[k_num_buttons];

    //wxDECLARE_EVENT_TABLE();
};


}   // namespace lenmus

#endif  // __LENMUS_IDFY_CADENCES_CTROL_H__
