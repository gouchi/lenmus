//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Copyright (c) 2010-2016 Cecilio Salmeron. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice, this
//      list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright notice, this
//      list of conditions and the following disclaimer in the documentation and/or
//      other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// For any comment, suggestion or feature request, please contact the manager of
// the project at cecilios@users.sourceforge.net
//---------------------------------------------------------------------------------------

#include "lomse_staffobjs_cursor.h"

#include "lomse_internal_model.h"


namespace lomse
{


//---------------------------------------------------------------------------------------
StaffObjsCursor::StaffObjsCursor(ImoScore* pScore)
    : m_pColStaffObjs( pScore->get_staffobjs_table() )
    , m_scoreIt(m_pColStaffObjs)
    , m_savedPos(m_scoreIt)
    , m_numInstruments( pScore->get_num_instruments() )
    , m_numLines( pScore->get_staffobjs_table()->num_lines() )
    , m_fScoreIsEmpty( m_scoreIt.is_end() )
    , m_pLastBarline(NULL)
{
    initialize_clefs_keys_times(pScore);
}

//---------------------------------------------------------------------------------------
StaffObjsCursor::~StaffObjsCursor()
{
}

//---------------------------------------------------------------------------------------
void StaffObjsCursor::initialize_clefs_keys_times(ImoScore* pScore)
{
    m_staffIndex.reserve(m_numInstruments);
    int staves = 0;
    for (int i=0; i < m_numInstruments; ++i)
    {
        m_staffIndex[i] = staves;
        staves += pScore->get_instrument(i)->get_num_staves();
    }

    m_clefs.reserve(staves);
    m_clefs.assign(staves, (ColStaffObjsEntry*)NULL);     //GCC complains if NULL not casted

    m_keys.reserve(staves);
    m_keys.assign(staves, (ColStaffObjsEntry*)NULL);

    m_times.reserve(m_numInstruments);
    m_times.assign(m_numInstruments, (ColStaffObjsEntry*)NULL);
}

//---------------------------------------------------------------------------------------
void StaffObjsCursor::move_next()
{
    ImoObj* pSO = imo_object();
    if (pSO->is_clef())
        save_clef();
    else if (pSO->is_key_signature())
        save_key_signature();
    else if (pSO->is_time_signature())
        save_time_signature();
    else if (pSO->is_barline())
        save_barline();

    ++m_scoreIt;
}

//---------------------------------------------------------------------------------------
void StaffObjsCursor::save_clef()
{
    int iInstr = num_instrument();
    int idx = m_staffIndex[iInstr] + staff();
    m_clefs[idx] = *m_scoreIt;
}

//---------------------------------------------------------------------------------------
void StaffObjsCursor::save_key_signature()
{
    int iInstr = num_instrument();
    int idx = m_staffIndex[iInstr] + staff();
    m_keys[idx] = *m_scoreIt;
}

//---------------------------------------------------------------------------------------
void StaffObjsCursor::save_barline()
{
    m_pLastBarline = dynamic_cast<ImoBarline*>( imo_object() );
}

//---------------------------------------------------------------------------------------
void StaffObjsCursor::save_time_signature()
{
    int iInstr = num_instrument();
    m_times[iInstr] = *m_scoreIt;
}

//---------------------------------------------------------------------------------------
ImoStaffObj* StaffObjsCursor::get_staffobj()
{
    return dynamic_cast<ImoStaffObj*>( imo_object() );
}

//---------------------------------------------------------------------------------------
ColStaffObjsEntry* StaffObjsCursor::get_clef_entry_for_instr_staff(int iInstr, int iStaff)
{
    int idx = m_staffIndex[iInstr] + iStaff;
    return m_clefs[idx];
}

//---------------------------------------------------------------------------------------
ImoClef* StaffObjsCursor::get_clef_for_instr_staff(int iInstr, int iStaff)
{
    ColStaffObjsEntry* pEntry = get_clef_entry_for_instr_staff(iInstr, iStaff);
    if (pEntry)
        return dynamic_cast<ImoClef*>( pEntry->imo_object() );
    else
        return NULL;
}

//---------------------------------------------------------------------------------------
int StaffObjsCursor::get_clef_type_for_instr_staff(int iInstr, int iStaff)
{
    ImoClef* pClef = get_clef_for_instr_staff(iInstr, iStaff);
    if (pClef)
        return pClef->get_clef_type();
    else
        return k_clef_undefined;
}

//---------------------------------------------------------------------------------------
ImoClef* StaffObjsCursor::get_applicable_clef()
{
    if (m_fScoreIsEmpty)
        return NULL;
    else
        return get_clef_for_instr_staff( num_instrument(), staff() );
}

//---------------------------------------------------------------------------------------
int StaffObjsCursor::get_applicable_clef_type()
{
    if (m_fScoreIsEmpty)
        return k_clef_undefined;
    else
        return get_clef_type_for_instr_staff( num_instrument(), staff() );
}

//---------------------------------------------------------------------------------------
ColStaffObjsEntry* StaffObjsCursor::get_key_entry_for_instr_staff(int iInstr, int iStaff)
{
    int idx = m_staffIndex[iInstr] + iStaff;
    return m_keys[idx];
}

//---------------------------------------------------------------------------------------
ImoKeySignature* StaffObjsCursor::get_key_for_instr_staff(int iInstr, int iStaff)
{
    ColStaffObjsEntry* pEntry = get_key_entry_for_instr_staff(iInstr, iStaff);
    if (pEntry)
        return dynamic_cast<ImoKeySignature*>( pEntry->imo_object() );
    else
        return NULL;
}

//---------------------------------------------------------------------------------------
ImoKeySignature* StaffObjsCursor::get_applicable_key()
{
    if (m_fScoreIsEmpty)
        return NULL;
    else
        return get_key_for_instr_staff( num_instrument(), staff() );
}

//---------------------------------------------------------------------------------------
int StaffObjsCursor::get_applicable_key_type()
{
    if (m_fScoreIsEmpty)
        return k_key_undefined;
    else
        return get_key_type_for_instr_staff( num_instrument(), staff() );
}

//---------------------------------------------------------------------------------------
int StaffObjsCursor::get_key_type_for_instr_staff(int iInstr, int iStaff)
{
    ImoKeySignature* pKey = get_key_for_instr_staff(iInstr, iStaff);
    if (pKey)
        return pKey->get_key_type();
    else
        return k_key_undefined;
}

//---------------------------------------------------------------------------------------
ImoTimeSignature* StaffObjsCursor::get_applicable_time_signature()
{
    if (m_fScoreIsEmpty)
        return NULL;
    else
        return get_time_signature_for_instrument( num_instrument() );
}

//---------------------------------------------------------------------------------------
ImoTimeSignature* StaffObjsCursor::get_time_signature_for_instrument(int iInstr)
{
    ColStaffObjsEntry* pEntry = get_time_entry_for_instrument(iInstr);
    if (pEntry)
        return dynamic_cast<ImoTimeSignature*>( pEntry->imo_object() );
    else
        return NULL;
}

//---------------------------------------------------------------------------------------
ColStaffObjsEntry* StaffObjsCursor::get_time_entry_for_instrument(int iInstr)
{
    return m_times[iInstr];
}

//---------------------------------------------------------------------------------------
void StaffObjsCursor::save_position()
{
    m_savedPos = m_scoreIt;
}

//---------------------------------------------------------------------------------------
void StaffObjsCursor::go_back_to_saved_position()
{
    m_scoreIt = m_savedPos;
}

//---------------------------------------------------------------------------------------
TimeUnits StaffObjsCursor::next_staffobj_timepos()
{
    ColStaffObjsEntry* pNextEntry = next_entry();
    if (pNextEntry)
        return pNextEntry->time();
    else
    {
        ImoStaffObj* pSO = get_staffobj();
        return time() + pSO->get_duration();
    }
}


}  //namespace lomse

