//==========================================================
// DebugNew.h
//
// Surcharge des opérateurs / fonctions d'allocation et de
// désallocation de la mémoire pour traquer les fuites
//
//==========================================================
#ifndef _DEBUG_
#include <cstdlib>
#include <cstring>
#endif

#ifdef _DEBUG_


#ifndef _DEBUGNEW_H
#define _DEBUGNEW_H

//==========================================================
// En-têtes
//==========================================================
#include "Debug/IM_MemoryManager.h"
#include <iostream>

////////////////////////////////////////////////////////////
/// Surcharge de l'operateur new \n
/// \n
/// [in] Size : Taille a allouer \n
/// [in] File : Fichier source \n
/// [in] Line : Ligne dans le fichier source \n
/// \n
/// [retval] Pointeur sur la zone allouee
///
////////////////////////////////////////////////////////////
inline void* operator new(std::size_t Size, const char* File, int Line)
{

    return fogrimmi::CMemoryManager::Instance().Allocate(Size, File, Line, false);

}


////////////////////////////////////////////////////////////
/// Surcharge de l'operateur new[] \n
/// \n
/// [in] Size : Taille a allouer \n
/// [in] File : Fichier source \n
/// [in] Line : Ligne dans le fichier source \n
/// \n
/// [retval] Pointeur sur la zone allouee \n
///
////////////////////////////////////////////////////////////
inline void* operator new[](std::size_t Size, const char* File, int Line)
{
    return fogrimmi::CMemoryManager::Instance().Allocate(Size, File, Line, true);

}


////////////////////////////////////////////////////////////
/// Surcharge de l'operateur delete \n
/// \n
/// [in] Ptr : Pointeur sur la zone a liberer \n
///
////////////////////////////////////////////////////////////
inline void operator delete(void* Ptr)
{
    fogrimmi::CMemoryManager::Instance().Free(Ptr, false);

}


////////////////////////////////////////////////////////////
/// Surcharge de l'opérateur delete \n
/// \n
/// [in] Ptr :  Pointeur sur la zone a liberer \n
/// [in] File : Fichier source \n
/// [in] Line : Ligne dans le fichier source \n
///
////////////////////////////////////////////////////////////
inline void operator delete(void* Ptr, const char* File, int Line)
{
    fogrimmi::CMemoryManager::Instance().NextDelete(File, Line);

    fogrimmi::CMemoryManager::Instance().Free(Ptr, false);

}


////////////////////////////////////////////////////////////
/// Surcharge de l'operateur delete[] \n
/// \n
/// [in] Ptr : Pointeur sur la zone a liberer \n
///
////////////////////////////////////////////////////////////
inline void operator delete[](void* Ptr)
{
    fogrimmi::CMemoryManager::Instance().Free(Ptr, true);

}


////////////////////////////////////////////////////////////
/// Surcharge de l'opérateur delete[] \n
/// \n
/// [in] Ptr :  Pointeur sur la zone à libérer \n
/// [in] File : Fichier source \n
/// [in] Line : Ligne dans le fichier source \n
///
////////////////////////////////////////////////////////////
inline void operator delete[](void* Ptr, const char* File, int Line)
{
    fogrimmi::CMemoryManager::Instance().NextDelete(File, Line);

    fogrimmi::CMemoryManager::Instance().Free(Ptr, true);

}




//----------------- Allocation desallocation securisee ---------------------


//-----------------------------------------------------------------------------
/// free securise : utilise pour le conteneur
inline void FreeSecurise (void *Ptr, const char* File, const unsigned long Line)
{
  fogrimmi::CMemoryManager::Instance().NextDelete(File, Line);

  fogrimmi::CMemoryManager::Instance().Free(Ptr, false);
}



/// Allocation de mémoire sécurisée (malloc) : test si malloc renvoie NULL
inline void* MallocSecurise (size_t Size, const char* File, const unsigned long Line)
{
  return fogrimmi::CMemoryManager::Instance().Allocate(Size, File, Line, false);
  // Retourne le pointeur

}

//-----------------------------------------------------------------------------

/// calloc sécurisé : test si calloc renvoie NULL
inline void* CallocSecurise (size_t nitems, size_t Size,
                      const char* File, const unsigned long Line)
{
  void* ptr;

  // Alloue de la mémoire
  ptr = fogrimmi::CMemoryManager::Instance().Allocate(nitems, File, Line, false);

  // Mise à zéro des octets (fonctionnalité de 'calloc'=malloc+memset)
  memset (ptr,0,Size);

  // Retourne le pointeur
  return ptr;
}

//-----------------------------------------------------------------------------

/// realloc sécurisé : vérifie que l'allocation se soit bien produite,
/// affiche une erreur sinon
inline void* ReallocSecurise (void* ptr, size_t Size,
                       const char* File, const unsigned long Line)
{


  // Si l'ancien pointeur actuel vaut NULL : appelle malloc
  if (ptr == NULL) return fogrimmi::CMemoryManager::Instance().Allocate(Size, File, Line, false);

  // Ajoute la taille des informations sur le pointeur
  //TailleAllouee = MallocCalculeTaille (size);

  // Renvoie le pointeur
  return fogrimmi::CMemoryManager::Instance().Allocate(Size, File, Line, false);
}

//-----------------------------------------------------------------------------

/// Crée une copie d'une chaîne avec test si l'allocation renvoie NULL
inline char* StrdupSecurise(const char* src, const char* nom_fich, const unsigned long num_ligne)
{
  // Tente de copier la chaine
  char* ptr;


  // Alloue la mémoire
  ptr = (char *)fogrimmi::CMemoryManager::Instance().Allocate(strlen(src)+1, nom_fich, num_ligne, true);


  // Copie la chaine
  strcpy (ptr,src);

  // Retourne le pointeur
  return ptr;
}




#endif // DEBUGNEW_H


///========================================================== \n
/// Définition de macros servant à automatiser le tracking \n
/// /!\ sortir des directives anti-réinclusions ! \n
///========================================================== \n

#ifndef new
    #define new    new(__FILE__, __LINE__)
    #define delete fogrimmi::CMemoryManager::Instance().NextDelete(__FILE__, __LINE__), delete
    #define malloc(size) MallocSecurise(size, __FILE__, __LINE__)
    #define realloc(ptr,new_size) ReallocSecurise(ptr, new_size, __FILE__, __LINE__)
    #define calloc(n,size) CallocSecurise(n, size, __FILE__, __LINE__)
    #define free(ptr) FreeSecurise(ptr, __FILE__, __LINE__)
    //#define strdup(src) StrdupSecurise(src, __FILE__, __LINE__)

    //const unsigned long TAILLE_TAMPON_OVERFLOW=16;

    // Nombres magiques pour valider un pointeur
    //#define MALLOC_NBR_MAGIQUE 0x1A71A25C

#endif

#endif // _DEBUG_
