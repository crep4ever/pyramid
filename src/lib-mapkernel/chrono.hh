/*
 * chrono : Classe pour chronométrer.
 * Copyright (C) 2008, Guillaume Damiand, guillaume.damiand@liris.cnrs.fr
 *               http://liris.cnrs.fr
 *
 * This file is part of chrono.
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
#ifndef CHRONO_HH
#define CHRONO_HH
//******************************************************************************
//#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
//******************************************************************************
/* Classe pour chronométrer. Le chrono est initialisé à 0 et
 * additionne les temps entre des start() et des stop(). reset()
 * permet de remettre le chrono à zéro.
 * La méthode displayRealTime permet de choisir si l'affichage du temps
 * est en mode réel (i.e. en 10-6 secondes) ou utilisateur (affichage
 * dépendant du temps passé s, ms ou us). Par défaut utilisateur.
 * La méthode setMode permet de choisir le mode de chronométrage :
 * REEL_MODE : horloge temps réel (temps réel écoulé depuis le start)
 * PUS_MODE  : horloge processus (temps consommé par le processus)
 * REEL_AND_PUS_MODE : les 2 en même temps, mode par défaut.
 */
//******************************************************************************
/// Type permettant de fixer le mode de chronométrage: REEL_MODE  : mode temps 
/// réel (temps réel écoulé), PUS_MODE : mode processus (temps consommé par le 
/// processus), REEL_AND_PUS_MODE : les 2 à la fois.
typedef enum { REEL_MODE, PUS_MODE, REEL_AND_PUS_MODE } TModeChrono;
//******************************************************************************
class CChrono
{
public:
  
  /// Construit un chrono initialisé à zéro.
  /// @param AMode mode de chronométrage (@see TModeChrono)
  /// @param ADisplayReal vrai si affichage unité 10-6 ou unité user sinon.
  CChrono ( TModeChrono AMode=REEL_AND_PUS_MODE, bool ADisplayReal=true);

  /// Constructeur par recopie.
  CChrono ( const CChrono& AChrono );

  /// Opérateur d'affectation. *this reçoit AChrono. @return *this.
  CChrono& operator=( const CChrono& AChrono );
  
  /// Opérateur d'addition pour FTotal (les champs FBefore et FAfter
  /// ne sont pas additionnés). 
  /// @return *this après sa modification.
  CChrono& operator+=( const CChrono& AChrono );
  
  /// Opérateur d'addition pour FTotal (les champs FBefore et FAfter
  /// ne sont pas additionnés). 
  /// @return le temps cumulé de *this et AChrono.
  CChrono operator+( const CChrono& AChrono ) const;
  
  /// Opérateur de soustraction pour FTotal (les champs FBefore et FAfter
  /// ne sont pas soustraits).
  /// @return *this après sa modification.
  CChrono& operator-=( const CChrono& AChrono );

  /// Opérateur de soustraction pour FTotal (les champs FBefore et FAfter
  /// ne sont pas soustraits).
  /// @return le temps représentant la différence *this - AChrono.
  CChrono operator-( const CChrono& AChrono ) const;

  /// Lance le chrono (i.e. initialise FBefore à l'heure actuelle).
  /// Ne fait rien si le chrono est déjà en train de tourner.
  void start ();

  /// Arrête le chrono et ajoute le temps passé depuis le dernier
  /// start dans FTotal. Ne fait rien si le chrono ne tourne pas.
  void stop ();

  /// Remet le chrono à zéro (i.e. FTotal).
  void reset();

  /// Retourne la chaine de caractère composée de AMessage puis du
  /// temps total du chrono. Si le chono tourne, il est arrêté
  /// afin de ne pas compter le temps de cette méthode puis redémarré
  /// ensuite.
  std::string getTime ( const std::string& AMessage );

  /// Affiche AMessage puis le temps total du chrono. Si le chono
  /// tourne, il est arrêté afin de ne pas compter le temps de cette
  /// méthode puis redémarré ensuite.
  void display ( const std::string& AMessage );

  /// Fixe le mode d'affichage :
  /// AValue=true  : mode réel (cad toujours en 10-6 secondes)
  /// AValue=false : mode User (cad l'unité change pour être
  ///                           appropriée en fonction de la valeur)
  void displayRealTime( bool AValue );

  /// Fixe le mode de calcul du temps (@see TModeChrono).
  /// Si le chrono est en train de tourner,  l'arête et le réinitialise.
  void setMode( TModeChrono AValue );

private:
  /// Convertit ATime en format texte, en utilisant FRealTime et
  /// FDisplayReal pour connaitre le mode d'affichage.
  std::string convertTimeval( const struct timeval& ATimeval ) const;  
  
  /// Récupère le temps écoulé en temps réel.
  void pickTimeReel( struct timeval& ATimeval ) const;
  
  /// Récupère le temps écoulé en temps processus.
  void pickTimePus( struct rusage& ARusage ) const;
  
  struct rusage FBeforePus; ///< Temps de départ du start en mode Pus.
  struct rusage FAfterPus;  ///< Temps du stop en mode Pus.
  struct rusage FTotalPus;  ///< Temps cumulé en mode Pus, somme des différents
			    ///<start-stop.
  
  struct timeval FBeforeReel; ///< Temps de départ du start en mode réel.
  struct timeval FAfterReel; ///< Temps du stop en mode réel.
  struct timeval FTotalReel; ///< Temps cumulé en mode réel.
  
  
  bool FDisplayReal; ///< Booléen à vrai ssi l'affichage est en
                     /// mode réel, faux si mode utilisateur.
  TModeChrono FMode; ///< Mode de chronométrage (@see TModeChrono).
  bool FRun; ///< Booléen à vrai ssi le crhono est en train de tourner.   

  friend std::ostream& operator<< (std::ostream&, const CChrono & );
};
//******************************************************************************
//* Ici le code, tout est en inline. *******************************************
//******************************************************************************
inline
CChrono::CChrono( TModeChrono AMode, bool ADisplayReal ) :
  FDisplayReal(ADisplayReal),
  FMode(AMode),
  FRun(false)
{
  FTotalPus.ru_utime.tv_sec  = 0; FTotalPus.ru_utime.tv_usec = 0;

  FTotalPus.ru_stime.tv_sec  = 0; FTotalPus.ru_stime.tv_usec = 0;
 
  FTotalReel.tv_sec = 0; FTotalReel.tv_usec = 0;  
}
//------------------------------------------------------------------------------
inline
CChrono::CChrono( const CChrono& AChrono ) :
  FBeforePus  (AChrono.FBeforePus),
  FAfterPus   (AChrono.FAfterPus),
  FTotalPus   (AChrono.FTotalPus),
  FBeforeReel (AChrono.FBeforeReel),
  FAfterReel  (AChrono.FAfterReel),
  FTotalReel  (AChrono.FTotalReel),
  FDisplayReal(AChrono.FDisplayReal),
  FMode       (AChrono.FMode),
  FRun        (AChrono.FRun)
{}
//******************************************************************************
inline
void addTimeval( struct timeval& ATimeval, const struct timeval& ATimeval2)
{
  ATimeval.tv_sec  += ATimeval2.tv_sec;
  ATimeval.tv_usec += ATimeval2.tv_usec;
  if ( ATimeval.tv_usec >= 1000000 )
    {
      ++ATimeval.tv_sec;
      ATimeval.tv_usec -= 1000000;
    }
}
//------------------------------------------------------------------------------
inline
void substractTimeval( struct timeval& ATimeval, const struct timeval& ATimeval2)
{
  ATimeval.tv_sec  -= ATimeval2.tv_sec;
  ATimeval.tv_usec -= ATimeval2.tv_usec;
  if ( ATimeval.tv_usec < 0 )
    {
      --ATimeval.tv_sec;
      ATimeval.tv_usec += 1000000;
    }
}
//------------------------------------------------------------------------------
inline
void CChrono::pickTimeReel( struct timeval& ATimeval ) const
{
  gettimeofday(&ATimeval,NULL);
}
//------------------------------------------------------------------------------
inline
void  CChrono::pickTimePus( struct rusage& ARusage ) const
{
  getrusage(RUSAGE_SELF, &ARusage);
}
//******************************************************************************
inline
CChrono& CChrono::operator=( const CChrono& AChrono )
{
  FBeforePus   = AChrono.FBeforePus;
  FAfterPus    = AChrono.FAfterPus;
  FTotalPus    = AChrono.FTotalPus;
  
  FBeforeReel  = AChrono.FBeforeReel;
  FAfterReel   = AChrono.FAfterReel;
  FTotalReel   = AChrono.FTotalReel;
  
  FDisplayReal = AChrono.FDisplayReal;
  FMode        = AChrono.FMode;
  FRun	       = AChrono.FRun;

  return *this;
}
//------------------------------------------------------------------------------
inline
CChrono& CChrono::operator+=( const CChrono& AChrono )
{
  addTimeval( FTotalPus.ru_utime, AChrono.FTotalPus.ru_utime );
  addTimeval( FTotalPus.ru_stime, AChrono.FTotalPus.ru_stime );
  addTimeval( FTotalReel, AChrono.FTotalReel );

  return *this;
}
//------------------------------------------------------------------------------
inline
CChrono CChrono::operator+( const CChrono& AChrono ) const
{
  CChrono res(*this);
  res += AChrono;

  return res;
}
//******************************************************************************
inline
CChrono& CChrono::operator-=( const CChrono& AChrono )
{
  substractTimeval( FTotalPus.ru_utime, AChrono.FTotalPus.ru_utime );
  substractTimeval( FTotalPus.ru_stime, AChrono.FTotalPus.ru_stime );
  substractTimeval( FTotalReel, AChrono.FTotalReel );

  return *this;
}
//------------------------------------------------------------------------------
inline
CChrono CChrono::operator-( const CChrono& AChrono ) const
{
  CChrono res(*this);
  res -= AChrono;

  return res;
}
//******************************************************************************
inline
void CChrono::start ()
{
  if ( !FRun )
    {
      pickTimePus ( FBeforePus );
      pickTimeReel( FBeforeReel );
      FRun = true;
    }
}
//------------------------------------------------------------------------------
inline
void CChrono::stop ()
{
  if ( FRun )
    {
      // Récupération de l'horloge courante.
      pickTimeReel( FAfterReel );
      pickTimePus ( FAfterPus );

      // Calcul de FAfter = FAfter-FBefore
      substractTimeval( FAfterPus.ru_utime, FBeforePus.ru_utime );
      substractTimeval( FAfterPus.ru_stime, FBeforePus.ru_stime );
      substractTimeval( FAfterReel, FBeforeReel );
      
      // Ajout dans FTotal
      addTimeval( FTotalPus.ru_utime, FAfterPus.ru_utime );
      addTimeval( FTotalPus.ru_stime, FAfterPus.ru_stime );
      addTimeval( FTotalReel, FAfterReel );
      
      FRun = false;
    }  
}
//------------------------------------------------------------------------------
inline
void CChrono::reset()
{
  FTotalPus.ru_utime.tv_sec  = 0; FTotalPus.ru_utime.tv_usec = 0;

  FTotalPus.ru_stime.tv_sec  = 0; FTotalPus.ru_stime.tv_usec = 0;
  
  FTotalReel.tv_sec = 0; FTotalReel.tv_usec = 0;  

  FRun = false;
}
//******************************************************************************
inline
void CChrono::setMode( TModeChrono AMode )
{
  FMode = AMode;
  if ( FRun )
    {
      stop();
      reset();
    }
}
//------------------------------------------------------------------------------
inline
void CChrono::displayRealTime( bool AValue )
{ FDisplayReal = AValue; }
//******************************************************************************
inline
std::string CChrono::convertTimeval ( const struct timeval & ATimeval ) const
{
  std::ostringstream res;
  
  if ( FDisplayReal )
    { // Mode réel.
	  res<<ATimeval.tv_sec<<'.'
	     <<std::setw(6)<<std::setfill('0')<<ATimeval.tv_usec<<" s";
    }
  else
    {
      // Mode utilisateur cad on prend l'unité appropriée.
      if ( ATimeval.tv_sec!=0 || ATimeval.tv_usec>=150000 )
	res<<ATimeval.tv_sec<<'.'
	   <<std::setw(2)<<std::setfill('0')
	   <<(int)(ATimeval.tv_usec/10000)<<" s";	
	{ // Ici il y a 0 secondes
	  if ( ATimeval.tv_usec>1500 )
		  res<<std::setprecision(2)<<std::fixed
				  <<ATimeval.tv_usec/1000.<<" ms (10-3 s)";
	  else
	    res<<ATimeval.tv_usec<<" us (10-6 s)";
	}
    }

  return res.str();
}
//******************************************************************************
inline
std::string CChrono::getTime ( const std::string& AMessage )
{
  bool rerun = false;
  std::ostringstream res;

  if ( FRun )
    {
      rerun = true;
      stop();
    }
  
  res<<AMessage<<" : ";
  if ( FMode==PUS_MODE || FMode==REEL_AND_PUS_MODE )
    {
      res<<convertTimeval(FTotalPus.ru_utime)<<" user, ";  
      res<<convertTimeval(FTotalPus.ru_stime)<<" sys";
    }
  
	if ( FMode==REEL_AND_PUS_MODE ) res<<", ";
	
  if ( FMode==REEL_MODE || FMode==REEL_AND_PUS_MODE )
    res<<convertTimeval(FTotalReel)<<" reel";

  if ( rerun )
    start();
  
  return res.str();
}
//------------------------------------------------------------------------------
inline
void CChrono::display ( const std::string& AMessage )
{  std::cout<<getTime(AMessage)<<std::endl; }
//------------------------------------------------------------------------------
inline
std::ostream& operator<< (std::ostream& os, const CChrono & c )
{
  os << c.FTotalReel.tv_sec<<'.'
     <<std::setw(6)<<std::setfill('0')<<c.FTotalReel.tv_usec;
  return os;
}
//******************************************************************************
#endif // CHRONO_HH
//******************************************************************************

