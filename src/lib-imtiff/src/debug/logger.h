//==========================================================
// Logger.h
//
// Définition de la classe ILogger
//
//==========================================================

#ifndef LOGGER_H
#define LOGGER_H

//==========================================================
// En-têtes
//==========================================================
#include <sstream>
#include <string>
#include "export.h"

namespace fogrimmi
{
  //==========================================================
  /// Classe de base abstraite pour les loggers
  //==========================================================
  class IM_EXPORT ILogger
  {
  public :

    //----------------------------------------------------------
    /// Destructeur
    //----------------------------------------------------------
    virtual ~ILogger();

    //----------------------------------------------------------
    /// Change l'instance du logger
    //----------------------------------------------------------
    static void SetLogger(ILogger* Logger);

    //----------------------------------------------------------
    /// Log un message (façon C)
    //----------------------------------------------------------
    static void Log(const char* Format, ...);

    //----------------------------------------------------------
    /// Log un message (façon C++)
    //----------------------------------------------------------
    static ILogger& Log();
    template <class T> ILogger& operator <<(const T& ToLog);

  protected :

    //----------------------------------------------------------
    /// Renvoie la date courante
    //----------------------------------------------------------
    std::string CurrentDate() const;

    //----------------------------------------------------------
    /// Renvoie l'heure courante
    //----------------------------------------------------------
    std::string CurrentTime() const;

  private :

    //----------------------------------------------------------
    /// Inscrit un message - a surdefinir dans les classes derivees
    //----------------------------------------------------------
    virtual void Write(const std::string& Message) = 0;

    //----------------------------------------------------------
    /// Détruit l'instance du logger
    //----------------------------------------------------------
    static void Destroy();

    //----------------------------------------------------------
    // Donnees membres
    //----------------------------------------------------------
    /// Pointeur sur le logger actuel
    static ILogger* s_Instance;

    friend class IM_Image;
  };


  //----------------------------------------------------------
  /// Surcharge de l'operateur << pour logger des messages
  //----------------------------------------------------------
  template <class T> ILogger& ILogger::operator <<(const T& ToLog)
    {
      std::ostringstream Stream;
      Stream << ToLog;
      Write(Stream.str());

      return Log();
    }

} // namespace fogrimmi


#endif // LOGGER_H
