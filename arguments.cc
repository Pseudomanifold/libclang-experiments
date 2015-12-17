#include "arguments.hh"

#include <algorithm>

void Arguments::addArgument( const std::string& argument )
{
  _arguments.push_back( argument );
}

void Arguments::removeArgument( const std::string& argument )
{
  auto position = std::find( _arguments.begin(), _arguments.end(), argument );
  if( position != _arguments.end() )
    _arguments.erase( position );
}

char** Arguments::getRawArguments() const
{
  char** rawArguments = new char*[ _arguments.size() ];
  unsigned int index  = 0;

  for( auto&& argument : _arguments )
  {
    auto length = argument.size();

    rawArguments[index]         = new char[ length + 1 ];
    rawArguments[index][length] = '\0';

    std::copy( argument.begin(), argument.end(),
               rawArguments[index] );

    ++index;
  }

  return rawArguments;
}
