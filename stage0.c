/* Program to generate text based on the context provided by input prompts.

  Skeleton program written by Artem Polyvyanyy, http://polyvyanyy.com/,
  September 2023, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.
  All included code is (c) Copyright University of Melbourne, 2023.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: Julian Lewis 1444859
  Dated:     20/09/2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* #DEFINE'S -----------------------------------------------------------------*/
#define SDELIM "==STAGE %d============================\n"   // stage delimiter
#define MDELIM "-------------------------------------\n"    // delimiter of -'s
#define THEEND "==THE END============================\n"    // end message
#define NOSFMT "Number of statements: %d\n"                 // no. of statements
#define NOCFMT "Number of characters: %d\n"                 // no. of chars
#define NPSFMT "Number of states: %d\n"                     // no. of states
#define TFQFMT "Total frequency: %d\n"                      // total frequency

#define CRTRNC '\r'                             // carriage return character

/* TYPE DEFINITIONS ----------------------------------------------------------*/
typedef struct state state_t;   // a state in an automaton
typedef struct node  node_t;    // a node in a linked list

struct node {                   // a node in a linked list of transitions has
    char*           str;        // ... a transition string
    state_t*        state;      // ... the state reached via the string, and
    node_t*         next;       // ... a link to the next node in the list.
};

typedef struct {                // a linked list consists of
    node_t*         head;       // ... a pointer to the first node and
    node_t*         tail;       // ... a pointer to the last node in the list.
} list_t;

struct state {                  // a state in an automaton is characterized by
    unsigned int    id;         // ... an identifier,
    unsigned int    freq;       // ... frequency of traversal,
    int             visited;    // ... visited status flag, and
    list_t*         outputs;    // ... a list of output states.
};

typedef struct {                // an automaton consists of
    state_t*        ini;        // ... the initial state, and
    unsigned int    nid;        // ... the identifier of the next new state.
} automaton_t;

/* USEFUL FUNCTIONS ----------------------------------------------------------*/
int mygetchar(void);            // getchar() that skips carriage returns

/* FUNCTION DECLARATIONS -----------------------------------------------------*/

char* read_statement(char *str, int *state_len);
void insert_statement(automaton_t *model, char *statement, int statement_len);
list_t* create_list(char* str, int str_len);
void free_list(list_t *list);

/* WHERE IT ALL HAPPENS ------------------------------------------------------*/
int
main(int argc, char *argv[]) {
    
    // Initialise model
    automaton_t model;
    state_t *init_state = (state_t*)malloc(sizeof(state_t));
    init_state->freq = 0;
    init_state->id=0;
    init_state->visited=0;
    model.ini = init_state;

    /*============================= STAGE 0 ==================================*/

    // Assign initial string for current statement
    char curr_char;
    int statement_len = 1;
    char *input;

    // Read statements until blank line is read
    while(statement_len > 0) {
        // Read statement from user into `input`
        input = (char*)malloc(sizeof(char));
        input = read_statement(input, &statement_len);

        // Add statement into automaton `model`
        
        // Free curr_statement pointer
        free(input);
    }
    // Destroy input pointer if it's not to be reassigned.
    input = NULL;
    /*=========================== END STAGE 0 ================================*/
    



    return EXIT_SUCCESS;        // algorithms are fun!!!
}

/* Reads from stdin character by character to dynamically sized character array
   Parameters: `str` - char* to heap memory
   Returns:    `str` - char* to (potentially different) heap memory addr
*/
char*
read_statement(char *str, int *state_len) {

    char curr_char;
    int curr_statement_len = 0, buffer_len = 1;
    // Read input statements from user
    while((curr_char = mygetchar()) != '\n') {
        
        // If the end of the buffer is reached, reallocate memory
        if(curr_statement_len >= buffer_len) {
            // Double `buffer_len` to maintain linear time-complexity
            buffer_len *= 2;
            str = (char*)realloc(str, sizeof(char) * buffer_len);
        }

        // Now we know the next character will fit in the buffer, write to it.
        str[curr_statement_len] = curr_char;

        // Increment length
        curr_statement_len++;
    }
    // Update `statement_length` and return pointer to string
    *state_len = curr_statement_len;
    return str;
}

/* Creates a dynamic linked list for a given string - attributing an automata
   state to each node in the list.
   Params: `str` string to be turned into a list
           `str_len' the length of the string
*/
list_t*
create_list(char* str, int str_len) {
    static int next_id=1;
    // Allocate memory for a list
    list_t* list = (list_t*)malloc(sizeof(list_t));
    state_t *new_state;
    node_t *new_node;
    // Error allocating list
    assert(list != NULL);

    int i;
    // Iterate through each char of the input string and 'insert at foot'
    for(i=0; i<str_len; i++) {
        new_state = (state_t*)malloc(sizeof(state_t));
        new_node = (node_t*)malloc(sizeof(node_t));
        // Initialise values of new node
        new_node->state;
        new_node->next = NULL;
        //new_node->str = 
        // Initialise values of state reached by node
        new_state->freq = 0;
        new_state->id = next_id;
        new_state->visited = 0;
        new_state->outputs = NULL;

        new_node->state = new_state;

        next_id++;

        if(list->head == NULL) {
            // Append first node
            list->head = list->tail = new_node;
        } else {
            list->tail->next = new_node;
            list->tail = new_node;
        }
    }

    return list;
}

/* Frees all allocated memory in a linked list (based on listops implementation)
*/
void
free_list(list_t *list) {
    node_t *curr, *prev;
	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}
	free(list);
}

/* Takes a training statement and inserts it into the automaton, creating states
   where necessary and incrementing frequencies on already existent states.
   Parameters: `automaton_t *model` pointer to the model to insert to
               `char *statement` the string statement to be inserted
   Returns: void
*/
void
insert_statement(automaton_t *model, char *statement, int statement_len) {
    
    
    
}

/* USEFUL FUNCTIONS ----------------------------------------------------------*/

/* An improved version of getchar(); skips carriage return characters.
   NB: Adapted version of the mygetchar() function by Alistair Moffat 
*/ 
int
mygetchar() {
    int c;
    while ((c=getchar())==CRTRNC);
    return c;
}

/* THE END -------------------------------------------------------------------*/
