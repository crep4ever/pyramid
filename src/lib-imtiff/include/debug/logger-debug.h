//==========================================================
// LoggerDebug.h
//
// Définition de la classe CLoggerDebug
//
//==========================================================

#ifndef LOGGERDEBUG_H
#define LOGGERDEBUG_H

//==========================================================
// En-têtes
//==========================================================
#include "logger.h"


namespace fogrimmi
{
  //==========================================================
  /// Logger inscrivant les messages dans un fichier
  //==========================================================
  class IM_EXPORT CLoggerDebug : public ILogger
  {
  private :

    //----------------------------------------------------------
    /// Log une message
    //----------------------------------------------------------
    virtual void Write(const std::string& Message);
  };

} // namespace fogrimmi


#endif // LOGGERDEBUG_H
