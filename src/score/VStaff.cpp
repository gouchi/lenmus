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

//--------------------------------------------------------------------------------------------------
// class lmVStaff: A generalization of the 'staff' concept.
//
//    The object lmVStaff (Virtual staff) is a generalization of the concept 'staff': it is a staff
//    with an great number of lines and spaces (almost infinite) so to represent all posible pitches,
//    but whose visual renderization can be controlled so
//    that only the specified (needed) lines are rendered i.e: the standard five-lines staff, the piano
//    grand staff, the single line staff used for rithm or unpitched music, or any other design you
//    would like.
//
//    An lmVStaff can contain many 'staffs' (groups of lines rendered toghether), depending on the layout
//    choosen. For example:
//    - the standard five-lines staff is an lmVStaff with one five-lines staff.
//    - the grand staff is an lmVStaff with two five-lines staves.
//    - the single line unpitched staff is an VSatff with one sigle line staff.
//
//    The concept of 'staff' is not modelled into the program, its is just a term to be used only
//    in documentation, algoritms explanations, etc.
//
//    An lmVStaff is composed by objects such as notes, rests, barlines, keys, and all other
//    musical signs traditionally used for writing music.
//    All this objects than can appear in a staff will be modeled by an abstract class 'lmStaffObj'.
//    Therefore an lmVStaff is, roughfly, a collection of StaffObjs and some attributes.
//
//    Bars are not modelled by objects. They are just the collection of lmStaffObj found between
//    two lmStaffObj of type 'barline' (and between the start of the score and the first barline).
//    Nevertheless, tha concept of bar is very important at least in two situations:
//        1. In interpretation (playing back the score). For example:  play from bar #7
//        2. When rendering the score, as all the objects in a bar must be rendered together in
//           the same paper line.
//    Due to this, althoug the bar is not modelled as an object, there exits methods in the
//    VStaff object to deal with bars.
//
//-------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Modelo
//   - Todos los StaffObjs que componen el pentagrama están en la colección m_cStaffObjs, ordenados por
//   orden de creación
//   - Cada staffobj:
//       - mantiene el núm del compas al que pertenece. Si se inserta o se borra un compas
//           hay que renumerar
//   - La colección m_cPoInicioCompas contiene un puntero al primer staffobj de cada compas
//
//   - THINK: Dado un Pentobj debería ser inmediato localizarle dentro de la colección. Para ello
//   en cada staffobj debería guardarse su índice en la colección. Ello obliga a realmacenar índices
//   tras una inserción o un borrado. La alternativa actual es recorrer secuencialmente la
//   colección. Ver método CIterador.PosicionarEnItem
//--------------------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "VStaff.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Score.h"
#include "Notation.h"
#include "../app/global.h"
#include "MetronomeMark.h"
#include "../graphic/GMObject.h"
#include "../graphic/ShapeStaff.h"
#include "../graphic/BoxSliceVStaff.h"

//implementation of the staves List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(VStavesList);


//constructor
lmVStaff::lmVStaff(lmScore* pScore, lmInstrument* pInstr, bool fOverlayered)
    : lmObject(pScore)
{
    //pScore is the lmScore to which this vstaff belongs.
    //Initially the lmVStaff will have only one standard five-lines staff. This can be
    //later modified invoking the 'AddStaff' method (REVIEW)

    m_pScore = pScore;
    m_pInstrument = pInstr;
    m_fOverlayered = fOverlayered;

    // default lmVStaff margins (logical units = tenths of mm)
    m_nHeight = 0;          //a value of 0 means 'compute it'
    m_leftMargin = 0;
    m_topMargin = 0;
    m_rightMargin = 0;
    m_bottomMargin = lmToLogicalUnits(1, lmCENTIMETERS);    // 1 cm

    //create one standard staff (five lines, 7.2 mm height)
    lmStaff* pStaff = new lmStaff(pScore);
    m_cStaves.Append(pStaff);

    //default value
    //! @todo review this fixed space before the clef
    m_nSpaceBeforeClef = TenthsToLogical(10, 1);    // one line of first staff

    g_pLastNoteRest = (lmNoteRest*)NULL;

}

lmVStaff::~lmVStaff()
{
    m_cStaves.DeleteContents(true);
    m_cStaves.Clear();
}

lmStaff* lmVStaff::AddStaff(int nNumLines, lmLUnits nMicrons)
{
    lmStaff* pStaff = new lmStaff(m_pScore, nNumLines, nMicrons);
    m_cStaves.Append(pStaff);

    return pStaff;

}

lmStaff* lmVStaff::GetStaff(int nStaff)
{
    //iterate over the collection of Staves (lmStaff Objects) to locate staff nStaff
    int iS;
    StaffList::Node* pNode = m_cStaves.GetFirst();
    for (iS=1 ; iS != nStaff && pNode; iS++ ) {
        pNode = pNode->GetNext();    //get next lmStaff
    }
    wxASSERT(pNode);
    return (lmStaff *)pNode->GetData();

}

lmStaff* lmVStaff::GetFirstStaff()
{
    m_pStaffNode = m_cStaves.GetFirst();
    return (m_pStaffNode ? (lmStaff *)m_pStaffNode->GetData() : (lmStaff *)m_pStaffNode);
}

lmStaff* lmVStaff::GetNextStaff()
{
    wxASSERT(m_pStaffNode);
    m_pStaffNode = m_pStaffNode->GetNext();
    return (m_pStaffNode ? (lmStaff *)m_pStaffNode->GetData() : (lmStaff *)m_pStaffNode);
}

lmStaff* lmVStaff::GetLastStaff()
{
    wxASSERT(m_pStaffNode);
    m_pStaffNode = m_cStaves.GetLast();
    return (m_pStaffNode ? (lmStaff *)m_pStaffNode->GetData() : (lmStaff *)m_pStaffNode);
}

lmLUnits lmVStaff::TenthsToLogical(lmTenths nTenths, int nStaff)
{
    lmStaff* pStaff = GetStaff(nStaff);
    wxASSERT(pStaff);
    return pStaff->TenthsToLogical(nTenths);

}

lmLUnits lmVStaff::GetStaffLineThick(int nStaff)
{
    lmStaff* pStaff = GetStaff(nStaff);
    wxASSERT(pStaff);
    return pStaff->GetLineThick();

}

void lmVStaff::UpdateContext(lmNote* pStartNote, int nStaff, int nStep,
                           int nNewAccidentals, lmContext* pCurrentContext)
{
    /*
    Note pStartNote (whose diatonic name is nStep) has accidentals that must be propagated to
    the context and to the following notes until the end of the measure or until a new accidental
    for the same step is found
    */


    //create a new context by updating current one
    lmStaff* pStaff = GetStaff(nStaff);
    //lmContext* pNewContext = pStaff->NewContext(pCurrentContext, nNewAccidentals, nStep);
    pStaff->NewContext(pCurrentContext, nNewAccidentals, nStep);

    /*! @todo
    For now, as we are not yet dealing with edition, it is not possible to
    insert notes in a score. Therefore, there are no notes after the one being
    processed (pStartNote). So the treatment to propagate accidentals until the
    start of the next measure is not yet implemented.
    */

    /*
    //propagate new context
    //define a forward iterator
    lmStaffObj* pSO = (lmStaffObj*) NULL;
    lmNoteRest* pNR = (lmNoteRest*)NULL;
    lmNote* pNote = (lmNote*)NULL;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
    pIter->MoveToObject(pStartNote);
    wxASSERT(pIter);
    pIter->MoveNext();

    while(!pIter->EndOfList())
    {
        pSO = pIter->GetCurrent();
        switch (pSO->GetClass()) {
            case eSFOT_NoteRest:
                if (pSO->GetStaffNum() == nStaff) {
                    pNR = (lmNoteRest*)pSO;
                    if (!pNR->IsRest()) {
                        pNote = (lmNote*)pSO;
                        if (pNote->UpdateContext(nStep, nNewAccidentals, pNewContext)) {
                            //if returns true, no context modification was necessary.
                            //So is not necessary to continue until the end of the measure
                            delete pIter;
                            return;
                        }
                    }
                }
                break;

            case eSFOT_Barline:
                //End of measure reached. End uptade process
                    delete pIter;
                    return;
                break;

            default:
                ;
        }
        pIter->MoveNext();
    }
    delete pIter;
    return;
    */

}


//---------------------------------------------------------------------------------------
// Methods for adding StaffObjs
//---------------------------------------------------------------------------------------

// adds a clef to the end of current StaffObjs collection
lmClef* lmVStaff::AddClef(EClefType nClefType, int nStaff, bool fVisible)
{
    wxASSERT(nStaff <= GetNumStaves());

    lmClef* pClef = new lmClef(nClefType, this, nStaff, fVisible);
    lmStaff* pStaff = GetStaff(nStaff);
    pStaff->NewContext(pClef);
    m_cStaffObjs.Store(pClef);
    return pClef;

}

// adds a spacer to the end of current StaffObjs collection
lmSpacer* lmVStaff::AddSpacer(lmTenths nWidth)
{
    lmSpacer* pSpacer = new lmSpacer(this, nWidth);
    m_cStaffObjs.Store(pSpacer);
    return pSpacer;

}

// returns a pointer to the lmNote object just created
lmNote* lmVStaff::AddNote(lmEPitchType nPitchType,
                    wxString sStep, wxString sOctave, wxString sAlter,
                    EAccidentals nAccidentals,
                    ENoteType nNoteType, float rDuration,
                    bool fDotted, bool fDoubleDotted,
                    int nStaff, bool fVisible,
                    bool fBeamed, lmTBeamInfo BeamInfo[],
                    bool fInChord,
                    bool fTie,
                    EStemType nStem)
{

    wxASSERT(nStaff <= GetNumStaves() );

    lmStaff* pStaff = GetStaff(nStaff);
    lmContext* pContext = pStaff->GetLastContext();

    lmNote* pNt = new lmNote(this, nPitchType,
                        sStep, sOctave, sAlter, nAccidentals,
                        nNoteType, rDuration, fDotted, fDoubleDotted, nStaff, fVisible,
                        pContext, fBeamed, BeamInfo, fInChord, fTie, nStem);

    m_cStaffObjs.Store(pNt);
    return pNt;

}

// returns a pointer to the lmRest object just created
lmRest* lmVStaff::AddRest(ENoteType nNoteType, float rDuration,
                      bool fDotted, bool fDoubleDotted,
                      int nStaff, bool fVisible,
                      bool fBeamed, lmTBeamInfo BeamInfo[])
{
    wxASSERT(nStaff <= GetNumStaves() );

    lmStaff* pStaff = GetStaff(nStaff);
    lmContext* pContext = pStaff->GetLastContext();

    lmRest* pR = new lmRest(this, nNoteType, rDuration, fDotted, fDoubleDotted, nStaff,
                        fVisible, pContext, fBeamed, BeamInfo);

    m_cStaffObjs.Store(pR);
    return pR;

}

lmWordsDirection* lmVStaff::AddWordsDirection(wxString sText, lmEAlignment nAlign,
                            lmLocation* pPos, lmFontInfo tFontData, bool fHasWidth)
{
    lmWordsDirection* pWD = new lmWordsDirection(this, sText, nAlign,
                                                 pPos, tFontData, fHasWidth);

    m_cStaffObjs.Store(pWD);
    return pWD;

}

lmMetronomeMark* lmVStaff::AddMetronomeMark(int nTicksPerMinute,
                        bool fParentheses, bool fVisible)
{
    lmMetronomeMark* pMM = new lmMetronomeMark(this, nTicksPerMinute,
                                               fParentheses, fVisible);
    m_cStaffObjs.Store(pMM);
    return pMM;

}

lmMetronomeMark* lmVStaff::AddMetronomeMark(ENoteType nLeftNoteType, int nLeftDots,
                        ENoteType nRightNoteType, int nRightDots,
                        bool fParentheses, bool fVisible)
{
    lmMetronomeMark* pMM = new lmMetronomeMark(this, nLeftNoteType, nLeftDots,
                                               nRightNoteType, nRightDots,
                                               fParentheses, fVisible);
    m_cStaffObjs.Store(pMM);
    return pMM;

}

lmMetronomeMark* lmVStaff::AddMetronomeMark(ENoteType nLeftNoteType, int nLeftDots,
                        int nTicksPerMinute, bool fParentheses, bool fVisible)
{
    lmMetronomeMark* pMM = new lmMetronomeMark(this, nLeftNoteType, nLeftDots,
                                               nTicksPerMinute,
                                               fParentheses, fVisible);
    m_cStaffObjs.Store(pMM);
    return pMM;

}


//for types eTS_Common, eTS_Cut and eTS_SenzaMisura
lmTimeSignature* lmVStaff::AddTimeSignature(ETimeSignatureType nType, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nType, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_SingleNumber
lmTimeSignature* lmVStaff::AddTimeSignature(int nSingleNumber, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nSingleNumber, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_Composite
lmTimeSignature* lmVStaff::AddTimeSignature(int nNumBeats, int nBeats[], int nBeatType,
                                        bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nNumBeats, nBeats, nBeatType, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_Multiple
lmTimeSignature* lmVStaff::AddTimeSignature(int nNumFractions, int nBeats[], int nBeatType[],
                                        bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nNumFractions, nBeats, nBeatType, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_Normal
lmTimeSignature* lmVStaff::AddTimeSignature(int nBeats, int nBeatType, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nBeats, nBeatType, this, fVisible);
    return AddTimeSignature(pTS);
}

lmTimeSignature* lmVStaff::AddTimeSignature(ETimeSignature nTimeSign, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nTimeSign, this, fVisible);
    return AddTimeSignature(pTS);
}


//common code for all time signatures types
lmTimeSignature* lmVStaff::AddTimeSignature(lmTimeSignature* pTS)
{
    m_cStaffObjs.Store(pTS);

    //Store current time signature
    //As time signatures are common to all staves we store it in all staves
    wxStaffListNode* pNode = m_cStaves.GetFirst();
    lmStaff* pStaff;
    for (; pNode; pNode = pNode->GetNext() ) {
        pStaff = (lmStaff *)pNode->GetData();
        pStaff->NewContext(pTS);
    }

    return pTS;
}

lmKeySignature* lmVStaff::AddKeySignature(int nFifths, bool fMajor, bool fVisible)
{
    lmKeySignature* pKS = new lmKeySignature(nFifths, fMajor, this, fVisible);
    m_cStaffObjs.Store(pKS);

    //Store current key signature
    //As key signatures are common to all staves we store it in all staves
    wxStaffListNode* pNode = m_cStaves.GetFirst();
    lmStaff* pStaff;
    for (; pNode; pNode = pNode->GetNext() ) {
        pStaff = (lmStaff *)pNode->GetData();
        pStaff->NewContext(pKS);
    }

    return pKS;
}

lmKeySignature* lmVStaff::AddKeySignature(EKeySignatures nKeySignature, bool fVisible)
{
    int nFifths = KeySignatureToNumFifths(nKeySignature);
    bool fMajor = IsMajor(nKeySignature);
    return AddKeySignature(nFifths, fMajor, fVisible);
}

int lmVStaff::GetNumMeasures()
{
    return m_cStaffObjs.GetNumMeasures();
}

void lmVStaff::DrawStaffLines(lmPaper* pPaper,
                              lmLUnits xFrom, lmLUnits xTo,
                              lmLUnits* pyLinTop, lmLUnits* pyLinBottom)
{
    // Draw all staff lines of this lmVStaff and store their sizes and positions

    if (GetOptionBool(_T("StaffLines.Hide")) ) return;


    lmLUnits yCur;

    ////DEBUG: draw top border of lmVStaff region
    //xFrom = pPaper->GetLeftMarginXPos();
    //xTo = pPaper->GetRightMarginXPos();
    //yCur = pPaper->GetCursorY();
    //pPaper->SketchLine(xFrom, yCur-1, xTo, yCur-1, *wxRED);
    ////-----------------------------------------


    //Set left position and lenght of lines, and save these values
    yCur = pPaper->GetCursorY() + m_topMargin;
    m_yLinTop = yCur;              //save y coord. for first line start point
    *pyLinTop = yCur;

    ////DEBUG: draw top border of first lmStaff region
    //pPaper->SketchLine(xFrom, yCur-1, xTo, yCur-1, *wxCYAN);
    ////-----------------------------------------

    //iterate over the collection of Staves (lmStaff Objects)
    StaffList::Node* pNode = m_cStaves.GetFirst();
    lmStaff* pStaff = (pNode ? (lmStaff *)pNode->GetData() : (lmStaff *)pNode);
    for ( ; pStaff; ) {
        //draw one staff
        for (int iL = 1; iL <= pStaff->GetNumLines(); iL++ ) {
            lmLUnits nStaffLineWidth = pStaff->GetLineThick();
            pPaper->SolidLine(xFrom, yCur, xTo, yCur,
                               nStaffLineWidth, eEdgeNormal, *wxBLACK);
            m_yLinBottom = yCur;  
            *pyLinBottom = yCur;

            //save line position
            yCur = yCur + pStaff->GetLineSpacing();
        }
        yCur = yCur - pStaff->GetLineSpacing() + pStaff->GetAfterSpace();

        //get next lmStaff
        pNode = pNode->GetNext();
        pStaff = (pNode ? (lmStaff *)pNode->GetData() : (lmStaff *)pNode);
    }
}

void lmVStaff::DrawStaffLines2(lmBoxSliceVStaff* pBox,
                              lmLUnits xFrom, lmLUnits xTo, lmLUnits yPos)
{
    //Computes all staff lines of this lmVStaff and creates the necessary shapes
	//to render them. Add this shapes to the received lmBox object.

    if (GetOptionBool(_T("StaffLines.Hide")) ) return;

    //Set left position and lenght of lines, and save these values
    lmLUnits yCur = yPos + m_topMargin;
    m_yLinTop = yCur;              //save y coord. for first line start point

    //iterate over the collection of Staves (lmStaff Objects)
    StaffList::Node* pNode = m_cStaves.GetFirst();
    lmStaff* pStaff = (pNode ? (lmStaff *)pNode->GetData() : (lmStaff *)pNode);
    for ( ; pStaff; )
	{
        //draw one staff
		lmShapeStaff* pShape = 
				new lmShapeStaff(pStaff, pStaff->GetNumLines(), 
								 pStaff->GetLineThick(), pStaff->GetLineSpacing(),
								 xFrom, yCur, xTo, *wxBLACK );
		pBox->AddShape(pShape);
        yCur = pShape->GetYBottom() + pStaff->GetAfterSpace();
		m_yLinBottom = pShape->GetYBottom() - pStaff->GetLineThick();  

        //get next lmStaff
        pNode = pNode->GetNext();
        pStaff = (pNode ? (lmStaff *)pNode->GetData() : (lmStaff *)pNode);
    }
}

void lmVStaff::SetUpFonts(lmPaper* pPaper)
{
    //iterate over the collection of Staves (lmStaff Objects) to set up the fonts
    // to use on that staff
    StaffList::Node* pNode = m_cStaves.GetFirst();
    for ( ; pNode; ) {
        SetFont((lmStaff *)pNode->GetData(), pPaper);
        pNode = pNode->GetNext();
    }
}

void lmVStaff::SetFont(lmStaff* pStaff, lmPaper* pPaper)
{
    // Font "LeMus Notas" has been designed to draw on a staff whose interline
    // space is of 512 FUnits. This gives an optimal rendering on VGA displays (96 pixels per inch)
    // as staff lines are drawn on exact pixels, according to the following relationships:
    //       Let dyLines be the distance between lines (pixels), then
    //       Font size = 3 * dyLines   (points)
    //       Scale = 100 * dyLines / 8     (%)
    //
    // Given a zooming factor (as a percentage, i.e. zoom=250.0%) fontsize can be computed as
    //       i = Round((zoom*8) / 100)
    //       dyLines = i        (pixels)
    //       FontSize = 3*i        (points)
    //
    // As all scaling takes place in the DC it is not necessary to allocate fonts of
    // different size as all scaling takes place in the DC. Then:
    //       Let dyLines be the distance between lines (logical units), then
    //       Font size = 3 * dyLines   (logical points)

    lmLUnits dyLinesL = pStaff->GetLineSpacing();

    // the font for drawing is scaled by the DC.
    pStaff->SetFontDraw( pPaper->GetFont((int)(3.0 * dyLinesL), _T("LenMus Basic") ) );        //logical points

    //wxLogMessage(_T("[lmVStaff::SetFont] dyLinesL=%d"), dyLinesL);

    //// the font for dragging is not scaled by the DC as all dragging operations takes
    //// place dealing with device units
    //int dyLinesD = pPaper->LogicalToDeviceY(100 * dyLinesL);
    //pStaff->SetFontDrag( pPaper->GetFont((3 * dyLinesD) / 100) );

    ////Calcula, en pixels, el grosor de las líneas (es de 51 FUnits, la décima parte de la distancia entre líneas)
    //m_nGrosorLineas = FUnitsToTwips(51) * nResolucion \ 1440
    //if (m_nGrosorLineas < 1) m_nGrosorLineas = 1;
    //sDbgFonts = sDbgFonts & "nGrosorLineas = " & m_nGrosorLineas & sCrLf
    //xUnits = m_nGrosorLineas


}



//=========================================================================================
// Methods for finding StaffObjs
//=========================================================================================

lmScoreObj* lmVStaff::FindSelectableObject(lmUPoint& pt)
{
    lmStaffObj* pSO;
    lmScoreObj* pChildSO;
    lmStaffObj* pCO;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_OptimizeAccess);
    //iterate over the collection of StaffObjs to look for a suitable lmStaffObj
    while(!pIter->EndOfList()) {
        pSO = pIter->GetCurrent();
        if (pSO->IsComposite()) {
            pCO = (lmStaffObj*)pSO;
            pChildSO = pCO->FindSelectableObject(pt);
            if (pChildSO) {
                delete pIter;
                return pChildSO;
            }
        }
        else {
            if (pSO->IsAtPoint(pt)) {
                delete pIter;
                return pSO;
            }
        }
        pIter->MoveNext();
    }
    delete pIter;
    return (lmScoreObj*) NULL;

}

lmLUnits lmVStaff::GetStaffOffset(int nStaff)
{
    //returns the Y offset to staff nStaff (1..n)
    wxASSERT(nStaff <= GetNumStaves() );
    lmLUnits yOffset = m_topMargin;

    // iterate over the collection of Staves (lmStaff Objects) to add up the
    // height and after space of all previous staves to the requested one
    lmStaff* pStaff;
    StaffList::Node* pNode = m_cStaves.GetFirst();
    for (int iS=1 ; iS < nStaff && pNode; iS++) {
        pStaff = (lmStaff *)pNode->GetData();
        yOffset += pStaff->GetHeight();
        yOffset += pStaff->GetAfterSpace();
        pNode = pNode->GetNext();
    }
    return yOffset;

}

wxString lmVStaff::Dump()
{
    wxString sDump = _T("");

    //iterate over the collection to dump the StaffObjs
    lmStaffObj* pSO;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_AsStored);
    while(!pIter->EndOfList())
    {
        pSO = pIter->GetCurrent();
        sDump += pSO->Dump();
        pIter->MoveNext();
    }
    delete pIter;

    //dump measures table
    sDump += _T("\nMeasures:\n");
    wxStaffObjsListNode* pNode;
    int iM;
    for (iM=1; iM <= m_cStaffObjs.GetNumMeasures(); iM++) {
        pNode = m_cStaffObjs.GetFirstInMeasure(iM);
        pSO = (lmStaffObj*)pNode->GetData();
        sDump += wxString::Format(_T("\tMeasure %d: starts with object Id %d\n"),
                                  iM, pSO->GetID() );
    }

    return sDump;

}

wxString lmVStaff::SourceLDP()
{
    wxString sSource;

    //iterate over the collection of StaffObjs
    lmStaffObj* pSO;
    lmStaffObjIterator* pIT = m_cStaffObjs.CreateIterator(eTR_AsStored);  //THINK: Should be eTR_ByTime?
    while(!pIT->EndOfList())
    {
        pSO = pIT->GetCurrent();
        sSource += pSO->SourceLDP();
        pIT->MoveNext();
    }
    delete pIT;

    return sSource;

}

wxString lmVStaff::SourceXML()
{
    wxString sSource = _T("TODO: lmVStaff XML Source code generation methods");

//    Dim oPo As IPentObj, iC As Long, sFuente As String
//    Dim oIT As CIterador
//
//    Set oIT = m_cStaffObjs.CreateIterator(eTR_AsStored)
//    iC = 1
//    Do While oIT.QuedanItems
//        Set oPo = oIT.GetItem
//        //determina si empieza compas
//        if (iC <= this.NumCompases {
//            if (oPo.ID = m_cStaffObjs.GetFirstInMeasure(iC).ID {
//                //si no es el primer compas cierra el precedente
//                if (iC != 1 { sFuente = sFuente & "    </measure>" & sCrLf
//                //inicia nuevo compas
//                sFuente = sFuente & "    <measure number=" & iC & """>" & sCrLf
//                iC = iC + 1
//            }
//        }
//        //obtiene el fuente del staffobj
//        sFuente = sFuente & oPo.FuenteXML & sCrLf
//        //avanza al siguiente staffobj
//        oIT.AdvanceCursor
//    Loop
//    //cierra último compas
//    sFuente = sFuente & "    </measure>" & sCrLf
//    FuenteXML = sFuente

    return sSource;

}

// the next two methods are mainly used for drawing the barlines. For that purpose it is necessary
// to know the y coordinate of the top most upper line of first staff and the bottom most lower
// line of the last staff.

//TODO: This methods must be moved to lmBoxSystem / lmBoxSlice

lmLUnits lmVStaff::GetYTop()
{
    return m_yLinTop;
}

lmLUnits lmVStaff::GetYBottom()
{
    return m_yLinBottom;
}

lmLUnits lmVStaff::GetVStaffHeight()
{
    if (m_nHeight == 0) {
        m_nHeight = m_topMargin + m_bottomMargin;
        // iterate over the collection of Staves (lmStaff Objects) to add up its
        // height and its after space
        lmStaff* pStaff;
        StaffList::Node* pNode = m_cStaves.GetFirst();
        for ( ; pNode; ) {
            pStaff = (lmStaff *)pNode->GetData();
            m_nHeight += pStaff->GetHeight();
            m_nHeight += pStaff->GetAfterSpace();
            pNode = pNode->GetNext();
        }
    }

    return m_nHeight;

}

//
//void lmVStaff::Get Contexto(iNota As Long) As Long
//    Contexto = m_anContexto(iNota)
//}
//
//void lmVStaff::Get GetNumLineas(iPent As Long) As Long
//    wxASSERT(iPent <= m_nNumStaves
//    GetNumLineas = m_nNumLineas(iPent)
//}
//
//void lmVStaff::Get EspaciadoEntrePentagramas() As Single
//    EspaciadoEntrePentagramas = pStaff->GetAfterSpace()
//}
//
//void lmVStaff::Let EspaciadoEntrePentagramas(rDecimas As Single)
//    pStaff->GetAfterSpace() = rDecimas
//}
//
//void lmVStaff::Get EspaciadoAntesParte() As Single
//    EspaciadoAntesParte = m_topMargin
//}
//
//void lmVStaff::Let EspaciadoAntesParte(rDecimas As Single)
//    m_topMargin = rDecimas
//}
//
//void lmVStaff::Get GetCurClave(iPent As Long) As EClefType
//    wxASSERT(iPent <= m_nNumStaves
//    GetCurClave = m_oCurClave(iPent).Valor
//}
//
//void lmVStaff::Get GetNumPentagramas() As Long
//    GetNumPentagramas = m_nNumStaves
//}
//
//void lmVStaff::AddDirectivaRepeticion(nTipo As EDirectivasRepeticion, nNum As Long, _
//        nX As Long, nY As Long, fXAbs As Boolean, fYAbs As Boolean)
//
//    //construye el staffobj de tipo "repeticion"
//    Dim oRepe As CPORepeticion
//    Set oRepe = new CPORepeticion
//    oRepe.ConstructorRepeticion this, nTipo, nNum, nX, nY, fXAbs, fYAbs
//    m_cStaffObjs.Almacenar oRepe
//
//End Sub
//
//void lmVStaff::AddDirectivaTexto(sTexto As String, _
//        nX As Long, nY As Long, fXAbs As Boolean, fYAbs As Boolean, _
//        Optional sFontName As String = "Arial", _
//        Optional nFontSize As Long = 10, _
//        Optional fBold As Boolean = False, _
//        Optional fItalic As Boolean = False)
//
//    //construye el staffobj de tipo "texto"
//    Dim oTxt As CPOTexto
//    Set oTxt = new CPOTexto
//    oTxt.ConstructorTexto this, sTexto, nX, nY, fXAbs, fYAbs, sFontName, nFontSize, fBold, fItalic
//    m_cStaffObjs.Almacenar oTxt
//
//End Sub
//
//void lmVStaff::AddIndicacionMetronomo(nTipoNotaIni As EMetronomo, _
//        nTipoNotaFin As EMetronomo, nVelocidad As Long, _
//        Optional fVisible As Boolean = True)
//
//    Dim oIndicacion As CPOIndicacion
//    Set oIndicacion = new CPOIndicacion
//    oIndicacion.ConstructorIndMetronomo nTipoNotaIni, nTipoNotaFin, nVelocidad, this, fVisible
//    m_cStaffObjs.Almacenar oIndicacion
//
//End Sub
//
//void lmVStaff::Get MetricaInicial() As ETimeSignature
//    MetricaInicial = m_oIniMetrica.Valor
//}
//
lmBarline* lmVStaff::AddBarline(EBarline nType, bool fVisible)
{
    //create and save the barline
    lmBarline* pBarline = new lmBarline(nType, this, fVisible);
    m_cStaffObjs.Store(pBarline);

    //save the contexts in the barline
    int nStaff;
    lmStaff* pStaff;
    lmContext* pContext;
    wxStaffListNode* pNode = m_cStaves.GetFirst();
    for (nStaff=1; pNode; pNode = pNode->GetNext(), nStaff++) {
        pStaff = (lmStaff *)pNode->GetData();
        pContext = pStaff->GetLastContext();
        pBarline->AddContext(pContext, nStaff);
    }

    //Reset contexts for the new measure that starts
    ResetContexts();

    return pBarline;

}

void lmVStaff::ResetContexts()
{
    /*
    Verify if current context is just the key signature accidentals. If not,
    create a new context.
    This method is invoked after a barline to reset the context if it was modified
    by accidentals in notes
    */

    // iterate over the collection of Staves
    lmStaff* pStaff;
    lmContext* pOldContext;
    lmContext* pNewContext;
    wxStaffListNode* pNode = m_cStaves.GetFirst();
    for (; pNode; pNode = pNode->GetNext() ) {
        pStaff = (lmStaff *)pNode->GetData();

        pOldContext = pStaff->GetLastContext();
        pNewContext = new lmContext(pOldContext->GetClef(),
                                    pOldContext->GeyKey(),
                                    pOldContext->GetTime() );
        bool fEqual = true;
        for (int i=0; i < 7; i++) {
            if (pOldContext->GetAccidentals(i) != pNewContext->GetAccidentals(i)) {
                fEqual = false;
                break;
            }
        }
        if (fEqual) {
            //current context is just the key signature accidentals. Continue using it.
            delete pNewContext;
        }
        else {
            //current contex has additional accidentals. Use the new clean one
            pStaff->NewContext(pNewContext);
        }
    }

}

//void lmVStaff::AddGrafObj(nTipo As EGrafObjs, _
//                Optional fVisible As Boolean = True, _
//                Optional nParm1 As Long)
//
//    Dim oGrafObj As CPOGrafObj
//    Set oGrafObj = new CPOGrafObj
//    oGrafObj.ConstructorGrafObj nTipo, this, fVisible, nParm1
//    m_cStaffObjs.Almacenar oGrafObj
//
//End Sub
//
//void lmVStaff::AddEspacio(Optional nEspacio As Long = 8)
//    AddGrafObj eGO_Espacio, True, nEspacio
//End Sub
//
//void lmVStaff::Get NumParte() As Long
//    //Devuelve el número que hace esta parte (1..n) dentro de los de su instrumento
//    NumParte = m_nStaff
//}

bool lmVStaff::GetXPosFinalBarline(lmLUnits* pPos)
{
    // returns true if a barline is found and in this case updates content
    // of variable pointed by pPos with the right x position of last barline
    // This method is only used by Formatter, in order to not justify the last system
    lmStaffObj* pSO = (lmStaffObj*) NULL;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_AsStored);
    pIter->MoveLast();
    while(!pIter->EndOfList())
    {
        pSO = pIter->GetCurrent();
        if (pSO->GetClass() == eSFOT_Barline) break;
        pIter->MovePrev();
    }
    delete pIter;

    //check that a barline is found. Otherwise no barlines in the score
    if (pSO->GetClass() == eSFOT_Barline) {
        *pPos = pSO->GetOrigin().x + pSO->GetSelRect().GetWidth();
        return true;
    }
    else
        return false;

}

void lmVStaff::NewLine(lmPaper* pPaper)
{
    //move x cursor to the left and advance y cursor the space
    //height of all stafves of this lmVStaff
    pPaper->NewLine(GetVStaffHeight());

}

void lmVStaff::DrawProlog(bool fMeasuring, int nMeasure, bool fDrawTimekey, lmPaper* pPaper)
{
    // The prolog (clef and key signature) must be rendered on each system,
    // but the matching StaffObjs only exist in the first system. Therefore, in the
    // normal staffobj rendering process, the prolog would be rendered only in
    // the first system.
    // So, for the other systems it is necessary to force the rendering
    // of the prolog because there are no StaffObjs representing it.
    // This method does it.
    //
    // To know what clef, key and time signature to draw we take this information from the
    // context associated to first note of the measure on each sttaf. If there are no notes,
    // the context is taken from the barline. If, finally, no context is found, no prolog
    // is drawn.

    lmLUnits nPrologWidth = 0;
    lmClef* pClef = (lmClef*)NULL;
    EClefType nClef = eclvUndefined;
    lmKeySignature* pKey = (lmKeySignature*)NULL;
    lmTimeSignature* pTime = (lmTimeSignature*)NULL;

    //AWARE when this method is invoked the paper position must be at the left marging,
    //at the start of a new system.
    lmLUnits xStartPos = pPaper->GetCursorX() + m_nSpaceBeforeClef;         //Save x to align all clefs
    lmLUnits yStartPos = pPaper->GetCursorY();

    //iterate over the collection of lmStaff objects to draw current cleft and key signature

    wxStaffListNode* pNode = m_cStaves.GetFirst();
    lmStaff* pStaff = (lmStaff*)NULL;
    lmLUnits yOffset = 0;
    lmLUnits xPos=0;
    lmLUnits nWidth=0;

    lmContext* pContext = (lmContext*)NULL;
    lmStaffObj* pSO = (lmStaffObj*) NULL;
    lmNoteRest* pNR = (lmNoteRest*)NULL;
    lmNote* pNote = (lmNote*)NULL;
    for (int nStaff=1; pNode; pNode = pNode->GetNext(), nStaff++)
    {
        pStaff = (lmStaff *)pNode->GetData();
        xPos = xStartPos;

        //locate first context for this staff
        pContext = (lmContext*)NULL;
        lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
        pIter->AdvanceToMeasure(nMeasure);
        while(!pIter->EndOfList()) {
            pSO = pIter->GetCurrent();
            if (pSO->GetClass() == eSFOT_NoteRest) {
                pNR = (lmNoteRest*)pSO;
                if (!pNR->IsRest() && pNR->GetStaffNum() == nStaff) {
                    //OK. Note fount. Take context
                    pNote = (lmNote*)pSO;
                    pContext = pNote->GetContext();
                    break;
                }
            }
            else if (pSO->GetClass() == eSFOT_Barline) {
                lmBarline* pBar = (lmBarline*)pSO;
                pContext = pBar->GetContext(nStaff);
                break;
            }
            pIter->MoveNext();
        }
        delete pIter;

        if (pContext) {
            pClef = pContext->GetClef();
            pKey = pContext->GeyKey();
            pTime = pContext->GetTime();

            //render clef
            if (pClef) {
                nClef = pClef->GetClefType();
				if (pClef->IsVisible()) {
					lmUPoint uPos = lmUPoint(xPos, yStartPos+yOffset);        //absolute position
					nWidth = pClef->DrawAt(fMeasuring, pPaper, uPos);
					xPos += nWidth;
				}
            }

            //render key signature
            if (pKey && pKey->IsVisible()) {
                wxASSERT(nClef != eclvUndefined);
                lmUPoint uPos = lmUPoint(xPos, yStartPos+yOffset);        //absolute position
                nWidth = pKey->DrawAt(fMeasuring, pPaper, uPos, nClef, nStaff);
                xPos += nWidth;
            }

            //if requested (flag fDrawTimekey), render time key (only on first staff)
            //if (fDrawTimekey And iStf = 1 {
            //    if (Not m_oCurMetrica Is Nothing {
            //        nTimeKey = m_oCurMetrica.Valor
            //        pPaper->PintarMetrica fMeasuring, nTimeKey, yDesplz
            //    }
            //}

        }

        //compute prolog width
        nPrologWidth = wxMax(nPrologWidth, xPos - xStartPos);

        //compute vertical displacement for next staff
        yOffset += pStaff->GetHeight();
        yOffset += pStaff->GetAfterSpace();

    }

    // update paper cursor position
    pPaper->SetCursorX(xStartPos + nPrologWidth);

}

lmSoundManager* lmVStaff::ComputeMidiEvents(int nChannel)
{
    /*
    nChannel is the MIDI channel to use for all events of this lmVStaff.
    Returns the lmSoundManager object. It is not retained by the lmVStaff, so it is caller
    responsibility to delete it when no longer needed.
    */

    //! @todo review this commented code
//    Dim nMetrica As ETimeSignature, nDurCompas As Long, nTiempoIni As Long
//
//    nMetrica = this.MetricaInicial
//    nDurCompas = GetDuracionMetrica(nMetrica)
//    nTiempoIni = nDurCompas - this.DuracionCompas(1)
//
//    wxASSERT(nTiempoIni >= 0        //El compas tiene más notas de las que caben
//
//    //Si el primer compas no es anacrusa, retrasa todo un compas para que
//    //se marque un compas completo antes de comenzar
//    if (nTiempoIni = 0 { nTiempoIni = nDurCompas

    //Create lmSoundManager and initialize MIDI events table
    lmSoundManager* pSM = new lmSoundManager();
    //! @todo review next line
//    pSM->Inicializar GetStaffsCompas(nMetrica), nTiempoIni, nDurCompas, this.NumCompases

    //iterate over the collection to create the MIDI events
    float rMeasureStartTime = 0;
    int nMeasure = 1;        //to count measures (1 based, normal musicians way)
    pSM->StoreMeasureStartTime(nMeasure, rMeasureStartTime);

    //iterate over the collection to create the MIDI events
    lmStaffObj* pSO;
    lmNoteRest* pNR;
    lmTimeSignature* pTS;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
    while(!pIter->EndOfList()) {
        pSO = pIter->GetCurrent();
        if (pSO->GetClass() == eSFOT_NoteRest) {
            pNR = (lmNoteRest*)pSO;
            pNR->AddMidiEvents(pSM, rMeasureStartTime, nChannel, nMeasure);
        }
        else if (pSO->GetClass() == eSFOT_Barline) {
            rMeasureStartTime += pSO->GetTimePos();        //add measure duration
            nMeasure++;
            pSM->StoreMeasureStartTime(nMeasure, rMeasureStartTime);
        }
        else if (pSO->GetClass() == eSFOT_TimeSignature) {
            //add a RhythmChange event to set up tempo (num beats, duration of a beat)
            pTS = (lmTimeSignature*)pSO;
            pTS->AddMidiEvent(pSM, rMeasureStartTime, nMeasure);
        }
        pIter->MoveNext();
    }
    delete pIter;

    return pSM;

}

lmNote* lmVStaff::FindPossibleStartOfTie(lmAPitch anPitch)
{
    //
    // This method is invoked from lmNote constructor to find if the note being created
    // (the "target note") is tied to a previous one ("the candidate" one).
    // This method explores backwards to try to find a note that can be tied with the received
    // as parameter (the "target note").
    //
    // Algorithm:
    // Find the first previous note of the same pitch, in this measure or
    // in the previous one
    //


    //define a backwards iterator
    bool fInPrevMeasure = false;
    lmStaffObj* pSO = (lmStaffObj*) NULL;
    lmNoteRest* pNR = (lmNoteRest*)NULL;
    lmNote* pNote = (lmNote*)NULL;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_AsStored);
    pIter->MoveLast();
    while(!pIter->EndOfList())
    {
        pSO = pIter->GetCurrent();
        switch (pSO->GetClass()) {
            case eSFOT_NoteRest:
                pNR = (lmNoteRest*)pSO;
                if (!pNR->IsRest()) {
                    pNote = (lmNote*)pSO;
                    if (pNote->CanBeTied(anPitch)) {
                        delete pIter;
                        return pNote;    // candidate found
                    }
                }
                break;

            case eSFOT_Barline:
                if (fInPrevMeasure) {
                    delete pIter;
                    return (lmNote*)NULL;        // no suitable note found
                }
                fInPrevMeasure = true;
                break;

            default:
                ;
        }
        pIter->MovePrev();
    }
    delete pIter;
    return (lmNote*)NULL;        //no suitable note found

}


//Function RepositionBar(iBar As Long, nShift As Long, nBarLeft As Long, _
//            nNewBarWidth As Long) As Long
//    //Shift the position of all StaffObjs in bar number iBar the amount given by parameter nShift.
//    //In addition, the position of the barline at the end of this bar is also shifted so that
//    //the new width on the bar becames nNewBarWidth. The shift amount applied to the barline
//    //is retuned as the result of this function.
//    //
//    //Parameters:
//    //iBar - the number of this bar (absolute, that is, from the begining of the score)
//    //nShift - the shift amount to apply to all StaffObjs of bar number iBar
//    //nBarLeft - the new left position for the start of this bar
//    //nNewBarWidth - the new width that this bar will have.
//
//
//    wxASSERT(iBar <= this.NumCompases
//
//    if (nNewBarWidth = 0 { Exit Function
//
//    Dim i As Long, nBarlineShift As Long
//    Dim oPo As IPentObj
//
//    //get staffobj on which the bar starts
//    Dim oIT As CIterador
//    Set oIT = m_cStaffObjs.CreateIterator(eTR_OptimizeAccess)
//    oIT.AdvanceToMeasure iBar
//    Do While oIT.QuedanItems
//        Set oPo = oIT.GetItem
//
//        //if this staffobj is the barline, shift it and exit
//        if (oPo.Tipo = eSFOT_Barline {
//            nBarlineShift = (nBarLeft + nNewBarWidth) - oPo.Left
//            oPo.Left = nBarLeft + nNewBarWidth - (oPo.Right - oPo.Left - 1)
//            Exit Do
//        }
//
//        //otherwise shift the staffobj
//        oPo.Left = oPo.Left + nShift
//
//        oIT.AdvanceCursor
//    Loop
//    RepositionBar = nBarlineShift
//
//}
//
////Desplaza la barra de fin de compas para que quede en la posición nLeft.
////Devuelve el desplazamiento aplicado a la barra
//Function SetAnchoCompas(iCompas As Long, nLeft As Long) As Long
//
//    wxASSERT(iCompas <= this.NumCompases
//
//    Dim i As Long
//    Dim oPo As IPentObj
//
//    //localizar el fin del compas
//    Dim oIT As CIterador
//    Set oIT = m_cStaffObjs.CreateIterator(eTR_OptimizeAccess)
//    oIT.AdvanceToMeasure iCompas
//    Do While oIT.QuedanItems
//        Set oPo = oIT.GetItem
//        if (oPo.Tipo = eSCOT_Barline { Exit Do
//        oIT.AdvanceCursor
//    Loop
//
//    i = nLeft - oPo.Left    //desplazamiento a aplicar
//    oPo.Left = nLeft - (oPo.Right - oPo.Left - 1)
//    SetAnchoCompas = i
//
//}
//
////Devuelve la duración del compas
////Al llevarse ahora una marca de tiempo, la duración del compas viene dada por la marca de
////tiempo de la barra de fin de compas, lo que simplifica el tratamiento
//Function DuracionCompas(iCompas As Long) As Long
//    wxASSERT(iCompas <= this.NumCompases
//
//    //Algoritmo: Localizar la barra de fin del compas y devolver su marca de tiempo
//
//    Dim oPo As IPentObj
//
//    //bucle hasta el fin del compas
//    Dim oIT As CIterador
//    Set oIT = m_cStaffObjs.CreateIterator(eTR_OptimizeAccess)
//    oIT.AdvanceToMeasure iCompas
//    Do While oIT.QuedanItems
//        Set oPo = oIT.GetItem
//        if (oPo.Tipo = eSCOT_Barline { Exit Do
//        oIT.AdvanceCursor
//    Loop
//
//    wxASSERT(oPo.Tipo = eSCOT_Barline      //no pueden existir compases que no acaben en barra
//    DuracionCompas = oPo.TimePos
//
//}

void lmVStaff::ShiftTime(float rTimeShift)
{
    /*
    Shifts the time counter and inserts a control lmStaffObj to signal the shift event
    This is necessary for rendering (to start a new thread) and for exporting the score:
     - in LDP, to create an element AVANCE/RETROCESO
     - in MusicXML: to create an element FORWARD/BACKWARD

    */

    //shift time counters
    m_cStaffObjs.ShiftTime(rTimeShift);

    //Insert a control object to signal the shift event so that we can start a
    //new thread at rendering
    lmSOControl* pControl = new lmSOControl(lmTIME_SHIFT, this, rTimeShift);
    m_cStaffObjs.Store(pControl);

}

lmSOControl* lmVStaff::AddNewSystem()
{
    /*
    Inserts a control lmStaffObj to signal a new system
    */

    //Insert the control object
    lmSOControl* pControl = new lmSOControl(lmNEW_SYSTEM, this);
    m_cStaffObjs.Store(pControl);
    return pControl;

}

//=========================================================================================
// Friend methods to be used only by lmFormatter objects
//=========================================================================================

//allow IViewer to navigate through the StaffObjs collection
lmStaffObjIterator* lmVStaff::CreateIterator(ETraversingOrder nOrder)
{
    return m_cStaffObjs.CreateIterator(nOrder);
}

////el posicionamiento relativo de objetos requiere conocer la
////posición de inicio del compas. Para ello, las funciones de dibujo lo guardan
//// aqui, de forma que el método GetXInicioCompas pueda devolver este valor
//Friend void lmVStaff::Let SetXInicioCompas(xPos As Long)
//    m_xInicioCompas = xPos
//}
//
//// End of friend methods only for IViewer =================================================
//
//
////==============================================================================================
//// Funciones que añaden StaffObjs al final del pentagrama a partir de un fuente.
////==============================================================================================
//
//
////sElemento es un elemento <Compas> :
////v1.0  <Compas> ::= ("C" [<Num>] {<Figura> | <Grupo>}* )
//void lmVStaff::CrearCompas(ByVal sElemento As String)
//
//    Dim oRaiz As CNodo
//    Set oRaiz = AnalizarFuente(sElemento)
//    AnalizarCompas oRaiz, this
//
//End Sub

//Devuelve una referencia al objeto CPONota creado.
//lmNoteRest* lmVStaff::AddNoteRestFromSource(wxString sText)
//{
//    lmLDPParser parserLDP;
//    lmLDPNode* pNode = parserLDP.ParseText(sText);
//
//    if (pNode->GetName() == _T("N")) {
//        pNR = parserLDP.AnalyzeNote(pNode, this);
//    } else {
//        pNR = parserLDP.AnalyzeRest(pNode, this);
//    }
//
//    return pNR;
//
//}
