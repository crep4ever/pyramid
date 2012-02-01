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
#include "script-engine.hh"
#include "controler.hh"
#include <QDebug>
//******************************************************************************

/// Meta types used by the script engine
Q_DECLARE_METATYPE(CControler*)

/// Helper function that converts type to script value
QScriptValue controlerToScriptValue( QScriptEngine * engine,
				     CControler * const & in )
{
  return engine->newQObject(in);
}
void controlerFromScriptValue( const QScriptValue & object,
			       CControler * & out )
{
  out = qobject_cast<CControler*>(object.toQObject());
}
//-----------------------------------------------------------------------------
CScriptEngine * CScriptEngine::m_engine = NULL;
//-----------------------------------------------------------------------------
CScriptEngine::CScriptEngine( QObject * AParent )
  : QScriptEngine( AParent )
{
  QScriptValue pyramid = newQObject( new CControler(NULL) );
  globalObject().setProperty("pyramid", pyramid );

  // Register CControler * as a type for the script engine
  qScriptRegisterMetaType( this,
			   controlerToScriptValue,
			   controlerFromScriptValue );

}
//-----------------------------------------------------------------------------
CScriptEngine::~CScriptEngine()
{}
//-----------------------------------------------------------------------------
CScriptEngine * CScriptEngine::engine()
{
  if( m_engine == NULL )
    m_engine = new CScriptEngine();
  return m_engine;
}
//-----------------------------------------------------------------------------
void CScriptEngine::execute( QTextStream & stream, const QString & filename )
{
  QScriptValue result = evaluate( stream.readAll(), filename );

  if( hasUncaughtException() )
    {
      int line = uncaughtExceptionLineNumber();
      qDebug() << "error at " << filename << ":"<< line << ": "
	       << result.toString();
    }
}
//-----------------------------------------------------------------------------
void CScriptEngine::execute( QFile & file )
{
  QTextStream stream( &file );
  execute( stream, file.fileName() );
}
//-----------------------------------------------------------------------------
void CScriptEngine::execute( const QString & filename )
{
  QFile file( filename );
  if( !file.open(QIODevice::ReadOnly) )
    {
      //qDebug << "Error: " << filename << " not found";
    }
  execute( file );
  file.close();
}
//-----------------------------------------------------------------------------
