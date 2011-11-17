//==========================================================
// Export.h
//
// Définition de macros d'exportation
// Utile pour creer une DLL windows
//
//==========================================================

#ifndef EXPORT_H
#define EXPORT_H

namespace fogrimmi
{
    //==========================================================
    // Macro d'exportation / importation
    //==========================================================

#ifdef _WIN32
    #ifdef IM_EXPORTS
    #   define IM_EXPORT __declspec(dllexport)
    #else
    #   define IM_EXPORT __declspec(dllimport)
    #endif
#else
#ifdef IM_EXPORTS
    #   define IM_EXPORT
    #else
    #   define IM_EXPORT
    #endif
#endif

    //==========================================================
    // Desactivation du warning 4251
    // "T1 necessite une interface DLL pour être utilise(e) par
    // les clients de class T2"
    //==========================================================
    #ifdef _MSC_VER
    #   pragma warning(disable : 4251)
    #endif

} // namespace fogrimmi


#endif // EXPORT_H
