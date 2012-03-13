//******************************************************************************
#include "inline-macro.hh"
#include "tile.hh"
#include "array.hh"
#include "matrix.hh"
#include "pyramidal-dart.hh"
#include "pyramidal-region.hh"
#include "svg-export.hh"
#include "dp-maxrow.hh"
#include <algorithm>
#include "heap.hh"
#include "dk-doublet.hh"
#include <limits>

#include INCLUDE_NON_INLINE("tile-extract.icc")
using namespace Map2d;
using namespace std;

//------------------------------------------------------------------------------
void CTile::projection()
{
  //std::cout<<"\n[start] CTile::projection()"<<std::endl;
  // Parcours des arêtes de la tuile up
  uint ratiox = upRatioX();  uint ratioy = upRatioY();
  CTile* up = tileUp();
  if(ratiox == 1 && ratioy==1)
    {
      //std::cout<<" ratios == 1 donc on duplique "<<std::endl;
      copyKhalimsky(up);
      return;
    }

  int done = up->getNewMark();
  // CSvgExporter svg;
  // svg.drawTileGeometry(up);

  CDoublet pstart, pend;
  for(CDynamicCoverageAll it(up); it.cont(); ++it)
    {
      CPyramidalDart* dart = static_cast<CPyramidalDart*>(*it);
      if(!up->dartBelongsToTopologicalBorder(dart)
	 && !up->isMarked(dart,done) )
	{
	  std::vector< std::vector<CDoublet> > edgesToProcess;
	  CDoublet init = up->getDoublet(dart);

	  //std::cout<<"init = "<<init<<std::endl;
	  CDoublet next(init); CDoublet down(next);
	  down.setX( (next.getX()+up->xmin())*ratiox );
	  down.setY( (next.getY()+up->ymin())*ratioy );
	  std::vector<CDoublet> edge;
	  if(isDoubletInTile(down))
	    {
	      //std::cout<<"1. down is in child = "<<down<<std::endl;
	      //std::cout<<"1. push = "<<next<<std::endl;
	      //copy.setX(copy.getX()*ratiox-(xmin()-tileUp()->xmin()*ratiox));
	      //copy.setY(copy.getY()*ratioy-(ymin()-tileUp()->ymin()*ratioy));
	      //std::vector<CDoublet> edge;
	      edge.push_back(next);
	    }
	  else
	    {
	      //std::cout<<"le doublet "<<next<<" n appartient pas à cette tuile fille"<<std::endl;
	    }
	  edgesToProcess.push_back(edge);
	  // On parcours l'arête
	  do{
	    // Calcul du doublet suivant
	    next.setNextPointel();
	    do
	      {
		next = next.getNextLinel();
	      }
	    while(!up->isLCell(next));
    
	    down.setX( (next.getX()+up->xmin())*ratiox );
	    down.setY( (next.getY()+up->ymin())*ratioy );
	    if(this->isPointelInTile(down))
	      {
		//la conversion des coordonées relatives à la tuile mère
		//en relatives par rapport à la tuile fille se fait dans le candidate posisitions
		//std::cout<<"2. down is in child = "<<down<<std::endl;
		//std::cout<<"2. push = "<<next<<std::endl;
		assert(!edgesToProcess.empty());
		edgesToProcess.back().push_back(next);
	      }
	    else if( edgesToProcess.empty() || !edgesToProcess.back().empty() ) //on crée une nouvelle arête 
	      {
		//std::cout<<" on crée une nouvelle arête"<<std::endl;
		std::vector<CDoublet> other;
		edgesToProcess.push_back(other);
	      }

	    if(up->isPCell(next) || next == init)
	      {
		break;
	      }
	  }
	  while(1);
	  //Calcul du plongement des pointels avec programmation dynamique
	  //processEdge(toProcess);
	  //Calcul du chemin de l'arête avec Dijkstra
	  //std::cout<<edgesToProcess.size()<<" arêtes à dk-traiter pour ce brin "<<std::endl;
	  for(uint i=0; i<edgesToProcess.size(); ++i)
	    if(!edgesToProcess[i].empty())
	      computePath(edgesToProcess[i], true);
	  //todo: on stocke le path dans une map avec le pointeur du brin up
	  
	  up->setMark(dart, done);
	  up->setMark(beta2(dart), done);
	}
    }
  up->negateMaskMark(done);
  up->freeMark(done);
  
  setId(++(up->FCount));
  if(index(2)==3){
    svg.drawTileGeometry(this);
  }
    
  //assert(false);
  //std::cout<<"\n[end] CTile::projection()"<<std::endl;
}

//------------------------------------------------------------------------------
uint CTile::gradient(const CDoublet & ADoublet)
{
  //std::cout<<"[start] CTile::gradient on linel "<<ADoublet<<std::endl;
  IM_Pixel pix1, pix2;
  switch(ADoublet.getLinel())
    {
    case XPOS:
      if(ADoublet.getY()==0 || ADoublet.getY()==height()) return 255;
      pix1.x = ADoublet.getX(); pix1.y = ADoublet.getY();
      pix2.x = ADoublet.getX(); pix2.y = ADoublet.getY()-1;
      break;
    case XNEG:
      if(ADoublet.getY()==0 || ADoublet.getY()==height()) return 255;
      pix1.x = ADoublet.getX()-1; pix1.y = ADoublet.getY();
      pix2.x = ADoublet.getX()-1; pix2.y = ADoublet.getY()-1;
      break;
    case YPOS:
      if(ADoublet.getX()==0 || ADoublet.getX()==width()) return 255;
      pix1.x = ADoublet.getX(); pix1.y = ADoublet.getY();
      pix2.x = ADoublet.getX()-1; pix2.y = ADoublet.getY();
      break;
    case YNEG:
      if(ADoublet.getX()==0 || ADoublet.getX()==width()) return 255;
      pix1.x = ADoublet.getX(); pix1.y = ADoublet.getY()-1;
      pix2.x = ADoublet.getX()-1; pix2.y = ADoublet.getY()-1;
      break;
    default: assert(false);
    }
  image()->getPixel( pix1, index(2) );
  image()->getPixel( pix2, index(2) );
  //cout<<"[end] CTile::gradient = "<<abs(image()->getPixelGreyValue(pix1)-image()->getPixelGreyValue(pix2))<<endl;
  return (uint) abs(image()->getPixelGreyValue(pix1) - image()->getPixelGreyValue(pix2));
}

//------------------------------------------------------------------------------
float CTile::gradientInverse(const CDoublet & ADoublet)
{
  return  (1.0/(gradient(ADoublet)+1))*256;
}

//------------------------------------------------------------------------------
std::vector<CDoublet> CTile::candidatePositions(const CDoublet & APointel)
{
  //todo: plutot que de stocker le vecteur de l'ensemble des pointels
  // il serait plus intéressant de calculer une seule matrice "offset"
  // à appliquer sur le pointel initial

  //std::cout<<"[start] CTile::candidatePosisitons for pointel "<< APointel <<std::endl;
  // Lorsqu'u pointel doit être plongé dans le niveau inférieur, 
  // il peut prendre ratiox*ratioy positions
  uint ratiox = upRatioX(); uint ratioy = upRatioY();
  //std::cout<<"ratiox = "<<ratiox<<" ratioy = "<<ratioy<<std::endl;
  std::vector<CDoublet> result; 
  
  //projection de la tuile mère vers la tuile fille

  CDoublet current(0, 0, XPOS);
  //int dir = 1;//( current.isPos( APointel.getLinel() ) )?1:-1;
  int i = APointel.getX()*ratiox-(xmin()-tileUp()->xmin()*ratiox);
  int j = APointel.getY()*ratioy-(ymin()-tileUp()->ymin()*ratioy);

  for(uint y=0; y<ratioy;++y)
    for(uint x=0; x<ratiox;++x)
      {
	//std::cout<<"candidate = "<< i+x << " " << j+y << std::endl;
	current.setX(i+x);
	current.setY(j+y);
	//on exclue les candidats sur le bord des tuiles
	if( (x==0 && y==0) || isPointelWithinTile(current) )
	  result.push_back(current);
      }
  
  //affichage
  //for(uint i=0; i<result.size(); ++i)
  //std::cout<<" candidate "<<result[i]<<std::endl;

  //std::cout<<"[end] CTile::candidatePosisitons "<<std::endl;
  return result;
}
//------------------------------------------------------------------------------
uint CTile::computePath(const std::vector<CDoublet> & AEdge, bool ADraw)
{
  //std::cout<<"[start] CTile::computePath"<<std::endl;
  
  //affichage de l'arête
  //for(uint i=0; i<AEdge.size(); ++i)
  //  std::cout<<" pointels à traiter "<<AEdge[i]<<std::endl;
  
  //Dijkstra
  Heap nodes;
  CDkDoublet* n1, *n2, *last;
  last = findNodes(AEdge, nodes);
  std::vector<CDkDoublet*> neighbors;
  while(!nodes.empty())
    {
      n1 = nodes.top(); //retourne le noeud de poids minimum
      if( n1->weight()==std::numeric_limits<float>::max() )
	break;
      nodes.pop();
      neighbors = findNeighbors(n1, nodes);
      for(uint i=0; i<neighbors.size(); ++i)
	{
	  n2 = neighbors[i];
	  float alt = n1->weight() + weight(n1, n2);
	  if( alt < n2->weight() )
	    {
	      n2->updateWeight(alt, nodes);
	      n2->setPrev(n1);
	    }
	}
      //assert((int) nodes.size()==--test);
    }

  //dessin du chemin minimal dans la matrice
  CDkDoublet* node = last;
  CDkDoublet* next;
  //std::cout<<" premier point du dk chemin = "<<node<<std::endl;
  setPCell(*node, true);
  assert(isPCell(*last));
  while(node->prev())
    {
      next = node;
      node = node->prev();
      //On positionne correctement le lignel
      if( node->getX() == next->getX()-1)
	node->setLinel(XPOS);
      else if( node->getX() == next->getX()+1)
	node->setLinel(XNEG);
      else if( node->getY() == next->getY()-1)
	node->setLinel(YPOS);
      else if( node->getY() == next->getY()+1)
	node->setLinel(YNEG);
      else
	std::cout<<"CTile::computePath : Warning node is not a neighbor of next"<<std::endl;
      
      setLCell(*node, true);
    }
  setPCell(*node, true);

  //std::cout<<"[end] CTile::computePath"<<std::endl;

  return 0;  
}

//------------------------------------------------------------------------------
CDkDoublet* CTile::findNodes(const std::vector<CDoublet> & AEdge, Heap & AHeap)
{
  //std::cout<<"[start] CTile::findNodes"<<std::endl;
  //tous les doublets à prendre en compte lors de la recherche de chemin minimal
  //entre ADoublet1 et ADoublet2 (ADoublet1<ADoublet2)
  std::vector<CDoublet> tmp;
  CDkDoublet* current = NULL;
  CDkDoublet* last = NULL;
  // Le graphe sur lequel on va appliquer Dijkstra est composé
  // de tous les pointels potentiels
  //sauf que le premier et le dernier pointel ne doivent pas être déplacés
  
  for(uint i=0; i<AEdge.size(); ++i)
    {
      tmp = candidatePositions(AEdge[i]);
      // On push tous les pointels dans le tas
      for(uint j=0; j<tmp.size(); ++j)
	{
	  current = new CDkDoublet(tmp[j]);
	  if(i==0 && j==0)
	    current->setWeight(0);
	  AHeap.push(current);
	  if(i==AEdge.size()-1 && j==0)
	      last = current;
	}
    }
  assert(last);
  return last;
  //std::cout<<"[end] CTile::findNodes"<<std::endl;
}

//------------------------------------------------------------------------------
float CTile::weight(CDkDoublet* ANode1, CDkDoublet* ANode2)
{
  //std::cout<<"[start] CTile::weight"<<std::endl;
  assert(ANode1 != ANode2);
  if( ANode2->getX() == ANode1->getX()+1)
    ANode1->setLinel(XPOS);
  else if( ANode2->getX() == ANode1->getX()-1)
    ANode1->setLinel(XNEG);
  else if( ANode2->getY() == ANode1->getY()+1)
    ANode1->setLinel(YPOS);
  else if( ANode2->getY() == ANode1->getY()-1)
    ANode1->setLinel(YNEG);
  else
    std::cout<<"CTile::weight : Warning Anode2 is not a neighbor of ANode1"<<std::endl;

  //std::cout<<"[end] CTile::weight"<<std::endl;
  return gradientInverse(*ANode1); 
}

//------------------------------------------------------------------------------
std::vector<CDkDoublet*> CTile::findNeighbors(CDkDoublet* ANode, const Heap & AHeap)
{
  //std::cout<<"[start] CTile::findNeighbors of "<<ANode<<std::endl;
  // todo: faire un parcours sur l'index. Et même mieux, faire une multimap
  // pour coder l'adjacence des pointels 

  std::vector<CDkDoublet*> result;
  typedef __gnu_pbds::priority_queue<CDkDoublet*, EdgeComp, rc_binomial_heap_tag> ThinHeap;
  ThinHeap nodes = AHeap.heap;
  ThinHeap::iterator it;
  for(it = nodes.begin(); it!=nodes.end(); ++it)
    {
      //On récupère tous les noeuds à une distance de 1
      uint dx = abs( (*it)->getX() - ANode->getX() );
      uint dy = abs( (*it)->getY() - ANode->getY() );
      //std::cout<<"test of node "<<*it<<" dx="<<dx<<" dy="<<dy<<std::endl;
      if( dx<=1 && dy<=1 && dx!=dy )
	  result.push_back(*it);
    }
  //std::cout<<"[end] CTile::findNeighbors nb ="<<result.size()<<std::endl;
  return result;
}
//------------------------------------------------------------------------------
void CTile::removeDanglingEdges()
{
  //assert(false); //on n'est pas censé utiliser ça
  
  int toDelete = getNewMark();
  for(CDynamicCoverageAll it(this); it.cont(); ++it)
    {
      if(!isMarked(*it,toDelete) )
	{
	if(beta1(*it) == beta2(*it))
	  {
	    //on éteint le plongement
	    CDoublet init = getDoublet(*it);
	    CDoublet next(init);
	    setLCell(init, false);
	    do
	      {
		init = next;
		next.setNextPointel();
		assert(next.getNextPointel()==init);
		int count = 0;
		do
		  {
		    next = next.getNextLinel();
		    ++count;
		    assert(count<5);
		  }
		while( !isLCell(next) && (next.getNextPointel()!=init) );
		setPCell(next,false);
		setLCell(next,false);	      		
		if( next.getNextPointel() == init)
		  break;
	      }
	    while(1);
	  
	    //on fait les coutures et on marque les brins à supprimer
	    linkBeta1(beta0(*it),beta1(beta1(*it)));
	    setMark(*it, toDelete);
	    setMark(beta1(*it), toDelete);
	  }
	else if ( beta0(*it) == beta2(*it) )
	  {
	    //on éteint le plongement
	    CDoublet init = getDoublet(beta0(*it));
	    CDoublet next(init);
	    //setPCell(init, false);
	    setLCell(init, false);
	    do
	      {
		init = next;
		next.setNextPointel();
		assert(next.getNextPointel()==init);
		int count = 0;
		do
		  {
		    next = next.getNextLinel();
		    ++count;
		    assert(count<5);
		  }
		while( !isLCell(next) && (next.getNextPointel()!=init) );
		setPCell(next,false);
		setLCell(next,false);
		if( next.getNextPointel() == init)
		    break;
	      }
	    while(1);
	  
	    //on fait les coutures et on marque les brins à supprimer
	    linkBeta1(beta0(beta0(*it)), beta1(*it));
	    setMark(*it, toDelete);
	    setMark(beta2(*it), toDelete);
	  }
	}
    }

  //On supprime tous les brins qui ont été marqués lors du vertex removal
  CDart* actu = NULL;
  for( CDynamicCoverageAll it( this ); it.cont();  )
    {
      actu = *it; 
      ++it;
      if ( isMarked(actu, toDelete) )
	{
	  assert(!isRepresentativeDart(actu));
	  delMapDart(actu);
	}
    }
  freeMark(toDelete);
}




//******************************************************************************
//Programmation dynamique
//******************************************************************************

//------------------------------------------------------------------------------
void CTile::processEdge(std::vector<CDoublet> AEdge)
{
  std::cout<<"[start] CTile::processEdge with "<<AEdge.size()<<" pointels"<<std::endl;
  //Programmation dynamique
  //xi: pointels du levelUp dont la posisition doit être reévaluée dans la fenêtre induite par les ratios
  //hi: gradient moyen sur la droite qui relie deux pixels
  //fi: maximum de tous les gradients calculés pour les positions possibles des xi 

  //affichage
  //for(uint i=0; i<AEdge.size(); ++i)
  //  std::cout<<" pointels à traiter "<<AEdge[i]<<std::endl;

  CMatrix<uint>* h[AEdge.size()-1];
  std::vector<CMaxRow*> f[AEdge.size()-1];

  for(uint i=0; i<AEdge.size()-1; ++i)
    {
      // Calcul de xi et xi+1
      std::vector<CDoublet> x1 = candidatePositions(AEdge[i]);
      std::vector<CDoublet> x2 = candidatePositions(AEdge[i+1]);
      
      //Calcul de hi
      h[i] = computeGradientTable(x1, x2); 
    
      //Mise à jour du hi+1 = fi+hi+1
      if(i>0) h[i] = updateGradientTable(h[i], f[i-1]);

      //Calcul de fi
      f[i] = computeMaximisationFunction(h[i], x2, x1);
    }

  //Maintenant qu'on a tous les fi, il suffit de faire marche arrière et de
  //récupérer tous les doublets qui vont définir notre nouveau plongement d'arête
  CDoublet doublet, first;
  uint value = 0;
  uint nb = AEdge.size()-1;
  std::vector<CDoublet> edge;
  //on récupère le doublet xi-1 associé au max du dernier fi
  for(uint k=0; k < f[nb-1].size(); ++k)
    if( f[nb-1][k]->value() > value )
      {
	value = f[nb-1][k]->value();
	first   = f[nb-1][k]->doublet1();
	doublet = f[nb-1][k]->doublet2();
      }
  edge.push_back(first);
  edge.push_back(doublet);
  //computeLine(first,doublet,false,true);

  for(int i=nb-2; i >=0; --i)
    for(uint k=0; k<f[i].size(); ++k)
      if( f[i][k]->doublet1()==edge.back() )
	{
	  doublet = f[i][k]->doublet2();
	  //computeLine(edge.back(),doublet,false,true);
	  edge.push_back(doublet);
	  break;
	}
  
  //Affichage du nouveau plongement de l'arête
  std::cout<<" nouveau plongement "<<std::endl;
  for(uint k=0; k<edge.size(); ++k)
    {
      std::cout<<" doublet  "<<edge[k]<<std::endl;
      //setPCell(edge[k], true);
    }
  
  CSvgExporter svg;
  svg.drawTileGeometry(this);

  assert(false);
  std::cout<<"[end] CTile::processEdge "<<std::endl;
}
//------------------------------------------------------------------------------
CMatrix<uint>* CTile::computeGradientTable(const std::vector<CDoublet> & ASet1, 
					   const std::vector<CDoublet> & ASet2)
{
  //std::cout<<"[start] CTile::computeGradientTable "<<std::endl;
  // La méthode décrit explicitement la fonction de maximisation h (gradient moyen sur le chemin
  // reliant Apointel1 à APointel2) pour toutes les positions possibles de APointel1 et 2
  assert(ASet1.size()==ASet2.size());

  CMatrix<unsigned int>* tab = new CMatrix<unsigned int>(ASet1.size(), ASet1.size());
  for(uint i=0; i<ASet1.size(); ++i)
    for(uint j=0; j<ASet2.size(); ++j)
      tab->setValue( i, j, computeLine(ASet1[i], ASet2[j], true, false) );

  //affichage de hi
  //std::cout<< " hi "<<std::endl; 
  //for(uint i=0; i<ASet1.size(); ++i)
  //  {
  //    for(uint j=0; j<ASet2.size(); ++j)
  //	std::cout<<tab->getValue(i, j)<<" ";
  //    std::cout<< std::endl; 
  //  }
  //std::cout<<"[end] CTile::computeGradientTable "<<std::endl;
  return tab;
}
//------------------------------------------------------------------------------
CMatrix<uint>* CTile::updateGradientTable(CMatrix<uint>* AHi, 
					  const std::vector<CMaxRow*> & AFi)
{
  //std::cout<<"[start] CTile::updateGradientTable "<<std::endl;
  assert(AFi.size()==AHi->getSizeY());
  
  for(uint j=0; j<AHi->getSizeY(); ++j)
    for(uint i=0; i<AHi->getSizeX(); ++i)
      AHi->setValue( i,j, AHi->getValue(i,j)+AFi[i]->value() );

  //affichage
  //for(uint i=0; i<AHi->getSizeX(); ++i)
  //  {
  //    for(uint j=0; j<AHi->getSizeY(); ++j)
  //	std::cout<<AHi->getValue(i,j)<<" ";
  //    std::cout<<std::endl;
  //  }

  //std::cout<<"[end] CTile::updateGradientTable "<<std::endl;
  return AHi;
}
//------------------------------------------------------------------------------
std::vector<CMaxRow*> CTile::computeMaximisationFunction(CMatrix<uint>* AHi, 
						  std::vector<CDoublet> ASet2, 
						  std::vector<CDoublet> ASet1)
{
  //std::cout<<"[start] CTile::computeMaximisationFunction "<<std::endl;
  //todo: voir si un CMaximisation ne pourrait pas se résumer à une simple
  // map<uint, CDoublet> ça simplifierait les choses

  //une matrice Fi est un vecteur de CMaxRow.
  //CMaxRow est le triplet (xi+i value xi) où les xi sont des pointels

  uint size = ASet2.size();
  uint value; uint index;
  std::vector<CMaxRow*> result;
  
  for(uint j=0; j<size; ++j)
    {
      CMaxRow* row = new CMaxRow();      
      row->setDoublet1(ASet2[j]);
      //on récupère la valeur maximale dans la colonne de Hi
      //avec le doublet xi-1 associé
      value = 0; index = 0;
      for(uint i=0; i<AHi->getSizeY(); ++i)
	if( AHi->getValue(i,j) >= value )
	  {
	    index = i;
	    value = AHi->getValue(i,j);
	    row->setValue( value );	
	  }
      //std::cout<<"push "<<value<<" Doublet "<<ASet1[index]<<std::endl;
      row->setDoublet2( ASet1[index] );
      result.push_back(row);
    }

  //affichage
  //for(uint i=0; i<size; ++i)
  //  std::cout<<result[i]->doublet1()<<" "
  //	     <<result[i]->value()<<" "
  //	     <<result[i]->doublet2()<<std::endl;
  
  //std::cout<<"[end] CTile::computeMaximisationFunction "<<std::endl;
  return result;
}
//------------------------------------------------------------------------------
std::multimap<uint, CDoublet> CTile::getColumnMaxValues(CMatrix<uint>* AHi, 
						   const std::vector<CDoublet> & ASet1)
{
  //std::cout<<"[start] CTile::getColumnMaxValues "<<std::endl;

  //affichage
  //for(uint k=0; k<ASet1.size(); ++k)
  //  std::cout<<"set1 = "<<ASet1[k]<<std::endl;

  std::multimap<uint, CDoublet> result;
  uint value = 0; uint index=0;
  CDoublet doublet;
  
  for(uint i=0; i<AHi->getSizeX(); ++i)
    {
      value = 0; index = 0;
      for(uint j=0; j<AHi->getSizeY(); ++j)
	if(AHi->getValue(i,j)>value)
	  {
	    index = j;
	    value = AHi->getValue(i,j);
	  }
      result.insert( pair<uint, CDoublet>(value, ASet1[index]) );
    }
  
  //affichage
  //std::multimap<uint, CDoublet>::iterator it;
  //for ( it=result.begin(); it!=result.end(); ++it )
  //  std::cout<<" Key (uint) = "<<it->first
  //	     <<" Value (CDoublet) = "<<it->second<<std::endl;
  
  //std::cout<<"[end] CTile::getColumnMaxValues "<<std::endl;
  return result;
}

//------------------------------------------------------------------------------
uint CTile::computeLine(const CDoublet & APointel1, const CDoublet & APointel2,
			bool AGradient, bool ADraw)
{
  //std::cout<<"[start] CTile::computeAverageGradient between "<< APointel1 << "and "<< APointel2<<std::endl;
  //assert(checkComputeLine());

  if(APointel1==APointel2) 
    {
      if(ADraw) 
	setPCell(APointel1);
      return 0;
    }

  //todo: on fait une copie mais on doit pouvoir s'en passer
  CDoublet pointel1(APointel1);  CDoublet pointel2(APointel2);
  // On permute au besoin les deux doublets de façon à ce que pointel1 soit plus petit que pointel2
  swapPointels(pointel1, pointel2);
  CDoublet current(pointel1);

  if(ADraw) setPCell(current, true);
  uint sum = 0; uint count = 0; 
  int x0 = pointel1.getX();  int y0 = pointel1.getY();
  int x1 = pointel2.getX();  int y1 = pointel2.getY();
  int dx = abs(x1-x0); int dy = abs(y1-y0);

  //droite horizontale
  if(dy==0)
    {
      while(current.getX()<=x1)
	{
	  current.setLinel(XNEG);
	  current.incX();
	  if(ADraw)
	    {
	      setLCell(current,true);
	      setPCell(current,true);
	    }
	  if(AGradient)
	    sum+=gradient(current);
	  ++count;
	}
      return (count==0)?0:sum/count;
    }

  //droite verticale
  if(dx==0)
    {
      while(current.getY()<=y1)
	{
	  current.setLinel(YNEG);
	  current.incY();
	  if(ADraw)
	    {
	      setLCell(current,true);
	      setPCell(current,true);
	    }
	  if(AGradient)
	    sum+=gradient(current);
	  ++count;
	}
      return (count==0)?0:sum/count;
    }

  //droite
  assert(dx!=0 && dy!=0);
  int offx = (x0<=x1?1:-1); int offy = (y0<=y1?1:-1);
  bool steep = (dx>dy);
  int error = 0;
  if (steep)
    {
      error = dx/dy;
      current.setLinel((offy==1)?XPOS:XNEG);
    }
  else
    {
      error = dy/dx;
      current.setLinel((offx==1)?YPOS:YNEG);
    }
  
  //todo: vu qu'on permute les pointels, offx=1 et c'est toujours xpos
  //donc pas besoin du test (offx==1)?XNEG:XPOS
  while(current.getX()<x1 || current.getY()<y1)
    {
      if(steep)
	if ( error<=0 )
	  {
	    current.setLinel((offy==1)?YNEG:YPOS);
	    current.incY(offy);
	    error=(dy)?dx/dy:0;
	    dx = abs(x1-current.getX());
	    dy = abs(y1-current.getY());
	  }
	else
	  {
	    current.setLinel((offx==1)?XNEG:XPOS);
	    current.incX(offx);
	    --error;
	  }
      else
	if ( error<=0 )
	  {
	    current.setLinel((offx==1)?XNEG:XPOS);
	    current.incX(offx);
	    error=(dx)?dy/dx:0;
	    dx = abs(x1-current.getX());
	    dy = abs(y1-current.getY());
	  }
	else
	  {
	    current.setLinel((offy==1)?YNEG:YPOS);
	    current.incY(offy);
	    --error;
	  }
      if(ADraw)
	{
	  setLCell(current,true);
	  setPCell(current,true);
	}
      if(AGradient)
	sum+=gradient(current);
      ++count;
    }

  // tab contient le gradient de tous les lignels du chemin pour aller de pointel1 à pointel2
  // on retourne la moyenne
  //std::cout<<"[end] CTile::computeAverageGradient = "<<sum/count<<std::endl;
  assert(count);
  return (count==0)?0:sum/count;
}

//------------------------------------------------------------------------------
void CTile::swapPointels(CDoublet APointel1, CDoublet APointel2)
{
  std::cout<<"[start] CTile::swapPointels"<<std::endl;
  if( !(APointel1.getX()<APointel2.getX()) &&
      !(APointel1.getX()==APointel2.getX() && APointel1.getY()<APointel2.getY()) )
    {
      CDoublet tmp(APointel1);
      APointel1 = APointel2;
      APointel2 = tmp;
    }
  std::cout<<"[end] CTile::swapPointels"<<std::endl;
}
