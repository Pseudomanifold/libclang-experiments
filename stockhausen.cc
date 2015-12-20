#include <clang-c/Index.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

std::map<std::string, std::string> typeToNoteMap1 =
{
    { "DeclStmt"  , "C" },
    { "ForStmt"   , "D" },
    { "ParmDecl"  , "E" },
    { "IfStmt"    , "F" },
    { "ReturnStmt", "G" }
};

std::map<std::string, std::string> typeToNoteMap2 =
{
  { "ParmDecl"      , "G"  },
  { "VarDecl"       , "G"  },
  { "DeclStmt"      , "G"  },
  { "DeclRefExpr"   , "C" },
  { "ReturnStmt"    , "E"  },
  { "CallExpr"      , "A"  },
  { "MemberRefExpr" , "A"  },
  { "UnaryOperator" , "D"  },
  { "BinaryOperator", "^D" },
  { "IfStmt"        , "F"  },
  { "ForStmt"       , "^F" },
  { "ContinueStmt"  , "B"  }
};

std::string getCursorKindName( CXCursorKind cursorKind )
{
  CXString kindName  = clang_getCursorKindSpelling( cursorKind );
  std::string result = clang_getCString( kindName );

  clang_disposeString( kindName );
  return result;
}

std::string getCursorSpelling( CXCursor cursor )
{
  CXString cursorSpelling = clang_getCursorSpelling( cursor );
  std::string result      = clang_getCString( cursorSpelling );

  clang_disposeString( cursorSpelling );
  return result;
}

CXChildVisitResult countDepth( CXCursor /* cursor */, CXCursor /* parent */, CXClientData clientData )
{
  unsigned int* depth = reinterpret_cast<unsigned int*>( clientData );
  *depth              = *depth + 1;

  return CXChildVisit_Recurse;
}

CXChildVisitResult functionVisitor( CXCursor cursor, CXCursor parent, CXClientData clientData )
{
  CXCursorKind parentKind = clang_getCursorKind( parent );
  CXCursorKind cursorKind = clang_getCursorKind( cursor );
  unsigned int* curLevel  = reinterpret_cast<unsigned int*>( clientData );

  std::string  note;
  unsigned int length = 0;

  bool parentIsFunction =  ( parentKind == CXCursor_FunctionDecl )
                        || ( parentKind == CXCursor_FunctionTemplate )
                        || ( parentKind == CXCursor_CXXMethod );

  if( parentIsFunction && cursorKind == CXCursor_ParmDecl )
  {
    CXToken* tokens        = nullptr;
    unsigned int numTokens = 0;

    auto&& translationUnit = clang_Cursor_getTranslationUnit( cursor );

    clang_tokenize( translationUnit,
                    clang_getCursorExtent( cursor ),
                    &tokens,
                    &numTokens );

    note   = typeToNoteMap2.at( getCursorKindName( cursorKind ) );
    length = numTokens;

    clang_disposeTokens( translationUnit,
                         tokens,
                         numTokens );
  }
  else if( !parentIsFunction )
  {
    unsigned int numChildren = 0;

    clang_visitChildren( cursor,
                         countDepth,
                         &numChildren );

    length = numChildren;

    auto&& translationUnit = clang_Cursor_getTranslationUnit( cursor );
    auto&& extent          = clang_getCursorExtent( cursor );
    CXToken* tokens        = nullptr;
    unsigned int numTokens = 0;

    clang_tokenize( translationUnit,
                    extent,
                    &tokens,
                    &numTokens );

    // FIXME: Make this configurable. Different ways of traversing the tree
    // might be interesting...
    length = numTokens;

    clang_disposeTokens( translationUnit, tokens, numTokens );

    if( length > 8 )
      length = 8;

    auto itPos = typeToNoteMap2.find( getCursorKindName( cursorKind ) );
    if( itPos != typeToNoteMap2.end() )
      note = itPos->second;
    else
      std::cerr << getCursorKindName( cursorKind ) << ": No note assigned\n";
  }

  if( !note.empty() )
  {
    unsigned int octave = *curLevel < 2 ? *curLevel : 2;

    if( octave >= 1 )
      --octave;

    if( octave >= 1 )
    {
      std::transform( note.begin(), note.end(), note.begin(), ::tolower );
      --octave;
    }

    std::cout << note << std::string( octave, '\'' ) << length << " ";
  }

  unsigned int nextLevel = *curLevel + 1;

  clang_visitChildren( cursor,
                       functionVisitor,
                       &nextLevel );


  return CXChildVisit_Continue;
}

CXChildVisitResult visitor( CXCursor cursor, CXCursor parent, CXClientData clientData )
{
  CXSourceLocation location = clang_getCursorLocation( cursor );
  if( clang_Location_isFromMainFile( location ) == 0 )
    return CXChildVisit_Continue;

  CXCursorKind cursorKind = clang_getCursorKind( cursor );

  unsigned int curLevel = *( reinterpret_cast<unsigned int*>( clientData ) );

  bool isFunctionOrMethod = cursorKind == CXCursorKind::CXCursor_CXXMethod
                         || cursorKind == CXCursorKind::CXCursor_FunctionDecl
                         || cursorKind == CXCursorKind::CXCursor_FunctionTemplate;

  std::ostringstream stream;
  stream << std::string( curLevel, '-' ) << " " << getCursorKindName( cursorKind );

  if( isFunctionOrMethod )
    stream << " (" << getCursorSpelling( cursor ) << ")\n";
  else
    stream << "\n";

  std::cerr << stream.str();

  bool visitFunction =  isFunctionOrMethod && (   curLevel == 0
                                               || clang_getCursorKind( parent ) == CXCursor_Namespace );

  if( visitFunction )
  {
    unsigned int curLevel = 0;
    clang_visitChildren( cursor, functionVisitor, &curLevel );

    std::cout << "Z\n";
  }
  else
  {
    unsigned int nextLevel = curLevel + 1;

    clang_visitChildren( cursor,
                         visitor,
                         &nextLevel ); 
  }

  return CXChildVisit_Continue;
}

int main( int argc, char** argv )
{
  if( argc < 2 )
    return -1;

  CXIndex index        = clang_createIndex( 0, 1 );
  CXTranslationUnit tu = clang_createTranslationUnit( index, argv[1] );
  CXCursor rootCursor  = clang_getTranslationUnitCursor( tu );

  // FIXME: Need to handle errors in TU creation and index creation. Need to
  // check the documentation here...

  unsigned int treeLevel = 0;

  std::cout << "X:1\n"
            << "T:" << argv[1] << "\n"
            << "M:C\n"
            << "L:1/16\n"
            << "K:C\n";

  clang_visitChildren( rootCursor, visitor, &treeLevel );

  clang_disposeTranslationUnit( tu );
  clang_disposeIndex( index );
  return 0;
}
