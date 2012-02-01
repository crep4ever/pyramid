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
#include "config.hh"
//******************************************************************************
void usage( const char * )
{}
//------------------------------------------------------------------------------
int main( int argc, char * argv[] )
{
  QApplication app(argc, argv);

  QApplication::setOrganizationName("XLIM-SIC");
  QApplication::setOrganizationDomain("univ-poitiers.fr");
  QApplication::setApplicationName(PYRAMID_APPLICATION_NAME);
  QApplication::setApplicationVersion(PYRAMID_VERSION);

  Q_INIT_RESOURCE(pyramid);

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
  QDir translationDirectory;
  QString translationFilename = QString("songbook_%1.qm").arg(QLocale::system().name());
  QString directory;

  translationDirectory = QDir(PYRAMID_DATA_PATH);
  if (translationDirectory.exists())
    directory = translationDirectory.absoluteFilePath("lang");
  else
    directory = QDir::current().absoluteFilePath("lang");

  QTranslator translator;
  translator.load(translationFilename, directory);
  app.installTranslator(&translator);

  CMainWindow mainWindow;
  mainWindow.show();
  return app.exec();
}
