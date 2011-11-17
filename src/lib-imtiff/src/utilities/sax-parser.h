
#ifndef _SAXPARSER_H_
#define _SAXPARSER_H_


#include "/usr/include/libxml++-2.6/libxml++/libxml++.h"
//#include <libxml++/libxml++.h>

#include <map>

namespace fogrimmi
{
  //==========================================================
  /// Gestion de document XML par la methode SAX
  //==========================================================
  class IM_SaxParser : public xmlpp::SaxParser
    {
    public:
      /// Constructeur par defaut
      IM_SaxParser();
      /// Destructeur
      virtual ~IM_SaxParser();

      /// liste d'attributs
      typedef std::multimap<Glib::ustring, Glib::ustring> attributes;
      /// Liste de nodes
      typedef std::multimap<Glib::ustring, attributes> nodes;

      /// retourne l'arbre
      nodes getNodes(){return tree;};

    protected:
      //overrides:
      /// callback
      virtual void on_start_document();
      /// callback
      virtual void on_end_document();
      /// callback
      virtual void on_start_element(const Glib::ustring& name,
				    const AttributeList& properties);
      /// callback
      virtual void on_end_element(const Glib::ustring& name);
      /// callback
      virtual void on_characters(const Glib::ustring& characters);
      /// callback
      virtual void on_comment(const Glib::ustring& text);
      /// callback
      virtual void on_warning(const Glib::ustring& text);
      /// callback
      virtual void on_error(const Glib::ustring& text);
      /// callback
      virtual void on_fatal_error(const Glib::ustring& text);


      /// arbre
      nodes tree;



    };
}//fogrimmi

#endif //_SAXPARSER_H

