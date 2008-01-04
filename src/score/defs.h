//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
//    This file contains some code copied (and modified) from file include/wx/gdicmn.h
//    from wxWidgets 2.8.0 project:
//      - class lmURect is based in wxRect code.
//      - class lmUSize is based in wxSize code.
//    Author:       Julian Smart
//    Copyright (c) Julian Smart
//
//-------------------------------------------------------------------------------------

// Common definitions for all score kernel

#ifndef __LM_DEFS_H__        //to avoid nested includes
#define __LM_DEFS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "defs.cpp"
#endif

#include <list>

#if defined( __WXMSW__ ) && defined( _DEBUG )
// for debugging: Detecting and isolating memory leaks with Visual C++
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif

#if defined( __WXMSW__ ) && defined( _DEBUG )
#define new DEBUG_NEW
#endif

#include "wx/gdicmn.h"



//// Types used for portability and legibility

//units
typedef float lmLUnits;             // lmPaper logical units.
typedef float lmTenths;             // Staff relative units.
typedef int lmPixels;               // Device units (usually pixels).  32 bits int

typedef wxRealPoint lmUPoint;       // a point in logical units
typedef wxPoint lmDPoint;           // a point in device units


//some limits
#define lmMAX_VOICE		8		//num of voices supported

// DC user mode
#define lmDC_MODE    wxMM_LOMETRIC    // mode for DC (DC unit: one tenth of mm)

// factor to multiply the program internal unit (logical units) so that
// they became DC units: one tenth of mm

//#define lmSCALE      1.0              // logical units: one tenth of millimiter
#define lmSCALE      0.1              // logical units: one cent of millimiter
//#define lmSCALE      0.01             // logical units: one micron

enum lmEUnits {
    lmMICRONS = 0,
    lmMILLIMETERS,
    lmCENTIMETERS,
    lmINCHES,
    lmTENTHS,
	lmLUNITS
};

// defined in TheApp.cpp
extern lmLUnits lmToLogicalUnits(int nValue, lmEUnits nUnits);
extern lmLUnits lmToLogicalUnits(double rValue, lmEUnits nUnits);


enum lmEClefType
{
    lmE_Undefined = 0,
    lmE_Sol,
    lmE_Fa4,
    lmE_Fa3,
    lmE_Do1,
    lmE_Do2,
    lmE_Do3,
    lmE_Do4,
    lmE_Percussion,
    // other clefs not available for exercises
    lmE_Do5,
    lmE_Fa5,
    lmE_Sol1,
    lmE_8Sol,       //8 above
    lmE_Sol8,       //8 below
    lmE_8Fa,        //8 above
    lmE_Fa8,        //8 below

};
// AWARE enum constats lmEClefType are going to be ramdomly generated in object
// Generators. The next constants defines de range.
#define lmMIN_CLEF        lmE_Sol
#define lmMAX_CLEF        lmE_Percussion
// AWARE enum constats lmEClefType are going to be used as indexes in ClefConstrains


enum lmEStemType
{
    lmSTEM_DEFAULT = 0,   //default: as decided by program
    lmSTEM_UP,            //up: force stem up
    lmSTEM_DOWN,          //down: force stem down
    lmSTEM_NONE,          //none: force no stem
    lmSTEM_DOUBLE         //double: force doble line. Direction as decided by program
};

// Beaming: type of beaming
enum lmEBeamType {
    eBeamNone = 0,
    eBeamBegin,
    eBeamContinue,
    eBeamEnd,
    eBeamForward,
    eBeamBackward
};



//-----------------------------------------------------------------------------------------------
// Note type
// This definition of note type has the following properties:
//
//   a)  2 ^ (NoteType - 2)   is the number used as divider in the American English name
//                            of the note. Examples:
//                                crochet (quarter) = 2^(4-2) = 4
//                                quaver (eighth) = 2^(5-2) = 8
//   b)  2 ^ (10 - NoteType)  is the note's duration, relative to the shortest note (256th).
//                            So the longest one (longa) last 1024 units and
//                            the shortest one (256th, semigarrapatea) last 1 unit.
//-----------------------------------------------------------------------------------------------
enum lmENoteType
{
    eLonga = 0,     // es: longa            en-UK: longa                    en-USA: long
    eBreve,         // es: breve, cuadrada  en-UK: breve                    en-USA: double whole
    eWhole,         // es: redonda,         en-UK: semibreve                en_USA: whole
    eHalf,          // es: blanca,          en-UK: minim                    en_USA: half
    eQuarter,       // es: negra,           en-UK: crochet                  en_USA: quarter
    eEighth,        // es: corchea,         en-UK: quaver                   en_USA: eighth
    e16th,          // es: semicorchea,     en-UK: semiquaver               en_USA: 16th
    e32th,          // es: fusa,            en-UK: demisemiquaver           en_USA: 32nd
    e64th,          // es: semifusa,        en-UK: hemidemisemiquaver       en_USA: 64th
    e128th,         // es: garrapatea       en-UK: semihemidemisemiquaver   en_USA: 128th
    e256th          // es: semigarrapatea   en-UK: ?                        en_USA: 256th
};

// to facilitate access to standard notes' duration.
enum lmENoteDuration
{
    eLongaDuration = 1024,
    eBreveDottedDuration = 768,
    eBreveDuration = 512,
    eWholeDottedDuration = 384,
    eWholeDuration = 256,
    eHalfDottedDuration = 192,
    eHalfDuration = 128,
    eQuarterDottedDuration = 96,
    eQuarterDuration = 64,
    eEighthDottedDuration = 48,
    eEighthDuration = 32,
    e16hDottedDuration = 24,
    e16thDuration = 16,
    e32thDottedDuration = 12,
    e32thDuration = 8,
    e64thDottedDuration = 6,
    e64thDuration = 4,
    e128thDottedDuration = 3,
    e128thDuration = 2,
    e256thDuration = 1
};



// accidental signs
enum lmEAccidentals
{
    lm_eNoAccidentals = 0,
    eNatural,               // es: becuadro
    lm_eFlat,                  // es: bemol
    lm_eSharp,                 // es: sostenido
    lm_eFlatFlat,              // es: doble bemol
    lm_eDoubleSharp,           // es: doble sostenido (single sign)
    lm_eSharpSharp,            // es: doble sostenido (two sharp signs)
    lm_eNaturalFlat,           // es: becuadro bemol
    lm_eNaturalSharp,          // es: becuadro sostenido
    lm_eQuarterFlat,
    lm_eQuarterSharp,
    lm_eThreeQuartersFlat,
    lm_eThreeQuartersSharp
};

//! key signatures
enum lmEKeySignatures
{
    earmDo = 0,
    earmSol,
    earmRe,
    earmLa,
    earmMi,
    earmSi,
    earmFas,
    earmDos,
    earmDob,
    earmSolb,
    earmReb,
    earmLab,
    earmMib,
    earmSib,
    earmFa,
    //AWARE minor keys must go after major keys. This is exploited in IsMajor() global
    //function
    earmLam,
    earmMim,
    earmSim,
    earmFasm,
    earmDosm,
    earmSolsm,
    earmResm,
    earmLasm,
    earmLabm,
    earmMibm,
    earmSibm,
    earmFam,
    earmDom,
    earmSolm,
    earmRem
};
// AWARE enum constats lmEKeySignatures are going to be ramdomly generated in object
// Generators. The next constant defines de maximum and minimum values.
#define lmMIN_KEY  earmDo
#define lmMAX_KEY  earmRem
#define lmNUM_KEYS lmMAX_KEY - lmMIN_KEY + 1
// AWARE names for key signatures are defined in object lmKeySignature. There is an
// array, named sKeySignatureName, and the key signatures names are defined
// assuming a predefined order in the enum lmEKeySignatures
// AWARE items 'earmDo' to 'earmFa' are used as indexes in DlgCfgEarIntervals.
//  'earmDo' must be 0 and consecutive orden must be kept.


//! TimeSignatureType indicates the signature encoding being used
enum lmETimeSignatureType
{
    eTS_Normal = 1,        // it is a single fraction
    eTS_Common,            // it is 4/4 but represented by a C symbol
    eTS_Cut,            // it is 2/4 but represented by a C/ simbol
    eTS_SingleNumber,    // it is a single number with an implied denominator
    eTS_Multiple,        // multiple fractions, i.e.: 2/4 + 3/8
    eTS_Composite,        // composite fraction, i.e.: 3+2/8
    eTS_SenzaMisura        // no time signature is present
};

enum lmETimeSignature
{
    emtr24 = 1,  //  2/4
    emtr34,      //  3/4
    emtr44,      //  4/4
    emtr68,      //  6/8
    emtr98,      //  9/8
    emtr128,     // 12/8
    emtr28,      //  2/8
    emtr38,      //  3/8
    emtr22,      //  2/2
    emtr32,      //  3/2
};
// AWARE enum constats lmETimeSignature are going to be ramdomly generated in object
// Generators. The next constant defines de maximum and minimum values.
#define lmMIN_TIME_SIGN  emtr24
#define lmMAX_TIME_SIGN  emtr32


// note/rest position on a measure
#define lmUNKNOWN_BEAT  -2      //unknown (possibly, time signature is not set)
#define lmOFF_BEAT      -1      //off-beat


// Placement indicates whether something is above or below another element,
// such as a note or a notation.
enum lmEPlacement
{
    ep_Default = 0,
    ep_Above,
    ep_Below
};

//noteheads
enum lmENoteHeads
{
    enh_Longa = 1,
    enh_Breve,
    enh_Whole,              //Whole note (redonda)
    enh_Half,               //Half note (blanca)
    enh_Quarter,            //Quarter note (negra)
    enh_Cross               //Cross (for percussion) (aspa)
};

// pitch type
enum lmEPitchType
{
    lm_ePitchAbsolute = 0,  // absolute pitch (MusicXML style), equal temperament tuning system
    lm_ePitchRelative,      // relative pitch (LDP style), equal temperament tuning system
    lm_ePitchNotDefined,    // pitch value is not yet defined
};

// barlines
enum lmEBarline
{
    lm_eBarlineSimple = 1,          //thin line
    lm_eBarlineDouble,              //two thin lines
    lm_eBarlineEnd,                 //thin-thick lines
    lm_eBarlineStartRepetition,     //thick-thin-two dots
    lm_eBarlineEndRepetition,       //two dots-thin-thick
    lm_eBarlineStart,               //thick-thin
    lm_eBarlineDoubleRepetition,    //two dots-thin-thin-two dots
	lm_eBarlineEOS,					//Non-visible. For control purposes
};


//-------------------------------------------------------------------------------------
// class lmUSize. Size object in lmLUnits. Based on wxSize
//-------------------------------------------------------------------------------------

class lmUSize
{
public:
    // members are public for compatibility, don't use them directly.
    lmLUnits x, y;

    // constructors
    lmUSize() : x(0.0), y(0.0) { }
    lmUSize(lmLUnits ux, lmLUnits uy) : x(ux), y(uy) { }

    //// no copy ctor or assignment operator - the defaults are ok

    //bool operator==(const lmUSize& sz) const { return x == sz.x && y == sz.y; }
    //bool operator!=(const lmUSize& sz) const { return x != sz.x || y != sz.y; }

    //lmUSize operator+(const lmUSize& sz) const { return lmUSize(x + sz.x, y + sz.y); }
    //lmUSize operator-(const lmUSize& sz) const { return lmUSize(x - sz.x, y - sz.y); }
    //lmUSize operator/(lmLUnits i) const { return lmUSize(x / i, y / i); }
    //lmUSize operator*(lmLUnits i) const { return lmUSize(x * i, y * i); }

    //lmUSize& operator+=(const lmUSize& sz) { x += sz.x; y += sz.y; return *this; }
    //lmUSize& operator-=(const lmUSize& sz) { x -= sz.x; y -= sz.y; return *this; }
    //lmUSize& operator/=(const lmLUnits i) { x /= i; y /= i; return *this; }
    //lmUSize& operator*=(const lmLUnits i) { x *= i; y *= i; return *this; }

    //void IncTo(const lmUSize& sz)
    //    { if ( sz.x > x ) x = sz.x; if ( sz.y > y ) y = sz.y; }
    //void DecTo(const lmUSize& sz)
    //    { if ( sz.x < x ) x = sz.x; if ( sz.y < y ) y = sz.y; }

    //void IncBy(lmLUnits dx, lmLUnits dy) { x += dx; y += dy; }
    //void IncBy(const lmUSize& sz) { IncBy(sz.x, sz.y); }
    //void IncBy(lmLUnits d) { IncBy(d, d); }

    //void DecBy(lmLUnits dx, lmLUnits dy) { IncBy(-dx, -dy); }
    //void DecBy(const lmUSize& sz) { DecBy(sz.x, sz.y); }
    //void DecBy(lmLUnits d) { DecBy(d, d); }


    //lmUSize& Scale(float xscale, float yscale)
    //    { x = (lmLUnits)(x*xscale); y = (lmLUnits)(y*yscale); return *this; }

    //// accessors
    //void Set(lmLUnits xx, lmLUnits yy) { x = xx; y = yy; }
    //void SetWidth(lmLUnits w) { x = w; }
    //void SetHeight(lmLUnits h) { y = h; }

    lmLUnits GetWidth() const { return x; }
    lmLUnits GetHeight() const { return y; }

    //bool IsFullySpecified() const { return x != wxDefaultCoord && y != wxDefaultCoord; }

    //// combine this size with the other one replacing the default (i.e. equal
    //// to wxDefaultCoord) components of this object with those of the other
    //void SetDefaults(const lmUSize& size)
    //{
    //    if ( x == wxDefaultCoord )
    //        x = size.x;
    //    if ( y == wxDefaultCoord )
    //        y = size.y;
    //}

    //// compatibility
    //lmLUnits GetX() const { return x; }
    //lmLUnits GetY() const { return y; }
};


//-------------------------------------------------------------------------------------
// class lmURect code. A real rectangle (lmLUnits). Based on wxRect
//-------------------------------------------------------------------------------------

class lmURect
{
public:
    lmURect()
        : x(0.0), y(0.0), width(0.0), height(0.0)
        { }
    lmURect(float xx, float yy, float ww, float hh)
        : x(xx), y(yy), width(ww), height(hh)
        { }
    lmURect(const wxRealPoint& topLeft, const wxRealPoint& bottomRight);
    lmURect(const wxRealPoint& pt, const wxSize& size)
        : x(pt.x), y(pt.y), width(size.x), height(size.y)
        { }
    lmURect(const wxSize& size)
        : x(0.0), y(0.0), width(size.x), height(size.y)
        { }

    // default copy ctor and assignment operators ok

    float GetX() const { return x; }
    void SetX(float xx) { x = xx; }

    float GetY() const { return y; }
    void SetY(float yy) { y = yy; }

    float GetWidth() const { return width; }
    void SetWidth(float w) { width = w; }

    float GetHeight() const { return height; }
    void SetHeight(float h) { height = h; }

    wxRealPoint GetPosition() const { return wxRealPoint(x, y); }
    void SetPosition( const wxRealPoint &p ) { x = p.x; y = p.y; }

    lmUSize GetSize() const { return lmUSize(width, height); }
    void SetSize( const lmUSize &s ) { width = s.GetWidth(); height = s.GetHeight(); }

    bool IsEmpty() const { return (width <= 0) || (height <= 0); }

    float GetLeft()   const { return x; }
    float GetTop()    const { return y; }
    float GetBottom() const { return y + height - 1; }
    float GetRight()  const { return x + width - 1; }

    void SetLeft(float left) { x = left; }
    void SetRight(float right) { width = right - x + 1; }
    void SetTop(float top) { y = top; }
    void SetBottom(float bottom) { height = bottom - y + 1; }

    wxRealPoint GetTopLeft() const { return GetPosition(); }
    wxRealPoint GetLeftTop() const { return GetTopLeft(); }
    void SetTopLeft(const wxRealPoint &p) { SetPosition(p); }
    void SetLeftTop(const wxRealPoint &p) { SetTopLeft(p); }

    wxRealPoint GetBottomRight() const { return wxRealPoint(GetRight(), GetBottom()); }
    wxRealPoint GetRightBottom() const { return GetBottomRight(); }
    void SetBottomRight(const wxRealPoint &p) { SetRight(p.x); SetBottom(p.y); }
    void SetRightBottom(const wxRealPoint &p) { SetBottomRight(p); }

    wxRealPoint GetTopRight() const { return wxRealPoint(GetRight(), GetTop()); }
    wxRealPoint GetRightTop() const { return GetTopRight(); }
    void SetTopRight(const wxRealPoint &p) { SetRight(p.x); SetTop(p.y); }
    void SetRightTop(const wxRealPoint &p) { SetTopLeft(p); }

    wxRealPoint GetBottomLeft() const { return wxRealPoint(GetLeft(), GetBottom()); }
    wxRealPoint GetLeftBottom() const { return GetBottomLeft(); }
    void SetBottomLeft(const wxRealPoint &p) { SetLeft(p.x); SetBottom(p.y); }
    void SetLeftBottom(const wxRealPoint &p) { SetBottomLeft(p); }

    //// operations with rect
    //lmURect& Inflate(wxCoord dx, wxCoord dy);
    //lmURect& Inflate(const wxSize& d) { return Inflate(d.x, d.y); }
    //lmURect& Inflate(wxCoord d) { return Inflate(d, d); }
    //lmURect Inflate(wxCoord dx, wxCoord dy) const
    //{
    //    lmURect r = *this;
    //    r.Inflate(dx, dy);
    //    return r;
    //}

    //lmURect& Deflate(wxCoord dx, wxCoord dy) { return Inflate(-dx, -dy); }
    //lmURect& Deflate(const wxSize& d) { return Inflate(-d.x, -d.y); }
    //lmURect& Deflate(wxCoord d) { return Inflate(-d); }
    //lmURect Deflate(wxCoord dx, wxCoord dy) const
    //{
    //    lmURect r = *this;
    //    r.Deflate(dx, dy);
    //    return r;
    //}

    //void Offset(wxCoord dx, wxCoord dy) { x += dx; y += dy; }
    //void Offset(const wxRealPoint& pt) { Offset(pt.x, pt.y); }

    lmURect& Intersect(const lmURect& rect);
    lmURect Intersect(const lmURect& rect) const
    {
        lmURect r = *this;
        r.Intersect(rect);
        return r;
    }

    lmURect& Union(const lmURect& rect);
    lmURect Union(const lmURect& rect) const
    {
        lmURect r = *this;
        r.Union(rect);
        return r;
    }

    //// compare rectangles
    //bool operator==(const lmURect& rect) const;
    //bool operator!=(const lmURect& rect) const { return !(*this == rect); }

    // return true if the point is (not strictly) inside the rect
    bool Contains(float x, float y) const;
    bool Contains(const wxRealPoint& pt) const { return Contains(pt.x, pt.y); }
    // return true if the rectangle is (not strictly) inside the rect
    bool Contains(const lmURect& rect) const;

    // return true if the rectangles have a non empty intersection
    bool Intersects(const lmURect& rect) const;


    //// these are like Union() but don't ignore empty rectangles
    //lmURect operator+(const lmURect& rect) const;
    //lmURect& operator+=(const lmURect& rect)
    //{
    //    *this = *this + rect;
    //    return *this;
    //}


    //// centre this rectangle in the given (usually, but not necessarily,
    //// larger) one
    //lmURect CentreIn(const lmURect& r, float dir = wxBOTH) const
    //{
    //    return lmURect(dir & wxHORIZONTAL ? r.x + (r.width - width)/2 : x,
    //                  dir & wxVERTICAL ? r.y + (r.height - height)/2 : y,
    //                  width, height);
    //}

    //lmURect CenterIn(const lmURect& r, float dir = wxBOTH) const
    //{
    //    return CentreIn(r, dir);
    //}

public:
    float x, y, width, height;
};


// conversions
#define lmUPointToPoint(uPoint) \
        wxPoint((int)floor(uPoint.x + 0.5), (int)floor(uPoint.y + 0.5))
#define lmDPointToPoint(vPoint) \
        vPoint

#define lmUSizeToSize(uSize) \
        wxSize((int)floor(uSize.x + 0.5), (int)floor(uSize.y + 0.5))



//---------------------------------------------------------------------------------------
//Location data
//---------------------------------------------------------------------------------------

enum lmELocationType
{
	lmLOCATION_DEFAULT = 0,			//user didn't specify location
	lmLOCATION_USER_RELATIVE,		//user specified a relative location
    lmLOCATION_USER_ABSOLUTE,		//user specified an absolute location
	lmLOCATION_COMPUTED,			//computed default location
};

typedef struct lmLocationStruct {
    float x;
    float y;
    lmELocationType xType;
    lmELocationType yType;
    lmEUnits xUnits;
    lmEUnits yUnits;
} lmLocation;

//global variables used as default initializators
extern lmLocation g_tDefaultPos;          // defined in StaffObj.cpp



//---------------------------------------------------------------------------------------
//Helper functions for common operations
//---------------------------------------------------------------------------------------

extern double round(double val);
extern float round(float val);

// string methods

extern bool StrToDouble(wxString sValue, double* pNumber);
extern wxString DoubleToStr(double rNumber, int nDecimalDigits);



#endif    // __LM_DEFS_H__
