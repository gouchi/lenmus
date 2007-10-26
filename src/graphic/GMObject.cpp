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
#pragma implementation "GMObject.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GMObject.h"
#include "../app/Paper.h"
#include "../score/StaffObj.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//========================================================================================
//Implementation of class lmGMObject: the root object for the graphical model
//========================================================================================

static int m_IdCounter = 0;        //to assign unique IDs to GMObjects



lmGMObject::lmGMObject(lmEGMOType nType)
{
    m_nId = m_IdCounter++;      // give it an ID
    m_nType = nType;            // save its type

	//initializations
	m_uBoundsBottom = lmUPoint(0.0, 0.0);
    m_uBoundsTop = lmUPoint(0.0, 0.0);
}

lmGMObject::~lmGMObject()
{
}

bool lmGMObject::ContainsPoint(lmUPoint& pointL)
{
    //returns true if point received is within the limits of this Box
    return GetBounds().Contains(pointL);

}

void lmGMObject::DrawBoundsRectangle(lmPaper* pPaper, wxColour color)
{
    //draw a border around instrSlice region
    pPaper->SketchRectangle(m_uBoundsTop,
                            lmUSize(m_uBoundsBottom.x - m_uBoundsTop.x, m_uBoundsBottom.y - m_uBoundsTop.y),
                            color);

}




//========================================================================================
//Implementation of class lmBox: the container objects root
//========================================================================================


lmBox::lmBox(lmEGMOType nType) : lmGMObject(nType)
{
}

lmBox::~lmBox()
{
}




//========================================================================================
// Implementation of class lmShape: any renderizable object, such as a line,
// a glyph, a note head, an arch, etc.
//========================================================================================


lmShape::lmShape(lmEGMOType nType, lmObject* pOwner)
	: lmGMObject(nType)
{
	m_pOwner = pOwner;
}

lmShape::~lmShape()
{
}




//========================================================================================
// Implementation of class lmSimpleShape
//========================================================================================

lmSimpleShape::lmSimpleShape(lmEGMOType nType, lmObject* pOwner)
	: lmShape(nType, pOwner)
{
}

lmSimpleShape::~lmSimpleShape()
{
}



//========================================================================================
// Implementation of class lmCompositeShape
//========================================================================================


lmCompositeShape::lmCompositeShape(lmEGMOType nType, lmObject* pOwner)
	: lmShape(nType, pOwner)
{
    m_fGrouped = true;	//by default all constituent shapes are grouped
}

lmCompositeShape::~lmCompositeShape()
{
    //delete Components collection
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        delete m_Components[i];
    }
    m_Components.clear();
}

void lmCompositeShape::Add(lmShape* pShape)
{
    m_Components.push_back(pShape);

    ////compute new selection rectangle by union of individual selection rectangles
    //m_uSelRect.Union(pShape->GetSelRectangle());

    ////! @todo add boundling rectangle to bounds rectangle list
    //m_uBoundsRect = m_uSelRect;

}
