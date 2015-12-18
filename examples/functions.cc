int theAnswerToLifeTheUniverseAndEverything()
{
  return 42;
}

int square( int x )
{
  return x*x;
}

int cube( int x )
{
  return x*x*x;
}

unsigned int factorial( unsigned int i )
{
  if( i == 0 )
    return 1;
  else
    return i * factorial( i - 1 );
}
