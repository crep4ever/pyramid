//==========================================================
// LoggerFile.h
//
// Définition de la classe CLoggerFile
//
//==========================================================

#ifndef LOGGERFILE_H
#define LOGGERFILE_H

//==========================================================
// En-têtes
//==========================================================
#include "logger.h"
#include <fstream>


namespace fogrimmi
{
  //==========================================================
  /// Logger inscrivant les messages dans un fichier
  //==========================================================
  class IM_EXPORT CLoggerFile : public ILogger
  {
  public :

    //----------------------------------------------------------
    /// Constructeur par défaut
    //----------------------------------------------------------
    CLoggerFile(const std::string& Filename = "Output.log");

  private :

    //----------------------------------------------------------
    /// Destructeur
    //----------------------------------------------------------
    ~CLoggerFile();

    //----------------------------------------------------------
    /// Log une message
    //----------------------------------------------------------
    virtual void Write(const std::string& Message);

    //----------------------------------------------------------
    // Donnees membres
    //----------------------------------------------------------
    /// Fichier log
    std::ofstream m_File;

  };

} // namespace fogrimmi


#endif // LOGGERFILE_H
