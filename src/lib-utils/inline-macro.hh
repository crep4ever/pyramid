/*
 * Copyright (C) Moka Team, moka@sic.univ-poitiers.fr
 *               http://www.sic.univ-poitiers.fr/moka/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

//******************************************************************************
#ifndef INLINE_MACRO_HH
#define INLINE_MACRO_HH
//******************************************************************************

#ifdef INLINE_FLAG

// Compilation inline : les .icc sont inclus dans les .hh et pas dans les .cc
#define INLINE inline
#define INCLUDE_HEADER(file) "inline-macro.hh"
#define INCLUDE_INLINE(file) file
#define INCLUDE_NON_INLINE(file) "inline-macro.hh"

#else

// Compilation non-inline : c'est l'inverse
#define INLINE
#define INCLUDE_HEADER(file) file
#define INCLUDE_INLINE(file) "inline-macro.hh"
#define INCLUDE_NON_INLINE(file) file

#endif // INLINE_FLAG

//******************************************************************************
#endif // INLINE_MACRO_HH
//******************************************************************************
