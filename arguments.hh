#ifndef ARGUMENTS_HH
#define ARGUMENTS_HH

#include <string>
#include <vector>

class Arguments
{
public:
  void addArgument( const std::string& argument );
  void removeArgument( const std::string& argument );

  char** getRawArguments() const;

private:
  std::vector<std::string> _arguments;
};

#endif
