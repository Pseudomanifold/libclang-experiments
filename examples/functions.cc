int theAnswerToLifeTheUniverseAndEverything()
{
  return 42;
}

int sum( int a, int b, int c )
{
  return a + b + c;
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

  return i * factorial( i - 1 );
}

unsigned int fibonacci( unsigned int k )
{
  unsigned int N = 0;
  unsigned int a = 0;
  unsigned int b = 1;

  for( unsigned int i = 0; i < k; i++ )
  {
    N = a + b;
    a = b;
    b = N;
  }

  return N;
}
