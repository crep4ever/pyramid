//==========================================================
// Exception.h
//
// Exceptions définies
//
//==========================================================

#ifndef EXCEPTION_H
#define EXCEPTION_H

//==========================================================
// En-têtes
//==========================================================
#include <exception>
#include <string>

#include "logger-file.h"


namespace fogrimmi
{
  //==========================================================
  /// Classe de base pour les exceptions
  //==========================================================
  class IM_Exception : public std::exception
    {
    public :

      //----------------------------------------------------------
      /// Constructeur
      //----------------------------------------------------------
      IM_Exception(const std::string& Message = "");

      //----------------------------------------------------------
      /// Destructeur
      //----------------------------------------------------------
      virtual ~IM_Exception() throw();

      //----------------------------------------------------------
      /// Renvoie le message associe a l'exception
      //----------------------------------------------------------
      virtual const char* what() const throw();




    protected :

      //----------------------------------------------------------
      // Données membres
      //----------------------------------------------------------

      /// Message associe a l'exception
      std::string m_Message;
    };


  //==========================================================
  /// Exception lancee si une condition n'est pas verifiee
  //==========================================================
  struct IM_AssertException : public IM_Exception
  {
    IM_AssertException(const std::string& File, int Line, const std::string& Message);
  };
#ifdef _DEBUG_
#   define Assert(condition) if (!(condition)) throw IM_AssertException(__FILE__, __LINE__, "Condition non satisfaite\n\n" #condition)
#else
  inline void DoNothing(bool) {}
#   define Assert(condition) DoNothing(!(condition))
#endif


  //==========================================================
  /// Anomalie d'allocation memoire
  //==========================================================
  struct IM_BadDelete : public IM_Exception
  {
    IM_BadDelete(const void* Ptr, const std::string& File, int Line, bool NewArray);
  };


  //==========================================================
  /// Exception lancee lors d'erreur de chargement de fichiers
  //==========================================================
  struct IM_LoadingFailed : public IM_Exception
  {
    IM_LoadingFailed(const std::string& File, const std::string& Message);
  };

  //==========================================================
  /// Exception lancée lors d'erreur de chargement de fichiers
  //==========================================================
  struct IM_WritingFailed : public IM_Exception
  {
    IM_WritingFailed(const std::string& File, const std::string& Message);
  };


  //==========================================================
  /// Exception lancee lors de saturations de memoire
  //==========================================================
  struct IM_OutOfMemory : public IM_Exception
  {
    IM_OutOfMemory(const std::string& File, int Line, const std::string& Message);
  };

  //=============================================================
  /// Exception lancee lors d'erreur non supportee ou non attrapee
  //=============================================================
  struct IM_Unsupported : public IM_Exception
  {
    IM_Unsupported(const std::string& File, int Line, const std::string& Message);
  };

} // namespace fogrimmi


#endif // EXCEPTION_H
