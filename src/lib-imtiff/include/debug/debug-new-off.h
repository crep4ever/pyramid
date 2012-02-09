///========================================================== \n
/// DebugNewOff.h \n
/// \n
/// Annulation des macros new et delete, malloc, calloc .... \n
/// Necessaire lorsqu'on a inclu DebugNew.h dans un header, \n
/// il faut alors inclure DebugNewOff.h a la fin de celui-ci \n
/// pour ne pas perturber le comportement des headers inclus \n
/// par la suite \n
/// \n
///========================================================== \n

#ifndef DEBUGNEWOFF_H
#define DEBUGNEWOFF_H

#undef new
#undef delete

#undef malloc
#undef calloc
#undef realloc
//#undef strdup
#undef free

#endif // DEBUGNEWOFF_H
