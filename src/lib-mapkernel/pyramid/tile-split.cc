/*
 * Copyright (C) 2008-2011, Romain Goffe <romain.goffe@gmail.com>
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

#include "tile.hh"
#include "array.hh"
#include "coverage-region-pixels.hh"
#include "svg-export.hh"
#include "pyramidal-dart.hh"

using namespace Map2d;
using namespace pyramid;

void CTile::createMask(std::vector<CDoublet>& AMask)
{
  //std::cout<<"[start] CTile::createMask"<<std::endl;

  //std::cout<<"FRatioX = "<<FRatioX<<std::endl;
  //std::cout<<"FRatioY = "<<FRatioY<<std::endl;

  // les aretes verticales
  for(unsigned int i=0; i<FRatioX-1; ++i)
    {
      CDoublet doublet(getWidth()/FRatioX*(i+1),0,YPOS);
      for(unsigned int j=0; j<getHeight(); ++j)
	{
	  doublet.setY(j);
	  AMask.push_back(CDoublet(doublet));
	}
    }

  // les aretes horizontales
  for(unsigned int j=0; j<FRatioY-1; ++j)
    {
      CDoublet doublet(0, getHeight()/FRatioY*(j+1), XPOS);
      for(unsigned int i=0; i<getWidth(); ++i)
	{
	  doublet.setX(i);
	  AMask.push_back(CDoublet(doublet));
	}
    }

  assert(!AMask.empty());
  //std::cout<<"[end] CTile::createMask"<<std::endl;
}

void CTile::applyMask(std::vector<CDoublet>& AMask)
{
  //std::cout<<"[start] CTile::applyMask"<<std::endl;
  // insertion du masque en tant qu'éléments fictifs dans la géométrie
  std::vector<CDoublet>::iterator it;
  for(it=AMask.begin(); it!=AMask.end(); ++it)
    {

      if(!isLCell(*it))
	{
	  if( !isPCell(*it) )
	    setFictivePCell(*it);

	  if( !isPCell((*it).getNextPointel()) )
	    setFictivePCell((*it).getNextPointel());

	  setFictiveLCell(*it, true);
	}

      //std::cout<<"mask = "<<*it<<std::endl;
    }
  //std::cout<<"[end] CTile::applyMask"<<std::endl;
}

void CTile::insertVertexOnFictivePointels()
{
  //std::cout<<"[start] CTile::insertVertexOnFictivePointels"<<std::endl;

  // on parcours les arêtes initiales
  // si on trouve un pointel fictif, on insère un sommet de degré 2
  for(CDynamicCoverageAll it( this ); it.cont(); ++it)
    {
      CDoublet next = getDoublet(*it);
      // On parcours l'arête
      do
	{
	  next.setNextPointel();
	  do
	    {
	      next = next.getNextLinel();
	    }
	  while(!isLCell(next));

	  // On insère le sommet si le pointel est fictif
	  if( isFictivePCell(next) )
	    {
	      if( !isPCell(next) )
		{
		  setPCell(next);
		  insertVertexOnEdge(*it, next);
		}
	      break;
	    }
	}
      while(1);
    }
  //std::cout<<"[end] CTile::insertVertexOnFictivePointels"<<std::endl;
}

void CTile::createMaskUnitEdges(std::vector<CDoublet>& AMask)
{
  //std::cout<<"[start] CTile::createMaskUnitEdges"<<std::endl;

  std::vector<CDoublet>::iterator it;
  unsigned int debug = 0;
  for(it=AMask.begin(); it!=AMask.end(); ++it)
    {
      setLCell(*it, true);
      setFictiveLCell(*it, false);

      // attention, ça marche bien dans le cas ou le masque est une grille
      // marche pas si les chemins de lignels sont quelconques
      if( isFictiveLCell((*it).getNextLinel()) ||
	  isFictiveLCell((*it).getPrevLinel()) )
	setPCell(*it);

      // création des aretes élémentaires
      CPyramidalDart* dart1 = static_cast<CPyramidalDart*>(addMapDart());
      CPyramidalDart* dart2 = static_cast<CPyramidalDart*>(addMapDart());

      linkBeta1(dart1,dart2);
      dart1->setDoublet(*it);

      linkBeta1(dart2, dart1);
      dart2->setDoublet((*it).getNextPointel());

      linkBeta2(dart1,dart2);

      ++debug;
    }

  //valable pour le 1er niveau de l'image multi-red avec tile side=8
  //assert(debug==16);

  //std::cout<<"[end] CTile::createMaskUnitEdges"<<std::endl;
}

void CTile::sewAroundPointel(const CDoublet& APointel, const CArray* AArray, int toDelete)
{
  //std::cout<<"[start] CTile::sewAroundPointel"<<std::endl;

  CDoublet d(APointel);
  setFictivePCell(d, false);

  CDart* tab[4];
  unsigned int degree = 0;
  for( unsigned int j = 0 ; j < 4 ; ++j )
    {
      //std::cout<<" le doublet d = "<<d<<std::endl;
      tab[degree] = AArray->getDart(d);
      if(tab[degree]!=NULL && !isMarked(tab[degree],toDelete))
	++degree;

      //if(tab[degree] == NULL)
      //std::cout<<" pas de brin trouvé dans le tableau"<<std::endl;
      //if(tab[degree] != NULL && isMarked(tab[degree], toDelete))
      //std::cout<<" brin "<<getDoublet(tab[degree])<<" marqué toDelete"<<std::endl;

      d.setPrevLinel();
    }

  assert(d==APointel);
  assert(degree>1);
  assert(degree<5);

  if(degree == 2)
    {
      // sommets de degré 2
      //std::cout<<"sommet de degré 2"<<std::endl;
      //std::cout<<"linkBeta1 de "<<getDoublet(beta0(tab[0]))<<" avec "<<getDoublet(beta1(tab[0]))<<std::endl;
      linkBeta1(beta2(tab[1]), beta1(tab[0]));
      //std::cout<<"linkBeta1 de "<<getDoublet(beta0(tab[1]))<<" avec "<<getDoublet(beta1(tab[1]))<<std::endl;
      linkBeta1(beta2(tab[0]), beta1(tab[1]));
      //std::cout<<"linkBeta2 de "<<getDoublet(beta2(tab[0]))<<" avec "<<getDoublet(beta2(tab[1]))<<std::endl;
      linkBeta2(beta2(tab[0]), beta2(tab[1]));

      setMark(tab[0],toDelete);
      setMark(tab[1],toDelete);

    }
  else
    {
      // sommets de degré 3 et 4
      //std::cout<<"sommet de degré "<<degree<<std::endl;
      assert(degree==3 || degree==4);
      for(unsigned int j = 0; j<degree-1; ++j)
	{
	  //std::cout<<"linkBeta1 de "<<getDoublet(beta2(tab[j+1]))<<" avec "<<getDoublet(tab[j])<<std::endl;
	  linkBeta1(beta2(tab[j+1]), tab[j]);
	}
      //std::cout<<"linkBeta1 de "<<getDoublet(beta2(tab[0]))<<" avec "<<getDoublet(tab[degree-1])<<std::endl;
      linkBeta1(beta2(tab[0]), tab[degree-1]);
      setPCell(d, true);
    }
  //std::cout<<"[end] CTile::sewAroundPointel"<<std::endl;
}

void CTile::sewMapWithMask(std::vector<CDoublet>& AMask)
{
  //std::cout<<"[start] CTile::sewMapWithMask"<<std::endl;

  // on crée la structure associative brin/doublet
  CArray* array = new CArray(0, 0, getWidth(), getHeight());

  for(CDynamicCoverageAll it( this ); it.cont(); ++it)
    {
      //std::cout<<" remplissage = "<<getDoublet(*it)<<std::endl;
      array->setDart(*it);
    }

  int toDelete = getNewMark();

  // on fait les coutures des brins autour des pointels réels
  std::vector<CDoublet>::iterator it;
  for(it=AMask.begin(); it!=AMask.end(); ++it)
    {
      if(isFictivePCell(*it))
      	{
	  //std::cout<<"1. couture autour du sommet"<<*it<<std::endl;
	  sewAroundPointel((*it), array, toDelete);
	  //std::cout<<"2. couture autour du sommet"<<(*it).getNextPointel()<<std::endl;
	  sewAroundPointel((*it).getNextPointel(), array, toDelete);
	}
    }
  //std::cout<<"fin des coutures"<<std::endl;
  delete array;

  CDart * actu = NULL;
  int debug;
  for( CDynamicCoverageAll it( this ); it.cont(); )
    {
      actu = *it;
      ++it;
      debug = 0;
      if ( isMarked(actu, toDelete) )
	{
	  ++debug;
	  delMapDart(actu);
	}
    }

  freeMark(toDelete);

  assert(isMapOk());

  //std::cout<<"[end] CTile::sewMapWithMask"<<std::endl;
}

void CTile::createSons()
{
  std::cout<<"[start] CTile::createSons"<<std::endl;


  //  unsigned int count = 0;
  for(unsigned int i=0; i<FRatioX; ++i)
    for(unsigned int j=0; j<FRatioY; ++j)
      {
	// Nouvelle tuile fille
	CTile* son = new CTile(getWidth(), getHeight());
	son->setMergeThresholds(FMergeThresholds);
	son->setOrigine(getX()*(i+1), getY()*(j+1));
	son->setRatioX(FRatioX);
	son->setRatioY(FRatioY);
	son->setIndex(2, getIndex(2));

	// La matrice
	//son->copyKhalimsky2(count++, this);

	//son->createTopTile();

	// on met les bords droite et bas
	CDoublet doublet(getWidth(),0,YPOS);
	son->setPCell(doublet);
	for(unsigned int i = 0; i<getHeight(); ++i)
	  {
	    doublet.setY(i);
	    son->setLCell(doublet);
	  }

	doublet.setX(0);
	doublet.setY(getHeight());
	doublet.setLinel(XPOS);
	son->setPCell(doublet);
	for(unsigned int i = 0; i<getWidth(); ++i)
	  {
	    doublet.setX(i);
	    son->setLCell(doublet);
	  }
	son->setPCell(doublet.getNextPointel());

	std::cout<<"export svg matrice"<<std::endl;

	//affichage matrice
	std::ostringstream filename;
	std::string sep = "-";

	filename<<".//output//geom"<<sep<<"tile"<<sep<<son->getId()<<".svg";
	ofstream ofs( filename.str().c_str() );

	CSvgCreator svg( ofs );
	svg.svgBegin( son->getHeight(), son->getWidth() );

	// Parcours de la matrice de Khalimsky
	// Affichage des pointels et des lignels allumés
	CDoublet temp;
	unsigned int width  = getWidth();
	unsigned int height = getHeight();

	for(unsigned int x=0; x<width+1; ++x)
	  for(unsigned int y=0; y<height+1; ++y)
	    {
	      temp.setX(x);
	      temp.setY(y);

	      // Affichage des pointels
	      if( son->isPCell(temp) )
		svg.circle(x,y,0.05, "class=\"pointel\" " );

	      // Affichage des lignels
	      temp.setLinel(XPOS);
	      if( son->isLCell(temp) )
		svg.line(x+0.2,y, x+0.8,y,"class=\"linel\" " );

	      temp.setLinel(YPOS);
	      if( son->isLCell(temp) )
		svg.line(x,y+0.2, x,y+0.8, "class=\"linel\" ");
	    }
	svg.svgEnd();
	ofs.close();
	std::cout<<"export svg matrice ok"<<std::endl;
      }

  std::cout<<"[end] CTile::createSons"<<std::endl;
}

void CTile::shareDartsWithSon(CTile* ASon)
{
  std::cout<<"[start] CTile::shareDartsWithSon"<<std::endl;

  // On retrouve géométriquement les brins de la copie splitée
  // qui appartiennent à la subdivision

  std::cout<<" récupération des brins"<<std::endl;
  assert(isMapOk());
  std::cout<<" ratio X = "<<FRatioX<<std::endl;
  std::cout<<" ratio Y = "<<FRatioY<<std::endl;

  std::cout<<" xmin = "<<ASon->getXmin()<<std::endl;
  std::cout<<" xmax = "<<ASon->getXmax()<<std::endl;
  std::cout<<" ymin = "<<ASon->getYmin()<<std::endl;
  std::cout<<" ymax = "<<ASon->getYmax()<<std::endl;

  // !! interdit de garder ça
  for(CDynamicCoverageAll it(this); it.cont(); ++it)
    {
      CDoublet doublet = getDoublet(*it);
      //les brins de l'intérieur
      if( doublet.getX()*FRatioX >= ASon->getXmin() && doublet.getX()*FRatioX <= ASon->getXmax() &&
	  doublet.getY()*FRatioY >= ASon->getYmin() && doublet.getY()*FRatioY <= ASon->getYmax() )
	{
	  if( doublet.getX()*FRatioX > ASon->getXmin() && doublet.getX()*FRatioX < ASon->getXmax() &&
	      doublet.getY()*FRatioY > ASon->getYmin() && doublet.getY()*FRatioY < ASon->getYmax() )
	    {

	      std::cout<<"bah"<<std::endl;
	      CPyramidalDart* dart = static_cast<CPyramidalDart*>(ASon->addMapDart());
	      CDoublet tmp = doublet;
	      tmp.setX(doublet.getX()*FRatioX);
	      tmp.setY(doublet.getY()*FRatioY);
	      ASon->setDoublet(dart, tmp);
	      ASon->linkDartUpDown(static_cast<CPyramidalDart*>(*it), dart);
	    }

	  if( (doublet.getX()*FRatioX==ASon->getXmin() &&
	       doublet.getY()*FRatioY==ASon->getYmin() &&
	       (doublet.getLinel()==XPOS || doublet.getLinel()==YPOS)) ||
	      (doublet.getX()*FRatioX==ASon->getXmax() &&
	       doublet.getY()*FRatioY==ASon->getYmin() &&
	       (doublet.getLinel()==XNEG || doublet.getLinel()==YPOS)) ||
	      (doublet.getY()*FRatioY==ASon->getYmax() &&
	       doublet.getX()*FRatioX==ASon->getXmin() &&
	       (doublet.getLinel()==XPOS || doublet.getLinel()==YNEG)) ||
	      (doublet.getY()*FRatioY==ASon->getYmax() &&
	       doublet.getX()*FRatioX==ASon->getXmax() &&
	       (doublet.getLinel()==XNEG || doublet.getLinel()==YNEG)) )
	    {
	      std::cout<<"bih: "<<doublet<<std::endl;
	      CPyramidalDart* dart = static_cast<CPyramidalDart*>(ASon->addMapDart());
	      CDoublet tmp = doublet;
	      tmp.setX(doublet.getX()*FRatioX);
	      tmp.setY(doublet.getY()*FRatioY);
	      ASon->setDoublet(dart, tmp);
	      ASon->linkDartUpDown(static_cast<CPyramidalDart*>(*it), dart);
	    }
	}
    }

  // Les coutures
  for( CDynamicCoverageAll it( ASon ); it.cont(); ++it )
    {
      CPyramidalDart* downDart = static_cast<CPyramidalDart*>(*it);
      CPyramidalDart* upDart = downDart->getDartUp();
      assert(upDart!=NULL);
      downDart->setBeta0(upDart->getBeta0()->getDartDown());
      downDart->setBeta1(upDart->getBeta1()->getDartDown());
      downDart->setBeta2(upDart->getBeta2()->getDartDown());
    }

  CDoublet ref(0,0,XPOS);
  for( CDynamicCoverageAll it( ASon ); it.cont(); ++it )
    {
      CPyramidalDart* downDart = static_cast<CPyramidalDart*>(*it);
      if(ASon->getDoublet(downDart)==ref)
	for( CDynamicCoverage1 it( ASon,downDart ); it.cont(); ++it )
	  ASon->linkBeta1(ASon->beta2(downDart),ASon->beta2(ASon->beta0(downDart)));
    }

  std::cout<<" carte du haut"<<std::endl;
  for( CDynamicCoverageAll it( this ); it.cont(); ++it )
    {
      CPyramidalDart* downDart = static_cast<CPyramidalDart*>(*it);
      downDart->printInfos();

    }

  std::cout<<" carte du bas"<<std::endl;
  for( CDynamicCoverageAll it( ASon ); it.cont(); ++it )
    {
      CPyramidalDart* downDart = static_cast<CPyramidalDart*>(*it);
      downDart->printInfos();
    }

  //assert(ASon->isMapOk());
  assert(ASon->checkDarts());

  std::cout<<" récupération des brins ok"<<std::endl;

  std::cout<<"\n[end] CTile::shareDartsWithSon"<<std::endl;

}

CTile* CTile::createChildrenBySplit()
{
  std::cout<<"\n[start] CTile::copySplitLinkTile()"<<std::endl;

  CTile* tile = createChildByCopy();
  std::vector<CDoublet> mask;
  tile->createMask(mask);
  tile->applyMask(mask);
  tile->insertVertexOnFictivePointels();
  tile->createMaskUnitEdges(mask);
  tile->sewMapWithMask(mask);

  //assert(tile->isMapOk());
  assert(tile->checkDarts());


  // créer le nouvelles tuiles filles
  // en mettant les brins de tile au bon endroit

  tile->createSons();

  // foreach son do
  //tile->shareDartsBetweenSons();

  /*
    std::cout<<" export carte"<<std::endl;
    //affichage de la carte produite
    std::ostringstream filename;
    std::string sep = "-";

    filename<<".//output//svg"<<sep<<"tile"<<sep<<dTile->getId()<<".svg";
    ofstream ofs( filename.str().c_str() );

    CSvgCreator svg( ofs );
    svg.svgBegin( dTile->getHeight(), dTile->getWidth() );
    for( CDynamicCoverageAll it(dMap); it.cont(); ++it)
    {

    CDoublet init = dMap->getDoublet(*it);
    CDoublet current(init);
    CDoublet next(init);
    CDoublet prev(init);

    float x1,y1,x2,y2;

    // On récupère le doublet précédent
    do{
    prev = prev.getPrevLinel();
    }
    while(!dMap->isLCell(prev));
    prev.setNextPointel();

    // On démarre le chemin
    //CCoordinate startPoint = calculateGap(CCoordinate(current.getX(), current.getY()), prev, current);
    CCoordinate startPoint = CCoordinate(current.getX(), current.getY());
    x1 = startPoint.getX(); y1 = startPoint.getY();

    //    if( dMap->isRepresentativeDart(*it) )
    //svg.pathBegin(x1,y1,"class=\"representativeDart\" marker-end=\"url(#arrow)\" ");
    //else
    svg.pathBegin(x1,y1,"class=\"dart\" marker-end=\"url(#arrow)\"  ");

    // Parcours de l'arête
    do{

    x1 = current.getX();
    y1 = current.getY();

    // Calcul du doublet suivant
    next.setNextPointel();
    do{
    next = next.getNextLinel();
    }
    while(!dMap->isLCell(next));

    //CCoordinate endPoint = calculateGap(CCoordinate(next.getX(),next.getY()),current, next);
    CCoordinate endPoint = CCoordinate(next.getX(),next.getY());
    x2 = endPoint.getX(); y2 = endPoint.getY();

    // On rajoute le point au chemin
    svg.pathLineTo(x2,y2);

    // On passe au doublet suivant
    current.setNextPointel();
    if (dMap->isDegreTwoPointel(current))
    current = dMap->getOtherLinel(current);

    }
    // L'arête se termine par un pointel ou par le linel initial (boucle).
    while ( !dMap->isPCell(current) && current != init );

    svg.pathEnd();
    }

    // Parcours de la matrice de Khalimsky
    // Affichage des pointels et des lignels allumés
    CDoublet temp;
    unsigned int width  = getWidth();
    unsigned int height = getHeight();

    for(unsigned int x=0; x<width+1; ++x)
    for(unsigned int y=0; y<height+1; ++y)
    {
    temp.setX(x);
    temp.setY(y);

    // Affichage des pointels
    if( dMap->isPCell(temp) )
    svg.circle(x,y,0.05, "class=\"pointel\" " );

    // Affichage des lignels
    temp.setLinel(XPOS);
    if( dMap->isLCell(temp) )
    svg.line(x+0.2,y, x+0.8,y,"class=\"linel\" " );

    temp.setLinel(YPOS);
    if( dMap->isLCell(temp) )
    svg.line(x,y+0.2, x,y+0.8, "class=\"linel\" ");
    }


    svg.svgEnd();
    ofs.close();
    std::cout<<" export carte ok"<<std::endl;

  */

  // 2. retrouver les régions de chaque tuile
  // et reconstruire leur arbre d'inclusion



  std::cout<<"[end] CTile::copySplitLinkTile\n"<<std::endl;

  assert(false);

  return tile;
}
