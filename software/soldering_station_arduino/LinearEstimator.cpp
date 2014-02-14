
#include "LinearEstimator.h"
#include <Arduino.h>

int LinearEstimator::estimateY(int x)
{
  return ((long)(b*((long)x)))/B_MULTIPLIER + a;
}

int LinearEstimator::estimateX(int y)
{
  return (long)((long)y - (long)a)*B_MULTIPLIER/b;
}


// See the formulea for a and b here: http://www.ensignsoftware.net/images/54-2.gif
void LinearEstimator::linest(int n, int x[], int y[])
{
  long sigmax = 0;
  long sigmay = 0;
  long sigmaxy = 0;
  long sigmax2 = 0;
  int k;
  
  for(k=0; k < n; k++) {
    sigmax += x[k];
    sigmay += y[k];
    sigmaxy += ((long)x[k])*((long)y[k]);
    sigmax2 += ((long)x[k])*((long)x[k]);
  }
  
  b = (n*sigmaxy - sigmax*sigmay)*B_MULTIPLIER/(n*sigmax2 - sigmax*sigmax);
  a = (sigmay - b*sigmax/B_MULTIPLIER) / n;
}

