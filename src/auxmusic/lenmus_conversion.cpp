//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma implementation "lenmus_conversion.h"
//#endif
//
//// For compilers that support precompilation, includes <wx.h>.
//#include <wx/wxprec.h>
//
//#ifdef __BORLANDC__
//#pragma hdrstop
//#endif
//
//#include "../score/Score.h"
//#include "lenmus_conversion.h"
//
//
//namespace lenmus
//{
//
//
//lmConverter::lmConverter()
//{
//}
//
//
////---------------------------------------------------------------------------------------
//// Notes
////---------------------------------------------------------------------------------------
//
//bool lmConverter::NoteToBits(wxString sNote, NoteBits* pBits)
//{
//    //Returns true if error
//    //- sNote must be letter followed by a number (i.e.: "c4" ) optionally precedeed by
//    //  accidentals (i.e.: "++c4")
//    //- It is assumed that sNote is trimmed (no spaces before or after data)
//    //  and lower case.
//
//    //split the string: accidentals and name
//    wxString sAccidentals;
//    switch (sNote.length()) {
//        case 2:
//            sAccidentals = "";
//            break;
//        case 3:
//            sAccidentals = sNote.substr(0, 1);
//            sNote = sNote.substr(1, 2);
//            break;
//        case 4:
//            sAccidentals = sNote.substr(0, 2);
//            sNote = sNote.substr(2, 2);
//            break;
//        default:
//            return true;   //error
//    }
//
//    //compute step
//    int nStep = StepToInt( sNote.Left(1) );
//    if (nStep == -1) return true;   //error
//    pBits->nStep = nStep;
//
//    //compute octave
//    wxString sOctave = sNote.substr(1, 1);
//    long nOctave;
//    if (!sOctave.ToLong(&nOctave)) return true;    //error
//    pBits->nOctave = (int)nOctave;
//
//    //compute accidentals
//    int nAccidentals = AccidentalsToInt(sAccidentals);
//    if (nAccidentals == -999) return true;  //error
//    pBits->nAccidentals = nAccidentals;
//
//    //compute step semitones
//    pBits->nStepSemitones = StepToSemitones( pBits->nStep );
//
//    return false;  //no error
//
//}
//
//bool lmConverter::DPitchToBits(DiatonicPitch nPitch, NoteBits* pBits)
//{
//    //Returns true if error
//    //Accidentals are set to 'none' (zero)
//
//    //compute step
//    pBits->nStep = DPitch_Step(nPitch);
//
//    //compute octave
//    pBits->nOctave = DPitch_Octave(nPitch);
//
//    //compute accidentals
//    pBits->nAccidentals = 0;
//
//    //compute step semitones
//    pBits->nStepSemitones = StepToSemitones( pBits->nStep );
//
//    return false;  //no error
//
//}
//
//
//wxString lmConverter::NoteBitsToName(NoteBits& tBits, EKeySignature nKey)
//{
//    static wxString m_sSteps = "cdefgab";
//
//    // Get the accidentals implied by the key signature.
//    // Each element of the array refers to one note: 0=Do, 1=Re, 2=Mi, 3=Fa, ... , 6=Si
//    // and its value can be one of: 0=no accidental, -1 = a flat, 1 = a sharp
//    int nAccidentals[7];
//    lmComputeAccidentals(nKey, nAccidentals);
//
//    //compute accidentals note accidentals
//    wxString sResult = "";
//    if (tBits.nAccidentals == 1)
//        sResult = "+";
//    else if (tBits.nAccidentals == 2)
//        sResult = "x";
//    else if (tBits.nAccidentals == -1)
//        sResult = "-";
//    else if (tBits.nAccidentals == -2)
//        sResult = "--";
//
//    //change note accidentals to take key into account
//    if (nAccidentals[tBits.nStep] != 0) {
//        if (tBits.nAccidentals == nAccidentals[tBits.nStep])
//            sResult = "";   //replace note accidental by key accidental
//        else if (tBits.nAccidentals == 0)
//            sResult = "=";  //force a natural
//        //else
//            //leave note accidentals
//    }
//
//    // compute step letter
//    sResult += m_sSteps.substr(tBits.nStep, 1);
//
//    // compute octave
//    sResult += wxString::Format("%d", tBits.nOctave);
//
//    return sResult;
//
//}
//
//int lmConverter::StepToSemitones(int nStep)
//{
//    //Returns -1 if error
//    // 'c'=0, 'd'=2, 'e'=4, 'f'=5, 'g'=7, 'a'=9, 'b'=11
//    if (nStep == 0)  return 0;
//    if (nStep == 1)  return 2;
//    if (nStep == 2)  return 4;
//    if (nStep == 3)  return 5;
//    if (nStep == 4)  return 7;
//    if (nStep == 5)  return 9;
//    if (nStep == 6)  return 11;
//    return -1;  //error
//}
//
//int lmConverter::AccidentalsToInt(wxString sAccidentals)
//{
//    //Returns -999 if error
//    // '--'=-1, '-'=-1, ''=0, '+'=+1, '++'=+2 'x'=2
//    if (sAccidentals == "")     return 0;
//    if (sAccidentals == "-")    return -1;
//    if (sAccidentals == "--")   return -2;
//    if (sAccidentals == "+")    return 1;
//    if (sAccidentals == "++")   return 2;
//    if (sAccidentals == "x")    return 2;
//    return -999;
//}
//
//int lmConverter::StepToInt(wxString sStep)
//{
//    //Returns -1 if error
//    // 'c'=0, 'd'=1, 'e'=2, 'f'=3, 'g'=4, 'a'=5, 'b'=6
//    if (sStep == "c") return 0;
//    if (sStep == "d") return 1;
//    if (sStep == "e") return 2;
//    if (sStep == "f") return 3;
//    if (sStep == "g") return 4;
//    if (sStep == "a") return 5;
//    if (sStep == "b") return 6;
//    return -1;
//}
//
//
//}   //namespace lenmus
