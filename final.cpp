#include<iostream>
#include<pthread.h>
#include<semaphore.h>
using namespace std;
pthread_t threads[27];	//initializing array for holding thread ids
pthread_mutex_t lock;	//lock1
pthread_mutex_t lock2;	//lock2
pthread_mutex_t lock3;	//lock3
int invalidEntries=0;	//This variable will count number of invalid entries
bool ifInvalid=false;
sem_t sem1,sem2,sem3,sem4;
int number=0;

struct parameters		//structure for holding column and row
{
	int column;
	int row;
	
};

//Now initializing the main sudoku array
int sudoku[9][9] = {
	{6, 2, 4, 5, 3, 9, 1, 8, 7},
	{5, 1, 9, 6, 2, 8, 6, 3, 4},
	{8, 3, 6, 6, 1, 4, 2, 9, 5},
	{1, 4, 3, 8, 6, 5, 7, 2, 9},
	{9, 5, 8, 2, 4, 7, 3, 6, 1},
	{7, 6, 2, 3, 9, 1, 4, 5, 8},
	{3, 7, 1, 9, 5, 6, 8, 4, 2},
	{4, 9, 6, 1, 8, 2, 5, 7, 3},
	{2, 8, 5, 4, 7, 3, 9, 1, 6}
};
int sudoku2[9][9] = {
	{6, 2, 4, 5, 3, 9, 1, 8, 7},
	{5, 1, 9, 6, 2, 8, 6, 3, 4},
	{8, 3, 6, 6, 1, 4, 2, 9, 5},
	{1, 4, 3, 8, 6, 5, 7, 2, 9},
	{9, 5, 8, 2, 4, 7, 3, 6, 1},
	{7, 6, 2, 3, 9, 1, 4, 5, 8},
	{3, 7, 1, 9, 5, 6, 8, 4, 2},
	{4, 9, 6, 1, 8, 2, 5, 7, 3},
	{2, 8, 5, 4, 7, 3, 9, 1, 6}
};

int valid[9][9] = {
	{6, 2, 4, 5, 3, 9, 1, 8, 7},
	{5, 1, 9, 6, 2, 8, 6, 3, 4},
	{8, 3, 6, 6, 1, 4, 2, 9, 5},
	{1, 4, 3, 8, 6, 5, 7, 2, 9},
	{9, 5, 8, 2, 4, 7, 3, 6, 1},
	{7, 6, 2, 3, 9, 1, 4, 5, 8},
	{3, 7, 1, 9, 5, 6, 8, 4, 2},
	{4, 9, 6, 1, 8, 2, 5, 7, 3},
	{2, 8, 5, 4, 7, 3, 9, 1, 6}
};

//This array will hold the results for each column,row and box.. row#1 is for columns,row#2 for rows and row#3 for boxes
int result[3][9]={
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0}
};
int wrongCol[9]={22,22,22,22,22,22,22,22,22};	//this array would be used in checking of columns

int wrongRow[9]={22,22,22,22,22,22,22,22,22};	//this array would be used in checking of rows

int wrongBox[9]={22,22,22,22,22,22,22,22,22};	//this array would be used in checking of boxes

pthread_t boxThreads[9]={0,0,0,0,0,0,0,0,0};	//this array will hold thread ids of box threads and synchronise with wrongBox array
												//to know which thread has calculated which result
												
												

void *colThread(void* arg)			//thread for checking all columns
{

	int oldtype;	
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&oldtype);	//setting type of thread to be cancelled as soon as the request arrives
	
	parameters* colData=(parameters*)arg;
	
	int validation[9][3];	//this array will be used for checking uniqueness of numbers in columns
	int currNumber;		
	int colValid;		//this will be 1 if column is valid otherwise 0
	
	

	int i=colData->column;
		colValid=1;									//initliazing validity of column
		for(int j=0;j<9;++j)
		{
			sem_wait(&sem2);
			currNumber=sudoku[j][i];				//reading single entry from sudoku at a time
			sem_post(&sem2);
			if(currNumber<1 )	{					//if number is less than 0
				//cout<<"Warning: Invalid Entry!  Column "<<i<<" Index=["<<j<<"]["<<i<<"]"<<" has value less than 0"<<endl;
				if(!ifInvalid){
				pthread_mutex_lock(&lock);			//locking the invalidEntries
				invalidEntries++;
				pthread_mutex_unlock(&lock);		//unlocking the invalidEntries
				}
				colValid=0;							//setting invalidity of the column
				
				
				wrongCol[i]=11;						//if number < 1, then store 11 in wrongCol array
				sem_wait(&sem1);
				sudoku2[j][i]=0;
				sem_post(&sem1);
			}
			else if(currNumber>9 )	{					//if number is greater than 9
				//cout<<"Warning: Invalid Entry!  Column "<<i<<" Index=["<<j<<"]["<<i<<"]"<<" has value greater than 9"<<endl;
				if(!ifInvalid){
				pthread_mutex_lock(&lock);			//locking the invalidEntries
				invalidEntries++;
				pthread_mutex_unlock(&lock);		//unlocking the invalidEntries
				}
				
				colValid=0;							//setting invalidity of the column
				
				wrongCol[i]=12;						//if number > 9, then store 12 in wrongCol array 
				sem_wait(&sem1);
				sudoku2[j][i]=0;
				sem_post(&sem1);
			}	
			else{
				if(validation[currNumber-1][0]!=22){			//if the number is not already present in validation array,store 22 at the index equal to number-1
					validation[currNumber-1][0]=22;
					validation[currNumber-1][1]=j;
					validation[currNumber-1][2]=i;
					
				}
				else{
					
					if(!ifInvalid){
				pthread_mutex_lock(&lock);			//locking the invalidEntries
				invalidEntries++;
				pthread_mutex_unlock(&lock);		//unlocking the invalidEntries
				}
					colValid=0;							//setting invalidity of the column
					
					wrongCol[i]=currNumber;				//if number repeats,store that number in wrongCol array
					sem_wait(&sem1);
					sudoku2[validation[currNumber-1][1]][validation[currNumber-1][2]]=0;
					sudoku2[j][i]=0;
					sem_post(&sem1);
				}
			}
		}
		pthread_mutex_lock(&lock2);			//locking result array
		result[0][i]=colValid;
		pthread_mutex_unlock(&lock2);		//unlocking result array
	
	cout<<colData->column<<endl;
	
	if(colValid)								//if column is valid than return 1 to the main thread
		pthread_exit((void*)1);
	pthread_exit((void*)pthread_self());	//otherwise return id of the thread
}


void *rowThread(void* arg)			//thread for checking all rows
{

	int oldtype;	
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&oldtype);	//setting type of thread to be cancelled as soon as the request arrives
	parameters* rowData=(parameters*)arg;
	
	int i=rowData->row;
	int validation[9][3];	//this array will be used for checking uniqueness of numbers in columns
	int currNumber;		
	int rowValid;		//this will be 1 if row is valid otherwise 0
	
	

	
		rowValid=1;									//initliazing validity of rows
		for(int j=0;j<9;++j)
		{
			sem_wait(&sem2);
			currNumber=sudoku[i][j];				//reading single entry from sudoku at a time
			sem_post(&sem2);
			if(currNumber<1 )	{					//if number is less than 0
				
				if(!ifInvalid){
				pthread_mutex_lock(&lock);			//locking the invalidEntries
				invalidEntries++;
				pthread_mutex_unlock(&lock);		//unlocking the invalidEntries
				}
				rowValid=0;							//setting invalidity of the row
				
				wrongRow[i]=11;						//if number < 1, then store 11 in wrongRow array 
				sem_wait(&sem1);
				sudoku2[i][j]=0;
				sem_post(&sem1);
			}
			else if(currNumber>9 )	{					//if number is greater than 9
		
				if(!ifInvalid){
				pthread_mutex_lock(&lock);			//locking the invalidEntries
				invalidEntries++;
				pthread_mutex_unlock(&lock);		//unlocking the invalidEntries
				}
				rowValid=0;							//setting invalidity of the row
				
				wrongRow[i]=12;						//if number > 9, then store 12 in wrongRow array 
				sem_wait(&sem1);
				sudoku2[i][j]=0;
				sem_post(&sem1);
			}	
			else{
				if(validation[currNumber-1][0]!=22){			//if the number is not already present in validation array,store 22 at the index equal to number-1
					validation[currNumber-1][0]=22;
					validation[currNumber-1][1]=i;
					validation[currNumber-1][2]=j;
					
				}
				else{
					
					if(!ifInvalid){
				pthread_mutex_lock(&lock);			//locking the invalidEntries
				invalidEntries++;
				pthread_mutex_unlock(&lock);		//unlocking the invalidEntries
				}
					rowValid=0;							//setting invalidity of the row
					
					wrongRow[i]=currNumber;				//if number repeats,store that number in wrongRow array
					sem_wait(&sem1);
					sudoku2[validation[currNumber-1][1]][validation[currNumber-1][2]]=0;
					sudoku2[i][j]=0;
					sem_post(&sem1);
				}
			}
		}
		pthread_mutex_lock(&lock2);			//locking result array
		result[1][i]=rowValid;
		pthread_mutex_unlock(&lock2);		//unlocking result array
	
	
	if(rowValid)								//if rows is valid than return 1 to the main thread
		pthread_exit((void*)1);
	pthread_exit((void*)pthread_self());	//otherwise return id of the thread
	
}
void *boxThread(void* arg)				//thread for checking single box
{
	int oldtype;	
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&oldtype);	//setting type of thread to be cancelled as soon as the request arrives
	
	parameters* data=(parameters*)arg;		//casting (void*)arg  into (parameters*)arg
	int box=data->row+(data->column/3);		//from the rows and columns, it will identify box number 
	int validation[9][3];						//this will be used in checking uniqueness of entries
	int currNumber;
	int boxvalid=1;					//to store if the box is valid
	for(int i=data->row;i<data->row+3;++i){
		for(int j=data->column;j<data->column+3;++j)
		{
			sem_wait(&sem2);
			currNumber=sudoku[j][i];				//reading single entry from sudoku at a time
			sem_post(&sem2);
			if(currNumber<1 )	{					//if number is less than 0
				
				if(!ifInvalid){
				pthread_mutex_lock(&lock);			//locking the invalidEntries
				invalidEntries++;
				pthread_mutex_unlock(&lock);		//unlocking the invalidEntries
				}
				boxvalid=0;							//setting that box is invalid
			
				pthread_mutex_lock(&lock3);
				wrongBox[box]=11;					//if number < 1, then store 11 in wrongBox array 
				boxThreads[box]=pthread_self();		//store thread id at the same index but in boxThreads array
				pthread_mutex_unlock(&lock3);
				sem_wait(&sem1);
				sudoku2[i][j]=0;
				sem_post(&sem1);
			}
			else if(currNumber>9 )	{					//if number is greater than 9
			
				if(!ifInvalid){
				pthread_mutex_lock(&lock);			//locking the invalidEntries
				invalidEntries++;
				pthread_mutex_unlock(&lock);		//unlocking the invalidEntries
				}
				boxvalid=0;							//setting that box is invalid
				
				pthread_mutex_lock(&lock3);
				wrongBox[box]=12;					//if number > 9, then store 12 in wrongBox array 
				boxThreads[box]=pthread_self();		//store thread id at the same index but in boxThreads array
				pthread_mutex_unlock(&lock3);
				sem_wait(&sem1);
				sudoku2[i][j]=0;
				sem_post(&sem1);
			}	
			else{
				if(validation[currNumber-1][0]!=22){			//if the number is not already present in validation array,store 22 at the index equal to number-1
					validation[currNumber-1][0]=22;
					validation[currNumber-1][1]=i;
					validation[currNumber-1][2]=j;

				}
				else{
					
					if(!ifInvalid){
				pthread_mutex_lock(&lock);			//locking the invalidEntries
				invalidEntries++;
				pthread_mutex_unlock(&lock);		//unlocking the invalidEntries
				}
					boxvalid=0;							//setting that box is invalid
					
					pthread_mutex_lock(&lock3);		//locking two resouces wrongBox and boxThreads
					wrongBox[box]=currNumber;		//if number is repeating, then store it in wrongBox array 
					boxThreads[box]=pthread_self();	//store thread id at the same index but in boxThreads array
					pthread_mutex_unlock(&lock3);	//unlocking two resouces wrongBox and boxThreads
					sem_wait(&sem1);
					sudoku2[validation[currNumber-1][1]][validation[currNumber-1][2]]=0;
					sudoku2[i][j]=0;
					sem_post(&sem1);
				}
			}
			
		}
	}
	pthread_mutex_lock(&lock2);		//locking result array
	result[2][box]=boxvalid;
	
	pthread_mutex_unlock(&lock2);		//unlocking result array
	
	
	if(boxvalid)	//if box is valid than return 1 to the main thread
		pthread_exit((void*)1);
	
	pthread_exit((void*)pthread_self());	//otherwise return thread id
	
}

bool checkSudoku(int row,int col)
{
	parameters* data=new parameters;
	data->row=row;
	data->column=col;
	int columnThread=0+col;
	pthread_create(&threads[columnThread], NULL, colThread, data);		//creating thread 1 for checking columns
	int rThread=9+row;
	pthread_create(&threads[rThread], NULL, rowThread, data);		//creating thread 2 for checking rows
	
	int num=(row-row%3)+(col-col%3)+18;		
	cout<<"boxcreate"<<endl;
	data->row=row-row%3;
	data->column=col-col%3;
	pthread_create(&threads[num], NULL, boxThread, data);	//creating thread for each box
	cout<<"LALA"<<endl;
	
	
	long th_result[3];			//for catching thread ids of threads that return invalid result
	void *status;
	bool retValue;
	for(int i=0,j=0;i<27;i++){
		if(i==columnThread||i==rThread||i==num){
			pthread_join(threads[i],&status);
			th_result[j]=(long)status;	//storing thread ids
			j++;
			if((long)status!=1)
				retValue=false;
		}
	}
	return retValue;
}


bool invalidEntryLocation(int &r,int &c)	//checking where the invalid entry is located
{
	for (r=0; r<9;r++){  
        for (c=0; c<9;c++){
				sem_wait(&sem1);
            if (sudoku2[r][c] == 0){sem_post(&sem1);  
                return true;}
					sem_post(&sem1);}
}
    return false; 
}

bool solution()
{
	int row,col;
	if(!invalidEntryLocation(row,col))
		return true;
	// consider digits 1 to 9  
	cout<<row<<"  "<<col<<endl;
    for (int num = 1; num <= 9; num++)  
    {  sem_wait(&sem1);
		sudoku2[row][col]=num;
		sem_post(&sem1);
		cout<<"enter1"<<endl;
        // if looks promising  
        if (checkSudoku(row,col))  
        {  cout<<"enter2"<<endl;
            // make tentative assignment 
						
  
            // return, if success, yay!  
            if (solution())  
                return true;  
  
            // failure, unmake & try again  
			/*sem_wait(&sem1);
            sudoku2[row][col] = sudoku[row][col];  
			sem_post(&sem1);*/
        }  
    }  
    return false; // this triggers backtracking 
}
int main()
{
	
	for(int i=0;i<9;i++)
		for(int j=0;j<9;j++)
			sudoku2[i][j]=sudoku[i][j];
	
for(int i=0;i<9;i++){
			for(int j=0;j<9;j++){
				cout<<sudoku2[i][j]<<" ";
			}
			cout<<endl;
		}	
		cout<<endl;
	if (sem_init(&sem1,0,1) != 0) 	//initializing the sem1
    { 
        printf("\n sem 1 init has failed\n"); 
        return 1; 
    }
	if (sem_init(&sem2,0,1) != 0) 	//initializing the sem2
    { 
        printf("\n sem 2 init has failed\n"); 
        return 1; 
    }
	if (sem_init(&sem3,0,1) != 0) 	//initializing the sem3
    { 
        printf("\n sem 3 init has failed\n"); 
        return 1; 
    }
	if (sem_init(&sem4,0,1) != 0) 	//initializing the sem3
    { 
        printf("\n sem 4 init has failed\n"); 
        return 1; 
    }
	if (pthread_mutex_init(&lock, NULL) != 0) 	//initializing the lock1
    { 
        printf("\n mutex 1 init has failed\n"); 
        return 1; 
    }
	if (pthread_mutex_init(&lock2, NULL) != 0) 	//initializing the lock2
    { 
        printf("\n mutex 2 init has failed\n"); 
        return 1; 
    }
	if (pthread_mutex_init(&lock3, NULL) != 0) 	//initializing the lock3
    { 
        printf("\n mutex 3 init has failed\n"); 
        return 1; 
    }
	
	//checking total number of invalid entries and storing them in valid array
	int threadIndex=0;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {						
			if (i%3 == 0 && j%3 == 0) {
				parameters *data2 = new parameters[9];	
				data2->row = i;		
				data2->column = j;
				pthread_create(&threads[threadIndex++], NULL, boxThread, data2); // 3x3 subsection threads
			}
			if (i == 0) {
				parameters *columnData = new parameters[9];	
				columnData->row = i;		
				columnData->column = j;
				pthread_create(&threads[threadIndex++], NULL, colThread, columnData);	// column threads
			}
			if (j == 0) {
				parameters *rowData = new parameters[9];	
				rowData->row = i;		
				rowData->column = j;
				pthread_create(&threads[threadIndex++], NULL, rowThread, rowData); // row threads
			}
		}
	}
	for (int i = 0; i < 27; i++) {
		pthread_join(threads[i], NULL);			// Wait for all threads to finish
	}
	ifInvalid=true;
	
for(int i=0;i<9;i++){
			for(int j=0;j<9;j++){
				cout<<sudoku2[i][j]<<" ";
			}
			cout<<endl;
		}
	cout<<endl;
	solution();
	/*if(solution())
	{
		for(int i=0;i<9;i++){
			for(int j=0;j<9;j++){
				cout<<sudoku[i][j]<<" ";
			}
			cout<<endl;
		}*/
		
		for(int i=0;i<9;i++){
			for(int j=0;j<9;j++){
				cout<<sudoku2[i][j]<<" ";
			}
			cout<<endl;
		}
	
	



	
	
	
	
	
	cout<<"\n\n Total Number of Invalid Entries="<<invalidEntries<<endl;	//printing total number of Invalid Entries
	

	
	
	cout<<endl<<endl<<endl;
	pthread_mutex_destroy(&lock);	//destroying lock 1
	pthread_mutex_destroy(&lock2);	//destroying lock 2
	pthread_mutex_destroy(&lock3);	//destroying lock 3
	return 0;
}