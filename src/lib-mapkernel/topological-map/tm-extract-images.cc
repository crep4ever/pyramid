/*
 * lib-mapkernel : Un noyau de 2-cartes et des opérations.
 * Copyright (C) Moka Team, moka@sic.univ-poitiers.fr
 *               http://www.sic.univ-poitiers.fr/moka/
 *
 * This file is part of lib-mapkernel
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
#include "inline-macro.hh"
#include "topological-map.hh"
#include "limited-stack.hh"
#include "orders.hh"
#include "coverage-edge-linels.hh"
#include INCLUDE_NON_INLINE("tm-extract-images.icc")
//******************************************************************************
using namespace Map2d;
using namespace std;
//******************************************************************************
CChrono Map2d::chronoShiftEdges;
CChrono Map2d::chronoIsEdgeToremove;
CChrono Map2d::chronoVertexRemoval;
long int nbAppelCanRemoveEdge = 0;
long int nbEdgeParcouru = 0;
//******************************************************************************
CDart* CTopologicalMap::makeBorder(int ALargX, int ALargY)
{
#ifdef DEBUG_EXTRACT_IMAGE
   std::cout << "CTopologicalMap::makeBorder " << ALargX << " " << ALargY
   << " ..." << std::flush;
#endif

#ifdef DISPLAY_TIME_EXTRACT_IMAGE
   CChrono c;
   c.start();
#endif

   CDart *first, *last, *tmp;
   int x, y;

   /* Brin à gauche du premier pixel. */
   CDoublet Doublet(0, 0, YPOS);
   first = addMapDart(Doublet, FInclusionTreeRoot);

   /* Ajout du brin de la diagonale */
   Doublet.setY(1);
   tmp = addMapDart(Doublet, FInclusionTreeRoot);
   linkBeta1(first, tmp);
   last = tmp;

   /* Création du bordsuperieur de l'image. */
   x = ALargX;
   y = 0;

   /*initialisation du doublet*/
   Doublet.setY(y);
   Doublet.setLinel(XNEG);

   while (x > 0)
   {
      Doublet.setX(x);
      tmp = addMapDart(Doublet, FInclusionTreeRoot);
      assert(tmp != NULL);
      linkBeta1(last, tmp);
      last = tmp;
      x--;
   }

   //le brin suivant le dernier brin créé est le brin first:
   linkBeta1(last, first); /*on ferme le bord!*/

#ifdef DEBUG_EXTRACT_IMAGE
   std::cout << "OK." << std::endl;
#endif

#ifdef DISPLAY_TIME_EXTRACT_IMAGE
   c.stop();
   c.display("Création du bord supérieur");
#endif

   return first;
}
//******************************************************************************
void CTopologicalMap::destroyBorder(CDart* ADart)
{
#ifdef DISPLAY_TIME_EXTRACT_IMAGE
   CChrono c;
   c.start();
#endif

   CDynamicCoverage1 it(this, ADart);
   while (it.cont())
   {
      delMapDart(it++);
   }

#ifdef DISPLAY_TIME_EXTRACT_IMAGE
   c.stop();
   c.display("Destruction du bord");
#endif
}
//******************************************************************************
void CTopologicalMap::computeInclusionTree()
{
  assert( !FUFTreeMode );
  
#ifdef DEBUG_EXTRACT_IMAGE
   std::cout << "\n CTopologicalMap::computeInclusionTree " << std::endl;
#endif

#ifdef DISPLAY_TIME_EXTRACT_IMAGE
   CChrono c;
   c.start();
#endif

   CRegion* toTreat = FInclusionTreeRoot->getFirstSon();

   // On n'a plus besoin de la liste donc on remet à NULL les 2 champs fils
   // et frere de la racine.
   FInclusionTreeRoot->setFirstSon(NULL);
   FInclusionTreeRoot->setBrother(NULL);

   if ( toTreat==NULL ) return;
   
   CRegion* tmp = NULL;
   CRegion* father = FInclusionTreeRoot;

   CRegion* actu = NULL;
   CRegion* firstRegion = NULL;

   toTreat->setBrother(NULL);
   while (toTreat != NULL)
     {
      actu = toTreat;
      toTreat = toTreat->getFirstSon(); // La prochaine region
      
      if (actu->getFather() == NULL) // Région pas encore traitée.
      {
         assert(actu->getRepresentativeDart()!=NULL);
         assert(!isMarkedDeleted(actu->getRepresentativeDart()));
         
         father = getRegion(beta2(actu->getRepresentativeDart()));
	 assert( father->isInfiniteRegion() ||
		 father->getFirstPixel()<actu->getFirstPixel() );

         CDynamicCoverageCC it(this, actu->getRepresentativeDart());
         while (it.cont())
         {
            tmp = getRegion(*it);

            if (!tmp->isInfiniteRegion() && tmp->getFather() == NULL)
            {
	      assert( father->isInfiniteRegion() ||
		      father->getFirstPixel()<tmp->getFirstPixel() );
	      
              // On va ajouter la région tmp comme fils de father
               // mais avant il faut l'enlever de la "liste" des régions.

               // on met a jour les 2 cellules voisines
               if (tmp->getFirstSon() != NULL)
                  tmp->getFirstSon()->setBrother(tmp->getBrother());

               if (tmp->getBrother() != NULL)
                  tmp->getBrother()->setFirstSon(tmp->getFirstSon());

               // Si tmp était la prochaine région, elle ne l'est plus.
               if (toTreat == tmp) toTreat = tmp->getFirstSon();

               // Maintenant on peut retirer la cellule tmp.
               tmp->setFirstSon(NULL);
               tmp->setBrother(NULL);

               // Enfin on peut ajouter tmp à l'arbre d'inclusion.
               tmp->setProfondeur(father->getProfondeur() + 1);
               // Si c'est la première région de la composante connexe,
               // On l'ajoute en fils de father
               if (firstRegion == NULL)
               {
                  father->addSon(tmp);
                  firstRegion = tmp;
               }
               // Sinon on l'ajoute dans la liste des régions se trouvant
               // dans la même composante connexe de firstRegion
               else
               {
                  firstRegion->addSameCC(tmp);
               }
            }

            ++it;
         }
	 
         firstRegion = NULL;
      }
   }
#ifdef DISPLAY_TIME_EXTRACT_IMAGE
   c.stop();
   c.display("Calcul de l'arbre d'inclusion");
#endif
   
#ifdef DEBUG_EXTRACT_IMAGE
   std::cout << "OK." << std::endl;
#endif
}
//******************************************************************************
CDart* CTopologicalMap::extractTopologicalMapMainLoop(CImage2D* AImage,
      CDart* ALast, unsigned int ALevel)
{
   assert( AImage != NULL );
   assert( AImage->isOk() );
   assert( FUFTreeMode );
   
#ifdef DISPLAY_TIME_EXTRACT_IMAGE
   chronoShiftEdges.reset();
   chronoIsEdgeToremove.reset();
   chronoVertexRemoval.reset();

   CChrono c;
   c.start();

   CChrono chronoProcessEdge;
   CChrono chronoProcessVertex;
#endif

   //déclatration des variables
   CDart* last   = ALast;
   CDart* up   = NULL;
   CDart* nextLast = NULL;
   CDart* vertex   = NULL;  //brin désignant le sommet

   CRegion* currentRegion = NULL;
   bool newRegion  = false;
   CDoublet current;

   // Extraction de la carte.
   for (unsigned int y = 0;y <= AImage->getYSize();++y)
   {
      for (unsigned int x = 0;x <= AImage->getXSize();++x)
      {

#ifdef DEBUG_EXTRACT_IMAGE
         std::cout << "DEBUT traitement du pixel ("
         << x << ", " << y << ")";
         if (y < AImage->getYSize() && x < AImage->getXSize())
            cout << " = " << AImage->getPixel(x, y) << endl;
         else
            cout << " inf " << endl;
#endif

#ifdef DISPLAY_TIME_EXTRACT_IMAGE
         chronoProcessEdge.start();
#endif

         // 1. On calcule les brins up à partir du brin last.
         up = computeUpFromLast(last);

         /*
           On cherche la région d'appartenance du pixel courant. Quand on
           ne la trouve pas, c'est que c'est la première fois qu'on la
           rencontre et donc on la crée.
         */
         currentRegion = NULL;
         newRegion = false;

         if (!AImage->isPixelInImage(x, y))
         {
            currentRegion = FInclusionTreeRoot;
         }
         else if (AImage->samePixelActuLeft(x, y) &&
                  AImage->samePixelActuUp(x, y))
         {
            //si les pixels last et up sont identiques
            //et si les regions de last et up sont différentes
            //alors ces regions doivent être fusionnées.
	   unionRegionRoot(getRegion(last), getRegion(up));
	   currentRegion = getRegion(last);
         }
         else if (AImage->samePixelActuLeft(x, y))
            currentRegion = getRegion(last);
         else if (AImage->samePixelActuUp(x, y))
            currentRegion = getRegion(up);
         else
         {
            // On crée la région.
	   currentRegion = new CRegion(CCoordinate(x,y));
	   ++FNbRegions;
	   newRegion = true;

            // Et on la rajoute à la fin de la liste.
            // Rappel, la dernière cellule est désigné par le frère
            // de FInclusionTreeRoot, et le chainage s'effectue
            // par les pointeur fils et frère.
            FInclusionTreeRoot->getBrother()
            ->setFirstSon(currentRegion); // next

            currentRegion->
            setBrother(FInclusionTreeRoot->getBrother()); // prev
            FInclusionTreeRoot->setBrother(currentRegion);

#ifdef CONTOUR_SIMPLIFICATION
            currentRegion->FMoments.setMap(this);
#endif // CONTOUR_SIMPLIFICATION
         }

         // 2. On crée le doublet correspondant au pixel courant qui
         // est rattaché à la carte déja créé.
         current  = CDoublet(x, y);
         nextLast = createSquareFace(last, up, current, currentRegion);
	 currentRegion->incBoundarySize(4);

         if (!currentRegion->isInfiniteRegion())
         {
            currentRegion->addPixel(FImage->getPixel(x, y),nextLast);
         }

         /* Si on vient de créer la région, on doit
            initialiser son représentant. */
         if (newRegion)
         {         
            currentRegion->setRepresentativeDart(beta2(last));
         }
         
         //3. On conserve un brin (réel) incidents au sommet central.
         vertex = last;

         // 4. On teste d'abord les deux suppressions d'arêtes possibles.
         if ((ALevel > 0 || !AImage->isPixelInImage(x, y)) &&
               AImage->samePixelActuLeft(x, y))
         {
#ifdef DEBUG_EXTRACT_IMAGE
            std::cout << "Same_left " << std::flush;
#endif
            /* Test si c'est le brin représentant le pointel est supprimé*/
            if (beta2(last) == vertex || last == vertex)
               vertex = beta02(vertex);

            topoEdgeRemoval(last);
	    currentRegion->incBoundarySize(-1);
         }
         else
         {
#ifdef DEBUG_EXTRACT_IMAGE
            std::cout << "NoSame_left " << std::flush;
#endif
            current.setLinel(YPOS);
            FKhalimsky->setLCell(current, true);
         }

         if ((ALevel > 0 || !AImage->isPixelInImage(x, y)) &&
               AImage->samePixelActuUp(x, y))
         {
#ifdef DEBUG_EXTRACT_IMAGE
            std::cout << "Same_up " << std::flush;
#endif
            /* Test si c'est le brin représentant le pointel est supprimé*/
            if (beta2(up) == vertex || up == vertex)
            {
               vertex = beta02(vertex);

               if (beta2(up) == vertex || up == vertex)
                  vertex = NULL;
            }

            topoEdgeRemoval(up);
	    currentRegion->incBoundarySize(-1);
         }
         else
         {
#ifdef DEBUG_EXTRACT_IMAGE
            std::cout << "NoSame_up " << std::flush;
#endif
            current.setLinel(XPOS);
            FKhalimsky->setLCell(current, true);
         }

#ifdef DISPLAY_TIME_EXTRACT_IMAGE
         chronoProcessEdge.stop();

         chronoProcessVertex.start();
#endif

         // 5. Et enfin la suppression de sommet lorsque c'est possible.
         if (vertex != NULL && beta1(vertex) != vertex)
            //si on n'est pas dans le cas d'une boucle
         {
            // Si le sommet est de degré 2 et qu'il
            // ne s'agit pas d'un cas dégénéré
            if (ALevel > 1 &&
                  beta02(vertex) == beta21(vertex) &&
                  beta0(vertex) != beta2(vertex))
            {
               vertexRemoval(vertex);
            }
            else
               FKhalimsky->setPCell(current, true);
         }

#ifdef DISPLAY_TIME_EXTRACT_IMAGE
         chronoProcessVertex.stop();
#endif
         last = nextLast;

#ifdef DEBUG_EXTRACT_IMAGE
         std::cout << "FIN traitement du pixel ("
         << x << ", " << y << ") " << std::endl;
#endif
      }
   }

#ifdef DEBUG_EXTRACT_IMAGE
   std::cout << "OK...Fin de la boucle du parcours des pixels." << std::endl;
#endif

#ifdef DISPLAY_TIME_EXTRACT_IMAGE
   c.stop();
   c.display("Extraction de la carte topologique");
   chronoProcessEdge.display("   processEdge");
   chronoProcessVertex.display("   processVertex");
   chronoVertexRemoval.display("   vertexRemoval");
#endif

   /* On détruit le bord initial de l'image. */
   destroyBorder(last);

#ifdef DEBUG_EXTRACT_IMAGE
   std::cout << "OK...Pour la destruction du bord." << std::endl;
#endif

#ifdef DEBUG_EXTRACT_IMAGE
   std::cout << "OK...Pour la suppression des sommets de degré réel 0."
   << std::endl;
#endif

   return last;
}
//******************************************************************************
void CTopologicalMap::relabelDarts()
{
  assert( FUFTreeMode );
  
   // 1. On parcours les brins de la carte pour effectuer une mise à
   //    jour de leur étiquettage
   for (CDynamicCoverageAll It(this);It.cont();++It)
   {
      // Si la région d'appartenance du brin n'est pas racine d'un arbre uf.
      if ( !isRegionRoot(*It) )
      {
	assert(getRegion(*It)->getFirstPixel() <
	       static_cast<CTopologicalDart*>(*It)->getRegion()
	       ->getFirstPixel());

         //on récupère la région avec laquelle il va falloir étiquetter
         //la région courante
         setRegion(*It, getRegion(*It));
      }
   }

   // 2. On parcours maintenant les régions à partir de la première région
   //    contenue dans le fils de FInclusionTreeRoot.
   //    On supprime toutes celles qui sont inutiles et on met à jour
   //    la liste chainée des régions

   //variable de parcours de la chaine
   CRegion* currentRegion = FInclusionTreeRoot->getFirstSon();
   while (currentRegion != NULL)
   {
      //si la région n'est pas racine d'un arbre uf elle doit être supprimée.
      if (  !isRegionRoot(currentRegion) )
      {
         CRegion* TempRegion = currentRegion; //contient la région à supprimer

         //normalement la région infinie n'est pas parcourue
         assert(TempRegion != FInclusionTreeRoot);
         // et il y a au moins une région avant la région courante
         // (au pire la région infinie).
         assert(TempRegion->getBrother() != NULL);

         //on met à jour la liste chainée des régions

         // prev -> next
         TempRegion->getBrother()->setFirstSon(TempRegion->getFirstSon());

         //si on est pas en bout de chaine
         if (TempRegion->getFirstSon() != NULL)
         {
            //prev <- next
            TempRegion->getFirstSon()->setBrother(TempRegion->getBrother());
         }

         //on conserve la région suivante dans la variable currentRegion
         currentRegion = TempRegion->getFirstSon();

         //on détruit la région courante par appel au destructeur.
         //Avant de la détruire on remet les champs de la région à NULL
         //afin de ne pas supprimer l'arbre union
         TempRegion->setFirstSon(NULL);
         TempRegion->setBrother(NULL);
         TempRegion->setNextSameCC(NULL);
         TempRegion->setFather(NULL);

         delete TempRegion;

         //on décrémente le nombre de régions de la carte
         --FNbRegions;
      }
      //sinon on passe directement à la région suivante
      else
      {
         currentRegion = currentRegion->getFirstSon();
      }
   }
}
//******************************************************************************
void CTopologicalMap::extractTopologicalMap(CImage2D* AImage,
      unsigned int ALevel)
{
   assert(AImage != NULL);

   if (! AImage->isOk()) return;

   /* On commence par vider tout. */
   empty();

   /* On initialise l'image, la matrice de Khalimsky et la région infinie. */
   FImage = AImage;

   FKhalimsky = new CKhalimsky(AImage->getXSize(),
                               AImage->getYSize());
   assert(FKhalimsky != NULL);

   // On crée la région infinie.
   FInclusionTreeRoot = new CInfiniteRegion;

#ifdef CONTOUR_SIMPLIFICATION
   FInclusionTreeRoot->FMoments.setMap(this);
#endif // CONTOUR_SIMPLIFICATION

   // Durant le parcours de l'image, on va utiliser FInclusionTreeRoot pour
   // stocker de manière temporaire la liste des régions rencontrées dans
   // l'ordre de parcours. Pour cela, le début de liste est FInclusionTreeRoot,
   // et la dernière cellule de la liste est désigné par le frère de
   // FInclusionTreeRoot ceci afin de pouvoir ajouter en fin en O(1).
   // Le champ FFather lie les régions entre elle de manière similaire
   // à dans un arbre union-find.
   FInclusionTreeRoot->setBrother(FInclusionTreeRoot);

   /* Et on peut commencer l'extraction de la carte. */
   /* 1. On crée le bord supérieur. */
   CDart* last = makeBorder(AImage->getXSize(), AImage->getYSize());

   setRepresentativeDart(FInclusionTreeRoot, last);

   // On passe en mode UFTree
   FUFTreeMode = true;

   /* 2. On parcours l'image en faisant les fusions nécessaires. */
   extractTopologicalMapMainLoop(AImage, last, ALevel);

   /* 3. On reétiquette les brins des régions fusionnées. */
   relabelDarts();

   // On sort du mode UFTree
   FUFTreeMode = false;
   
   /* 4. On calcule l'arbre d'inclusion des régions. */
   computeInclusionTree();
   
   if (!isMapOk())
   {   
      std::cerr << "PROBLEME après extractTopologicalMap" << std::endl;
      assert(false);
   }
   
#ifdef DEBUG_EXTRACT_IMAGE
   std::cout << "OK...Fin de extractTopologicalMap" << std::endl;
#endif
}
//******************************************************************************
void CTopologicalMap::simplifyMap()
{
   CDart* tmpDart = NULL;

   for (CDynamicCoverageAll it(this); it.cont(); ++it)
   {
      tmpDart = *it;

      assert(!isMarkedDeleted(tmpDart));
      
      //  if (!isMarkedDeleted(tmpDart))
      {
         if (beta21(tmpDart) == beta02(tmpDart))  //sommet de degré 2
         {
            if (!isEdgeLoop(tmpDart))          //pas de boucle
            {
               vertexRemoval(tmpDart);
            }
            else
            {
               FKhalimsky->setPCell(getDoublet(tmpDart), false);
            }
         }
      }
   }
}
//******************************************************************************
