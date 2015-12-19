/*
 * Frak --- A brainf*ck interpreter
 * Copyright (C) Bastian Rieck <bastian@rieck.ru>
 *
 * See LICENSE for details.
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <stack>
#include <utility>

// ---------------------------------------------------------------------

namespace Parser
{

/**
  Checks whether a given command is a valid Brainf*ck command. This auxiliary
  function is used to parse input files.

  @param c Character whose validity is going to be determined
  @returns true if \c c is a valid command
*/

bool isValid( char command )
{
  return    command == '>'
         || command == '<'
         || command == '+'
         || command == '-'
         || command == '.'
         || command == ','
         || command == '['
         || command == ']';

}

/**
  Parse a given input file an stores a stream of valid Brainf*ck commands in a
  vector.

  @param filename Input filename
  @returns Vector of commands
*/

std::vector<char>parse( const std::string& filename )
{
  std::ifstream in( filename );
  if( !in )
    throw std::runtime_error( "Unable to open input file" );

  std::vector<char> commands;

  std::copy_if( std::istreambuf_iterator<char>( in ),
                std::istreambuf_iterator<char>(),
                std::back_inserter( commands ),
                isValid );

  return commands;
}

/**
  Sets up a map of matching block statements. Assuming that the blocks are
  valid, this function will return a map for looking up the respective matching
  address of a given block.

  @param commands Vector of Brainf*ck commands
  @returns Map for finding matching blocks
*/

std::map<std::size_t, std::size_t> getBlockAddresses( const std::vector<char>& commands )
{
  typedef std::pair<char, std::size_t> paren;

  std::stack<paren> parens;
  std::map<std::size_t, std::size_t> addressMap;

  for( std::size_t i = 0; i < commands.size(); i++ )
  {
    if( commands[i] == '[' )
      parens.push( std::make_pair( commands[i], i ) );
    else if( commands[i] == ']' )
    {
      if( parens.empty() || parens.top().first != '[' )
        throw std::runtime_error( "Encountered unmatched parenthesis" );

      paren p = parens.top();
      parens.pop();

      std::size_t begin = p.second;
      std::size_t end   = i;

      // Store the block; I am aware that this is rather wasteful as the same
      // information is stored twice...
      //
      // FIXME: Use a bimap?
      addressMap.insert( std::make_pair( begin, end   ) );
      addressMap.insert( std::make_pair( end,   begin ) );
    }
  }

  if( !parens.empty() )
    throw std::runtime_error( "Encountered unmatched parenthesis" );

  return addressMap;
}

} // namespace "Parser"

// ---------------------------------------------------------------------

void usage()
{
  std::cout << "Usage: frak file\n\n"
            << "Copyright (C) 2014 Bastian Rieck\n"
            << "Permission is hereby granted, free of charge, to any person obtaining a copy of\n"
            << "this software and associated documentation files (the \"Software\"), to deal in\n"
            << "the Software without restriction, including without limitation the rights to\n"
            << "use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies\n"
            << "of the Software, and to permit persons to whom the Software is furnished to do\n"
            << "so, subject to the following conditions:\n\n"
            << "The above copyright notice and this permission notice shall be included in all\n"
            << "copies or substantial portions of the Software.\n\n"
            << "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
            << "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
            << "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
            << "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
            << "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
            << "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
            << "SOFTWARE.\n";
}

// ---------------------------------------------------------------------

int main(int argc, char* argv[])
{
  if( argc != 2 )
  {
    usage();
    return -1;
  }

  std::string inputFilename = argv[1];

  std::vector<char> commands                    = Parser::parse( inputFilename );
  std::map<std::size_t, std::size_t> addressMap = Parser::getBlockAddresses( commands );

  std::size_t ip = 0;
  std::size_t dp = 0;

  std::vector<char> memory( 30000, 0 );

  for( ; ip < commands.size();  )
  {
    char command = commands.at( ip );

    if( command == '>' )
      ++dp;
    else if( command == '<' )
      --dp;
    else if( command == '+' )
      ++memory.at( dp );
    else if( command == '-' )
      --memory.at( dp );
    else if( command == '.' )
      std::cout << memory.at( dp );
    else if( command == ',' )
    {
      char c;
      if( std::cin >> c )
        memory.at( dp ) = c;
      else
      {
        if( std::cin.eof() )
          return 0;
        else
          throw std::runtime_error( "Unable to obtain input" );
      }
    }
    else if( command == '[' )
    {
      if( memory.at( dp ) == 0 )
      {
        ip = addressMap.at( ip ) + 1;
        continue;
      }
    }
    else if( command == ']' )
    {
      if( memory.at( dp ) != 0 )
      {
        ip = addressMap.at( ip ) + 1;
        continue;
      }
    }

    ++ip;
  }

  return 0;
}
