#include <jni.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../../src/include/Logging.h"

#include "headers/ADE_defines.h"
#include "headers/ADE_typedefs.h"

#include "headers/ADE_blas_level2.h"
#include "headers/ADE_blas_level1.h"

#include "headers/ADE_Bench_Utils.h"

#define DIM (5)


//extern "C" void DummyTest();

int TestAde()
{
//	DummyTest();

	int incx = 1;
	int incy = 1;
	float alpha= 1,beta=1.0;
	static float x[DIM]={1,2,3,4,5};
	static float y[DIM]={0,0,0,0,0};
	static float A[DIM]={1,2,3,4,5};
	static float out[DIM],out2[DIM];
	int N=DIM;
	int k=0;

	ADE_blas_level1_T *p_Blas_L1;
	ADE_blas_level2_T *p_Blas_L2;

	char test_case[20]="sbmv";

    ADE_API_RET_T ret = ADE_DEFAULT_RET;
    ADE_UINT32_T i = 0;

    if (!strcmp(test_case,"axpy"))
    {
		ADE_Blas_level1_Init(&p_Blas_L1,ADE_REAL);
		ADE_Blas_level1_setALPHA(p_Blas_L1,&alpha);
		ADE_Blas_level1_setN(p_Blas_L1,N);
		ADE_Blas_level1_setINCX(p_Blas_L1,incx);
		ADE_Blas_level1_setINCY(p_Blas_L1,incy);
		ADE_Blas_level1_setX(p_Blas_L1,x);
		ADE_Blas_level1_setY(p_Blas_L1,y);
		ADE_Blas_level1_Print(p_Blas_L1);
		ADE_Blas_level1_axpy(p_Blas_L1);
		ADE_Blas_level1_Release(p_Blas_L1);
		PRINT_ARRAY(y,i,DIM,"%f");

		for(int i=0;i<DIM;i++)
			LOGD("[%u] = %f",i,y[i]);

    }
	else if (!strcmp(test_case,"sbmv"))
	{
		/*Init*/
		ADE_Blas_level2_Init(&p_Blas_L2,ADE_REAL);
		/*Config*/
		ADE_Blas_Level2_SetUplo(p_Blas_L2,'L');
		ADE_Blas_Level2_SetN(p_Blas_L2,DIM);
		ADE_Blas_Level2_SetK(p_Blas_L2,k);
		ADE_Blas_level2_SetAlpha(p_Blas_L2,&alpha);
		ADE_Blas_level2_SetA(p_Blas_L2,A);
		ADE_Blas_Level2_SetLda(p_Blas_L2,k+1);
		ADE_Blas_level2_SetX(p_Blas_L2,x);
		ADE_Blas_Level2_SetIncx(p_Blas_L2,1);
		ADE_Blas_level2_SetBeta(p_Blas_L2,&beta);
		ADE_Blas_level2_SetY(p_Blas_L2,y);
		ADE_Blas_Level2_SetIncy(p_Blas_L2,1);
		ADE_Blas_level2_Print(p_Blas_L2);

		/*Process*/
		ADE_Blas_level2_sbmv(p_Blas_L2);

		/*Release*/
		ADE_Blas_level2_Release(p_Blas_L2);

		for(int i=0;i<DIM;i++)
			LOGD("[%u] = %f",i,y[i]);
	}
	else
	{
		return -1;
	}
	return ret;
}

#define FNAME "/data/data/ade.benchmark/files/benchlog"

int BenchmarkAde()
{
    ADE_BENCH_T test_cases[1]={ADE_BLAS_L3_BM};
    ADE_BENCH_MAT_T mat_type[2]={ADE_BENCH_REAL,ADE_BENCH_CPLX};
    //FILE* p_debug_out=stdout;
    FILE* p_debug_out = fopen(FNAME, "w");
    ADE_INT32_T ret=0;

    ADE_INT32_T n_rows_A[10]={2};//{2,10,50,100,500,1000,2000,3000,5000,10000};
    ADE_INT32_T n_cols_A[10]={2};//{2,10,50,100,500,1000,2000,3000,5000,10000};
    ADE_INT32_T n_cols_B[10]={2};//{2,10,50,100,500,1000,2000,3000,5000,10000};

    ret=blas3_test_procedure(test_cases,1,mat_type,2,n_rows_A,n_cols_A,n_cols_B,1,p_debug_out);

    fclose(p_debug_out);
    p_debug_out = fopen(FNAME,"r");

    char line[256];

    while (fgets(line, sizeof(line), p_debug_out)) {
    	/* note that fgets don't strip the terminating \n, checking its
    	presence would allow to handle lines longer that sizeof(line) */
    	LOGD("%s", line);
    }


    return ret;
}


extern "C" {

JNIEXPORT void JNICALL Java_ade_benchmark_BenchmarkActivity_NativeTest(JNIEnv *e, jobject o)
{
	TestAde();
}


JNIEXPORT void JNICALL Java_ade_benchmark_BenchmarkActivity_NativeBenchmark(JNIEnv *e, jobject o)
{
	BenchmarkAde();
}

}

/*
int main()
{

    ADE_BENCH_T test_cases[1]={ADE_BLAS_L3_BM};
    ADE_BENCH_MAT_T mat_type[2]={ADE_BENCH_REAL,ADE_BENCH_CPLX};
    FILE* p_debug_out=stdout;
    ADE_INT32_T ret=0;

    ADE_INT32_T n_rows_A[10]={2,10,50};//{2,10,50,100,500,1000,2000,3000,5000,10000};
    ADE_INT32_T n_cols_A[10]={2,10,50};//{2,10,50,100,500,1000,2000,3000,5000,10000};
    ADE_INT32_T n_cols_B[10]={2,10,50};//{2,10,50,100,500,1000,2000,3000,5000,10000};


    ret=blas3_test_procedure(test_cases,1,mat_type,2,n_rows_A,n_cols_A,n_cols_B,3,p_debug_out);

    return ret;
}
*/
