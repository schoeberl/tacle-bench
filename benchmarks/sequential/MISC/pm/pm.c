/******************************************************************************
** File: pm.c
**
** HPEC Challenge Benchmark Suite
** Pattern Match Kernel Benchmark
**
** Contents:
**  This is the ANSI C Pattern Match kernel. It finds the closest
**  match of a pattern from a library of patterns.  The code below
**  serves as a reference implemenation of the pattern match kernel.
**
** Input/Output:
**  The template library is stored in file 
**             "./data/<dataSetNum>-pm-lib.dat".
**  The test pattern is stored in file 
**             "./data/<dataSetNum>-pm-pattern.dat".
**  The timing will be stored in file 
**             "./data/<dataSetNum>-pm-timing.dat".
**  The matched pattern index will be stored in file 
**             "./data/<dataSetNum>-pm-patnum.dat"
**
** Command:
**  pm <data set num>
**
** Author: Hector Chan
**         MIT Lincoln Laboratory
**
******************************************************************************/
/******************************************************************************
** MODIFIED BY: Rathijit Sen                                                          
**         		Universität des Saarlandes											 
**         		Saarbrücken															 
**                                                                               
** MODIFIED ON: 18 OCTOBER 2007                                                   		
**                                                                               
** COMMENTS:    removed dynamic alloc, file I/O
******************************************************************************/

#include "PcaCArray.h"
#include "../../../include/patmos.h"

/* The coefficients for the log and pow functions below */
float pow_coeff[19];
float log_coeff[16];

// arrays for the init function
uchar init_array_1[64];
float init_array_2[21];
float init_array_3[64];
float init_array_4[64];
float init_array_5[21];
float init_array_6[21];
float init_array_7[72];
float init_array_8[110];
float *lib_ptr[60];
float *pattern_ptr[60];
extern float lib_data[60][64];
extern float pattern_data[60][64];

// Own implementation of fabs
float fabs_( float n )
{
  if ( n>=0 )
    return n;
  else
    return -n;
}

/***********************************************************************/
/* We found out the bottle neck of this kernel was in the pow and log
 * functions. Therefore, we have implemented our own log and pow, instead
 * of using the float fp ones in the standard C math libary. This function
 * sets up the coefficients for the single fp log and pow functions. */
/***********************************************************************/
void setcoeff()
{
  pow_coeff[0]  = 0.5f;             /* 1/2! */
  pow_coeff[1]  = 0.166666667f;     /* 1/3! */
  pow_coeff[2]  = 0.041666666f;     /* 1/4! */
  pow_coeff[3]  = 8.333333333e-3f;
  pow_coeff[4]  = 1.388888889e-3f;
  pow_coeff[5]  = 1.984126984e-4f;
  pow_coeff[6]  = 2.480158730e-5f;
  pow_coeff[7]  = 2.755731922e-6f;
  pow_coeff[8]  = 2.755731922e-7f;
  pow_coeff[9]  = 2.505210839e-8f;
  pow_coeff[10] = 2.087675699e-9f;
  pow_coeff[11] = 1.605904384e-10f;
  pow_coeff[12] = 1.147074560e-11f;
  pow_coeff[13] = 7.647163732e-13f;
  pow_coeff[14] = 4.779477332e-14f;
  pow_coeff[15] = 2.811457254e-15f;
  pow_coeff[16] = 1.561920697e-16f;
  pow_coeff[17] = 8.220635247e-18f;
  pow_coeff[18] = 4.110317623e-19f;

  log_coeff[0]  = 0.333333333f;     /* 1/3 */
  log_coeff[1]  = 0.2f;             /* 1/5 */
  log_coeff[2]  = 0.142857143f;     /* 1/7 */
  log_coeff[3]  = 0.111111111f;     /* 1/9 */
  log_coeff[4]  = 9.090909091e-2f;  /* 1/11 */
  log_coeff[5]  = 7.692307692e-2f;  /* 1/13 */
  log_coeff[6]  = 6.666666667e-2f;  /* 1/15 */
  log_coeff[7]  = 5.882352941e-2f;  /* 1/17 */
  log_coeff[8]  = 5.263157895e-2f;  /* 1/19 */
  log_coeff[9]  = 4.761904762e-2f;  /* 1/21 */
  log_coeff[10] = 4.347826087e-2f;  /* 1/23 */
  log_coeff[11] = 0.04f;            /* 1/25 */
  log_coeff[12] = 3.703703704e-2f;  /* 1/27 */
  log_coeff[13] = 3.448275862e-2f;  /* 1/29 */
  log_coeff[14] = 3.225806452e-2f;  /* 1/31 */
  log_coeff[15] = 3.030303030e-2f;  /* 1/33 */
}

/***********************************************************************/
/* This single fp pow base 10 function implements the corresponding
 * Taylor series.  The loop has been unrolled to save ops. */
/***********************************************************************/
float pow10fpm (float exp)
{
  float mul = exp * LOG10;
  float const term = exp * LOG10;
  float ans = 1.0f;
  float const *fptr = pow_coeff;

  ans += mul;           mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul;

  return ans;
}

/***********************************************************************/
/* This single fp log base 10 function implements the corresponding
 * Taylor series. The loop has been unrolled to save ops. */
/***********************************************************************/
float log10fpm (float exp)
{
  float mul = (exp - 1.0f) / (exp + 1.0f);
  float ans = 0.0f;
  float const *fptr = log_coeff;
  float const term = mul * mul;

  ans  = mul; 
  mul *= term;

  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul; mul *= term;
  ans += *fptr++ * mul;

  ans *= 0.86858896381f;  /* ans = ans * 2 / log(10) */

  return ans;
}

float my_floor(float arg)
{
  if(!arg) return 0;
  if(arg>0) return (int)arg;
  return -((int)(-arg)+1);
}

float my_ceil(float arg)
{
  if(!arg) return 0;
  if(arg>0) return (int)(arg+1);
  return (int)(arg);
}

void my_memcpy(void *dest, void *src, int size)
{
 int i;
 _Pragma( "loopbound min 44 max 256" )
 for(i=0;i<size;i++) {
   ((unsigned char *)dest)[i]=((unsigned char *)src)[i];
 }
 return;
}

/***********************************************************************/
/* Allocate and initailize the test pattern, the template library, and
 * other necessary data structure. */
/***********************************************************************/
void init(PmData *pmdata, PcaCArrayFloat *lib, PcaCArrayFloat *pattern)
{
  int   elsize = sizeof(float);
  float x;

  /* Getting the input parameters from the PCA C array structure */
  pmdata->profile_size  = lib->size[1];
  pmdata->num_templates = lib->size[0]; 

  pmdata->elsize = elsize;
  pmdata->shift_ratio = 3.0f;

  pmdata->template_profiles_db = lib->data;
  pmdata->test_profile_db = pattern->data;

  /* Equivalent to shift_size = roundf((float)profile_size / shift_ratio) */
  x = (float)(pmdata->profile_size) / pmdata->shift_ratio;
  pmdata->shift_size = ((x - (int)(x)) < 0.5f) ? (int)my_floor(x) : (int)my_ceil(x);

  pmdata->template_exceed     = init_array_1;
  pmdata->test_exceed_means   = init_array_2;

  pmdata->template_copy       = init_array_3;
  pmdata->test_noise_db_array = init_array_4;

  pmdata->MSE_scores          = init_array_5;
  pmdata->mag_shift_scores    = init_array_6;

  pmdata->minimum_MSE_score   = init_array_7;
  pmdata->all_shifted_test_db = init_array_8;

  /* Set the coefficients for the log and pow functions */
  setcoeff();
}

/***********************************************************************/
/* Free up memory for all structures */
/***********************************************************************/
void clean(PmData *pmdata)
{
//  free(pmdata->test_exceed_means);
  pmdata->test_exceed_means = 0;

//  free(pmdata->template_exceed);
  pmdata->template_exceed = 0;

//  free(pmdata->template_copy);
  pmdata->template_copy = 0;

//  free(pmdata->test_noise_db_array);
  pmdata->test_noise_db_array = 0;

//  free(pmdata->MSE_scores);
  pmdata->MSE_scores = 0;

//  free(pmdata->mag_shift_scores);
  pmdata->mag_shift_scores = 0;

//  free(pmdata->minimum_MSE_score);
  pmdata->minimum_MSE_score = 0;

//  free(pmdata->all_shifted_test_db);
  pmdata->all_shifted_test_db = 0;
}

void my_memset(void *s, int c, int n)
{
  int i;
  _Pragma( "loopbound min 64 max 64" )
  for(i=0;i<n;i++) {
    ((unsigned char*)s)[i]=c;
  }
  return;
}

/***********************************************************************/
/* The pattern match kernel overlays two patterns to compute the likelihood 
 * that the two vectors match. This process is performed on a library of 
 * patterns. */
/***********************************************************************/
int pm(PmData *pmdata)
{
  const int    elsize         = pmdata->elsize;               /* size of a single fp number    */
  const int    shift_size     = pmdata->shift_size;           /* number of shifting to the left and right of the test profile */
  const int    profile_size   = pmdata->profile_size;         /* number of pixels in a pattern */
  const int    num_templates  = pmdata->num_templates;        /* number of library patterns    */
  float *test_profile_db      = pmdata->test_profile_db;      /* the test pattern              */
  float *template_profiles_db = pmdata->template_profiles_db; /* the library of patterns       */
  float *test_noise_db_array  = pmdata->test_noise_db_array;  /* the noise in the test pattern in an array for fast copy */
  float *all_shifted_test_db  = pmdata->all_shifted_test_db;  /* the shifted test pattern      */

  uint  match_index;                          /* the index of the most likely template that matches the test pattern */
  uint  min_MSE_index = shift_size + 1;       /* the index of the range shifts with the lowest mean square error */
  uint  num_template_exceed, num_test_exceed; /* the number of pixels exceeded the test pattern and a library template */

  uchar mag_shift_scores_flag;    /* flag that tells if the magnitude scaling loop has been run (existed just to save ops) */

  float test_peak, template_peak; /* the maximum pixels of the test pattern and a library template pattern */
  float template_noise;           /* the noise level of a library template */

  float noise_shift, noise_shift2; /* temporary storage for calculating the mse for range shifting */

  float min_MSE, match_score; /* temporary storage for finding the minimum mse */

  float sumWeights_inv = 1.0f / profile_size; /* the inverse of the weights used for calculating the mse */
  /* Note: weights for the kernel would be application dependent. They are set to 1 for our purposes */

  float mag_db;                        /* the magnitude shifts in dB */
  float power_shift, ave_power_ratio;  /* the diff of the avg shifted test profile power to the avg template power */
  float power_ratio;                   /* the mean power of the pixels of a template that exceeded twice test noise */

  float test_noise = ( pow10fpm(test_profile_db[0]*0.1f) +              /* noise level of the test pattern */
		       pow10fpm(test_profile_db[profile_size-1]*0.1f) ) * 0.5f;

  int half_shift_size = (int)my_ceil((float)(shift_size) / 2.0f); /* since "shift_size/2" is used a lot, so we create a var to hold it */
  int template_index, current_shift; /* indices */
  int patsize = profile_size*elsize; /* number of bytes of a pattern */

  float *minimum_MSE_score = pmdata->minimum_MSE_score;
  float *MSE_scores        = pmdata->MSE_scores;
  float *mag_shift_scores  = pmdata->mag_shift_scores;

  register float test_noise_db        = (test_noise == 0.0f) ? -100.0f : 10.0f * log10fpm(fabs_(test_noise)); /* test noise in dB */
  register float test_noise_db_plus_3 = test_noise_db + 3.0f; /* twice test noise in the power domain, approximately +3dB */

  register float *template_copy     = pmdata->template_copy;  
  register uchar *template_exceed   = pmdata->template_exceed;
  register float *test_exceed_means = pmdata->test_exceed_means;

  register int i, j; /* indices */

  register float tmp1;                          /* temporary storage for calculating the mse for range shifting */
  register float sum_exceed;                    /* the sum of the test pattern pixels exceeded twice test noise */
  register float template_exceed_mean=0;        /* the mean of a template pattern pixels exceeded twice test noise */
  register float weighted_MSE;                  /* temporary storage for computing the weighted MSE */

  /* These pointers are solely used for fast memory access */
  register float *cur_tp, *fptr, *fptr2, *fptr3, *endptr;
  register uchar *bptr;

  /* Having an array of test noise for fast copying of noise returns */
  _Pragma( "loopbound min 64 max 64" )
  for (i=0; i<profile_size; i++) {
    test_noise_db_array[i] = test_noise_db;
  }

  /* Finding the maximum pixels of the test pattern */
  fptr = test_profile_db;
  test_peak = *fptr++;
  _Pragma( "loopbound min 63 max 63" )
  for (i=1; i<profile_size; i++,fptr++) {
    if (test_peak < *fptr) {
      test_peak = *fptr;
    }
  }

  /* Paddle array for all the possible range shifts. Essentially, we are 
   * performing the following:
   *
   * Adding these two portions to the beginning and end of the test pattern
   *      |                          |
   *      V                          V
   *  |<------>|                 |<------>| 
   *            
   *               __       __
   *              |  |     |  |
   *             |    |___|    |
   *            |               |
   *  _________|                 |_________   <- test noise in dB domain
   * ---------------------------------------  <- zero
   *
   *           |<--------------->|
   *           original test pattern
   *
   *
   * The all_shifted_test_db will be accessed in a sliding window manner.
   */

  my_memcpy((void*) all_shifted_test_db, (void*) test_noise_db_array, elsize*half_shift_size);
  my_memcpy((void*) (all_shifted_test_db+half_shift_size), (void*) test_profile_db, elsize*profile_size);
  my_memcpy((void*) (all_shifted_test_db+half_shift_size+profile_size), (void*) test_noise_db_array, elsize*half_shift_size);

  /* Set the pixels to test noise in dB domain if pixel is less than test noise in dB */
  fptr = all_shifted_test_db + half_shift_size;
  _Pragma( "loopbound min 64 max 64" )
  for (i=0; i<profile_size; i++,fptr++) {
    if (*fptr < test_noise_db) {
      *fptr = test_noise_db;
    }
  }

  /* Calculating the mean of the pixels that exceeded twice test noise for each 
   * possible shift of the test profile */
  fptr2 = test_exceed_means;
  _Pragma( "loopbound min 21 max 21" )
  for (current_shift=0; current_shift<shift_size; current_shift++) {
    /* Pointer arithmetics to find the start and end pointers */
    if (current_shift < half_shift_size) {
      endptr = all_shifted_test_db + current_shift + profile_size;
      fptr   = all_shifted_test_db + half_shift_size;
    }
    else {
      endptr = all_shifted_test_db + half_shift_size + profile_size;
      fptr   = all_shifted_test_db + current_shift;
    }

    /* Summing the pixels that exceed twice test noise for the current shifts */
    sum_exceed = 0.0f;
    num_test_exceed = 0;
    _Pragma( "loopbound min 53 max 64" )
    while (fptr != endptr) {
      if (*fptr > test_noise_db_plus_3) {
        num_test_exceed++;
        sum_exceed += *fptr;
      }
      fptr++;
    }

    *fptr2++ = num_test_exceed ? sum_exceed / (float)(num_test_exceed) : 0.0f;
  }


  /* Loop over all the templates. Determine the best shift distance, then 
   * the best gain adjustment. */
  _Pragma( "loopbound min 60 max 60" )
  for (template_index=0; template_index<num_templates; template_index++) {
    cur_tp = template_profiles_db+(template_index*profile_size);

    /* Scale the template profile we're currently working on so that its peak
     * is equal to the peak of the test profile */

    /* --------------------------------------------------------------------
     * template_peak = max( template_profile ) */
    fptr = cur_tp;
    template_peak = *fptr++;
    _Pragma( "loopbound min 63 max 63" )
    for (i=1; i<profile_size; i++,fptr++) {
      if (template_peak < *fptr) {
        template_peak = *fptr;
      }
    }

    /* Additively adjust the noise level of this template profile in the
     * raw power domain so that its noise level matches the noise level
     * of the test profile */

    /* --------------------------------------------------------------------
       Setting up all the constants */

    noise_shift  = test_peak - template_peak;
    my_memset ((void*)template_exceed, 0, sizeof(char)*profile_size);
    sum_exceed = 0.0f;
    num_template_exceed = 0;

    /* -------------------------------------------------------------------- 
     * The following blocks are optimized code that essentially 
     * perform the operations immediately below. The calculation of the 
     * template noise constants is done once the exponentials are complete
     */

    /* template_profile = template_profile + test_peak - template_peak
     * template = 10 ^ (template_profile / 10)
     * template = template + test_noise - template_noise
     * if (input < fp_epsilon) then clip the input to -100 dB
     * template = log10( abs(template) )
     * template_profile = 10 * template + test_noise_db */

    fptr = cur_tp;
    _Pragma( "loopbound min 64 max 64" )
    for (i = 0; i < profile_size; i++) {
      tmp1 = *fptr + noise_shift;
      *fptr = pow10fpm(tmp1 * 0.1f);
      fptr++;

    }
      
    /* Calculates noise levels from first and last elements of the current 
       template */

    template_noise = (cur_tp[0] + cur_tp[profile_size - 1]) * 0.5f;
    noise_shift2 = test_noise - template_noise;

    fptr = cur_tp;
    _Pragma( "loopbound min 64 max 64" )
    for (i = 0; i < profile_size; i++) {
      tmp1 = *fptr + noise_shift2;

      if (tmp1 == 0.0f) {
        tmp1 = MIN_NOISE;
      }

      *fptr = 10.0f * log10fpm( fabs_(tmp1) ) + test_noise_db;

      /* Because many of the operations in the search for the best shift 
       * amount depend on knowledge of which pixels in the template 
       * have values exceeding twice test_noise (recall that 3db is roughly 
       * equivalent to a doubling of raw power), we'll put those indices in
       * template_exceed */

      if (*fptr > test_noise_db_plus_3) {
        template_exceed[i] = 1;
        num_template_exceed++;
        sum_exceed += *fptr;
      }

      fptr++;
    }

    /* Note: The following block has 4 different branches:
       1. Both the current template and the test pattern have values exceeded 
          twice test noise.
       2. Only the current template has values exceeded twice test noise.
       3. Only the test pattern has values exceeded twice test noise.
       4. Neither the current template nor the test pattern has values 
          exceeded twice test noise.
    */

    /* If there is at least one pixel in the template we're
     * currently working on whose value exceeds twice test_noise */
    if (num_template_exceed) {
      template_exceed_mean = sum_exceed / (float)(num_template_exceed);
      fptr3 = test_exceed_means;

      _Pragma( "loopbound min 21 max 21" )
      for (current_shift=0; current_shift<shift_size; current_shift++,fptr3++) {
        /* Work on a copy of the template we're currently working on */
        my_memcpy ((void*)template_copy, (void*)cur_tp, patsize);

        /* If there is at least one pixel in the shifted test profile
         * whose value exceeds twice test noise. */
        if (*fptr3 != 0.0f) {
	  /* CASE 1 */
          /* Considering only those pixels whose powers exceed twice 
           * test noise, compute the difference of the mean power in
           * template we're currently working on. */
          power_ratio = *fptr3 - template_exceed_mean;

	  /* Scale template values that exceed twice test noise by power ratio and
           * set the values that are less than test noise in db to test noise in db */
          fptr  = template_copy;
          bptr  = template_exceed;
          _Pragma( "loopbound min 64 max 64" )
          for (i=0; i<profile_size; i++,fptr++) {
            if (*bptr++)
              *fptr += power_ratio;

            if (*fptr < test_noise_db)
              *fptr = test_noise_db;
          }
        } /* if (*fptr3 != 0.0f) */
        else {
	  /* CASE 2 */
          /* Set those pixels in the template we're currently working on
           * whose values are less than test_noise to test_noise. */
          fptr = cur_tp;
          _Pragma( "loopbound min 64 max 64" )
          for (i=0; i<profile_size; i++) {
            if (*fptr++ < test_noise_db) {
              template_copy[i] = test_noise_db;
            }
          }
        } /* else ... if (num_test_exceed) */

        /* Compute the weighted MSE */
        weighted_MSE = 0.0f;
        fptr  = all_shifted_test_db + current_shift;
        fptr2 = template_copy;
        _Pragma( "loopbound min 64 max 64" )
        for (i=0; i<profile_size; i++) {
          tmp1 = *fptr++ - *fptr2++;
          weighted_MSE += tmp1 * tmp1;
        }

        /* ----------------------------------------------------------------
         * MSE_scores[current_shift] = weighted_MSE / sumWeights */
        MSE_scores[current_shift] = weighted_MSE * sumWeights_inv;

      } /* for current_shift */
    } else /* if (num_template_exceed) */ {
      fptr3 = test_exceed_means;

      _Pragma( "loopbound min 0 max 0" )
      for (current_shift=0; current_shift<shift_size; current_shift++) {
	/* CASE 3 */
        /* If there is at least one pixel that exceeds twice test noise */
        if (*fptr3++ != 0.0f) {
          fptr2 = cur_tp;
        } else {
	  /* CASE 4 */
          /* Work on a copy of the template we're currently working on. */
          my_memcpy ((void*)template_copy, (void*)cur_tp, patsize);

          fptr = cur_tp;
          _Pragma( "loopbound min 0 max 0" )
          for (i=0; i<profile_size; i++) {
            if (*fptr++ < test_noise_db) {
              template_copy[i] = test_noise_db;
            }
          }

          fptr2 = template_copy;
        }

        /* Compute the weighted MSE */
        weighted_MSE = 0.0f;
        fptr  = all_shifted_test_db + current_shift;
        _Pragma( "loopbound min 0 max 0" )
        for (i=0; i<profile_size; i++) {
          tmp1 = *fptr++ - *fptr2++;
          weighted_MSE += tmp1 * tmp1;
        }

        MSE_scores[current_shift] = weighted_MSE * sumWeights_inv;

      } /* for current_shift */
    } /* else .. if (num_template_exceed) */

    /* Finding the minimum MSE for range shifting */
    fptr = MSE_scores;
    min_MSE_index = 0;
    min_MSE = *fptr++;
    _Pragma( "loopbound min 20 max 20" )
    for (i=1; i<shift_size; i++,fptr++) {
      if (min_MSE > *fptr) {
        min_MSE = *fptr;
        min_MSE_index = i;
      }
    }

    /* Work on a copy of the template we're currently working on. */
    my_memcpy((void*)template_copy, (void*)cur_tp, patsize);

    mag_shift_scores_flag = 1;

    if (test_exceed_means[min_MSE_index] != 0.0f) {
      if (num_template_exceed) {
        /* Compute the difference of the average shifted test profile
         * power to the average template power */
        /* ave_power_ratio = (sum_exceed / (float)(num_test_exceed)) - template_exceed_mean; */
        ave_power_ratio = test_exceed_means[min_MSE_index] 
                          - template_exceed_mean;

        /* Loop over all possible magnitude shifts */
        _Pragma( "loopbound min 21 max 21" )
        for (j=0, mag_db=-5.0f; mag_db<=5.0f; mag_db+=0.5f) {
          power_shift = ave_power_ratio + mag_db;

          /* --------------------------------------------------------------
           * template_copy = template_profiles(template_exceed) + ave_power_ratio + mag_db */
          bptr  = template_exceed;
          _Pragma( "loopbound min 64 max 64" )
          for (i=0; i<profile_size; i++) {
            if (*bptr++) {
              template_copy[i] = cur_tp[i] + power_shift;
            }
          }

          /* Compute the weighted MSE */
          weighted_MSE = 0.0f;
          fptr  = all_shifted_test_db + min_MSE_index;
          fptr2 = template_copy;
          _Pragma( "loopbound min 64 max 64" )
          for (i=0; i<profile_size; i++) {
            tmp1 = *fptr++ - *fptr2++;
            weighted_MSE += tmp1 * tmp1;
          }

          mag_shift_scores[j++] = weighted_MSE * sumWeights_inv;
          
        } /* for mag_db */
      } /* if (num_template_exceed) */

    } else /* if (num_test_exceed) */ {
      /* Set those pixels in the template we're currently working on
       * whose values are less than test_noise to test_noise. */
      fptr = cur_tp;
      _Pragma( "loopbound min 64 max 64" )
      for (i=0; i<profile_size; i++) {
        if (*fptr++ < test_noise_db) {
          template_copy[i] = test_noise_db;
        }
      }

      /* Compute the weighted MSE */
      weighted_MSE = 0.0f;
      fptr = all_shifted_test_db + min_MSE_index;
      fptr2 = template_copy;
      _Pragma( "loopbound min 64 max 64" )
      for (i=0; i<profile_size; i++) {
        tmp1 = *fptr++ - *fptr2++;
        weighted_MSE += tmp1 * tmp1;
      }

      minimum_MSE_score[template_index] = weighted_MSE * sumWeights_inv;

      mag_shift_scores_flag = 0;
    } /* if (num_test_exceed) */

    /* If magnitude shifting has performed above */
    if (mag_shift_scores_flag) {
      /* Find the minimum MSE for magnitude scaling */
      fptr = mag_shift_scores;
      min_MSE = *fptr++;
      _Pragma( "loopbound min 20 max 20" )
      for (i=1; i<21; i++,fptr++) {
        if (min_MSE > *fptr) {
          min_MSE = *fptr;
        }
      }

      minimum_MSE_score[template_index] = min_MSE;
    }

  } /* for template_index */

  /* Find the minimum mean square error */
  fptr = minimum_MSE_score;
  match_index = 0;
  match_score = *fptr++;
  _Pragma( "loopbound min 59 max 59" )
  for (i=1; i<num_templates; i++,fptr++) {
    if (match_score > *fptr) {
      match_score = *fptr;
      match_index = i;
    }
  }

  return match_index;
}

void read_lib(PcaCArrayFloat *lib)
{
   int i;
   lib->rctype=1;
   lib->ndims=2;
   lib->size[0]=60;
   lib->size[1]=64;
   lib->size[2]=0;

   _Pragma( "loopbound min 60 max 60" )
   for(i=0; i<60; i++) {
     lib_ptr[i]= lib_data[i];
   }

   lib->data=*lib_ptr;
   lib->datav=(void *)lib_ptr;
   return;
}

void read_pattern(PcaCArrayFloat *pattern)
{
   int i;
   pattern->rctype=1;
   pattern->ndims=2;
   pattern->size[0]=60;
   pattern->size[1]=64;
   pattern->size[2]=0;
   _Pragma( "loopbound min 60 max 60" )
   for(i=0;i<60;i++) {
     pattern_ptr[i]=pattern_data[i];
   }
   pattern->data=*pattern_ptr;
   pattern->datav=(void *)pattern_ptr;
   return;
}


int main( void )
{
volatile _SPM unsigned int *cyc_ptr_low = (volatile _SPM unsigned int *) 0xF0020004;
volatile _SPM unsigned int *cyc_ptr_high = (volatile _SPM unsigned int *) 0xF0020000;

  unsigned long long cyc_ptr_low_saved = (unsigned long long)*cyc_ptr_low;
  unsigned long long cyc_ptr_high_saved = (unsigned long long)*cyc_ptr_high;

  unsigned long long start = cyc_ptr_low_saved | (cyc_ptr_high_saved<<32);
  PmData         pmdata;
  PcaCArrayFloat lib, pattern;
  uint result;

  read_lib(&lib);
  read_pattern(&pattern);
  init(&pmdata, &lib, &pattern);
  result = pm(&pmdata);
  clean(&pmdata);


  cyc_ptr_low_saved = (unsigned long long)*cyc_ptr_low;
  cyc_ptr_high_saved = (unsigned long long)*cyc_ptr_high;

  unsigned long long end = cyc_ptr_low_saved | (cyc_ptr_high_saved<<32);
  printstats(start, end);

  return 0;
}
/* ----------------------------------------------------------------------------
Copyright (c) 2006, Massachusetts Institute of Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are  
met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of the Massachusetts Institute of Technology nor  
       the names of its contributors may be used to endorse or promote 
       products derived from this software without specific prior written 
       permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF  
THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------- */
