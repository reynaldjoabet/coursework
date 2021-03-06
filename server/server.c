// Cwk2: server.c - multi-threaded server using readn() and writen()

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include "rdwrn.h"
#include <sys/utsname.h>
#include <time.h>
#include <time.h>		// for clock() - a standard library function
#include <sys/time.h>		// gettimeofday() - a system call
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>

#define SERVER_PORT 50031

   struct timeval tv1;

int found=0;// using this as a flag to indicate if there is a name match among the files present in the directory.
   // 0 is false while 1 represents true


  char string_ip_address[INET_ADDRSTRLEN];// global variable
// thread function
void *client_handler(void *);

typedef struct {
    int id_number;
    int age;
    float salary;
} employee;

void get_and_send_employee(int, employee *);
int* generate_random_numbers();
void send_hello(int);
void get_and_send_name_and_student_id(int);
void get_and_send_five_random_numbers(int,int *);
void get_and_send_file_names(int socket);
struct utsname server_uname_information();
void get_and_send_server_uname_information(int,struct utsname *);
char * get_ip_address();
void send_file(int);
int count=0;
void get_and_send_server_time(int);
// signal handler to be called on receipt of (in this case) SIGTERM
static  void handler(int sig, siginfo_t *siginfo, void *context)
{

	 struct timeval tv2;

    printf("\n\nPID: %d, UID: %ld\n", siginfo->si_pid, (long) siginfo->si_status);
 
    //printf("%u ",context);

    // get "wall clock" time at end
    if (gettimeofday(&tv2, NULL) == -1) {
        perror("gettimeofday error");
        exit(EXIT_FAILURE);
    }
    // in microseconds...
 float execution_time= (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec);


 // in microseconds...
    printf("\nTotal execution time = %f seconds\n\n",execution_time);


     exit(EXIT_SUCCESS);
}



// you shouldn't need to change main() in the server except the port number
int main(void)
{
    int listenfd = 0, connfd = 0;

     struct sigaction act={};// initialize to all 0s

    // memset(&act, 0, sizeof(act));

    // the SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler
    // this also enables the handling function to be called like so 
    // void  hndler(int signo, siginfo_t *info, void *context);
    
    
    // this is a pointer to a function
    act.sa_sigaction =handler;//handler function pointer

    act.sa_flags = SA_SIGINFO;

   // if successful, returns 0 else returns -1
    if (sigaction(SIGINT, &act, NULL) == -1) {// set the signal handler for SIGINT
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    
 

  
    struct sockaddr_in serv_addr;// declares internet socket address for server
    struct sockaddr_in client_addr; //
    socklen_t socksize = sizeof(struct sockaddr_in);
    listenfd = socket(AF_INET, SOCK_STREAM, 0); // socket returns a file descriptor
    memset(&serv_addr, '0', sizeof(serv_addr));// clean buffer

    serv_addr.sin_family = AF_INET; // sets the socket internet address family to AF_INET, which is for IPV4 addresses
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //  socket internet address-binds to any address if server has multiple addresses eg INADDR_LOOPBACK
    serv_addr.sin_port = htons(SERVER_PORT); //converts host byte order into  network byte order

     struct sockaddr_in* ipv4address=(struct sockaddr_in*) &serv_addr;
     struct in_addr ipaddress= ipv4address->sin_addr;// access the sin_addr member of in_addr
      //char string_ip_address[INET_ADDRSTRLEN]; // declares an array of length INET_ADDRSTRLEN
     if(inet_ntop(AF_INET,&ipaddress,string_ip_address,INET_ADDRSTRLEN)==NULL){
     perror("Failed to obtain ip of server");
     exit(EXIT_FAILURE);

     }// convert ip address to string



    bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)); // binds, casting  serv_addr to sockaddr

    // if 
    if (listen(listenfd, 10) == -1) {
	perror("Failed to listen");
	exit(EXIT_FAILURE);
    }
    // end socket setup

  

 // get "wall clock" time at start
    // once the client connects, the start time is set
    if (gettimeofday(&tv1, NULL) == -1) {
        perror("gettimeofday error");
        exit(EXIT_FAILURE);
    }


   
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    while (1) {
	printf("Waiting for a client to connect...\n");
	connfd =
	    accept(listenfd, (struct sockaddr *) &client_addr, &socksize);
	printf("Connection accepted...\n");

	pthread_t sniffer_thread;
        // third parameter is a pointer to the thread function, fourth is its actual parameter
	if (pthread_create
	    (&sniffer_thread, NULL, client_handler,
	     (void *) &connfd) < 0) {
	    perror("could not create thread");
	    exit(EXIT_FAILURE);
	}
	//Now join the thread , so that we dont terminate before the thread
	//pthread_join( sniffer_thread , NULL);
	printf("Handler assigned\n");
    }




       


    close(connfd);
    close(listenfd);
    

      
 

    // never reached...
    // ** should include a signal handler to clean up
    exit(EXIT_SUCCESS);
} // end main()

// thread function - one instance of each for each connected client
// this is where the do-while loop will go
void *client_handler(void *socket_desc)  // client_hndler needs to accept a pointer and return a pointer
{
   
    //Get the socket descriptor
    int connfd = *(int *) socket_desc;
    
 free(socket_desc);// free pointer
    //get_and_send_server_time(connfd);

    /*
    send_hello(connfd);

    employee *employee1;
    employee1 = (employee *) malloc(sizeof(employee));

    int i;
    for (i = 0; i < 5; i++) {
	printf("(Counter: %d)\n", i);
	get_and_send_employee(connfd, employee1);
	printf("\n");
    }

    free(employee1);
*/

     struct utsname uts=server_uname_information();// get the server information

     int choice;
     readn(connfd, (int *) &choice,sizeof(int) );
      int *random_number_pointer=generate_random_numbers();

     //printf("choiceeee %d\n",choice);
     switch(choice){
      case 1: 
	      get_and_send_name_and_student_id( connfd);
	      break;
      case 2:
	      get_and_send_five_random_numbers(connfd,random_number_pointer);

	      break;
      case 3:
	      get_and_send_server_uname_information(connfd,&uts);

	      break;
      case 4:
	      get_and_send_file_names(connfd);
	      break;
      case 5: 
	      send_file(connfd); 
	      break;
      case 6:
	     
	      break;
     
     } while(choice!=6);
    


   
    printf("\n\n number of clients is %d",count);
  


    printf("Thread %lu exiting\n", (unsigned long) pthread_self());
   
    // always clean up sockets gracefully
    shutdown(connfd, SHUT_RDWR);
    close(connfd);

    return NULL;
}  // end client_handler()

// how to send a string
void send_hello(int socket)
{
    char hello_string[] = "hello SP student";

    size_t n = strlen(hello_string) + 1;// strlen() returns the length of the string, without a null character
                                         // the +1 adds
    writen(socket, (unsigned char *) &n, sizeof(size_t));// send the amount of characters to be sent to client 	
    writen(socket, (unsigned char *) hello_string, n);	  // send data
} // end send_hello()

// as before...
void get_and_send_employee(int socket, employee * e)
{
    size_t payload_length;

    size_t n =
     readn(socket, (unsigned char *) &payload_length, sizeof(size_t));
    printf("payload_length is: %zu (%zu bytes)\n", payload_length, n);
    n=readn(socket, (unsigned char *) e, payload_length);

    printf("Age is %d\n", e->age);
    printf("id is %d\n", e->id_number);
    printf("Salary is %6.2f\n", e->salary);
    printf("(%zu bytes)\n", n);

    // make arbitrary changes to the struct & then send it back
    e->age++;
    e->salary += 1.0;

    writen(socket, (unsigned char *) &payload_length, sizeof(size_t));
    writen(socket, (unsigned char *) e, payload_length);
}  // end get_and_send_employee()




// generate five random numbers
int * generate_random_numbers(){
        const  int max_number=1000;
        static  int random_numbers[5];// array of five random numbers
        int i;// to be used in for loop
      const int size= sizeof(random_numbers)/sizeof(int);// length of array, which in this case will always be five
        srand((unsigned)time(NULL)); // seed generator for pseudo-random numbers

        // populate the array with random numbers from 0 to 1000
        for(i=0; i<size; i++){

                random_numbers[i]=rand() % max_number;
                        }

          return random_numbers;
}// end of random number generator


void  write_file(char *file,int socket)
{

    struct stat sb;
    FILE *fp;
    found=1;

     if (stat(file, &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    
   static int payload_length;

   char data[payload_length];
    fp=fopen(file,"r");
     if(fp==NULL){ // attempt to read file
        strcpy(data,"Error reading file");
	payload_length=strlen(data);
        writen(socket, (unsigned int *) &payload_length, sizeof(int));
        writen(socket, (unsigned char *)data, payload_length);

     }else {

	     payload_length=sb.st_size;
   
        writen(socket, (unsigned int *) &payload_length, sizeof(int));

	while(fgets(data,payload_length,fp)>0){
        writen(socket, (unsigned char *)data, payload_length);

	}
           
     }
  fclose(fp);//used to close the file
}

// main function to send file to client
void send_file(int socket){
       char message[65];
    struct dirent **namelist;
    static int payload_length;
  static   int n;
     // static size_t name_size;
         char file_name[100]="hello.txt";      
//	readn(socket, (size_t *) &name_size, sizeof(size_t));
  
	//char file_name[name_size];
  //      readn(socket, (unsigned char *) file_name, name_size);

	
    // tell scandir to scan the directory(./upload) and store an array of pointers to directory entries  in namelist
    // alphasort is used to sort the array aphabetically
    // scandir uses malloc to allocate the space
    // scandir returns the number of entries in the array and stores it in n


    if ((n = scandir(".", &namelist, NULL, alphasort)) == -1){
        
       strcpy(message,"Folder upload does not exist in current directory");
        payload_length=strlen(message);
        writen(socket, (unsigned int *) &payload_length, sizeof(int));
        writen(socket, (unsigned char *)message, payload_length);

    }
    else {
         if(n==0){
         strcpy(message,"upload directory is empty");
        payload_length=strlen(message);
        writen(socket, (unsigned int *) &payload_length, sizeof(int));
        writen(socket, (unsigned char *)message, payload_length);
         
       }
         else{
        while (n--) {
//        
           if(strcmp(namelist[n]->d_name,file_name)==0){
           write_file(namelist[n]->d_name,socket);// only called if the entered file name and a file name in the directory match
           free(namelist[n]);   // free the
	   }
	}
	
          free(namelist);// free the entire array


        if(found==0){
        strcpy(message,"Directory upload does not contain any file with the given name");
        payload_length=strlen(message)+1;
        writen(socket, (unsigned int *) &payload_length, sizeof(int));
        writen(socket, (unsigned char *)message, payload_length);
        

      } // end of inner else block

    }// end of inner else block
} // end of outer else block

  
}


void get_and_send_name_and_student_id(int client_socket){
     char student_name[45]="Liyeuk Reynald Joabet ";
      char student_id[]=  " S1906586";

   
     strcat(student_name,string_ip_address);// concatenate student_name and ip address
     strcat(student_name,student_id);// concatenate the result of the first concatenation with student_id
      size_t length=strlen(student_name);//length of message to be sent
 
      size_t size=length+1;
     // send length of message to client first
      writen(client_socket, (size_t *) &size, sizeof(size_t));// send the amount of characters to be sent to client  
      writen(client_socket, (unsigned char *)student_name, size);  
}

void get_and_send_five_random_numbers(int socket,int *p){
        const int length=15;
        char message[length];// 15 is the maximum we can have as the maximum random number is 999. 3 digit  times 5 
	for(int i=0;i<5;i++){
       sprintf(&message[strlen(message)],"%d",*(p+i));

      }


	// send length of message to client first
      writen(socket, (unsigned int *) &length, sizeof(int));// send the amount of characters to be sent to client
      writen(socket, (unsigned char *)message, length);


      printf("%s\n",message);
}

struct utsname server_uname_information(){
         struct utsname uts;
        if(uname(&uts)== -1){
        perror("uname error");
        exit(EXIT_FAILURE);
      }


  return uts;	
}

void get_and_send_server_uname_information(int socket,struct utsname * uts){

     size_t payload_length=sizeof(*(uts));// calculate the size of the struct to send to client

   // send length of message to client first
    writen(socket, (unsigned char *) &payload_length, sizeof(size_t));// send the amount of characters to be sent to client
   writen(socket, (unsigned char *)uts, payload_length);


    //printf("hello and welcome %lu ",sizeof(&(uts)));
    //printf("Node name:    %s\n",  uts->nodename);
    //printf("System name:  %s\n",  uts->sysname);
    //printf("Release:      %s\n",  uts->release);
    //printf("Version:      %s\n",  uts->version);
  //  printf("Machine:      %s\n",  uts->machine);
    //printf("(%zu bytes)\n", n);

    

}


 void get_and_send_server_time(int socket){
	  size_t payload_length;
 
      time_t t;    // always look up the manual to see the error conditions
    //  here "man 2 time"
    if ((t = time(NULL)) == -1) {
	perror("time error");
	exit(EXIT_FAILURE);
    }

    // localtime() is in standard library so error conditions are
    //  here "man 3 localtime"
    struct tm *tm;
    if ((tm = localtime(&t)) == NULL) {
	perror("localtime error");
	exit(EXIT_FAILURE);
    }    

    payload_length=sizeof(*(tm));


     writen(socket, (unsigned int *) &payload_length, sizeof(size_t));
     writen(socket, (unsigned char *)tm, payload_length);

    printf("%s\n", asctime(tm));
    printf("%lu \n",payload_length);



 }

void stat_file(char *file,char message[])
{
  
  const char separator[]="  ";
    struct stat sb;


     if (stat(file, &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    //printf("File type:            %s    ",file);

     switch (sb.st_mode & S_IFMT){
    case S_IFREG:
 
         strcat(message,separator);
        strcat(message,file);
        break;
    default:
        // do nothing
        break;
    }
    
    
}




void get_and_send_file_names(int socket){
    

      char message[1000]="";
    struct dirent **namelist;
    int payload_length;
    int n;
    // tell scandir to scan the directory(./upload) and store an array of pointers to directory entries  in namelist
    // alphasort is used to sort the array aphabetically
    // scandir uses malloc to allocate the space
    // scandir returns the number of entries in the array and stores it in n
    


    if ((n = scandir(".", &namelist, NULL, alphasort)) == -1){
	perror("scandir");

       strcpy(message,"Folder upload does not exist in current directory");
        payload_length=strlen(message);
        writen(socket, (unsigned int *) &payload_length, sizeof(int));
        writen(socket, (unsigned char *)message, payload_length);

       printf("%s \n\n",message);

    }
    else {
	 if(n==0){
	 strcpy(message,"upload directory is empty");
        payload_length=strlen(message);
        writen(socket, (unsigned int *) &payload_length, sizeof(int));
        writen(socket, (unsigned char *)message, payload_length);

       printf("%s \n\n",message);

	 }
	 else{ 
	while (n--) {
//	    printf("File name:		  %s\n", namelist[n]->d_name);
     stat_file(namelist[n]->d_name,message);
	   free(namelist[n]);	// free the 
	}
	free(namelist);// free the entire array

        if(sizeof(message)==0){
	strcpy(message,"Directory files are not regular files");
        payload_length=strlen(message);
        writen(socket, (unsigned int *) &payload_length, sizeof(int));
        writen(socket, (unsigned char *)message, payload_length);

       printf("%s \n\n",message);

	}
	else{
	payload_length=strlen(message);
        writen(socket, (unsigned int *) &payload_length, sizeof(int));
        writen(socket, (unsigned char *)message, payload_length);
      } // end of inner else block 


    }// end of inner else block
//    printf("%s\n\n ",message);
} // end of outer else block
}
