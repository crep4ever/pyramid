//==========================================================
// IM_File.h
//
// Définition de la classe CFile
//
//==========================================================

#ifndef FILE_H
#define FILE_H

//==========================================================
// En-têtes
//==========================================================

#include "export.h"
#include <sstream>
#include <string>


namespace fogrimmi
{
  //==========================================================
  /// Classe facilitant la manipulation des fichiers
  //==========================================================
  class IM_EXPORT CFile
  {
  public :

    //----------------------------------------------------------
    /// Constructeur à partir d'un std::string
    //----------------------------------------------------------
    CFile(const std::string& Name = "unknown");

    //----------------------------------------------------------
    /// Constructeur à partir d'un const char*
    //----------------------------------------------------------
    CFile(const char* Name);

			
    //----------------------------------------------------------
    /// Indique si le fichier existe ou non
    //----------------------------------------------------------
    bool Exists() const;

    //----------------------------------------------------------
    /// Renvoie le nom du fichier avec son chemin complet
    //----------------------------------------------------------
    const std::string& Fullname() const;

    //----------------------------------------------------------
    /// Renvoie le nom du fichier sans son chemin
    //----------------------------------------------------------
    std::string Filename() const;

    //----------------------------------------------------------
    /// Renvoie le nom du fichier sans extension ni chemin
    //----------------------------------------------------------
    std::string ShortFilename() const;

    //----------------------------------------------------------
    /// Renvoie l'extension du fichier
    //----------------------------------------------------------
    std::string Extension() const;

    // template <class T> CFile& operator <<(const T& ToFile);

  private :

    //----------------------------------------------------------
    // Données membres
    //----------------------------------------------------------
    /// Chemin complet du fichier
    std::string m_Name;

    /*
    //----------------------------------------------------------
    /// Inscrit un message - à surdéfinir dans les classes dérivées
    //----------------------------------------------------------
    void Write(const std::string& Message);
    */
  };

  /*
  //----------------------------------------------------------
  /// Surcharge de l'opérateur << pour logger des messages
  //----------------------------------------------------------
  template <class T>CFile& CFile::operator <<(const T& ToFile)
  {
  std::ostringstream Stream;
  Stream << ToFile;
  Write(Stream.str());

  return this;
  }
  */

} // namespace fogrimmi


#endif // FILE_H
