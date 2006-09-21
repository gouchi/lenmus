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
/*! @file TimeSignature.cpp
    @brief Implementation file for class lmTimeSignature
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"
#include "Score.h"

//-------------------------------------------------------------------------------------------------
// lmTimeSignature object implementation
//-------------------------------------------------------------------------------------------------

//
//constructors and destructor
//

//constructors for type eTS_Normal
lmTimeSignature::lmTimeSignature(int nBeats, int nBeatType, lmVStaff* pVStaff, bool fVisible) :
    lmSimpleObj(eTPO_TimeSignature, pVStaff, 1, fVisible, sbDRAGGABLE)
{
    m_nType = eTS_Normal;
    m_nBeats = nBeats;
    m_nBeatType = nBeatType;
}

lmTimeSignature::lmTimeSignature(ETimeSignature nTimeSign, lmVStaff* pVStaff, bool fVisible) :
    lmSimpleObj(eTPO_TimeSignature, pVStaff, 1, fVisible, sbDRAGGABLE)
{
    m_nType = eTS_Normal;
    m_nBeats = GetNumUnitsFromTimeSignType(nTimeSign);
    m_nBeatType = GetBeatTypeFromTimeSignType(nTimeSign);
}

//constructor for types eTS_Common, eTS_Cut and eTS_SenzaMisura
lmTimeSignature::lmTimeSignature(ETimeSignatureType nType, lmVStaff* pVStaff, bool fVisible) :
    lmSimpleObj(eTPO_TimeSignature, pVStaff, 1, fVisible, sbDRAGGABLE)
{
    m_nType = nType;
    wxASSERT(false);    //! @todo not yet implemented
}

//constructor for type eTS_SingleNumber
lmTimeSignature::lmTimeSignature(int nSingleNumber, lmVStaff* pVStaff, bool fVisible) :
    lmSimpleObj(eTPO_TimeSignature, pVStaff, 1, fVisible, sbDRAGGABLE)
{
    m_nType = eTS_SingleNumber;
    wxASSERT(false);    //! @todo not yet implemented
}

//constructor for type eTS_Composite
lmTimeSignature::lmTimeSignature(int nNumBeats, int nBeats[], int nBeatType, lmVStaff* pVStaff,
                             bool fVisible) :
    lmSimpleObj(eTPO_TimeSignature, pVStaff, 1, fVisible, sbDRAGGABLE)
{
    m_nType = eTS_Composite;
    wxASSERT(false);    //! @todo not yet implemented
}

//constructor for type eTS_Multiple
lmTimeSignature::lmTimeSignature(int nNumFractions, int nBeats[], int nBeatType[],
                             lmVStaff* pVStaff, bool fVisible) :
    lmSimpleObj(eTPO_TimeSignature, pVStaff, 1, fVisible, sbDRAGGABLE)
{
    m_nType = eTS_Multiple;
    wxASSERT(false);    //! @todo not yet implemented
}

//void lmTimeSignature::GetValor() As ETimeSignature
//    Valor = m_nTimeSignature
//}

wxString lmTimeSignature::Dump()
{
    //! @todo take into account TimeSignatures types other than eTS_Normal
    wxString sDump = _T("");

    switch (m_nType) {
        case eTS_Normal :
            sDump = wxString::Format(
                _T("%d\tTime Sign. %d/%d\tTimePos=%.2f\n"),
                m_nId, m_nBeats, m_nBeatType, m_rTimePos );
            break;
        default:
            sDump = wxString::Format(
                _T("%d\tTime Sign. Type=%d\tTimePos=%.2f\n"),
                m_nId, m_nType, m_rTimePos );
    }
    return sDump;
}

wxString lmTimeSignature::SourceLDP()
{
    wxString sSource = wxString::Format(_T("         (time %d %d"), m_nBeats, m_nBeatType);
    if (!m_fVisible) { sSource += _T(" no_visible"); }
    sSource += _T(")\n");
    return sSource;

}

wxString lmTimeSignature::SourceXML()
{
//    IPentObj_FuenteXML = sEmpty
    return _T("");
}

void lmTimeSignature::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC)
{    
    if (fMeasuring) {
        // get the shift to the staff on which the time key must be drawn
        lmLUnits yShift = m_pVStaff->GetStaffOffset(m_nStaffNum);

        // store glyph position
        m_glyphPos.x = 0;
        m_glyphPos.y = yShift - m_pVStaff->TenthsToLogical( 40, m_nStaffNum );
    }

    DrawTimeSignature(fMeasuring, pPaper, (m_fSelected ? g_pColors->ScoreSelected() : *wxBLACK) );

}
// returns the width of the draw (logical units)
lmLUnits lmTimeSignature::DrawTimeSignature(bool fMeasuring, lmPaper* pPaper, wxColour colorC)
{    
    pPaper->SetFont(*m_pFont);

    wxString sTopGlyphs = wxString::Format(_T("%d"), m_nBeats );
    wxString sBottomGlyphs = wxString::Format(_T("%d"), m_nBeatType );

    if (fMeasuring) {
        lmLUnits nWidth1, nHeight1, nWidth2, nHeight2;
        pPaper->GetTextExtent(sTopGlyphs, &nWidth1, &nHeight1);
        pPaper->GetTextExtent(sBottomGlyphs, &nWidth2, &nHeight2);

        // store selection rectangle measures and position (relative to m_paperPos)
        m_selRect.width = wxMax(nWidth1, nWidth2);
        m_selRect.height = m_pVStaff->TenthsToLogical( 40, m_nStaffNum );
        m_selRect.x = m_glyphPos.x;
        m_selRect.y = m_glyphPos.y + m_pVStaff->TenthsToLogical( 40, m_nStaffNum );

        //compute Beats and BeatsType positions so that one is centered on the other
        if (nWidth2 > nWidth1) {
            //bottom number wider
            m_xPosTop = (nWidth2 - nWidth1) / 2;
            m_xPosBottom = 0;
        }
        else {
            //bottom number wider
            m_xPosTop = 0;
            m_xPosBottom = (nWidth1 - nWidth2) / 2;
        }

        // set total width (incremented in one line for after space)
        m_nWidth = m_selRect.width + m_pVStaff->TenthsToLogical(10, m_nStaffNum);    //one line space
        return m_nWidth;
    }
    else {
        //Time signature is common to all lmVStaff staves, but it is only present, as lmStaffObj, in
        //the first staff. Therefore, for renderization, it is necessary to repeat it for
        //each staff
        //wxLogMessage(_T("[lmTimeSignature::DrawTimeSignature]"));
        pPaper->SetTextForeground(colorC);
        lmLUnits yOffset = 0;
        lmStaff* pStaff = m_pVStaff->GetFirstStaff();
        for (int nStaff=1; pStaff; pStaff = m_pVStaff->GetNextStaff(), nStaff++) {
            // Draw the time signature
            wxPoint pos = GetGlyphPosition();
            pPaper->DrawText(sTopGlyphs, pos.x + m_xPosTop, pos.y + yOffset );
            pPaper->DrawText(sBottomGlyphs, pos.x + m_xPosBottom,
                            pos.y + yOffset + m_pVStaff->TenthsToLogical( 20, nStaff ) );

            //compute vertical displacement for next staff
            yOffset += pStaff->GetHeight();
            yOffset += pStaff->GetAfterSpace();
        }
        return 0;
    }

}


void lmTimeSignature::PrepareGlyphs()
{
//    
//    if Not fMeasuring {
//        Escribir CStr(sTopGlyphs), Int(m_xCursor), CLng(m_yCursor - 10# * m_ndyLines + yShift)
//        Escribir CStr(sBottomGlyphs), Int(m_xCursor), CLng(m_yCursor - 8# * m_ndyLines + yShift)
//    }
//    m_xCursor = m_xCursor + 30# * m_rEscala     //avance tras la m�trica
//
//}
//
}

void lmTimeSignature::AddMidiEvent(lmSoundManager* pSM, float rMeasureStartTime, int nMeasure)
{
    //  Add a MIDI event of type RithmChange
    //! @todo Deal with non-standard time signatures

    float rTime = m_rTimePos + rMeasureStartTime;

    //transform beat type into duration in LDP notes duration units
    int nBeatDuration = GetBeatDuration(m_nBeatType);

    //add the RithmChange event
    pSM->StoreEvent( rTime, eSET_RithmChange, 0, m_nBeats, nBeatDuration, this, nMeasure);
    
}

//! @todo Code all following methods needed for dragging and selection rectangle

wxBitmap* lmTimeSignature::GetBitmap(double rScale)
{
    //! @todo
    return (wxBitmap*)NULL;
}

void lmTimeSignature::MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, wxPoint& ptOffset, 
                        const wxPoint& ptLog, const wxPoint& dragStartPosL, const wxPoint& ptPixels)
{
}

wxPoint lmTimeSignature::EndDrag(const wxPoint& pos)
{
    //! @todo
    return wxPoint(0,0);
}

lmLUnits lmTimeSignature::DrawAt(bool fMeasuring, lmPaper* pPaper, wxPoint pos, wxColour colorC)
{
    //! @todo
    return 0;
}

lmTenths lmTimeSignature::GetSelRectHeight()
{
    //! @todo
    return 0;
}

lmTenths lmTimeSignature::GetSelRectShift()
{
    //! @todo
    return 0;
}

lmTenths lmTimeSignature::GetGlyphOffset()
{
    //! @todo
    return 0;
}

wxString lmTimeSignature::GetLenMusChar()
{
    //! @todo
    return _T("4/4");
}


//----------------------------------------------------------------------------------------
// global functions related to TimeSignatures
//----------------------------------------------------------------------------------------

//! returns the numerator of time signature fraction
int GetNumUnitsFromTimeSignType(ETimeSignature nTimeSign)
{
    switch (nTimeSign) {
        case emtr24:
            return 2;
        case emtr34:
            return 3;
        case emtr44:
            return 4;
        case emtr28:
            return 2;
        case emtr38:
            return 3;
        case emtr22:
            return 2;
        case emtr32:
            return 3;
        case emtr68:
            return 6;
        case emtr98:
            return 9;
        case emtr128:
            return 12;
        default:
            wxASSERT(false);
            return 4;
    }
}

int GetNumBeatsFromTimeSignType(ETimeSignature nTimeSign)
{
    switch (nTimeSign) {
        case emtr24:
            return 2;
        case emtr34:
            return 3;
        case emtr44:
            return 4;
        case emtr28:
            return 1;
        case emtr38:
            return 1;
        case emtr22:
            return 2;
        case emtr32:
            return 3;
        case emtr68:
            return 2;
        case emtr98:
            return 3;
        case emtr128:
            return 4;
        default:
            wxASSERT(false);
            return 4;
    }
}

int GetBeatTypeFromTimeSignType(ETimeSignature nTimeSign)
{
    switch (nTimeSign) {
        case emtr24:
        case emtr34:
        case emtr44:
            return 4;

        case emtr28:
        case emtr38:
        case emtr68:
        case emtr98:
        case emtr128:
            return 8;

        case emtr22:
        case emtr32:
            return 2;

        default:
            wxASSERT(false);
            return 4;
    }
}

//! returns beat duration (in LDP notes duration units)
float GetBeatDuration(ETimeSignature nTimeSign)
{
    int nBeatType = GetBeatTypeFromTimeSignType(nTimeSign);
    return GetBeatDuration(nBeatType);
}

//! returns beat duration (in LDP notes duration units)
float GetBeatDuration(int nBeatType)
{
    switch(nBeatType) {
        case 1:    
            return pow(2, (10 - eWhole));
        case 2:    
            return pow(2, (10 - eHalf));
        case 4:    
            return pow(2, (10 - eQuarter));
        case 8:    
            return (1.5 * pow(2, (10 - eQuarter)) );
        case 16:    
            return pow(2, (10 - eEighth));
        default:
            wxASSERT(false);
            return 0;     //compiler happy
    }    
}

//! Returns the required duration for a measure in the received time signature
float GetMeasureDuration(ETimeSignature nTimeSign)
{
    float rNumBeats = (float)GetNumBeatsFromTimeSignType(nTimeSign);
    return rNumBeats * GetBeatDuration(nTimeSign);
}

//bool IsBinaryTimeSignature(ETimeSignature nTimeSign)
//{
//    switch (nTimeSign) {
//        case emtr24:
//        case emtr34:
//        case emtr44:
//        case emtr28:
//        case emtr22:
//        case emtr32:
//            return true;
//        default:
//            return false;
//    }
//
//}

