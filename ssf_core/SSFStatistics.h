#ifndef _SSF_STATISTICS_
#define _SSF_STATISTICS_



/*  Generate a random permutation of n numbers (0 to n-1) and return the k first. */
vector<size_t> GenerateRandomPermutation(size_t n, size_t k);

/* start the random generator */
void InitializeRandomNumberGenerator();

/* generate a random size_t number */
size_t GenerateSIZETRandomNumber();


extern default_random_engine glb_RandonGenerator;

#endif