//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Copyright (c) 2010-2015 Cecilio Salmeron. All rights reserved.
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

#include "lomse_glyphs.h"

#include "lomse_injectors.h"


namespace lomse
{


//---------------------------------------------------------------------------------------
//the glyphs table for SMuFL compliant fonts
//IMPORTANT: The table inicialization values MUST be ordered following the
//          enum EGlyphIndex, defined in lomse_glyphs.h
//---------------------------------------------------------------------------------------
const GlyphData m_glyphs_smufl[] =
{
//Notheads (U+E0A0 - U+E0FF)
    GlyphData(0xE95D),  // Longa note
    GlyphData(0xE0A0),  // Breve note, Double whole
    GlyphData(0xE0A2),  // Whole note
    GlyphData(0xE0A3),  // Half note
    GlyphData(0xE0A4),  // Quarter note notehead
    GlyphData(0xE0A9),  // Cross notehead

//Individual notes (U+E1D0 - U+E1EF)
    GlyphData(0xE1D4),  // Half note, stem down
    GlyphData(0xE1D3),  // Half note, stem up
    GlyphData(0xE1D6),  // Quarter note, stem down
    GlyphData(0xE1D5),  // Quarter note, stem up
    GlyphData(0xE1D8),  // Eight note, flag bottom
    GlyphData(0xE1D7),  // Eight note, flag top
    GlyphData(0xE1DA),  // 16th note, flag bottom
    GlyphData(0xE1D9),  // 16th note, flag top
    GlyphData(0xE1DC),  // 32nd note, flag bottom
    GlyphData(0xE1DB),  // 32nd note, flag top
    GlyphData(0xE1DE),  // 64th note, flag bottom
    GlyphData(0xE1DD),  // 64th note, flag top)),
    GlyphData(0xE1E0),  // 128th note, flag bottom
    GlyphData(0xE1DF),  // 128th note, flag top)),
    GlyphData(0xE1E2),  // 256th note, flag bottom
    GlyphData(0xE1E1),  // 256th note, flag top)),
    GlyphData(0xE1E7),  // Dot (for dotted notes)

//Articulation (U+E4A0 - U+E4BF)
    GlyphData(0xE4A0),  // accent_above,
    GlyphData(0xE4A1),  // accent_below,
    GlyphData(0xE4A2),  // staccato_above,
    GlyphData(0xE4A3),  // staccato_below,
    GlyphData(0xE4A4),  // tenuto_above,
    GlyphData(0xE4A5),  // tenuto_below,
    GlyphData(0xE4A6),  // staccatissimo_above,
    GlyphData(0xE4A7),  // staccatissimo_below,
    GlyphData(0xE4A8),  // staccatissimo_wedge_above,
    GlyphData(0xE4A9),  // staccatissimo_wedge_below,
    GlyphData(0xE4AA),  // staccatissimo_stroke_above,
    GlyphData(0xE4AB),  // staccatissimo_stroke_below,
    GlyphData(0xE4AC),  // marcato_above,
    GlyphData(0xE4AD),  // marcato_below,
    GlyphData(0xE4AE),  // marcato_staccato_above,
    GlyphData(0xE4AF),  // marcato_staccato_below,
    GlyphData(0xE4B0),  // accent_staccato_above,
    GlyphData(0xE4B1),  // accent_staccato_below,
    GlyphData(0xE4B2),  // tenuto_staccato_above,
    GlyphData(0xE4B3),  // tenuto_staccato_below,
    GlyphData(0xE4B4),  // tenuto_accent_above,
    GlyphData(0xE4B5),  // tenuto_accent_below,
    GlyphData(0xE4B6),  // stress_above,
    GlyphData(0xE4B7),  // stress_below,
    GlyphData(0xE4B8),  // unstress_above,
    GlyphData(0xE4B9),  // unstress_below,
    GlyphData(0xE4BA),  // laissez_vibrer_above,
    GlyphData(0xE4BB),  // laissez_vibrer_below,
    GlyphData(0xE4BC),  // marcato_tenuto_above,
    GlyphData(0xE4BD),  // marcato_tenuto_below,

//Holds and pauses (U+E4C0 - U+E4DF)
    GlyphData(0xE4C0),  // Fermata above (arch)
    GlyphData(0xE4C1),  // Fermata below (arch)
    GlyphData(0xE4C4),  // Fermata above (angled)
    GlyphData(0xE4C5),  // Fermata below (angled)
    GlyphData(0xE4C6),  // Fermata above (quare)
    GlyphData(0xE4C7),  // Fermata below (quare)
    GlyphData(0xE4CE),  // breath-mark (comma)
    GlyphData(0xE4CF),  // breath-mark (tick)
    GlyphData(0xE4D0),  // breath-mark (V)
    GlyphData(0xE4D1),  // caesura

//Rests (U+E4E0 - U+E4FF)
    GlyphData(0xE4E1),  // Longa rest     //larga
    GlyphData(0xE4E2),  // Breve rest, Double Whole   //breve, cuadrada
    GlyphData(0xE4E3),  // Whole rest    //whole redonda
    GlyphData(0xE4E4),  // Half rest     //half blanca
    GlyphData(0xE4E5),  // Quarter rest  //quarter negra
    GlyphData(0xE4E6),  // Eight rest    //eighth corchea
    GlyphData(0xE4E7),  // 16th rest     //16th semicorchea
    GlyphData(0xE4E8),  // 32nd rest     //32nd fusa
    GlyphData(0xE4E9),  // 64th rest     //64th semifusa
    GlyphData(0xE4EA),  // 128th rest    //128th garrapatea
    GlyphData(0xE4EB),  // 256th rest    //256th semigarrapatea

//flags for notes (U+E240 - U+E25F)
    GlyphData(0xE241),  // Eight note flag down
    GlyphData(0xE243),  // 16th note flag down
    GlyphData(0xE245),  // 32nd note flag down
    GlyphData(0xE247),  // 64th note flag down
    GlyphData(0xE249),  // 128th note flag down
    GlyphData(0xE24B),  // 256th note flag down
    GlyphData(0xE240),  // Eight note flag up
    GlyphData(0xE242),  // 16th note flag up
    GlyphData(0xE244),  // 32nd note flag up
    GlyphData(0xE246),  // 64th note flag up)),
    GlyphData(0xE248),  // 128th note flag up
    GlyphData(0xE24A),  // 256th note flag up)),

//standard accidentals (U+E260 - U+E26F)
    GlyphData(0xE261),  // Natural accidental
    GlyphData(0xE262),  // Sharp accidental
    GlyphData(0xE260),  // Flat accidental
    GlyphData(0xE263),  // Double sharp accidental
    GlyphData(0xE264),  // Double flat accidental
    GlyphData(0xE26A),  // open_cautionary_accidental,   Accidental parenthesis, left
    GlyphData(0xE26B),  // close_cautionary_accidental   Accidental parenthesis, right

//clefs (U+E050 - U+E07F)
    GlyphData(0xE050),  // G clef
    GlyphData(0xE062),  // F clef
    GlyphData(0xE05C),  // C clef
    GlyphData(0xE06A),  // Percussion clef, block
    GlyphData(0xE052),  // G clef ottava bassa
    GlyphData(0xE053),  // G clef ottava alta
    GlyphData(0xE051),  // G clef quindicesima_bassa,
    GlyphData(0xE054),  // G clef quindicesima_alta
    GlyphData(0xE064),  // F clef ottava bassa
    GlyphData(0xE065),  // F clef ottava alta
    GlyphData(0xE063),  // F clef quindicesima_bassa,
    GlyphData(0xE066),  // F clef quindicesima_alta,

//time signatures (U+E080 - U+E09F)
    GlyphData(0xE080),  // Number 0
    GlyphData(0xE081),  // Number 1
    GlyphData(0xE082),  // Number 2
    GlyphData(0xE083),  // Number 3
    GlyphData(0xE084),  // Number 4
    GlyphData(0xE085),  // Number 5
    GlyphData(0xE086),  // Number 6
    GlyphData(0xE087),  // Number 7
    GlyphData(0xE088),  // Number 8
    GlyphData(0xE089),  // Number 9
    GlyphData(0xE08A),  // COMMON_TIME
    GlyphData(0xE08B),  // CUT_TIME

//Metronome marks (U+ECA0 - U+ECBF)
    GlyphData(0xECA2),  // small whole note
    GlyphData(0xECA3),  // small half note up
    GlyphData(0xECA5),  // small quarter note up
    GlyphData(0xECA7),  // small eighth note up
    GlyphData(0xECA9),  // small 16th note up
    GlyphData(0xECAB),  // small 32th note up
    GlyphData(0xECAD),  // small 64th note up
    GlyphData(0xECAF),  // small 128th note up
    GlyphData(0xECB1),  // small 256th note up
    GlyphData(0xECB7),  // augmentation dot

//Repeats (U+E040 - U+E04F)
    GlyphData(0xE046),  // Da Capo sign
    GlyphData(0xE045),  // Dal Segno sign
    GlyphData(0xE048),  // Coda sign
    GlyphData(0xE047),  // Segno sign

//Octaves (U+E510 - U+E51F)
    GlyphData(0xE510),  // Ottava alta sign

//Brass techniques (U+E5D0 - U+E5EF)
    GlyphData(0xE5D0),  // Scoop
    GlyphData(0xE5D5),  // Doit, medium
    GlyphData(0xE5D8),  // Lip fall, medium
    GlyphData(0xE5E0),  // Plop

//figured bass (U+EA50 - U+EA6F)
    GlyphData(0xEA50),  // Figured bass. Number 0
    GlyphData(0xEA51),  // Figured bass. Number 1
    GlyphData(0xEA52),  // Figured bass. Number 2
    GlyphData(0xEA54),  // Figured bass. Number 3
    GlyphData(0xEA55),  // Figured bass. Number 4
    GlyphData(0xEA57),  // Figured bass. Number 5
    GlyphData(0xEA5B),  // Figured bass. Number 6
    GlyphData(0xEA5D),  // Figured bass. Number 7
    GlyphData(0xEA60),  // Figured bass. Number 8
    GlyphData(0xEA61),  // Figured bass. Number 9
    GlyphData(0xEA66),  // Figured bass. Sharp symbol
    GlyphData(0xEA64),  // GLYPH_FIGURED_BASS_FLAT,                //Flat symbol
    GlyphData(0xEA65),  // GLYPH_FIGURED_BASS_NATURAL,             //Natural symbol
    GlyphData(0xEA6C),  // GLYPH_FIGURED_BASS_PLUS,                // +
        //TODO: Figured bass minus sign does not exists!
    GlyphData(0xEA6D),  // GLYPH_FIGURED_BASS_MINUS,               // -
    GlyphData(0xEA6A),  // GLYPH_FIGURED_BASS_OPEN_PARENTHESIS,    // (
    GlyphData(0xEA6B),  // GLYPH_FIGURED_BASS_CLOSE_PARENTHESIS,   // )
    GlyphData(0xEA5F),  // GLYPH_FIGURED_BASS_7_STRIKED,           // 7 with overlayered /

//Dynamics (U+E520 - U+E54F)
    GlyphData(0xE520),  // dynamic Piano                p
    GlyphData(0xE521),  // dynamic Mezzo                m
    GlyphData(0xE522),  // dynamic Forte                f
    GlyphData(0xE523),  // dynamic Rinforzando          r
    GlyphData(0xE524),  // dynamic Sforzando            s
    GlyphData(0xE525),  // dynamic Z                    z
    GlyphData(0xE526),  // dynamic Niente               n
    GlyphData(0xE527),  // dynamic PPPPPP               pppppp
    GlyphData(0xE528),  // dynamic PPPPP                ppppp
    GlyphData(0xE529),  // dynamic PPPP                 pppp
    GlyphData(0xE52A),  // dynamic PPP                  ppp
    GlyphData(0xE52B),  // dynamic PP                   pp
    GlyphData(0xE52C),  // dynamic MP                   mp
    GlyphData(0xE52D),  // dynamic MF                   mf
    GlyphData(0xE52E),  // dynamic PF                   pf
    GlyphData(0xE52F),  // dynamic FF                   ff
    GlyphData(0xE530),  // dynamic FFF                  fff
    GlyphData(0xE531),  // dynamic FFFF                 ffff
    GlyphData(0xE532),  // dynamic FFFFF                fffff
    GlyphData(0xE533),  // dynamic FFFFFF               ffffff
    GlyphData(0xE534),  // dynamic FortePiano           fp
    GlyphData(0xE535),  // dynamic Forzando             fz
    GlyphData(0xE536),  // dynamic Sforzando1           sf
    GlyphData(0xE537),  // dynamic SforzandoPiano       sfp
    GlyphData(0xE538),  // dynamic SforzandoPianissimo  sfpp
    GlyphData(0xE539),  // dynamic Sforzato             sfz
    GlyphData(0xE53A),  // dynamic SforzatoPiano        sfzp
    GlyphData(0xE53B),  // dynamic SforzatoFF           sffz
    GlyphData(0xE53C),  // dynamic Rinforzando1         rf
    GlyphData(0xE53D),  // dynamic Rinforzando2         rfz

//Common ornaments (U+E560 - U+E56F)
    GlyphData(0xE560),  // Slashed grace note stem up
    GlyphData(0xE561),  // Slashed grace note stem down
    GlyphData(0xE562),  // Grace note stem up
    GlyphData(0xE563),  // Grace note stem down
    GlyphData(0xE564),  // Slash for stem up grace note
    GlyphData(0xE565),  // Slash for stem down grace note
    GlyphData(0xE566),  // Trill
    GlyphData(0xE567),  // Turn
    GlyphData(0xE568),  // Inverted Turn
    GlyphData(0xE569),  // Turn with slash
    GlyphData(0xE56A),  // Turn up
    GlyphData(0xE56B),  // Inverted turn up
    GlyphData(0xE56C),  // Mordent
    GlyphData(0xE56D),  // Inverted mordent
    GlyphData(0xE56E),  // Tremblement
    GlyphData(0xE56F),  // Haydn ornament

//Other baroque ornaments (U+E570 - U+E58F)
    GlyphData(0xE587),  // Schleifer

//Multi-segment lines (U+EAA0 - U+EB0F)
    GlyphData(0xEAA4),  // Trill wiggle segment

//Tremolos (U+E220 - U+E23F)
    GlyphData(0xE220),  // Combining tremolo 1
    GlyphData(0xE221),  // Combining tremolo 2
    GlyphData(0xE222),  // Combining tremolo 3
    GlyphData(0xE223),  // Combining tremolo 4
    GlyphData(0xE224),  // Combining tremolo 5

//Wind techniques (U+E5F0 - U+E60F)
    GlyphData(0xE5F0),  // Double-tongue above
    GlyphData(0xE5F1),  // Double-tongue below
    GlyphData(0xE5F2),  // Triple-tongue above
    GlyphData(0xE5F3),  // Triple-tongue below
    GlyphData(0xE5F4),  // Closed hole
    GlyphData(0xE5F5),  // Three-quarters closed hole
    GlyphData(0xE5F6),  // Half-closed hole
    GlyphData(0xE5F7),  // Half-closed hole 2
    GlyphData(0xE5F8),  // Half-open hole
    GlyphData(0xE5F9),  // Open hole

//String techniques (U+E610 - U+E62F)
    GlyphData(0xE610),  // Down bow
    GlyphData(0xE612),  // Up bow
    GlyphData(0xE614),  // Harmonic
    GlyphData(0xE615),  // Half-harmonic

//Keyboard techniques (U+E650 - U+E67F)
    GlyphData(0xE650),  // Pedal mark
    GlyphData(0xE655),  // Pedal up mark
    GlyphData(0xE656),  // Half-pedal mark

//Handbells (U+E810 - U+E82F)
    GlyphData(0xE810),  // Martellato
    GlyphData(0xE811),  // Martellato lift
    GlyphData(0xE812),  // Hand martellato
    GlyphData(0xE813),  // Muted martellato
    GlyphData(0xE814),  // Mallet, bell suspended
    GlyphData(0xE815),  // Mallet, bell on table
    GlyphData(0xE816),  // Mallet lift
    GlyphData(0xE817),  // Pluck lift
    GlyphData(0xE818),  // Swing up
    GlyphData(0xE819),  // Swing down
    GlyphData(0xE81A),  // Swing
    GlyphData(0xE81B),  // Echo
    GlyphData(0xE81C),  // Echo 2
    GlyphData(0xE81D),  // Gyro
    GlyphData(0xE81E),  // Damp 3
    GlyphData(0xE81F),  // Belltree
    GlyphData(0xE820),  // Table single handbell
    GlyphData(0xE821),  // Table pair of handbells

};


//=======================================================================================
// MusicGlyphs implementation

MusicGlyphs::MusicGlyphs(LibraryScope* pLibScope)
    : m_pLibScope(pLibScope)
{
    update();
}

//---------------------------------------------------------------------------------------
void MusicGlyphs::update()
{
    if (m_pLibScope->is_music_font_smufl_compliant())
        m_glyphs = &m_glyphs_smufl[0];
}

}  //namespace lomse
