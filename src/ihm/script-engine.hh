/*
 * Copyright (C) 2009, Alexandre Dupas <alexandre.dupas@gmail.com>
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
#ifndef __SCRIPT_ENGINE_HH__
#define __SCRIPT_ENGINE_HH__
//******************************************************************************
#include <QtScript>
#include <QObject>
#include <QString>
//******************************************************************************

/**
 * \file script-engine.hh
 * Class handling the script engine of the application.
 *
 * \author Alexandre Dupas
 * \version 0.1
 * \date 13 June 2009
 * \class CScriptEngine "script-engine.hh" 
 * \brief CScriptEngine is a scripting engine allowing one to script 
 *  some part of the application
 */
class CScriptEngine : public QScriptEngine
{
  Q_OBJECT

public:
  /// getter on the instance
  static CScriptEngine * engine();

  /// Destructor
  ~CScriptEngine();

signals:

public slots:
  /// execute the script in file
  void execute( QTextStream & stream, const QString & filename = QString() );

  /// execute the script in file
  void execute( QFile & file );

  /// execute the script in the given file
  void execute( const QString & filename );

protected:

private:
  /// Constructor 
  CScriptEngine( QObject * parent = 0 );

  static CScriptEngine * m_engine;
};

#endif  // __SCRIPT_ENGINE_HH__
