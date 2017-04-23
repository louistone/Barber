#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <stdio.h>
#include <queue>

#define maxCus 25

using namespace std;

//define global variables
sem_t mySem;
sem_t waitLim;
sem_t lockArr;
sem_t lockWait;
int numChair;
queue<int> cusQueue;
queue<int> waitQ;

void* Customer(void* param)
{
    int* cusID = (int *)param;
    int ID, cutID;

    printf("Customer %d leaving for barber shop\n",*cusID);
    
    sem_wait(&lockArr);
    printf("Customer %d arrive at barber shop\n",*cusID);
    cusQueue.push(*cusID);
    sem_post(&lockArr);
 
    sem_wait(&waitLim);

    sem_wait(&lockWait);
    ID = cusQueue.front();
    cusQueue.pop();
    printf("Customer %d entering waiting room\n",ID);
    waitQ.push(ID);
    sem_post(&lockWait);
       
    sem_wait(&mySem);
    cutID = waitQ.front();
    waitQ.pop();
    printf("Customer %d waking the barber\n",cutID);
    sem_post(&waitLim);

    printf("The barber is cutting hair %d\n",cutID);
    printf("The barber has finished cutting hair %d\n",cutID);
    printf("The barber is sleeping\n");
    printf("Customer %d leaving barber shop\n",cutID);
    sem_post(&mySem);
    return NULL;
}

int main()
{
    //define variables
    int numCustomer;

    //get input from user
    cout<<"Maximum number of customers can only be 25;"<<endl;
    cout<<"Enter the number of customers and chairs: ";
    cin>>numCustomer>>numChair;

    //validate input
    while(numCustomer<0||numCustomer>25)
    {
        cout<<"Your input for the number of customers is invalid!"<<endl;
        cout<<"Please enter it again: ";
        cin>>numCustomer;
    }

    //declare thread and other variables
    //pthread_t thread[numCustomer];
    // pthread_t thread[numCustomer];
    pthread_t *thread = new pthread_t[numCustomer];
    int status;
    int *thread_arg=new int[numCustomer];
    sem_init(&mySem,0,1);
    sem_init(&waitLim,0,numChair);
    sem_init(&lockArr,0,1);
    sem_init(&lockWait,0,1);

    //create all threads one by one
    for(int i=0; i<numCustomer;i++){
        thread_arg[i]=i+1;
        
        status = pthread_create(&thread[i],NULL,Customer,(void*)&thread_arg[i]);
        
        if(status){
            cout<<"Creation failure"<<endl;
            return -1;
        }

    }

    //wait for each thread to complete
    for(int i=0;i<numCustomer;i++){
        status=pthread_join(thread[i],NULL);
        if(status){
            cout<<"pthread_join failure"<<endl;
            return -1;
        }
        
    }
      
    cout<<"The barber is going home for the day."<<endl;

    sem_destroy(&mySem);
    sem_destroy(&waitLim);
    sem_destroy(&lockArr);
    sem_destroy(&lockWait);
    delete[] thread_arg;
    delete[] thread;
    return 0;
}
