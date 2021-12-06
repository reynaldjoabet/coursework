// Cwk2: client.c - message length headers with variable sized payloads
//  also use of readn() and writen() implemented in separate code module

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "rdwrn.h"
#include <sys/utsname.h>
#include <time.h>
#include<ctype.h>

typedef struct {
    int id_number;
    int age;
    float salary;
} employee;

// how to send and receive structs
void send_and_get_employee(int socket, employee *e)  
{
    size_t payload_length = sizeof(employee);

    // send the original struct
    writen(socket, (unsigned char *) &payload_length, sizeof(size_t));	
    writen(socket, (unsigned char *) e, payload_length);	 		

    // get back the altered struct
    readn(socket, (unsigned char *) &payload_length, sizeof(size_t));	   
    readn(socket, (unsigned char *) e, payload_length);

    // print out details of received & altered struct
    printf("Age is %d\n", e->age);
    printf("id is %d\n", e->id_number);
    printf("Salary is %6.2f\n", e->salary);    
} // end send_and_get_employee()

// how to receive a string
void get_hello(int socket)
{
    char hello_string[32];
    size_t k;
    
    readn(socket, (unsigned char *) &k, sizeof(size_t));	
    readn(socket, (unsigned char *) hello_string, k);

    printf("Hello String: %s\n", hello_string);
    printf("Received: %zu bytes\n\n", k);
} // end get_hello()


//
void get_server_time(int socket)
{
    struct tm tm;
    size_t k;

    readn(socket, (unsigned int *) &k, sizeof(size_t));
    readn(socket, (unsigned char *)&tm, k);

    printf("Current Server TIme:  %s\n", asctime(&(tm)));
    printf("Received: %zu bytes\n\n", k);

} // end get_hello()



// function to read random numbers
void get_random_numbers(int socket,int choice){

         int k;
         writen(socket, (unsigned int *) &choice,sizeof(int) );
     
  	readn(socket,(unsigned int *) &k,sizeof(int));
	int length=k/sizeof(int);
	char message[length];
	readn(socket,(unsigned char *)message,k);

	printf("Random Number: %s\n",message);
	printf("Received: %d bytes\n\n", k);

       
}// end of get_random_numbers()



// function to read random numbers
void quit_client(int socket,int choice){
	// send option to close client
         writen(socket, (unsigned int *) &choice,sizeof(int));

}// en of function

void get_student_name_and_student_id(int socket,int choice){
   char message[45];// the maximum length of the message is less than 45
    size_t k;
     writen(socket, (int *) &choice,sizeof(int) );

    readn(socket, (unsigned int *) &k, sizeof(size_t));
    readn(socket, (unsigned char *)message, k);

    printf("Student Details: %s\n", message);
    printf("Received: %lu bytes\n\n", k);
 
    
} // end of get_student_name_and_student_id()

void get_file_copy_enhancement(int socket, int choice){
           

	printf("\nEnter file name\n\n");
	char file_name[100];
	scanf("%s",file_name);

	
       size_t size=strlen(file_name)+1;

        //
        writen(socket, (int *) &choice,sizeof(int) );//  send choice to server
       writen(socket, (size_t *) &size, sizeof(size_t)); // take sizeof(size_t) bytes  from (&size) and write to socket
       writen(socket, (unsigned char *)&file_name, size);


}


void get_server_uname_information(int socket,int choice){

        size_t payload_length;// calculate the size(in bytes) that utsname occupies in memory
	struct utsname uts;
     
     writen(socket, (int *) &choice,sizeof(int) );//  send choice to server

    readn(socket, (size_t *) &payload_length, sizeof(size_t));// read number of bytes to be sent by server to client
    readn(socket, (unsigned char *)&uts, payload_length);

     // print out details of received  utsnme structure
      printf("Node name   :       %s\n",  uts.nodename);
      printf("System name :       %s\n",  uts.sysname);
      printf("Release     :       %s\n",  uts.release);
      printf("Version     :       %s\n",  uts.version);
      printf("Machine     :       %s\n",  uts.machine);
      printf("(%zu bytes)\n", payload_length);

     
}

void get_file_names(int socket,int choice){
	const size_t payload_length;// calculate the size(in bytes) that utsname occupies in memory
        char message[1000]="";
        const char delimiter[]="  ";

     writen(socket, (int *) &choice,sizeof(int) );//  send choice to server

    readn(socket, (unsigned int *) &payload_length, sizeof(int));// read number of bytes to be sent by server to client
    readn(socket, (unsigned char *)message, payload_length);

    // use strtok() to split the received message into a series of tokens based on the separator used by the server
    //strtok returns a pointer to the token split up, otherwise a null pointer if the string cannot be split
    
    printf("File names in the server current working directory are listed below :\n\n");

    // extract the first token
     char *token=strtok(message,delimiter);
     //loop through the string to extrct all other tokens
     while(token !=NULL){
    printf("%s \n\n",token);//print token

    token=strtok(NULL,delimiter);
     }
     printf("(%zu bytes)\n", payload_length);

}

int main(void)
{
    // *** this code down to the next "// ***" does not need to be changed except the port number
    int sockfd = 0;
    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	perror("Error - could not create socket");
	exit(EXIT_FAILURE);
    }

    //memset(&serv_addr, '0', sizeof(serv_addr));// clean buffer

    serv_addr.sin_family = AF_INET;

    // IP address and port of server we want to connect to
    serv_addr.sin_port = htons(50031);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // try to connect...
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)  {
	perror("Error - connect failed");
	exit(1);
    } else
       printf("Connected to server...\n");

    // ***
    // your own application code will go here and replace what is below... 
    // i.e. your menu etc.
    //
    //
    //
  

//      get_server_time(sockfd);
      
    
      int choice;

	do {
       
        printf("\n 1. Get and display your name and student ID\n");
	printf(" 2. Get and display an array of 5 random numbers\n");
	printf(" 3. Get and display the uname information of the server\n");
	printf(" 4. Get and display a list of file names\n");
	printf(" 5. File copy enhancement\n");
	printf(" 6. Quit\n\n");

	scanf("%d",&choice);

         switch(choice){
		case 1 :
			 get_student_name_and_student_id(sockfd,choice);
		    break;
	        case 2 :
		     get_random_numbers(sockfd,choice);
		     
		    break;
                case 3 :
		   get_server_uname_information(sockfd,choice);
		   
		    break;
                case 4 :
		  get_file_names(sockfd,choice);
		    break;
                case 5 :
		   get_file_copy_enhancement(sockfd,choice);               
		    break;
                case 6 :
                 quit_client(sockfd,choice);
		    break;
         	default :
		
	       printf("\n\nInvalid input.Try again,you entered %d. Correct option is between 1 and 6\n\n",choice);
            
	     }

	}while(choice !=6); 
   


	


// get a string from the server
    //get_hello(sockfd);

   
    // send and receive a changed struct to/from the server
   // employee *employee1;		
    //employee1 = (employee *) malloc(sizeof(employee));

   /* 
    // arbitrary values
    employee1->age = 23;
    employee1->id_number = 3;
    employee1->salary = 13000.21;

    int i;
    for (i = 0; i < 5; i++) {
         printf("(Counter: %d)\n", i);
	 send_and_get_employee(sockfd, employee1);
         printf("\n");
    }

   */ 

   // free(employee1);

    // *** make sure sockets are cleaned up


printf("Closing client socket\n");

   shutdown(sockfd,SHUT_RDWR);

    close(sockfd);

    exit(EXIT_SUCCESS);

} // end main()
