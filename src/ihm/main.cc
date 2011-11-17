/*
 * Copyright (C) 2008-2011, Romain Goffe <romain.goffe@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

//******************************************************************************
#include <QApplication>
#include "script-engine.hh"
#include "main-window.hh"
//******************************************************************************
void usage( const char * )
{}
//------------------------------------------------------------------------------
int main( int argc, char * argv[] )
{
  QApplication app(argc, argv);
  Q_INIT_RESOURCE(pyramid);

  QString version = QString("0.1 (%1)")
    .arg(QDate::currentDate().toString(Qt::SystemLocaleLongDate));
  QCoreApplication::setApplicationVersion(version);
  QCoreApplication::setOrganizationName("XLIM-SIC");
  QCoreApplication::setOrganizationDomain("univ-poitiers.fr");
  QCoreApplication::setApplicationName("pyramid");

  bool isGui = true;
  bool isScript = false;

  QString filename;

  for ( int i = 1; i < argc; ++i )
    {
      if (   !strcmp(argv[i], "-h") 
	     || !strcmp(argv[i], "-?") 
	     || !strcmp(argv[i], "--help"))
	{ 
	  usage(argv[0]);
	  exit(EXIT_SUCCESS);
	}
      else if( !strcmp(argv[i], "--nogui") )
	{
	  isGui = false;
	}
      else if( !strcmp(argv[i], "--gui") )
	{
	  isGui = true;
	}
      else
	{
	  // process script file or other parameter unknown
	  QString parameter(argv[i]);
	  if( parameter.endsWith(".qs") )
	    {
	      isScript = true;
	      filename = parameter;
	    }
	}
    }

  if( isScript )
    CScriptEngine::engine()->execute( filename );
  
  if( !isGui )
    exit(EXIT_FAILURE);
  
  // Localization
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8")) ;
  QString locale = QLocale::system().name().section('_', 0, 0);
  QString translation = QString("pyramid_%1").arg(locale) + ".qm";
  QString dir;
  const QDir systemDir("/usr/share/pyramid/translations", "*.qm");
  const QDir userDir("/usr/local/share/pyramid/translations", "*.qm");
  if (systemDir.entryList(QDir::Files | QDir::Readable).contains(translation))
    dir = systemDir.absolutePath();
  else if (userDir.entryList(QDir::Files | QDir::Readable).contains(translation))
    dir = userDir.absolutePath();
  else
    dir = QString("%1%2lang").arg(QDir::currentPath()).arg(QDir::separator());

  QTranslator translator;
  translator.load(QString("pyramid_%1").arg(locale), dir);
  app.installTranslator(&translator);
  
  CMainWindow mainWindow;
  mainWindow.show();
  return app.exec();
}
