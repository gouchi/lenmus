//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Copyright (c) 2010-2013 Cecilio Salmeron. All rights reserved.
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

#ifndef __LOMSE_SHAPES_STORAGE_H__        //to avoid nested includes
#define __LOMSE_SHAPES_STORAGE_H__

#include "lomse_basic.h"
#include <map>
#include <list>
using namespace std;

namespace lomse
{

//forward declarations
class GmoShape;
class GmoBox;
class RelAuxObjEngraver;
class ImoObj;


// helper class to store shapes under construction and its engravers
//---------------------------------------------------------------------------------------
class ShapesStorage
{
protected:
	std::map<ImoObj*, RelAuxObjEngraver*> m_engravers;
	std::list< pair<GmoShape*, int> > m_readyShapes;

public:
    ShapesStorage() {}
    ~ShapesStorage();

    //engravers
    inline void save_engraver(RelAuxObjEngraver* pEngrv, ImoObj* pImo) {
        m_engravers[pImo] = pEngrv;
    }
    RelAuxObjEngraver* get_engraver(ImoObj* pImo);
    inline void remove_engraver(ImoObj* pImo) { m_engravers.erase(pImo); }
    void shape_ready_for_gmodel(ImoObj* pImo, int layer);

    //final shapes
    void add_ready_shapes_to_model(GmoBox* pBox);
    void delete_ready_shapes();

    //suppor for debug and unit tests
    void delete_engravers();

};


}   //namespace lomse

#endif    // __LOMSE_SHAPES_STORAGE_H__

