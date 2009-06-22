//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
//-------------------------------------------------------------------------------------

#include "HarmonyExercisesData.h"


/*---
 TODO:
   Idea:
    comprobar siempre lo que identifica un acorde:
      - tipo lmEChordType ( <==> intervalos )
      - root note 
      - inversion
      opciones: num. measures

---*/
int  nHarmonyExercise1ChordsToCheck = 0;
lmDPitch nExercise1NotesDPitch[nMAX_E1BCHORDS];
lmEChordType nExercise1ChordType[nMAX_E1BCHORDS];
int nHarmonyExcerciseType;

/*---- TODO: REMOVE
// Exercise 2 checks:
//   root note
//   chord type
// Root notes
const int nMAX_E2BCHORDS = nMAX_E1BCHORDS;
extern int  nHarmonyExercise2ChordsToCheck;
extern lmDPitch nExercise2BassNotesDPitch[nMAX_E2BCHORDS];
extern lmEChordType nExercise2ChordType[nMAX_E2BCHORDS];  --*/
