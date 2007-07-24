//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Cadence.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif


#include "Cadence.h"
#include "Conversion.h"
#include "../ldp_parser/AuxString.h"
#include "../exercises/Generators.h"
#include "../score/KeySignature.h"

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

// an entry for the table to convert harmonic function to chord intervals
typedef struct lmFunctionDataStruct {
    wxString    sFunction;      // harmonic function
    wxString    sChordMajor;    // nil if not usable
    wxString    sChordMinor1;   // alternative 1 or nil if not usable in minor mode
    wxString    sChordMinor2;   // alternative 2 or nil if no alternative
} lmFunctionData;


// Conversion table: harmonic function to chord intervals.
// Inversions are not necessary except in minor modes. In these cases the inversion
// is notated without the slash (IVm6, 
// AWARE: The maximum number of notes in a chord is defined in 'ChordManager.h', constant
//        lmNOTES_IN_CHORD. Currently its value is 6. Change this value if you need more
//        notes.

static const lmFunctionData m_aFunctionData[] = {
    //                                    minor key      minor key
    //Function        Major key           option 1       option 2
    //-----------    ------------------  -------------   --------------
    {_T("I"),       _T("M3,p5"),        _T("m3,p5"),    _T("nil") },
    {_T("II"),      _T("m3,p5"),        _T("m3,d5"),    _T("m3,p5") },
    {_T("IIm"),     _T("m3,p5"),        _T("nil"),      _T("nil") },
    {_T("III"),     _T("m3,p5"),        _T("M3,p5"),    _T("M3,a5") },
    {_T("IV"),      _T("M3,p5"),        _T("m3,p5"),    _T("M3,p5") },
    {_T("IVm"),     _T("m3,p5"),        _T("nil"),      _T("nil") },
    {_T("IVm6"),    _T("nil"),          _T("M3,M6"),    _T("nil") },    // IVm/6 used in Phrygian half cadence
    {_T("V"),       _T("M3,p5"),        _T("M3,p5"),    _T("m3,p5") },
    {_T("V7"),      _T("M3,p5,m7"),     _T("M3,p5,m7"), _T("nil") },
    {_T("Va5"),     _T("M3,a5"),        _T("nil"),      _T("nil") },
    {_T("Vd5"),     _T("M3,d5"),        _T("M3,d5"),    _T("m3,d5") },
    {_T("VI"),      _T("m3,p5"),        _T("M3,p5"),    _T("#,m3,d5") },
    {_T("VII"),     _T("m3,d5"),        _T("#,m3,d5"),  _T("M3,p5") },
    {_T("IIb6"),    _T("m3,m6"),        _T("m3,m6"),    _T("nil") },    // IIb/6 used in neapolitan sixth
	{_T("IId"),     _T("m3,d5"),		_T("nil"),      _T("nil") },	// IId used in augmented sixth
};

static wxString m_sCadenceName[lm_eCadMaxCadence+1];
static bool m_fStringsInitialized = false;


// AWARE: Array indexes are in correspondence with enum lmECadenceType
// AWARE: Change constant lmCHORDS_IN_CADENCE to increment number of chords
static const wxString aFunction[lm_eCadMaxCadence][lmCHORDS_IN_CADENCE] = {
    // Perfect authentic cadences
    { _T("V"),      _T("I") },      //Perfect_V_I
    { _T("V7"),     _T("I") },      //Perfect_V7_I
    { _T("Va5"),    _T("I") },      //Perfect_Va5_I
    { _T("Vd5"),    _T("I") },      //Perfect_Vd5_I
    // Plagal cadences
    { _T("IV"),     _T("I") },      //Plagal_IV_I
    { _T("IVm"),    _T("I") },      //Plagal_IVm_I
    { _T("II/6"),   _T("I") },      //Plagal_IIc6_I
    { _T("IIm/6"),  _T("I") },      //Plagal_IImc6_I
    // Imperfect authentic cadences
    // AWARE: This entry is not used. Inversions are encoded in aImperfect[]
	{ _T("V"),      _T("I") },      //Imperfect_V_I
    // Deceptive cadences
    { _T("V"),      _T("IV") },     //Deceptive_V_IV
    { _T("V"),      _T("IVm") },    //Deceptive_V_IVm
    { _T("V"),      _T("VI") },     //Deceptive_V_VI
    { _T("V"),      _T("VIm") },    //Deceptive_V_VIm
    { _T("V"),      _T("IIm") },    //Deceptive_V_IIm
    { _T("V"),      _T("III") },    //Deceptive_V_III
    { _T("V"),      _T("VII") },    //Deceptive_V_VII
    // Half cadences
    { _T("IIm/6"),  _T("V") },      //Half_IImc6_V
    { _T("IV"),     _T("V") },      //Half_IV_V
    { _T("I"),      _T("V") },      //Half_I_V
    { _T("I/64"),   _T("V") },      //Half_Ic64_V 
    { _T("IVm6"),   _T("V") },      //Half_IV6_V (Phrygian)
    { _T("II"),     _T("V") },      //Half_II_V
    { _T("IId/6"),  _T("V") },      //Half_IIdimc6_V (Neapolitan sixth)
    { _T("IIb6"),   _T("V") },      //Half_VdeVdim5c64_V (augmented sixth)
};

// Chords for imperfect cadence
static const wxString aImperfect[4][lmCHORDS_IN_CADENCE] = {
    { _T("V/6"),    _T("I") },      
    { _T("V/64"),   _T("I") },    
    { _T("V/64"),   _T("I/6") },   
    { _T("V"),      _T("I/64") }, 
};

//-------------------------------------------------------------------------------------
// Implementation of lmCadence class


lmCadence::lmCadence()
{
    m_fCreated = false;
    m_nNumChords = 0;
}

bool lmCadence::Create(lmECadenceType nCadenceType, EKeySignatures nKey, bool fUseGrandStaff)
{
    // return true if cadence created
    // if fUseGrandStaff is true chords will have 4 notes, with root note in 2/3 octaves

    //save parameters
    m_nType = nCadenceType;
    m_nKey = nKey;

    //get chords
    wxString sFunct;
    int iC;
    for (iC=0; iC < lmCHORDS_IN_CADENCE; iC++)
    {
        // get the function
        if (nCadenceType == lm_eCadImperfect_V_I)
            sFunct = aImperfect[lmRandomGenerator::RandomNumber(0, 3)][iC];
        else
            sFunct = aFunction[nCadenceType][iC];

        // if no function, exit loop. No more chords in cadence
        if (sFunct == _T("")) {
            if (iC > 1) break;      //no error
            //table maintenace error
            wxLogMessage(_T("[lmCadence::Create] No harmonic function!. nCadenceType=%d, iC=%d"),
                    nCadenceType, iC);
            return false;
        }

        //get the chord intervals
        wxString sIntervals = SelectChord(sFunct, nKey, &m_nInversions[iC]);
 /*dbg*/ wxLogMessage(_T("[lmCadence::Create] sFunct='%s', chord intervals='%s'"),
                sFunct.c_str(), sIntervals.c_str() );
        if (sIntervals == _T("")) {
            //error: no chord for choosen function
            wxLogMessage(_T("[lmCadence::Create] No chord found for nCadenceType=%d, nKey=%d, iC=%d"),
                    nCadenceType, nKey, iC);
            return false;
        }

        //Get root note for this key signature and clef
        wxString sRootNote = GetRootNote(sFunct, nKey, eclvSol, fUseGrandStaff);
        g_pLogger->LogTrace(_T("lmCadence"),
                _T("[lmCadence::Create] sFunc='%s', nKey=%d, sRootNote='%s'"),
				sFunct.c_str(), nKey, sRootNote.c_str());

       //Prepare the chord
        m_aChord[iC].Create(sRootNote, sIntervals, nKey, false);    //fUseGrandStaff);
        m_nNumChords++;
    }

    // generate first chord
    std::vector<lmHChord> aFirstChord;
    int nValidFirst = GenerateFirstChord(aFirstChord, &m_aChord[0], m_nInversions[0]);
    int nValidNext = 0;
    while (nValidNext == 0 && nValidFirst > 0) {
        // choose first chord: one at random
        int iSel = lmRandomGenerator::RandomNumber(1, nValidFirst);
        int nValid = 0;
        int iC;
        for (iC=0; iC < (int)aFirstChord.size(); iC++) {
            if (aFirstChord[iC].nReason == lm_eChordValid) {
                if (iSel == ++nValid) break;
            }
        }
        // chord iC is the selected one.
        m_Chord[0] = aFirstChord[iC];
        wxLogMessage(_T("[lmCadence::Create] Selected : %s, %s, %s, %s"),
                    aFirstChord[iC].GetPrintName(0).c_str(),
                    aFirstChord[iC].GetPrintName(1).c_str(),
                    aFirstChord[iC].GetPrintName(2).c_str(),
                    aFirstChord[iC].GetPrintName(3).c_str() );

        nValidNext = GenerateNextChord(&m_aChord[1], m_nInversions[1], 0);
        if (nValidNext == 0) {
            wxLogMessage(_T("[lmCadence::Create] Second chord invalid. New attempt"));
            // mark first chord as invalid
            aFirstChord[iC].nReason = lm_eChordDiscarded;
            nValidFirst--;
        }
    }

    m_fCreated = true;
    return true;
}

lmCadence::~lmCadence()
{
}

lmChordManager* lmCadence::GetChord(int iC)
{
    // returns a pointer to chord iC (0..n-1).
    // if iC is out of range returns NULL pointer

    if (iC >=0 && iC < m_nNumChords)
        return &m_aChord[iC];
    else
        return (lmChordManager*) NULL;
}


wxString lmCadence::SelectChord(wxString sFunction, EKeySignatures nKey, int* pInversion)
{
    // returns the intervals that form the chord, or empty string if errors
    // and updates variable pointed by pInversion to place the number of the
    // applicable inversion

    //Strip out inversions
    wxString sFunc;
    int iSlash = sFunction.find(_T('/'));
    if (iSlash != (int)wxStringBase::npos) {
        sFunc = sFunction.substr(0, iSlash);
        wxString sInv = sFunction.substr(iSlash+1);
        if (sInv==_T("6"))
            *pInversion = 1;
        else if (sInv==_T("64"))
            *pInversion = 2;
        else {
            wxLogMessage(_T("[lmCadence::SelectChord] Conversion table maintenance error. Unknown inversion code '%s'"), sInv.c_str());
            *pInversion = 0;
        }
    }
    else {
        sFunc = sFunction;
        *pInversion = 0;
    }

    // look for function
    int iF, iMax = sizeof(m_aFunctionData)/sizeof(lmFunctionData);
    for (iF=0; iF < iMax; iF++) {
        if (m_aFunctionData[iF].sFunction == sFunc) break;
    }
    if (iF == iMax) {
        // table maintenance error
        wxLogMessage(_T("[lmCadence::SelectChord] Conversion table maintenance error. Function '%s' not found. Key=%d"),
            sFunction.c_str(), nKey);
        return _T("");      // not valid chord
    }

    if (IsMajor(nKey))
    {
        // major key: return chord for major key
        wxString sChord = m_aFunctionData[iF].sChordMajor;
        if (sChord == _T("nil"))
            return _T("");        // not valid chord
        if (sChord == _T("?")) {
            wxLogMessage(_T("[lmCadence::SelectChord] Conversion table maintenance error. Undefined chord. Function '%s', Key=%d"),
                sFunction.c_str(), nKey);
            return _T("");        // not valid chord
        }
        else
            return sChord;
    }
    else
    {
        // minor key: return one of the chords for minor key
        wxString sChord1 = m_aFunctionData[iF].sChordMinor1;
        wxString sChord2 = m_aFunctionData[iF].sChordMinor2;
        if (sChord1 == _T("nil"))
            return _T("");        // not valid chord
        else if (sChord2 == _T("nil"))
            return sChord1;
        else {
            lmRandomGenerator oRnd;
            if (oRnd.FlipCoin())
                return sChord1;
            else
                return sChord2;
       }
    }
}

wxString lmCadence::GetRootNote(wxString sFunct, EKeySignatures nKey, EClefType nClef,
                                bool fUseGrandStaff)
{
    // TODO: Take clef into account

    //Get root note for this key signature
    int nRoot = GetRootNoteIndex(nKey);    //index (0..6, 0=Do, 1=Re, 3=Mi, ... , 6=Si) to root note

    // add function grade
    size_t nSize = sFunct.length();
    if (nSize > 2) {
        if (sFunct.compare(0, 3, _T("VII")) == 0)
            nRoot += 6;
        else if (sFunct.compare(0, 3, _T("III")) == 0)
            nRoot += 2;
        else if (sFunct.compare(0, 2, _T("VI")) == 0)
            nRoot += 5;
        else if (sFunct.compare(0, 2, _T("IV")) == 0)
            nRoot += 3;
        else if (sFunct.compare(0, 2, _T("II")) == 0)
            nRoot += 1;
        else if (sFunct.compare(0, 1, _T("V")) == 0)
            nRoot += 4;
    }
    else if (nSize > 1) {
        if (sFunct.compare(0, 2, _T("VI")) == 0)
            nRoot += 5;
        else if (sFunct.compare(0, 2, _T("IV")) == 0)
            nRoot += 3;
        else if (sFunct.compare(0, 2, _T("II")) == 0)
            nRoot += 1;
        else if (sFunct.compare(0, 1, _T("V")) == 0)
            nRoot += 4;
    }
    else if (sFunct.compare(0, 1, _T("V")) == 0)
        nRoot += 4;

    nRoot = nRoot % 7;

    //Get accidentals for this note
    int nAccidentals[7];
    ComputeAccidentals(nKey, nAccidentals);

    //convert accidentals to symbol +, -
    wxString sRootNote = _T("");
    if (nAccidentals[nRoot] > 0)
        sRootNote = _T("+");
    else if (nAccidentals[nRoot] < 0)
        sRootNote = _T("-");

    // convert to note name. Octave depends on clef and use of grand-staff
    wxString sNotes;
    if (fUseGrandStaff) {
        sNotes = _T("c3d3e3f3g2a2b2");
    }
    else {
        if (nClef == eclvSol)
            sNotes = _T("c4d4e4f4g4a4b4");
        else
            wxASSERT(false);
    }
    sRootNote += sNotes.substr(nRoot*2, 2);
    return sRootNote;

}

wxString lmCadence::GetName()
{
    //get the functions
    int iC;
	wxString sName = CadenceTypeToName(m_nType) + _T(" : ");
    for (iC=0; iC < lmCHORDS_IN_CADENCE; iC++)
    {
        wxString sFunct = aFunction[m_nType][iC];
        if (sFunct == _T("")) break;
		if (iC != 0) sName += _T(" -> ");
		sName += sFunct;
	}
	return sName;

}

//------------------------------------------------------------------------------------

int lmCadence::GenerateFirstChord(std::vector<lmHChord>& aChords, lmChordManager* pChord, int nInversion)
{
    // Generates all possible chords for the first chord for a cadence.
    // Returns the number of valid chords found.
    // Generated chords are placed in vector aChords. They are filtered and marked

    lmConverter oConv;
    #define lmMAX_NOTES 12      // max notes in a set for a voice: 3 octaves * 4 notes

    // Ranges allowed by most theorists for each voice. Both notes inclusive
    // Soprano: d4-g5,  Alto: g3-d5,  Tenor: d3-g4,  Bass: e2-d4
	// 	        e4-a5	      a3-d5			 f3-g4		   e2-c4
    lmDPitch nMinAltoPitch = oConv.DPitch(lmSTEP_G, 3);
    lmDPitch nMaxAltoPitch = oConv.DPitch(lmSTEP_D, 5);
    lmDPitch nMinTenorPitch = oConv.DPitch(lmSTEP_D, 3);
    lmDPitch nMaxTenorPitch = oConv.DPitch(lmSTEP_G, 4);


    // 1. Get chord note-steps for the required harmonic function.
    int nNumNotes = pChord->GetNumNotes();   // notes in chord
    lmNoteBits oChordNotes[4];
    int nSteps[4];
    for (int i=0; i < nNumNotes; i++) {
        oChordNotes[i].nAccidentals = pChord->GetAccidentals(i);
        oChordNotes[i].nStep = nSteps[i] = pChord->GetStep(i);
        oChordNotes[i].nOctave = 0;
    }
    int nStep5 = nSteps[2];     // the fifth of the chord
    //START DBG -------------------------------------------------------------
    wxLogMessage(_T("[lmCadence::GenerateFirstChord] Base chord. Num notes = %d"), nNumNotes);
    wxString sNotes = _T("cdefgabc");
    for (int i=0; i < nNumNotes; i++) {
        wxLogMessage(_T("[lmCadence::GenerateFirstChord] note %s"),
                sNotes.substr(oChordNotes[i].nStep, 1).c_str() );
    }
    //END DBG ---------------------------------------------------------------


    // 2. Generates the note for the bass voice, placed in the lower part of the
    // Bass staff (notes: g2,a2,b2,c3,d3,e3,f3).
    lmHChord oChord;
    int iB = nInversion;
    if (oChordNotes[iB].nStep > 3)        // if note step is g, a or b
        oChordNotes[iB].nOctave = 2;      //  place note in octave 2
    else                            // else
        oChordNotes[iB].nOctave = 3;      //  place note in octave 3
    oChord.nNote[0] = oConv.DPitch(oChordNotes[iB].nStep, oChordNotes[iB].nOctave);
    oChord.nAcc[0] = oChordNotes[iB].nAccidentals;
    wxLogMessage(_T("[lmCadence::GenerateFirstChord] Bass = %s"),
                    oChord.GetPrintName(0).c_str() );

    // 3. Generate note for soprano voice
    //    At random, any of the chord notes (except the bass one)
    int iS = GenerateSopranoNote(oChordNotes, iB, nNumNotes);
    oChord.nNote[3] = oConv.DPitch(oChordNotes[iS].nStep, oChordNotes[iS].nOctave);
    oChord.nAcc[3] = oChordNotes[iS].nAccidentals;
    wxLogMessage(_T("[lmCadence::GenerateFirstChord] Soprano = %s"),
                    oChord.GetPrintName(3).c_str() );

    // 4. Generate the set of possible notes for Alto voice.
    //    All chord notes possible
    int nAltoSet[lmMAX_NOTES];
    int nAltoSetAcc[lmMAX_NOTES];
    int iAS = 0;
    int iA;
    for (iA=0; iA < nNumNotes; iA++) {
        lmDPitch nAltoPitch = oConv.DPitch(oChordNotes[iA].nStep, 3);
        lmDPitch nSopranoPitch = oChord.nNote[3];
        while (nAltoPitch < nSopranoPitch && nAltoPitch < nMaxAltoPitch) {
            if (nAltoPitch >= nMinAltoPitch) {
                nAltoSetAcc[iAS] = oChordNotes[iA].nAccidentals;
                nAltoSet[iAS++] = nAltoPitch;
                wxLogMessage(_T("[lmCadence::GenerateFirstChord] Alto = %s"),
                            oConv.GetEnglishNoteName(nAltoPitch).c_str() );
            }
            nAltoPitch += 7;
        }
    }

    // 5. Generate the set of possible notes for Tenor voice.
    //    All chord notes possible
    int nTenorSet[lmMAX_NOTES];
    int nTenorSetAcc[lmMAX_NOTES];
    int iTS = 0;
    int iT;
    for (iT=0; iT < nNumNotes; iT++) {
        lmDPitch nTenorPitch = oConv.DPitch(oChordNotes[iT].nStep, 3);
        while (nTenorPitch < nMaxTenorPitch) {
            if (nTenorPitch >= nMinTenorPitch) {
                nTenorSetAcc[iTS] = oChordNotes[iT].nAccidentals;
                nTenorSet[iTS++] = nTenorPitch;
                wxLogMessage(_T("[lmCadence::GenerateFirstChord] Tenor = %s"),
                            oConv.GetEnglishNoteName(nTenorPitch).c_str() );
            }
            nTenorPitch += 7;
        }
    }

    // Generate the set of possible chords
    int nNumChords = iAS * iTS;
    aChords.resize(nNumChords);
    int iC = 0;
    for (int iA=0; iA < iAS; iA++) {
        for (int iT=0; iT < iTS; iT++) {
            // bass
            aChords[iC].nNote[0] = oChord.nNote[0];
            aChords[iC].nAcc[0] = oChord.nAcc[0];
            // soprano
            aChords[iC].nNote[3] = oChord.nNote[3];
            aChords[iC].nAcc[3] = oChord.nAcc[3];
            // alto
            aChords[iC].nNote[2] = nAltoSet[iA];
            aChords[iC].nAcc[2] = nAltoSetAcc[iA];
            // tenor
            aChords[iC].nNote[1] = nTenorSet[iT];
            aChords[iC].nAcc[1] = nTenorSetAcc[iT];
            //
            aChords[iC].nReason = lm_eChordValid;
            aChords[iC].nNumNotes = 4;
            iC++;
        }
    }
    aChords.resize(nNumChords);

    //START DBG -------------------------------------------------------------
    wxLogMessage(_T("[lmCadence::GenerateFirstChord] Num Chords = %d"), nNumChords);
    for (int i=0; i < nNumChords; i++) {
        wxLogMessage(_T("[lmCadence::GenerateFirstChord] Chord %d : %s, %s, %s, %s"),
                i,
                aChords[i].GetPrintName(0).c_str(),
                aChords[i].GetPrintName(1).c_str(),
                aChords[i].GetPrintName(2).c_str(),
                aChords[i].GetPrintName(3).c_str() );
    }
    //END DBG ---------------------------------------------------------------

    int nValidChords = FilterChords(aChords, nNumChords, nSteps, nNumNotes, nStep5,
                                    (lmHChord*)NULL);
    //START DBG -------------------------------------------------------------
    wxLogMessage(_T("[lmCadence::GenerateFirstChord] Valid Chords = %d"), nValidChords);
    for (int i=0; i < nNumChords; i++) {
        if (aChords[i].nReason == lm_eChordValid) {
            wxLogMessage(_T("[lmCadence::GenerateFirstChord] Valid chord %d : %s, %s, %s, %s"),
                i,
                aChords[i].GetPrintName(0).c_str(),
                aChords[i].GetPrintName(1).c_str(),
                aChords[i].GetPrintName(2).c_str(),
                aChords[i].GetPrintName(3).c_str() );
        }
    }
    //END DBG ---------------------------------------------------------------

    return nValidChords;

}

int lmCadence::GenerateNextChord(lmChordManager* pChord, int nInversion, int iPrevHChord)
{
    // Generates the next chord for a cadence.
    // returns the number of valid chords generated

    lmConverter oConv;
    int iThisChord = iPrevHChord + 1;

    // 1. Get chord notes (no octave) for the required harmonic function
    int nNumNotes = pChord->GetNumNotes();   // notes in chord
    int nSteps[4];
    lmNoteBits oChordNotes[4];
    for (int i=0; i < nNumNotes; i++) {
        oChordNotes[i].nAccidentals = pChord->GetAccidentals(i);
        oChordNotes[i].nStep = nSteps[i] = pChord->GetStep(i);
        oChordNotes[i].nOctave = 0;
    }
    int nStep5 = nSteps[2];     // the fifth of the chord
    //dbg
    wxLogMessage(_T("[lmCadence::GenerateNextChord] Base chord. Num notes = %d"), nNumNotes);
    wxString sNotes = _T("cdefgabc");
    for (int i=0; i < nNumNotes; i++) {
        wxLogMessage(_T("[lmCadence::GenerateNextChord] note %s"),
                sNotes.substr(oChordNotes[i].nStep, 1).c_str() );
    }


    // 2. Generates the note for the bass voice, placed in the lower part of the
    // Bass staff (notes: g2,a2,b2,c3,d3,e3,f3).
    int iB = nInversion;
    if (oChordNotes[iB].nStep > 3)        // if note step is g, a or b
        oChordNotes[iB].nOctave = 2;      //  place note in octave 2
    else                            // else
        oChordNotes[iB].nOctave = 3;      //  place note in octave 3
    lmDPitch nBassPitch = oConv.DPitch(oChordNotes[iB].nStep, oChordNotes[iB].nOctave);
    m_Chord[iThisChord].nNote[0] = nBassPitch;
    m_Chord[iThisChord].nAcc[0] = oChordNotes[iB].nAccidentals;
    wxLogMessage(_T("[lmCadence::GenerateNextChord] Bass = %s"),
                    oConv.GetEnglishNoteName(m_Chord[iThisChord].nNote[0]).c_str() );

    // generate the set of possible notes for each chord note
    int nSetNotes[20];
    int nSetAcc[20];
    int nSet = 0;       //points to first not used entry = number of entries
    lmDPitch nMaxPitch = oConv.DPitch(lmSTEP_C, 6);
    int iN;             // point to note in process
    for (iN=0; iN < nNumNotes; iN++) {
        lmDPitch nDPitch = oConv.DPitch(oChordNotes[iN].nStep, 2);
        if (nDPitch < nBassPitch) nDPitch += 7;
        while (nDPitch <= nMaxPitch) {
            nSetNotes[nSet] = nDPitch;
            nSetAcc[nSet++] = oChordNotes[iN].nAccidentals;
            //wxLogMessage(_T("[lmCadence::GenerateNextChord] Added to set = %s (%d)"),
            //            oConv.GetEnglishNoteName(nDPitch).c_str(), nDPitch );
            nDPitch += 7;
        }
    }

    // sort notes set, ascending. Bubble method
    for (int i = 0; i < nSet; i++) {
        bool fSwap = false;
        int j = nSet -1;      // last element
        while(j != i) {
            int k = j-1;
            if (nSetNotes[j] < nSetNotes[k]) {
                fSwap = true;
                int nNote = nSetNotes[j];
                int nAcc = nSetAcc[j];
                nSetNotes[j] = nSetNotes[k];
                nSetAcc[j] = nSetAcc[k];
                nSetNotes[k] = nNote;
                nSetAcc[k] = nAcc;
            }
            j = k;
        }
        if (!fSwap) break;
    }
    //for (int i=0; i < nSet; i++) {
    //    wxLogMessage(_T("[lmCadence::GenerateNextChord] ordered set: note %d = %s"),
    //            i, oConv.GetEnglishNoteName(nSetNotes[i]).c_str() );
    //}

    //for each previous chord note (except bass) find the nearest ones to move to
    //and for the sets of eligible notes.  To simplify, all arrays include the bass
    //voice (array index 0) but it is not used.
    lmDPitch nElegibleNote[4][5];       //four voices, five possible alternatives for each voice
    int nElegibleAcc[4][5];
    int nE[4];                          //number of found alternatives for each voice.

    for (iN = 1; iN < 4; iN++) {
        lmDPitch nNote = m_Chord[iPrevHChord].nNote[iN];
        int nAcc = m_Chord[iPrevHChord].nAcc[iN];
        nE[iN] = 0;     // no notes yet
        // find this note in set
        for(int i=0; i < nSet; i++) {
            if (nNote == nSetNotes[i]) {
                // note in chord. keep it.
                nElegibleNote[iN][nE[iN]] = nNote;
                nElegibleAcc[iN][nE[iN]++] = nAcc;

                // and add also the next and previous ones (two up and two down)
                if (i > 0) {
                    nElegibleNote[iN][nE[iN]] = nSetNotes[i-1];
                    nElegibleAcc[iN][nE[iN]++] = nSetAcc[i-1];
                }
                if (i > 1) {
                    nElegibleNote[iN][nE[iN]] = nSetNotes[i-2];
                    nElegibleAcc[iN][nE[iN]++] = nSetAcc[i-2];
                }
                if (i+1 < nSet) {
                    nElegibleNote[iN][nE[iN]] = nSetNotes[i+1];
                    nElegibleAcc[iN][nE[iN]++] = nSetAcc[i+1];
                }
                if (i+2 < nSet) {
                    nElegibleNote[iN][nE[iN]] = nSetNotes[i+2];
                    nElegibleAcc[iN][nE[iN]++] = nSetAcc[i+2];
                }
                break;
            }
            else if (nNote < nSetNotes[i]) {
                // note not in chord. Take the two nearest ones up and down
                if (i > 0) {
                    nElegibleNote[iN][nE[iN]] = nSetNotes[i-1];
                    nElegibleAcc[iN][nE[iN]++] = nSetAcc[i-1];
                }
                if (i > 1) {
                    nElegibleNote[iN][nE[iN]] = nSetNotes[i-2];
                    nElegibleAcc[iN][nE[iN]++] = nSetAcc[i-2];
                }
                nElegibleNote[iN][nE[iN]] = nSetNotes[i];
                nElegibleAcc[iN][nE[iN]++] = nSetAcc[i];
                if (i+1 < nSet) {
                    nElegibleNote[iN][nE[iN]] = nSetNotes[i+1];
                    nElegibleAcc[iN][nE[iN]++] = nSetAcc[i+1];
                }
                break;
            }
        }
    }
    //START DBG -------------------------------------------------------------
    wxLogMessage(_T("[lmCadence::GenerateNextChord] Elegible set S: Num.notes=%d, %s, %s, %s, %s, %s"),
        nE[3],
        oConv.GetEnglishNoteName(nElegibleNote[3][0]).c_str(),
        oConv.GetEnglishNoteName(nElegibleNote[3][1]).c_str(),
        oConv.GetEnglishNoteName(nElegibleNote[3][2]).c_str(),
        oConv.GetEnglishNoteName(nElegibleNote[3][3]).c_str(),
        oConv.GetEnglishNoteName(nElegibleNote[3][4]).c_str() );
    wxLogMessage(_T("[lmCadence::GenerateNextChord] Elegible set A: Num.notes=%d, %s, %s, %s, %s, %s"),
        nE[2],
        oConv.GetEnglishNoteName(nElegibleNote[2][0]).c_str(),
        oConv.GetEnglishNoteName(nElegibleNote[2][1]).c_str(),
        oConv.GetEnglishNoteName(nElegibleNote[2][2]).c_str(),
        oConv.GetEnglishNoteName(nElegibleNote[2][3]).c_str(),
        oConv.GetEnglishNoteName(nElegibleNote[2][4]).c_str() );
    wxLogMessage(_T("[lmCadence::GenerateNextChord] Elegible set T: Num.notes=%d, %s, %s, %s, %s, %s"),
        nE[1],
        oConv.GetEnglishNoteName(nElegibleNote[1][0]).c_str(),
        oConv.GetEnglishNoteName(nElegibleNote[1][1]).c_str(),
        oConv.GetEnglishNoteName(nElegibleNote[1][2]).c_str(),
        oConv.GetEnglishNoteName(nElegibleNote[1][3]).c_str(),
        oConv.GetEnglishNoteName(nElegibleNote[1][4]).c_str() );
    //END DBG ---------------------------------------------------------------

    // create the set of possible chords
    int nNumChords = nE[1] * nE[2] * nE[3];
    std::vector<lmHChord> aChords(nNumChords);
    int iC = 0;
    for (int iS=0; iS < nE[3]; iS++) {
        for (int iA=0; iA < nE[2]; iA++) {
            for (int iT=0; iT < nE[1]; iT++) {
                // bass
                aChords[iC].nNote[0] = m_Chord[iThisChord].nNote[0];
                aChords[iC].nAcc[0] = m_Chord[iThisChord].nAcc[0];
                // tenor
                aChords[iC].nNote[1] = nElegibleNote[1][iT];
                aChords[iC].nAcc[1] = nElegibleAcc[1][iT];

                if (aChords[iC].nNote[1] > aChords[iC].nNote[0]) { 
                    // alto
                    aChords[iC].nNote[2] = nElegibleNote[2][iA];
                    aChords[iC].nAcc[2] = nElegibleAcc[2][iA];

                    if (aChords[iC].nNote[2] > aChords[iC].nNote[1]) { 
                        // soprano
                        aChords[iC].nNote[3] = nElegibleNote[3][iS];
                        aChords[iC].nAcc[3] = nElegibleAcc[3][iS];

                        if (aChords[iC].nNote[3] > aChords[iC].nNote[2]) { 
                            aChords[iC].nReason = lm_eChordValid;
                            aChords[iC].nNumNotes = 4;
                            iC++;
                        }
                    }
                }
            }
        }
    }
    nNumChords = iC;

    //START DBG -------------------------------------------------------------
    wxLogMessage(_T("[lmCadence::GenerateNextChord] Num possible chords = %d"), nNumChords);
    for (int i=0; i < nNumChords; i++) {
        wxLogMessage(_T("[lmCadence::GenerateNextChord] Possible chord %d : %s, %s, %s, %s"),
                i,
                oConv.GetEnglishNoteName(aChords[i].nNote[0]).c_str(),
                oConv.GetEnglishNoteName(aChords[i].nNote[1]).c_str(),
                oConv.GetEnglishNoteName(aChords[i].nNote[2]).c_str(),
                oConv.GetEnglishNoteName(aChords[i].nNote[3]).c_str() );
    }
    //END DBG ---------------------------------------------------------------

    //Filter invalid chords
    int nValidChords = FilterChords(aChords, nNumChords, nSteps, nNumNotes, nStep5,
                                    &m_Chord[iPrevHChord]);
    //START DBG -------------------------------------------------------------
    wxLogMessage(_T("[lmCadence::GenerateNextChord] Valid Chords = %d"), nValidChords);
    for (int i=0; i < nNumChords; i++) {
        if (aChords[i].nReason == lm_eChordValid) {
            wxLogMessage(_T("[lmCadence::GenerateNextChord] Valid chord %d : %s, %s, %s, %s"),
                i,
                oConv.GetEnglishNoteName(aChords[i].nNote[0]).c_str(),
                oConv.GetEnglishNoteName(aChords[i].nNote[1]).c_str(),
                oConv.GetEnglishNoteName(aChords[i].nNote[2]).c_str(),
                oConv.GetEnglishNoteName(aChords[i].nNote[3]).c_str() );
        }
        else {
            wxLogMessage(_T("[lmCadence::GenerateNextChord] Invalid chord %d : %s, %s, %s, %s - %s"),
                i,
                oConv.GetEnglishNoteName(aChords[i].nNote[0]).c_str(),
                oConv.GetEnglishNoteName(aChords[i].nNote[1]).c_str(),
                oConv.GetEnglishNoteName(aChords[i].nNote[2]).c_str(),
                oConv.GetEnglishNoteName(aChords[i].nNote[3]).c_str(),
                aChords[i].GetReason() );
        }
    }
    //END DBG ---------------------------------------------------------------

    if (nValidChords < 1) return nValidChords;
    // choose one at random
    lmRandomGenerator oRnd;
    int iSel = oRnd.RandomNumber(1, nValidChords);
    int nValid = 0;
    for (iC=0; iC < nNumChords; iC++) {
        if (aChords[iC].nReason == lm_eChordValid) {
            if (iSel == ++nValid) break;
        }
    }
    // chord iC is the selected one.
    m_Chord[iThisChord] = aChords[iC];
    //START DBG -------------------------------------------------------------
    wxLogMessage(_T("[lmCadence::GenerateNextChord] Selected : %s, %s, %s, %s"),
        oConv.GetEnglishNoteName(aChords[iC].nNote[0]).c_str(),
        oConv.GetEnglishNoteName(aChords[iC].nNote[1]).c_str(),
        oConv.GetEnglishNoteName(aChords[iC].nNote[2]).c_str(),
        oConv.GetEnglishNoteName(aChords[iC].nNote[3]).c_str() );
    //END DBG ---------------------------------------------------------------

    return nValidChords;
}


int lmCadence::GenerateSopranoNote(lmNoteBits oChordNotes[4], int iBass,
                                          int nNumNotes)
{
    // Generates a note for the Soprano voice, located in the upper part of the
    // upper staff (notes: g4,a4,b4,c5,d5,e5,f5).

    // Algorithm: choose at random one of the chord notes (except the bass one),
    // and place it on the allowed notes range (notes: g4,a4,b4,c5,d5,e5,f5)


    // 1. choose at random one of the chord notes (except the bass one)
    lmRandomGenerator oRnd;
    int nWatchDog = 0;
    int iS = iBass;
    while (iS == iBass) {
        iS = oRnd.RandomNumber(0, nNumNotes-1);
        if (nWatchDog++ == 1000) {
            wxLogMessage(_T("Program error: Loop detected in lmCadence::GenerateSopranoNote"));
            return iBass;   // at least, this one exists!
        }
    }

    // 2. place note on the allowed notes range (notes: g4,a4,b4,c5,d5,e5,f5)
    if (oChordNotes[iS].nStep > 3)        // if note step is g, a or b
        oChordNotes[iS].nOctave = 4;      //  place note in octave 4
    else                            // else
        oChordNotes[iS].nOctave = 5;      //  place note in octave 5

    return iS;

}

int lmCadence::FilterChords(std::vector<lmHChord>& aChords, int nNumChords, int nSteps[4],
                            int nNumSteps, int nStep5, lmHChord* pPrevChord)
{
    // Parameters:
    //  - nStep5 is the fifth of the chord
    //
    // Validations:
    // + 1. B < T < A < S
    // + 2. A <= T+7; S <= A+7
    // + 3. The chord is complete (has all note steps)
    // + 4. The fifth is not doubled
    // + 5. No parallel motion of perfect octaves, perfect fifths, and unisons
    // + 6. Voice overlap: when a voice moves above or below a pitch previously sounded by another voice.
    // + 7. If bass moves by step all other voices moves in opposite direction to bass
    // + 8. Scale degree seven (the leading tone) should resolve to tonic.
	// + 9. The leading tone is never doubled
	// - Stay within the ranges of the voices.
    // - the seventh of a chord should always resolve down by second.
    // - Resolve chromatic alterations by step in the same direction than the alteration.


    //locate leading tone in previous chord
    int iLeading = -1;      // index to leading note in previous chord or -1 if none
    int nRoot = GetRootNoteIndex(m_nKey);    //index (0..6, 0=Do, 1=Re, 3=Mi, ... , 6=Si) to root note
    int nStepLeading = (nRoot == 0 ? 6 : nRoot-1);      //scale degree seven
    if (pPrevChord) {
        //Check if leading tone is present in previous chord
        for (int iN=0; iN < 4; iN++) {
            if (lmConverter::GetStepFromDPitch(pPrevChord->nNote[iN]) == nStepLeading) {
                iLeading = iN;
                break;
            }
        }
    }

    //locate chromatic alterations (accidentals not in key signature)
    int nAlter[4] = {0,0,0,0};
    if (pPrevChord) {
        int nKeyAccidentals[6];
        ComputeAccidentals(m_nKey, nKeyAccidentals);
        for (int iN=0; iN < 4; iN++) {
            int nStep = lmConverter::GetStepFromDPitch(pPrevChord->nNote[iN]);
            nAlter[iN] = pPrevChord->nAcc[iN] - nKeyAccidentals[nStep];
        }
    }



    for (int i=0; i < nNumChords; i++)
    {
        //1. notes in ascending sequence (not to allow voices crossing). No duplicates
        if (aChords[i].nReason == lm_eChordValid &&
            (aChords[i].nNote[1] <= aChords[i].nNote[0] ||
             aChords[i].nNote[2] <= aChords[i].nNote[1] ||
             aChords[i].nNote[3] <= aChords[i].nNote[2] ))
                aChords[i].nReason = lm_eVoiceCrossing;

        //2. notes interval not greater than one octave (except bass-tenor)
        if (aChords[i].nReason == lm_eChordValid &&
            (aChords[i].nNote[2] - aChords[i].nNote[1] > 7 ||
             aChords[i].nNote[3] - aChords[i].nNote[2] > 7 ))
                aChords[i].nReason = lm_eGreaterThanOctave;

        //3. all steps are in the chord
        if (aChords[i].nReason == lm_eChordValid) {
            bool fFound[4] = { false, false, false, false };
            for (int iN=0; iN < 4; iN++) {
                for(int j=0; j < nNumSteps; j++) {
                    if (lmConverter::GetStepFromDPitch(aChords[i].nNote[iN]) == nSteps[j]) {
                        fFound[j] = true;
                        break;
                    }
                }
            }
            bool fValid = true;
            for(int j=0; j < nNumSteps; j++) {
                fValid &= fFound[j];
            }
            if (!fValid)
                aChords[i].nReason = lm_eChordIncomplete;
        }

        //4. The fifth is not doubled
        //9. The leading tone is never doubled
        if (aChords[i].nReason == lm_eChordValid) {
            int nFifths = 0;
            int nLeadingTones = 0;
            for (int iN=0; iN < 4; iN++) {
                int nStep = lmConverter::GetStepFromDPitch(aChords[i].nNote[iN]);
                if (nStep == nStep5)
                    nFifths++;
                else if (nStep == nStepLeading)
                    nLeadingTones++;
            }
            if (nFifths > 1)
                aChords[i].nReason = lm_eFifthDoubled;
            if (nLeadingTones > 1)
                aChords[i].nReason = lm_eLeadingToneDoubled;
        }

        //5. No parallel motion of perfect octaves or perfect fifths
        int aMotion[4];
        if (pPrevChord && aChords[i].nReason == lm_eChordValid) {
            int nUpFifth = 0;
            int nUpOctave = 0;
            int nDownFifth = 0;
            int nDownOctave = 0;
            for (int iN=0; iN < 4; iN++) {
                int nMotion = aChords[i].nNote[iN] - pPrevChord->nNote[iN];
                if (nMotion != 0) {
                    if (nMotion > 0)
                        nMotion++;
                    else
                        nMotion--;
                }
                if (nMotion == 5) 
                    nUpFifth++;
                else if (nMotion == 8) 
                    nUpOctave++;
                else if (nMotion == -5) 
                    nDownFifth++;
                else if (nMotion == -8) 
                    nDownOctave++;

                aMotion[iN] = nMotion;
            }
            if (nUpFifth > 1 || nUpOctave > 1 || nDownFifth > 1 || nDownOctave > 1)
                aChords[i].nReason = lm_eFifthOctaveMotion;
        }

        //6. No voice overlap
        if (pPrevChord && aChords[i].nReason == lm_eChordValid) {
            bool fUpMotion;
            for (int iN=0; iN < 4; iN++) {
                if (aChords[i].nNote[iN] != pPrevChord->nNote[iN]) {
                    fUpMotion = (aChords[i].nNote[iN] > pPrevChord->nNote[iN]);
                    if (iN < 3 && fUpMotion && aChords[i].nNote[iN] > pPrevChord->nNote[iN+1]) {
                        aChords[i].nReason = lm_eVoiceOverlap;
                        break;
                    }
                    if (iN > 0 && !fUpMotion && aChords[i].nNote[iN] < pPrevChord->nNote[iN-1]) {
                        aChords[i].nReason = lm_eVoiceOverlap;
                        break;
                    }
                }
            }
        }

        // 7. If bass moves by step all other voices moves in opposite direction to bass
        if (pPrevChord && aChords[i].nReason == lm_eChordValid) {
            if (aMotion[0] == 1) {
                // bass motion: up
                for (int iN=1; iN < 4; iN++) {
                    if (aMotion[iN] > 0) {
                        aChords[i].nReason = lm_eNotContraryMotion;
                        break;
                    }
                }
            }
            if (aMotion[0] == -1) {
                // bass motion: down
                for (int iN=1; iN < 4; iN++) {
                    if (aMotion[iN] < 0) {
                        aChords[i].nReason = lm_eNotContraryMotion;
                        break;
                    }
                }
            }
        }

        //8. Scale degree seven (the leading tone) should resolve to tonic.
        if (pPrevChord && iLeading >=0 && aChords[i].nReason == lm_eChordValid) {
            // leading tone should move up by step
            if (aMotion[iLeading] != 2) {
                aChords[i].nReason = lm_eLeadingResolution;
            }
        }

        //9. Resolve chromatic alterations [by step] in the same direction than the alteration.
        if (pPrevChord && aChords[i].nReason == lm_eChordValid) {
            for (int iN=0; iN < 4; iN++) {
                if (aChords[i].nNote[iN] != pPrevChord->nNote[iN]) {
                    if (((aChords[i].nNote[iN] > pPrevChord->nNote[iN]) && nAlter[i] < 0) ||
                        ((aChords[i].nNote[iN] < pPrevChord->nNote[iN]) && nAlter[i] > 0) )
                    {
                        aChords[i].nReason = lm_eChromaticAlter;
                        break;
                    }
                }
            }
        }


    }

    // count valid chords
    int nValid = 0;
    for (int i=0; i < nNumChords; i++)
        if (aChords[i].nReason == lm_eChordValid) nValid++;

    return nValid;
}

wxString lmCadence::GetNotePattern(int iChord, int iNote)
{
    lmNoteBits oNB;
    lmConverter::DPitchToBits(m_Chord[iChord].nNote[iNote], &oNB);
    oNB.nAccidentals = m_Chord[iChord].nAcc[iNote];
    return lmConverter::NoteBitsToName(oNB, m_nKey);
}


//----------------------------------------------------------------------------------------
//global functions
//----------------------------------------------------------------------------------------

wxString CadenceTypeToName(lmECadenceType nType)
{
    wxASSERT(nType <= lm_eCadMaxCadence);

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    if (!m_fStringsInitialized)
    {
	    // Terminal cadences
		    // Perfect authentic cadences
        m_sCadenceName[lm_eCadPerfect_V_I] = _("Perfect");
        m_sCadenceName[lm_eCadPerfect_V7_I] = _("Perfect");
        m_sCadenceName[lm_eCadPerfect_Va5_I] = _("Perfect");
        m_sCadenceName[lm_eCadPerfect_Vd5_I] = _("Perfect");
			// Plagal cadences
        m_sCadenceName[lm_eCadPlagal_IV_I] = _("Plagal");
        m_sCadenceName[lm_eCadPlagal_IVm_I] = _("Plagal");
        m_sCadenceName[lm_eCadPlagal_IIc6_I] = _("Plagal");
        m_sCadenceName[lm_eCadPlagal_IImc6_I] = _("Plagal");

		// Transient cadences
			// Imperfect authentic cadences
	    m_sCadenceName[lm_eCadImperfect_V_I] = _("Imperfect authentic");
			// Deceptive cadences
        m_sCadenceName[lm_eCadDeceptive_V_IV] = _("Deceptive");
        m_sCadenceName[lm_eCadDeceptive_V_IVm] = _("Deceptive");
        m_sCadenceName[lm_eCadDeceptive_V_VI] = _("Deceptive");
        m_sCadenceName[lm_eCadDeceptive_V_VIm] = _("Deceptive");
        m_sCadenceName[lm_eCadDeceptive_V_IIm] = _("Deceptive");
        m_sCadenceName[lm_eCadDeceptive_V_III] = _("Deceptive");
        m_sCadenceName[lm_eCadDeceptive_V_VII] = _("Deceptive");
			// Half cadences
        m_sCadenceName[lm_eCadHalf_IImc6_V] = _("Half cadence");
        m_sCadenceName[lm_eCadHalf_IV_V] = _("Half cadence");
        m_sCadenceName[lm_eCadHalf_I_V] = _("Half cadence");
        m_sCadenceName[lm_eCadHalf_Ic64_V] = _("Half cadence");
        m_sCadenceName[lm_eCadHalf_IV6_V] = _("Half cadence");
        m_sCadenceName[lm_eCadHalf_II_V] = _("Half cadence");
        m_sCadenceName[lm_eCadHalf_IIdimc6_V] = _("Half cadence");
        m_sCadenceName[lm_eCadHalf_VdeVdim5c64_V] = _("Half cadence");
        m_sCadenceName[lm_eCadLastHalf] = _("Half cadence");

        m_fStringsInitialized = true;
    }

    return m_sCadenceName[nType];

}

//int NumNotesInScale(lmECadenceType nType)
//{
//    wxASSERT(nType < est_Max);
//    return 1 + ((tData[nType].sIntervals).Length() / 3);
//}
