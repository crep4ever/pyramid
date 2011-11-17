//******************************************************************************
#include "dss.h"
#include <iostream>
using namespace std;
//******************************************************************************
// Calcule le dss associé aux directions dir (suite de 0 pour x+1 et 1
// pour y+1) à partir du point de départ APoint
// Si trace=vrai, affiche le résultat de chaque étape de addPointRight
// Si negx est vrai, 0 correspond à x-1
// Si negy est vrai, 1 correspond à y-1
// Si inv_xy est vrai, 0 correspond à y et 1 à y.
CDss computeDss( char* dir,
		 const Point<int> & APoint=Point<int>(0,0),
		 bool trace=false,
		 bool negx=false, bool negy=false, bool inv_xy=false)

{
  CDss res;

  int lg = strlen(dir);
  if ( lg<1 )
    {
      cout<<"Un dss doit avoir au moins 2 points."<<endl;
      return res;
    }

  Point<int> point(APoint);
  if ( dir[0]=='0' )
    {
      if ( !inv_xy ) point.x += ( negx ? -1 : +1 );
      else           point.y += ( negy ? -1 : +1 );
    }
  else // ( dir[0]=='1' )
    {
      if ( !inv_xy ) point.y += ( negy ? -1 : +1 );
      else           point.x += ( negx ? -1 : +1 );
    }
  
  res.init(APoint.x,APoint.y,point.x,point.y);

  if ( trace ) cout<<"Init : "<<res<<endl;

  for (int i=1; i<lg; ++i)
    {
      if ( dir[i]=='0' )
	{
	  if ( !inv_xy ) point.x += ( negx ? -1 : +1 );
	  else           point.y += ( negy ? -1 : +1 );
	}
      else // ( dir[i]=='1' )
	{
	  if ( !inv_xy ) point.y += ( negy ? -1 : +1 );
	  else           point.x += ( negx ? -1 : +1 );
	}

      int add = res.addPointRight(point);

      if ( trace )
	cout <<"  "<<point<<" Res:"<<add<<" "<<res<<endl;
    }

  return res;
}

//******************************************************************************
// Calcule le dss associé aux directions dir (suite de 0 pour x+1 et 1
// pour y+1) à partir du point de départ APoint en ajoutant des points à gauche.
// Si trace=vrai, affiche le résultat de chaque étape de addPointLeft
// Si negx est vrai, 0 correspond à x-1
// Si negy est vrai, 1 correspond à y-1
// Si inv_xy est vrai, 0 correspond à y et 1 à y.
CDss computeDssLeft( char* dir,
		     const Point<int> & APoint=Point<int>(0,0),
		     bool trace=false,
		     bool negx=false, bool negy=false, bool inv_xy=false)

{
  CDss res;

  int lg = strlen(dir);
  if ( lg<1 )
    {
      cout<<"Un dss doit avoir au moins 2 points."<<endl;
      return res;
    }

  Point<int> point(APoint);
  if ( dir[0]=='0' )
    {
      if ( !inv_xy ) point.x += ( negx ? -1 : +1 );
      else           point.y += ( negy ? -1 : +1 );
    }
  else // ( dir[0]=='1' )
    {
      if ( !inv_xy ) point.y += ( negy ? -1 : +1 );
      else           point.x += ( negx ? -1 : +1 );
    }
  
  res.init(point.x,point.y,APoint.x,APoint.y);

  if ( trace ) cout<<"Init : "<<res<<endl;

  for (int i=1; i<lg; ++i)
    {
      if ( dir[i]=='0' )
	{
	  if ( !inv_xy ) point.x += ( negx ? -1 : +1 );
	  else           point.y += ( negy ? -1 : +1 );
	}
      else // ( dir[i]=='1' )
	{
	  if ( !inv_xy ) point.y += ( negy ? -1 : +1 );
	  else           point.x += ( negx ? -1 : +1 );
	}

      int add = res.addPointLeft(point);

      if ( trace )
	cout <<"  "<<point<<" Res:"<<add<<" "<<res<<endl;
    }

  return res;
}

// Fonction pour tester un DSS dans toutes les 8 directions possibles.
void testDssAllCases( char* dir, const Point<int> & APoint=Point<int>(0,0),
		      bool trace=false )
{
  CDss res;
  
  res=computeDss(dir,APoint,trace,false,false,false); if ( !trace ) cout<<res<<endl;
  res=computeDss(dir,APoint,trace,true ,false,false); if ( !trace ) cout<<res<<endl;
  res=computeDss(dir,APoint,trace,false,true ,false); if ( !trace ) cout<<res<<endl;
  res=computeDss(dir,APoint,trace,true ,true ,false); if ( !trace ) cout<<res<<endl;

  res=computeDss(dir,APoint,trace,false,false,true); if ( !trace ) cout<<res<<endl;
  res=computeDss(dir,APoint,trace,true ,false,true); if ( !trace ) cout<<res<<endl;
  res=computeDss(dir,APoint,trace,false,true ,true); if ( !trace ) cout<<res<<endl;
  res=computeDss(dir,APoint,trace,true ,true ,true); if ( !trace ) cout<<res<<endl;
}

// Fonction pour tester un DSS puis la même reconnaissance en sens inverse.
void testDssBothDir( char* dir, const Point<int> & APoint=Point<int>(0,0),
		      bool trace=false )
{
  CDss res,res2,res3;
  
  // On fait la reconnaissance dans le sens +x,+y à partir de APoint
  cout<<(res=computeDss(dir,APoint,trace,false,false,false))<<endl;
  res.drawPixels(cout);
  
  // On affiche le dss inversé par la méthode reverse
  res.reverse();cout<<res<<endl;
  res.drawPixels(cout);
  
  // Et on fait la reconnaissance dans le sens -x -y à partir de la
  // fin de la reconnaissance précédente.
  char dirRev[strlen(dir)];
  int i,j;
  for ( i=res.getNbPixels()-2, j=0; i>=0; --i, ++j )
    dirRev[j]=dir[i];
  dirRev[j]=0;

  cout<<"dir="<<dir<<"  dirRev="<<dirRev<<endl;
  
  cout<<(res2=computeDss(dirRev,res.FStartingPoint,trace,true,true,false))<<endl;
  res2.drawPixels(cout);

  // Et on fait la reconnaissante dans le sens +x et +y en ajoutant à Gauche.
  cout<<(res3=computeDssLeft(dirRev,res.FStartingPoint,trace,true,true,false))<<endl;
  res3.drawPixels(cout);
}


// Fonction pour tester un DSS puis la même reconnaissance en sens inverse.
void testDssBothDirQuadrant( char* dir, const Point<int> & APoint=Point<int>(0,0),
		      bool trace=false )
{
  CDss res,res2,res3,res4;

  // On fait la reconnaissance dans le sens +x,+y à partir de APoint
  cout<<(res=computeDss(dir,APoint,trace,false,false,false))<<endl;
    res.drawPixels(cout);
  
	res.rotateFirstQuadrant();
	cout <<"Rotated DSS: "<<res<<endl;
	res.drawPixels(cout);
	
	cout<<"************"<<endl;
	// On fait la reconnaissance dans le sens -x,+y à partir de APoint
  cout<<(res=computeDss(dir,APoint,trace,true,false,false))<<endl;
    res.drawPixels(cout);
  
	res.rotateFirstQuadrant();
	cout <<"Rotated DSS: "<<res<<endl;
	res.drawPixels(cout);
	
	cout<<"************"<<endl;
	// On fait la reconnaissance dans le sens -x,-y à partir de APoint
  cout<<(res=computeDss(dir,APoint,trace,true,true,false))<<endl;
    res.drawPixels(cout);
  
	res.rotateFirstQuadrant();
	cout <<"Rotated DSS: "<<res<<endl;
	res.drawPixels(cout);
	
	cout<<"************"<<endl;
	// On fait la reconnaissance dans le sens x,-y à partir de APoint
  cout<<(res=computeDss(dir,APoint,trace,false,true,false))<<endl;
    res.drawPixels(cout);
  
	res.rotateFirstQuadrant();
	cout <<"Rotated DSS: "<<res<<endl;
	res.drawPixels(cout);
	
}

//Fonction pour tester l'union de 2 dss puis l'union dans l'autre sens
void testUnionBothDir( const CDss& ADss1, const CDss& ADss2 )
{
  cout<<"dss1 = "<<ADss1<<endl; // ADss1.drawPixels(cout);
  cout<<"dss2 = "<<ADss2<<endl; // ADss2.drawPixels(cout);

  CDss dss(ADss1);
  cout <<"\t Union out="<<(int)dss.unionDSS(ADss2)<<endl;
  cout <<"New parameters = "<<dss<<endl;

  cout<<"**** Union dans l'autre sens ****"<<endl;
  CDss dss1(ADss2); dss1.reverse();
  CDss dss2(ADss1); dss2.reverse();

  cout<<"dss1 = "<<dss1<<endl; // dss1.drawPixels(cout);
  cout<<"dss2 = "<<dss2<<endl; // dss2.drawPixels(cout);  
  
  cout <<"\t Union out="<<(int)dss1.unionDSS(dss2)<<endl;
  cout <<"New parameters = "<<dss1<<endl;
}

//******************************************************************************
// Différents tests de reconnaissance
void test_klette()
{
  cout << "########### TEST Klette"<<endl;

  testDssAllCases("010001000100100001",Point<int>(0,0),false);

  cout << "########### FIN TEST Klette"<<endl;
}

void test_klette_et_inv()
{
  cout << "########### TEST Klette et inverse"<<endl;
  
  testDssBothDir("010001000100100001",Point<int>(-5,0),false);

  cout << "########### FIN TEST Klette et inverse"<<endl;
}

void test_rotateFirstQuadrant()
{
	cout << "########### TEST rotateFirstQuadrant"<<endl;
  
  testDssBothDirQuadrant("010001000100100001",Point<int>(-5,0),false);

  cout << "########### FIN TEST rotateFirstQuadrant"<<endl;
	
}


void test_reco()
{
  cout << "########### TEST Reco"<<endl;

  testDssAllCases("01001",Point<int>(10,10),false);
  
  cout << "########### FIN TEST Reco"<<endl;
}

void test_staircase()
{
  cout << "########### TEST Staircase"<<endl;

  testDssAllCases("01011",Point<int>(5,-8),false);
  
  cout << "########### FIN Staircase"<<endl;
}

void test_staircase2()
{
  cout << "########### TEST Staircase 2"<<endl;

  testDssAllCases("00101");
  
  cout << "########### FIN Staircase 2"<<endl;
}

void test_vertical_length2()
{
  //On teste le démarrage avec un segment vertical de longueur 2 puis octant 1
  
  cout << "########### TEST Vertical 2"<<endl;

  testDssAllCases("1100",Point<int>(10,10));
  
  cout << "########### FIN TEST Vertical 2"<<endl;
}

void test_L()
{
  cout << "########### TEST L"<<endl;

  testDssBothDir("01");

  //  testDssBothDir("10");
  
  cout << "########### FIN L"<<endl;
}

//******************************************************************************
// Test sur les unions
void test_union()
{
  cout << "########### TEST Union simple"<<endl;

  CDss dss1 = computeDss("00100",Point<int>(10,10));
  CDss dss2 = computeDss("00100",Point<int>(14,11));

  testUnionBothDir(dss1,dss2);
  
  cout <<"########### FIN Union simple"<<endl;
}

void test_union2()
{
  cout << "########### TEST avec MAJ"<<endl;

  CDss dss1 = computeDss("00100",Point<int>(10,10));
  CDss dss2 = computeDss("001",Point<int>(14,11));

  testUnionBothDir(dss1,dss2);

  cout <<"########### FIN TEST avecMAJ"<<endl;
}

void test_union_quadrant()
{
  cout << "########### TEST QUADRANT avec MAJ"<<endl;

  CDss dss1 = computeDss("110",Point<int>(10,10));
  CDss dss2 = computeDss("01",Point<int>(11,12));

  testUnionBothDir(dss1,dss2);

  cout <<"########### FIN QUADRANT TEST avecMAJ"<<endl;
}

main()
{
  // test_klette(); // Test de Klette dans tout les sens => OK

  // test_klette_et_inv(); // Test de Klette dans un sens puis en sens
                          // inverse à partir de la fin du premier => OK

  // test_rotateFirstQuadrant();
	
  // test_L(); // test d'un L dans les 2 sens : OK

  // test_reco(); // OK
  
  // test_staircase(); // OK
  // test_staircase2(); // OK

  // test_vertical_length2(); // OK

  
  test_union(); // OK
  //  test_union2(); // OK

  //   test_union_quadrant(); // Ok fusion impossible
}
