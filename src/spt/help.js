/*******************************************************************************
   Copyright (C) 2011-2018 SequoiaDB Ltd.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*******************************************************************************/
 const help = function( val ) {
   if ( val == undefined )
   {
	  println("   --Get help information:");
      println("   help(<method>)                                     - Help on specified method, e.g. help(\'sleep\').");
	  println("   help('help')                                       - For more detail of help.");
	  println("");
	  println("   --Global functions: ");
	  println("   showClass([className])                             - Show all class name or class's function name.");
	  globalHelp() ;
	  println("");
	  println("   --General commands: ");
      println("   clear                                              - Clear the terminal screen.");
      println("   history -c                                         - Clear the history.");
      println("   quit                                               - Exit.");
   }
   else
   {
      globalHelp( val ) ;
   }
}

