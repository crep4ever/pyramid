//==========================================================
// MemoryManager.h
//
// Définition de la classe CMemoryManager
//
//==========================================================

#ifndef _MEMORYMANAGER_H
#define _MEMORYMANAGER_H

//==========================================================
// En-têtes
//==========================================================
//#include <Config/Export.h>
//#include "Singleton.h"
#include "export.h"
#include "file.h"
#include <fstream>
#include <map>
#include <stack>
#include <string>



namespace fogrimmi
{
  //==========================================================
  /// Gestionnaire de memoire - detecte les fuites
  //==========================================================
  class IM_EXPORT CMemoryManager
  {

  public :


    //----------------------------------------------------------
    /// Renvoie l'instance de la classe
    //----------------------------------------------------------
    static CMemoryManager& Instance();


    //----------------------------------------------------------
    /// Ajoute une allocation memoire
    //----------------------------------------------------------
    void* Allocate(std::size_t Size, const CFile& File, int Line, bool Array);

    //----------------------------------------------------------
    /// Retire une allocation memoire
    //----------------------------------------------------------
    void Free(void* Ptr, bool Array);

    //----------------------------------------------------------
    /// Sauvegarde les infos sur la desallocation courante
    //----------------------------------------------------------
    void NextDelete(const CFile& File, int Line);


  private :

    //----------------------------------------------------------
    /// Constructeur par défaut
    //----------------------------------------------------------
    CMemoryManager();

    //----------------------------------------------------------
    /// Destructeur
    //----------------------------------------------------------
    ~CMemoryManager();

    //----------------------------------------------------------------
    // Données membres
    //----------------------------------------------------------------
    /// Instance de la classe
    static CMemoryManager Inst;



    //----------------------------------------------------------
    /// Inscrit le rapport sur les fuites de memoire
    //----------------------------------------------------------
    void ReportLeaks();

    //----------------------------------------------------------
    /// Structure definissant les informations recuperer par l'allocateur avec placement
    //----------------------------------------------------------
    struct TBlock
    {
      /// Taille allouee
      std::size_t Size;
      /// Fichier contenant l'allocation
      CFile       File;
      /// Ligne de l'allocation
      int         Line;
      /// Est-ce un objet ou un tableau ?
      bool        Array;
    };
    /// table d'allocation desallocation en memoire, permet de tracer
    typedef std::map<void*, TBlock> TBlockMap;

    //----------------------------------------------------------
    // Données membres
    //----------------------------------------------------------
    /// Fichier de sortie pour le log
    std::ofstream      m_File;
    /// Blocs de memoire alloues
    TBlockMap          m_Blocks;

    /// Pile dont le sommet contient la ligne et le fichier de la prochaine desallocation
    std::stack<TBlock> m_DeleteStack;
  };


} // namespace fogrimmi


#endif // MEMORYMANAGER_H
