/*
 * unionFind : Classe pour manipuler des arbres union-find.
 * Copyright (C) 2008, Guillaume Damiand, guillaume.damiand@liris.cnrs.fr
 *               http://liris.cnrs.fr
 *
 * This file is part of unionFind.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//******************************************************************************
#ifndef UNION_FIND_HH
#define UNION_FIND_HH
//******************************************************************************
#include <cassert>
#include <cstdio>
//******************************************************************************
/// Classe permettant de manipuler des arbres union-find (cf article
/// de Tarjan). Un élément connait son père, sa classe d'équivalence
/// est représentée par la racine de l'arbre et se retrouve avec la
/// méthode find. La fusion de deux arbres s'effectue avec la méthode merge.
//******************************************************************************
class CUFTree
{
public:
    /// Constructeur, par défaut l'élément est seul dans sa classe.
    CUFTree();

    /// Constructeur avec initialisation du père. L'élément est dans la
    /// même classe que ATree.
    CUFTree( CUFTree* ATree );

    /// Destructeur. Ne détruit pas le père car la destruction doit
    /// se faire par un autre mécanisme. En effet, ici on ne connait pas
    /// les feuilles...
    ~CUFTree();

    /// @return le nombre d'éléments dans l'ensemble. Attention, valide
    /// uniquement pour les racines des arbres.
    unsigned int getNbElements() const;

    /// @return l'élément racine dans lequel se trouve l'élément
    /// courant. De plus, utilise l'heuristique de compression du chemin.
    CUFTree* find();

    /// Fusionne l'arbre contenant this avec l'arbre contenant ATree.
    /// Met le plus petit arbre sous le plus gros.
    /// Si this et ATree sont déjà dans le même arbre, ne fait rien.
    void merge( CUFTree& ATree );

    /// Fusionne les arbres UF mais ici avec un pointeur au lieu
    /// d'une référence. @see merge
    void merge( CUFTree* ATree );

private:
    CUFTree* FFather;      ///< Le père, this pour les racines.
    unsigned int FHauteur; ///< Borne sup sur la hauteur de l'arbre (valables
    ///< seulement pour les racines)
    unsigned int FNbElements; ///< Nb d'éléments dans l'ensemble (valables
    /// seulement pour les racines)
};
//******************************************************************************
//* Ici le code, tout est en inline. *******************************************
//******************************************************************************
inline
CUFTree::CUFTree() :
        FFather( this ),
        FHauteur( 0 ),
        FNbElements( 1 )
{}
//------------------------------------------------------------------------------
inline
CUFTree::CUFTree( CUFTree* ATree ) :
        FFather( ATree ),
        FHauteur( 0 ),
        FNbElements( 1 )
{ ++ATree->find()->FNbElements; }
//------------------------------------------------------------------------------
inline
CUFTree::~CUFTree()
{}
//------------------------------------------------------------------------------
inline
unsigned int CUFTree::getNbElements() const
{ return FNbElements; }
//******************************************************************************
inline
CUFTree* CUFTree::find()
{
    CUFTree* res = FFather;
    while ( res->FFather != res )
    {
        res = res->FFather;
    }

    CUFTree* actu = this;
    CUFTree* next = NULL;
    while ( actu != res )
    {
        next = actu->FFather;
        actu->FFather = res;
        actu = next;
    }
    return res;
}
//******************************************************************************
inline
void CUFTree::merge( CUFTree& ATree )
{
    CUFTree* tree1 = find();
    CUFTree* tree2 = ATree.find();

    if ( tree1 == tree2 ) return; // Les 2 arbres sont déjà dans la même classe.

    // Lors de la fusion, on met le plus petit sous le plus grand.
    if ( tree1->FHauteur < tree2->FHauteur )
    {
        tree1->FFather = tree2;
        tree2->FNbElements += tree1->FNbElements;
    }
    else
    {
        tree2->FFather = tree1;
        tree1->FNbElements += tree2->FNbElements;
        if ( tree1->FHauteur == tree2->FHauteur )
            ++tree1->FHauteur; // Ici la hauteur de l'arbre 1 augmente.
    }
}
//******************************************************************************
inline
void CUFTree::merge( CUFTree* ATree )
{ merge( *ATree ); }
//******************************************************************************
#endif // UNION_FIND_HH
//******************************************************************************
