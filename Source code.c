/*
 * project2 : Reading data from a file and displaying it on a plot
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct file{					// A user defined variable to store the file names given in a linked list
	
	char filename[48];				// Memory = 48 bytes 				
	struct file *near;				// Memory = 8 bytes
}file_t;						// Total memory for the struct = 56 bytes

typedef struct node{					// A user defined variable to store the information in a file depending on user inputs (-m, -p or -t)

	char name[48];					// Memory = 48 byte		A variable to store the names
	int add;					// Memory = 4 bytes		A variable to to store the sum of the values under the parameter entetred (-m, -p or -t)
	struct node *near;				// Memory = 8 bytes		A variable of type struct node pointer to keep the address of the next node, whose host name starts with the same letter
	struct node *next;				// Memory = 8 bytes		A variable of type struct node pointer to keep the address of the next node entered according the order in the file 
}node_t;						// Total memory for the struct = 72 bytes (for memory allignment)

// Function prototypes
					
int stringToIntPart(char []);					// A function to convert a string with numbers only, into an integer
int stringToIntTime(char []);					// A function to convert time with numbers and special characters (Ex- ":", ".", etc), into an integer of minutes
void bubbleSort();					// A function to sort a dynamic array
void plot(int l);					// A function to plot the graph
int sum();						// A function to add the elements of an array together

char *r, *tp ;							// Declare to global variables of type char pointer to store the user parameters entered (*r for the number of rows to display and *tp for the type, whether -m, -p or -t)							
FILE *fp = NULL;						// Declare globally, a file pointer to be used in the linked list storing the file names
int scale;							// A variable of type int, to determine whether the graph has to be scaled or not
node_t *inputs;							// A variable of type node_t pointer to point at the head node of the file information linked list

int main(int argc , char** argv){				// The main starts here. The main accepts command line arguments

	char *fl;						// Declare variables of type char pointer, to identify what command line arguments must be followed in the execution of the program

	node_t  *temp, *trav;					// Declare node pointers to read the content in the files into linked lists
	file_t *ref , *tempf , *travf;				// Declare node pointers to read the names of all files entered in the command line into a linked list
	ref = (file_t*)malloc(sizeof(file_t));			// Create the head node of the linked list keeping file names. Adding new nodes to the tail will be followed.
        ref->near = NULL;					
	travf = ref;
	int z1 = 0, z2 = 0, z3 = 0;                             // Variables of type int to check for errors in the file entries
        int repeat, index;  					// Variables of type int used when looping through the linked list to look for nodes with the same name

	int longest = 0;    	                            	// A variable to store the length of the longest name to be displayed on the gra
	char buff[50];						// A string variable to read the content of the files.
        
	// Variables needed to keep track of the number of fields and rows in the file
	int tot_row = 0;
        int field_count = 0;
	
	// Handling command line arguments	
	// If no arguments other tha the name of the file
	if (argc ==1){
		printf("No input files were given\nusage: %s [-l length] [-m | -t | -p] [--scaled] filename1 filename2 ..\n", argv[0]);
		return 0;
	// If additional arguments are given
	}else {
		// Variables used to detect errors
		int flag = 0, count1 = 0;		// To detect invalid options for "-l"				
		int check1 = 0 , check2 = 0 ;		// To detect wrong file types and no file entries at all
		int check3 = 0, check5 = 0;		// For errors related to -m, -p and -t (multiple plots)	
		int check4 = 0, try4 = 0;		// Detecting wrong file types
		int try = 0;				// To handle file entry related errors
		int checkfile = 0;	
		int rows =0 , type = 0 , file;		// To determine how many rows to print in the plot. To determine whether to display -m, -p or -t. To determine the files that need to be opened

		// A for loop to run through all command line arguments entered and indentify errors and store the file names in a linked list
		for (int i = 1 ; i < argc ; ++i){
			
			// Variables to access nearby arguments
			int j = i+1;
			char* s = argv[j];
			char* str = argv[i];
			
			// "-l" related errors
			if (strcmp (argv[i] , "-l") == 0){
				if (argc == j){ 
					printf("Not enough options for [-l]\nusage: %s [-l length] [-m | -t | -p] [--scaled] filename1 filename2 ..\n", argv[0]);
					return 0;
				} else if(strcmp (argv[j] , "-l") == 0){
					printf("Invalid options for [-l]\nusage: %s [-l length] [-m | -t | -p] [--scaled] filename1 filename2 ..\n", argv[0]);
					return 0;	
				}

				count1 =0;			
				flag = 0;
				int flag1 = 0;
				int try0 = 0, try1 = 0;
				for (int k = 0 ; s[k] != '\0'; ++k){
					count1 = count1 + 2;
					
					// For "-l (zero)"
					if (s[k] >=48 && s[k] <= 57){
						
						flag = flag+2;
						if (s[k] == 48){
							try0 = try0 +2;	
						}
					}
					// For "-l (negative number)"
					if (s[0] == '-'){
						try1 = 1;
					
						if (s[k] >=48 && s[k] <= 57){
							flag1 = flag1 +2;
						}
						
					}
				}
				if (flag1+2 == count1 && try1 == 1){				// Negative number of rows
					printf("Invalid option(negative) for [-l]\nusage: %s [-l length] [-m | -t | -p] [--scaled] filename1 filename2 ..\n", argv[0]);
                                        return 0;
				}
				if (flag == count1 && flag == try0){				// Zero rows
					return 0;
				}
					
				if (flag != count1){						// Argument proceeding -l is not an integer
                   			check2 = 1;
					break;
				}else if (flag == count1) {					// If correct argument proceeds
					i++;							
					rows = i;
					r = argv[rows];						// Update the row count to the latest entered number
				}

			// -t, -m, -p related errors
			}else if(strcmp (argv[i] , "-t") == 0 || strcmp (argv[i] , "-p") == 0 || strcmp (argv[i] , "-m") == 0){	
				check3++;
				type = i;
				tp = argv[type];

				// Multiple plotting parameter testing
				if (check3 == 1){
					for (int k = i+1 ; k <argc ; ++k){
						if ( i != k && (strcmp(argv[i] , argv[k]) == 0)){
							check5++;		
						}
					}
				}
			
			// Invalid flag entries
			}else if (str[0] == '-'){
				int len = 0;
				for (int k = 0 ; str[k] != '\0' ; ++k){
					len++;
				}
				if (str[1] == '-'){			// --scaled related (double flag related)
					scale = i;
					if (len != 8 || (str[2] !='s' || str[3] != 'c' || str[4] != 'a' || str[5] !='l' || str[6]!='e' || str[7] != 'd') ){
										
						printf("Invalid option [%s]\nusage: %s [-l length] [-m | -t | -p] [--scaled] filename1 filename2 ..\n", str, argv[0]);
						return 0;
					}
				}else if (str[1] != 'm' || str[1] != 't' || str[1] != 'p' ){		// Single flag related entries
					printf("Invalid option [%s]\nusage: %s [-l length] [-m | -t | -p] [--scaled] filename1 filename2 ..\n", str, argv[0]);
                                        return 0;
				}
			// Handling invalid file entries
			}else if (str[0] != '-' ){							// If not a flag, nor an integer proceeding -l
				for (int k = 0 ; str[k] != '\0'; ++k){
                                        if (str[k] == '.' && (str[k+1] != 'c' || str[k+2] != 's' || str[k+3] != 'v') && checkfile == 0){
                                                check4 = 1;
						
					}else if (str[k] == '.' && str[k+1] == 'c' && str[k+2] == 's' && str[k+3] == 'v'){
						check4 = 0;
						break;
					}else{
						check4 = 1;
				
					}
                                }
				if (check4 == 1){
					try4 = 1;
					
				}
			}

			if(str[0] != '-'  ){       
  				// No .csv files
				try = 1;
				for (int k = 0 ; str[k] != '\0'; ++k){
                                        if (str[k] == '.' && str[k+1] == 'c' && str[k+2] == 's' && str[k+3] == 'v'){
                                                check1 = 1;
						file = i;
						fl = argv[file];
						fp = fopen(fl, "r");
        					if (!fp && try4 == 0){
                					checkfile = 1;
							break;
       	 					}
						// Store the file names in a linked list
						strcpy(travf->filename , fl);
						tempf = (file_t*)malloc(sizeof(file_t));
						tempf->near = NULL;
						travf->near = tempf;
						travf = tempf;
						break;
                                        }else {
						check1 = 0;
					}
                                }
			}
		

		}
		// Handling erros according to precedence
		if(check2 == 1){
			printf("Invalid options for [-l]\nusage: %s [-l length] [-m | -t | -p] [--scaled] filename1 filename2 ..\n", argv[0]);
			return 0;
		}else if (check3 > 1 && check3 != check5 + 1){
			printf("Cannot plot multiple parameters in same graph.\nusage: %s [-l length] [-m | -t | -p] [--scaled] filename1 filename2 ..\n", argv[0]);
			return 0;
		}else if (checkfile == 1){
			printf("Cannot open one or more given files\n");
                        return 0;
		}else if ((flag == count1 && check1 == 0 && try == 0) || (check3 >=1 && flag == 0 && count1 ==0 && check1 == 0 && check2 == 0 && check4 == 0 && check5 == 0 && try4 == 0)){
			printf("No input files were given\nusage: %s [-l length] [-m | -t | -p] [--scaled] filename1 filename2 ..\n", argv[0]);
			return 0;
		}else if ((flag == count1 && check1 == 0 && try == 1) || (flag != count1 && check1 == 0 && try == 1) || (try4 == 1)){
			printf("Only .csv files should be given as inputs.\n");
			return 0;
		}	
		
		// Store the contents of the files in dynaically created nodes. The list is created to the tail node format
		// Default values for the number of rows and the plotting parameter
                if (rows == 0){
                        r = "10";
                }
                if (type == 0){
                        tp = "-m";
                }
		
		node_t *loop, *keep;									// varaibles of type node_t pointer to loop through the linked list
		node_t *array[27];									// A variable of type array node_t pointer to point towards the head node of the first node whose name starts with the corresponding letter (EX: A or a as [0], B or b as [1])
													// This helps break the linked list into 26 parts, making the comparison of strings faster and more efficient
		for (int i = 0; i <27 ; ++i){								
			array[i] = NULL;								// Initialize the pointers to NULL
		}

		// Loop through to keep creating nodes until all files entered are opened
		for (travf = ref ; travf->near != NULL ; travf = travf->near){
			fp = fopen(travf->filename, "r");						// Open the files to read 						 
			
                        while (fgets(buff, 50, fp)){
                        	field_count = 0;
				repeat = 0;

                                tot_row++;				// To count the total number of rows in all files
                                
				char *field = strtok(buff, ",");	// Read the entries in a file seperated by a comma
				z1 = 0, z2 = 0, z3 = 0;

                                while(field){				// A while loop to go through the three field in the file
                                	if (field_count ==0){
						z1 = 1;
						if(field[0] > 96 && field[0] < 123){			// If the first letter is in lower case, find its upper case value for the ease of comparison 
                                                        index = field[0]-32-65;		// Find the corresponding array index beginning with 0 for A 
                                                }else if (field[0] >=65 && field[0] <=90){					// If in upper case already
                                                	index = field[0]-65;
                                                }else{
							index = 26;
						}

						temp = (node_t*)malloc(sizeof(node_t));		// Create dynamically a node to store the information in the file
						temp->add = 0;
						if (tot_row == 1){				// For the very first entry, create a node without any comparison
							repeat = 1;
							inputs = temp;
							array[index] = temp;
							// Set both address holding variables to NULL
							temp->next = NULL;
							temp->near = NULL;
							
							trav = temp;
							strcpy(trav->name, field);
							if (strcmp(tp , "-m") == 0){		// Add the number of meetings hosted
								trav->add = trav->add + 1;
							}
						}else{						// For enries after the first, compare its host name with the ones in the loop
							repeat = 1;
							if(array[index] == NULL){		// If the index with the number for this letter is not pointing to any node, add the first node to this header
								repeat =2;
								trav->next = temp;		
								temp->next = NULL;
								array[index] = temp;
								array[index]->near = NULL;
								trav=temp;
								if (strcmp(tp , "-m") == 0){	// Add the number of meetings hosted
                                                                       trav->add = trav->add + 1;
                                                                }
								strcpy(trav->name, field);

							}else{					// If there are nodes that exist with host names starting with the same letter

								for (loop = array[index] ; loop != NULL ; loop= loop->near){	// Run through the linked list under the particular array header
									if (strcmp(loop->name , field) == 0){			// If there already exists a node for the host
										repeat =0;
										break;
									
									}
									keep=loop;
								}	
							}
							if (repeat == 1){							// If no node exists for the host
                                                                temp->next = NULL;
								temp->near = NULL;
                                                                trav->next = temp;
								keep->near = temp;
                                                                trav = temp;
                                                                if (strcmp(tp , "-m") == 0){
                                                                       trav->add = trav->add + 1;
                                                                }

								strcpy(trav->name, field);
							}else if (repeat == 0 && strcmp(tp, "-m") ==0 ){			// If there exists a node for the host
								free(temp);
								loop->add = loop->add + 1;
							}
						}
					

                                        }
                                        if (field_count ==1){									// To add the number of particicpants if -p is entered
						z2 = 1;
						if (repeat == 0){
                                                        if (strcmp(tp, "-p" ) == 0){
                                                                loop->add = loop->add + stringToIntPart(field);
                                                        }
                                                }else if (repeat == 1 || repeat == 2 ){
                                                        if (strcmp(tp, "-p") == 0){
                                                                trav->add = stringToIntPart(field);
                                                        }
                                                }

                                        
					}
                                        if (field_count ==2){									// To add the number of minutes if -t is entered
						z3 = 1;
						if (repeat == 0 ){
                                                        if (strcmp(tp, "-t" ) == 0){
                                                                loop->add = loop->add + stringToIntTime(field);
                                                        }
                                                }else if (repeat == 1 || repeat == 2){
                                                        if (strcmp(tp, "-t") == 0){
                                                                trav->add = stringToIntTime(field);
                                                        }
                                                }

                                        	
					}
					
                                        field = strtok(NULL , ",");
                                        field_count++;

                                }
				// Handling errors in the entries of the fies
				if (z3 == 0 || z2 == 0|| z1 == 0){
                                                printf("File/s contain wrong entries.\n");
                                                return 0;
                                }

                          }
                          fclose(fp);			// Close the opened file
		}					// Move to the next file
				
	}						// End of command line argument handling
        

	//********************************************************************************************************************************************************************************************//
		
	bubbleSort();					// Call the function to sort the linked list from maximum to minimum values
	
	trav = inputs;					// Use trav to run through the linked list
	for (int i = 0 ; i < stringToIntPart(r) ; ++i){
                        if (trav == NULL){		
                                break;
                        }
                        if (strlen(trav->name) > longest){
                                longest = strlen(trav->name);
                        }
			trav = trav->next;
        }
	
	printf("\n");					// To position the graph in the right place
	plot(longest);					// Call the function to plot the graph

       	return 0;									// Terminate main() and free() the dynamically allocated arrays									
}


// Function declaration for string to integer convertions.
// Function parameters: The number string needed to be converted
int stringToIntPart(char str[]){
    int i=0,sum=0;
    
    while(str[i]!='\0'){
         if(str[i]< 48 || str[i] > 57){
             return 0;
	 }else{
             sum = sum*10 + (str[i] - 48);
             i++;
         }

    }

    return sum;					// Return the sonverted integer
}

// Function parameter to convert time to integer minutes
// Function parameter: The time in hh:mm:ss format
int stringToIntTime(char str[]){
    int i=0,sum=0;
        int timea[2];
        int try = -1;
    while(str[i]!='\0'){
         if(str[i]< 48 || str[i] > 57){			// When the character is not a number, take the converted integer and make minutes
             try++;
             if (try == 0){				// If hours
                timea[try] = 60*sum;			// Convert to minutes
             }else if (try == 1){			// If minutes
                timea[try] = sum;			// Do not change
             }
             sum = 0;
             i++;
             continue;
         }
         else{						// When the character is a number, convert it to an integer
             sum = sum*10 + (str[i] - 48);
             i++;
         }

    }

    return timea[0] + timea[1];				// Return the sum of the total minutes
}

// Function declaration to sort the dynamic arrays from maximum to minimum
// Function parameters: The dynamic array to be sorted, the size of the array, the address of the dynamic array holding the names 
        
void bubbleSort(){
        int done = 0;
        node_t *temp, *track;
        while(!done){
                done = 1;
		track = NULL;
                for (temp = inputs; temp->next != NULL; temp = temp->next){

                        if((temp->add)<(temp->next->add)){
				
				done = 0;
				
				if(track==NULL){
					inputs = temp->next;
					temp->next = inputs->next;
					inputs->next = temp;
					temp = inputs;
				}else{
					track->next = temp->next;
					temp->next = track->next->next;
					track->next->next = temp;
					temp = track->next;
				}
			}        
			track = temp;
		}
        }
}

// Function to get the number of digits in a number
// Function parameters: The integer number
int digits(int n){

	int count = 0;

	int qut = 1;
	
	for (int i = 0; qut>0 ; ++i){
		count++;
		qut = n/10;
		n = qut;
	}
	return count;				// Return the number of digits in the number
}

// A function to add the numbers of an elements together
// Function parameters: The array whose elements need to be added
int sum(){

	int s = 0;
	node_t *trav;
	for (trav = inputs ; trav != NULL ; trav = trav->next){
		s = s+ trav->add;
		
	}
	return s;				// Return the sum of the elements
}

// Function to plot graphs
// Function Parameters: An integer (length of the longest name, The array with the parameter's respective figures to be shown on the graph, The address to the dynamic student name list)

void plot(int l){

        int sum1;

        sum1 = sum();
        int tab;
        int limit1 , limit2;
	node_t *trav;							// To go through the linked list for the plot
	trav = inputs;							// Start from the header node
        for (int k = 0 ; k < stringToIntPart(r) ; ++k){			// A for loop to run through the main linked list in the order of precedence
                if (trav == NULL){
                        break;
                }
                int printingArea;					// A variable to store the value of the printing area

                for (int row = 1 ; row <=3 ; ++row){
                        tab = strlen(trav->name);

                        if (k == 0 && scale > 0){					// If needs to be scaled and is the very first host name
                                limit1 = 80 - tab - 1 - digits(trav->add);		// The printing limit for the second pattern row
                                printingArea = 80 - l - 2 - 1 - digits(inputs->add);	// The printing areaa of the first entry
                                limit2 = 80 - digits(trav->add);			// The printing limit for the 1st and 3rd pattern rows

                        }else if (k != 0 && scale > 0) {				// If needs to be scaled and is an name but the first

                                limit1 = l - tab + 1 + 1 + ((printingArea )*(float)trav->add/inputs->add);	// The printing limit for the second pattern row
                                limit2 = limit1 + tab + 1;							// The printing limit for the 1st and 3rd pattern rows

                        }else if (scale == 0 ){						// If not scaled

                                limit1 = l + 2 + 1 + ((80 - l - 2 - 1 - digits(inputs->add)) *(float) trav->add /sum1) - tab - 1 ;		// The printing limit for the second pattern row
                                limit2 = limit1 + tab + 1;											// The printing limit for the 1st and 3rd pattern rows
                        }
                        if (row == 2){					// In the second row, print the number and the number
                                printf(" %s", trav->name);		// Print the name
                                tab = strlen(trav->name);		// Check the length of the name

                                for (int col = 1; col <= limit1 + 1 ; ++col){		// A for loop to print the columns
                                        if (col <= l-tab+1){				// Print the initial spaces
                                                printf(" ");
                                        }else if (col == l-tab+2){			
                                                printf("\u2502");			// Print the starring line of the graph
                                        }else if (col == limit1 + 1){
                                                printf("%d", trav->add);		// Print the value		
                                        }else if (col > l - tab + 2)  {	
                                                printf("\u2591");			// Print the pattern
                                        }
                                }
			}else{								// For the 1st and second row of the unicode pattern for one host
                                for (int col = 1 ; col <= limit2  ; ++ col){
                                        if (col == l+3){
                                                printf("\u2502");			// Print the intial line of the plot
                                        }else if (col > l+2 ){
                                                printf("\u2591");			// Print the pattern
                                        }else if (col < l + 3) {
                                                printf(" ");				// Print the spaces
                                        }
                                }
                        }
                        printf("\n");							
                }
                for (int col = 1; col <= l + 3; ++col){					// To print an extra vertical line after the last entry
                        if (col == l + 3){
                                printf("\u2502");
                        }else{
                                printf(" ");
                        }
                }
		trav = trav->next;							// Move to the next node
                printf("\n");
        }
	// To print the bottom line of the graph
        for (int col = 1; col <= 80; ++col){
                if (col == l + 3){
                        printf("\u2514");						// Print the L
                }else if (col < l+3){
                        printf(" ");							// Spaces till you get to the right place
                }else{
                        printf("\u2500");						// Print the horizontal line
                }
        }
        printf("\n");
}

