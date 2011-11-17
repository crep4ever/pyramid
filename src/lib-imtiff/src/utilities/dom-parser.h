
#ifndef _DOMPARSER_H_
#define _DOMPARSER_H_

#include <libxml++/libxml++.h>

#include <iostream>

namespace fogrimmi
{
  //==========================================================
  /// Gestion de document XML par la methode DOM
  //==========================================================
  class IM_DomParser
  {
  public:
    /// Constructeur
    IM_DomParser(const Glib::ustring &filePath);
    /// Destructeur
    ~IM_DomParser();
    /// Test une expression xpath et renvoie les noeuds trouves
    xmlpp::NodeSet find(const Glib::ustring &xpath);

  private:
    /// Dom parser libxml++
    xmlpp::DomParser* parser;




  };
}// namespace fogrimmi

#endif //_DOMPARSER_H_

