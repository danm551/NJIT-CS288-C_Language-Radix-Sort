#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>

#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>

#define UNSIGNED 0
#define SIGNED	1
#define OK 1
#define NOK 0
#define NELM 100		/* default 100 elements */
#define N 1048576		/* 2^30 or 1 meg elements  */
//#define N 2097152
//#define N 4194304
//#define N 8388608
//#define N 16777216
//#define N 33554432
 
void selection_sort();
void merge_sort();
void msort_recursive();

void int_radix_sort();
void float_radix_sort();
void swap();
void self_check();

void init_lst();
void print_lst();

int n, lst[N],tmp[N];

int main(int argc,char **argv) {
	
  long int del_sec,del_msec;
  struct timeval tv_s,tv_e;

  if (argc>1) n = atoi(argv[1]);
  else n = NELM;
  printf("n=%d\n",n);
  init_lst(lst,n);
  //print_lst(lst,n);

  gettimeofday(&tv_s, NULL);
  del_sec = tv_s.tv_sec; 
  //selection_sort(lst,n);
  //merge_sort(lst,tmp,0, n-1);
  //msort_recursive(lst,tmp,0, n/2, n-1);
  int_radix_sort(lst,tmp,n);
  //float_radix_sort(lst,tmp,n);
  gettimeofday(&tv_e, NULL); 

  printf("Time: %d\n",tv_e.tv_sec - del_sec);

  //print_lst(lst,n);
  self_check(lst,n);

	return 0;
}

void selection_sort(int list[],int n){
	int temp, i, k;

  for(i = 0; i < n; i++){
		for(k = 0; k < n; k++){
			if(list[i] > list[k] && i < k){
				temp = list[i];
				list[i] = list[k];
				list[k] = temp;
			}
		}	
	}
}

void merge_sort(int list[], int temp[], int low, int high){
  int mid;

	if(low < high){
		mid = (low+high)/2;
		merge_sort(list,temp,low,mid);
		merge_sort(list,temp,mid+1,high);
		msort_recursive(list,temp,low,mid,high);
	}

//msort_recursive(list, temp, 0, n-1);
}

//use recursion
void msort_recursive(int list[], int temp[], int low, int mid, int high){
	int i, m, k, l;
	
	//temp[5000000];
	l = low;
	i = low;
	m = mid+1;

	while((l <= mid) && (m <= high)){
		if(list[l] <= list[m]){
			temp[i] = list[l];
			l++;
		}
		else{
			temp[i] = list[m];
			m++;
		}
		i++;
	}
	if(l > mid){
		for(k = m; k <= high; k++){
			temp[i] = list[k];
			i++;
		}
	}
	else{
			for(k = l; k <= mid; k++){
					temp[i] = list[k];
					i++;
			}
	}
	for(k = low; k <= high; k++){
				list[k] = temp[k];
	}
}

void int_radix_sort(){	
	int group; //because extracting 8 bytes
	int buckets = 1 << 8; //using size 256
	int map[buckets];	
	int mask = buckets - 1;
	int i;
	int cnt[buckets];
	int flag = NULL;
	int partition;
	int *src, *dst;

	for(group = 0; group < 32; group += 8){ //group = 8, number of bytes we want per round, we want 4 rounds
		//cnt	
		for(int i = 0; i < buckets; i++){
			cnt[i] = 0;
		}
		for(int j = 0; j < n; j++){ 
			i = (lst[j]>>group)&mask;
			cnt[i]++; 
			tmp[j] = lst[j];
		}

		//map
		map[0] = 0;
		for(int i = 1; i < buckets; i++){
			map[i] = map[i-1] + cnt[i-1];
		}

		//move
		for(int j = 0; j < n; j++){	
			i = (tmp[j]>>group)&mask;
			lst[map[i]] = tmp[j];
			map[i]++;
		}
	}

	//negative check
	for (i=0;i<n-1;i++)
     	if (lst[i+1] < 0){
			flag = SIGNED;
			partition = i+1;
			break; 
		}
		else
			flag = UNSIGNED;

	if(flag == 1){
		printf("swap\n");
		swap(lst, n, partition);
	}
	else
		printf("no swap\n");
}

void float_radix_sort(){
	//float lst[10] = {47914.05,-49914.04,96112.06,105788.04,122270.07,-101745.01,101987.03,7228.10,-204878.21,205836.11};
	int group; //because extracting 8 bytes
	int buckets = 1 << 8; //using size 256
	int map[buckets];	
	int mask = buckets - 1;
	int i;
	int cnt[buckets];
	int flag = NULL;
	int partition;
	int *src, *dst;
	int x;

	for(group = 0; group < 64; group += 8){ //group = 8, number of bytes we want per round, we want 4 rounds
		//cnt	
		for(int i = 0; i < buckets; i++){
			cnt[i] = 0;
		}
		for(int j = 0; j < n; j++){
			x = *(unsigned long*)&lst[j]; 
			i = (x>>group)&mask;
			cnt[i]++; 
			tmp[j] = lst[j];
		}
	
		//map
		map[0] = 0;
		for(int i = 1; i < buckets; i++){
			map[i] = map[i-1] + cnt[i-1];
		}

		//move
		for(int j = 0; j < n; j++){
			x = *(unsigned long*)&tmp[j];	
			i = (x>>group)&mask;
			lst[map[i]] = tmp[j];
			map[i]++;
		}
	}

	//negative check
	for (i=0;i<n-1;i++)
     	if (lst[i+1] < 0){
			flag = SIGNED;
			partition = i+1;
			break; 
		}
		else
			flag = UNSIGNED;

	if(flag == 1){
		printf("swap\n");
		swap(lst, n, partition, flag);
	}
	else
		printf("no swap\n");
}

void swap(float *lst, int n, int partition, int flag){ //if unsigned integers are present, this swaps

	float temp[n];
	int low = n - partition;
	int j = 0, k = (n-1), i, half;

	if(flag = SIGNED){
		for(i = 0; i < n; i++){
			if(lst[i] < 0){
				half = i;
				break;
			}
		}
		
		for(i = 0; i < n-half; i++){
			temp[i] = lst[k];
			k--;
		}

		k = (n-half);
		for(i = 0; i < half; i++){
			temp[k] = lst[i];
			k++;
		}


	printf("TEMP:  ");
	for(int h = 0; h < n; h++){
		printf("%f, \n", temp[h]);
	}


		for(i = 0; i < n; i++)
			lst[i] = temp[i];

	}
	else{
		for(int i = partition; i < n; i++){
			temp[j] = lst[i];
			j++;
		}

		j = 0;
		for(int i = low; i < n; i++){
			temp[i] = lst[j];
			j++;
		}
	
		for(int i = 0; i < n; i++)
			lst[i] = temp[i];
	}
}

void print_lst(int *l,int n){
  int i;
  for (i=0; i<n; i++) {
    printf("%d ",l[i]);
  }
  printf("\n");
}

void init_lst(int *l,int n){
  int i;
    //int seed = time(0) % 100;	/* seconds since 1/1/1970 */
    //printf ("seed=%d\n", seed);
  srand(1234);			/* SEED */
  for (i=0; i<n; i++) {
    l[i] = rand();
  }
}

void self_check(int *list,int n) {
  int i,j,flag=OK,*listp;

  listp = list;
  for (i=0;i<n-1;i++)
     if (listp[i] > listp[i+1]) { flag = NOK; break; }

  if (flag == OK) printf("sorted\n");
  else printf("NOT sorted at %d\n",i);
}

