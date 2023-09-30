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
#define SINGLE_CHAR_STR_LEN 2
#define REVERSE_TRAVERSAL 1
#define REGULAR_TRAVERSAL 0
#define CONTINUE 0
#define STOP 1

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
} automaton_t;

/* FUNCTION DECLARATIONS -----------------------------------------------------*/

char *read_until_blank(char *input, int *input_len);
char* read_statement(char *str, int *state_len);
void init_state(state_t *new, int *num_states);
void insert_statement(automaton_t *model, char *statement, int statement_len, int *num_states);
list_t* insert_at_tail(list_t *list, char *str, state_t *next_state);
int free_state(state_t *curr_state);
node_t *greatest_output(node_t *current_node);
void make_prediction(automaton_t *model, char *prompt, int prompt_len);
int rec_traverse(state_t *curr_state, int (*action)(state_t*), int reverse, int stop);
int is_compressable(state_t *x);
void perform_compression(automaton_t *model);

/* USEFUL FUNCTIONS ----------------------------------------------------------*/
int mygetchar(void);            // getchar() that skips carriage returns

/* WHERE IT ALL HAPPENS ------------------------------------------------------*/
int
main(int argc, char *argv[]) {
    
    // Define and initialise automaton
    automaton_t model;
    state_t initial_state;
    int num_states = 0;
    init_state(&initial_state, &num_states);
    model.ini = &initial_state;

    /*============================= STAGE 0 ==================================*/
    // Assign initial string for current statement
    int statement_len;
    int num_statements = 0, num_chars = 0;
    char *input;

    // Read statement from user into `input`
    input = (char*)malloc(sizeof(char));
    assert(input!=NULL);
    input = read_statement(input, &statement_len);
    
    // Read statements until blank line is read
    while(statement_len > 0) {

        // Add training statement to `model`
        num_statements++;
        num_chars += statement_len;
        insert_statement(&model, input, statement_len, &num_states);

        // Free previous input pointer and read from user again
        free(input);
        input = (char*)malloc(sizeof(char));
        assert(input!=NULL);
        input = read_statement(input, &statement_len);
        
    }
    // Destroy input pointer if it's not to be reassigned.
    if(input!=NULL) {
        free(input);
    }
    input = NULL;
    // Training complete, print model details
    printf(SDELIM, 0);
    printf(NOSFMT, num_statements);
    printf(NOCFMT, num_chars);
    printf(NPSFMT, num_states);
    /*=========================== END STAGE 0 ================================*/
    
    /*============================= STAGE 1 ==================================*/
    printf(SDELIM, 1);
    // Read statement from user into `input`
    input = (char*)malloc(sizeof(char));
    assert(input!=NULL);
    input = read_statement(input, &statement_len);

    // Read statements until blank line is read
    while(statement_len > 0) {

        // Make prediction using model
        make_prediction(&model, input, statement_len);

        // Free previous input pointer and read from user again
        free(input);
        input = (char*)malloc(sizeof(char));
        assert(input!=NULL);
        input = read_statement(input, &statement_len);
        
    }
    // Destroy input pointer if it's not to be reassigned.
    if(input!=NULL) {
        free(input);
    }
    input = NULL;
    /*=========================== END STAGE 1 ================================*/

    /*============================= STAGE 2 ==================================*/
    printf(SDELIM, 2);
    // Read number of compression steps to perform
    int comp_steps;
    scanf("%d", &comp_steps);
    // Remove '\n' from stdin input buffer
    getchar();
    // Perform compression steps
    for(int i=0; i<comp_steps; i++) {
        perform_compression(&model);
    }
    // Read statement from user into `input`
    input = (char*)malloc(sizeof(char));
    assert(input!=NULL);
    input = read_statement(input, &statement_len);

    // Read statements until blank line is read
    while(statement_len > 0) {

        // Make prediction using model
        make_prediction(&model, input, statement_len);

        // Free previous input pointer and read from user again
        free(input);
        input = (char*)malloc(sizeof(char));
        assert(input!=NULL);
        input = read_statement(input, &statement_len);
    }
    // Destroy input pointer if it's not to be reassigned.
    if(input!=NULL && statement_len > 0) {
        printf("Statement length: %d\n", statement_len);
        free(input);
    }
    input = NULL;
    /*=========================== END STAGE 2 ================================*/
    printf("Freeing model\n");

    // Free model and exit :)
    rec_traverse(model.ini, free_state, REVERSE_TRAVERSAL, CONTINUE);
    printf("Done freeing model\n");

    return EXIT_SUCCESS;        // algorithms are fun!!!
}

/* Initialises an empty automata state
   Parameters: `new` state_t pointer to new state
   Returns: void
*/
void
init_state(state_t *new, int *num_states) {
    static int id=0;
    *num_states += 1;
    new->freq = 0;
    new->id = id;
    new->visited = 0;
    new->outputs = NULL;
    
    id++;
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
    while((curr_char = mygetchar()) != '\n' && curr_char != EOF) {
        
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
    // Null-terminate input
    if(buffer_len <= curr_statement_len) {
        buffer_len++;
        str = (char*)realloc(str, sizeof(char) * buffer_len);
    }
    str[curr_statement_len] = '\0';
    
    // Update `statement_length` and return pointer to string
    *state_len = curr_statement_len;
    return str;
}

/* Takes a training statement and inserts it into the automaton, creating states
   where necessary and incrementing frequencies on already existent states.
   Parameters: automaton_t* `model` pointer to the model to insert to
               char* `statement` the character array statement to be inserted
               int `statement_len` the length of the statement to be added

   Returns: void
*/
void
insert_statement(automaton_t *model, char *statement, int statement_len, int *num_states) {
    
    // Initialise current state being checked
    state_t *curr_state = model->ini, *next_state;
    
    //printf("\nInserting new statement!\n");
    
    list_t *output_list;
    node_t *output_node;
    char *new_transition_str;
    char comp_str[SINGLE_CHAR_STR_LEN];
    int match_found;

    comp_str[1] = '\0';
    // Iterate through characters in the input
    for(int i=0; i<statement_len; i++) {
        curr_state->freq++;
        comp_str[0] = statement[i];
        match_found = 0;
        // Check if there are any outputs from the current state
        if(curr_state->outputs==NULL) {
            //printf("No outputs from state[%d], creating one with id=%d\n", curr_state->id, curr_state->id + 1);
            // No outputs, we must create one
            // Malloc list
            output_list = (list_t*)malloc(sizeof(list_t));
            assert(output_list!=NULL);
            output_list->head = output_list->tail = NULL;
            // Initialise next state
            next_state = (state_t*)malloc(sizeof(state_t));
            assert(next_state!=NULL);
            init_state(next_state, num_states);
            
            // Initialise transition string
            new_transition_str = (char*)malloc(sizeof(char) * SINGLE_CHAR_STR_LEN);
            assert(new_transition_str!=NULL);
            new_transition_str[0] = statement[i];
            new_transition_str[1] = '\0';
            
            
            output_list = insert_at_tail(output_list, new_transition_str, next_state);
            curr_state->outputs = output_list;
            
            // Now traverse to the newly created output
            curr_state = curr_state->outputs->head->state;
        } else {
            
            // There are outputs, check if any strs match current char
            // Traverse list of outputs
            output_node = curr_state->outputs->head;
            while(output_node!=NULL) {
                //printf("Checking statement character '%c' against output with char '%c'\n", statement[i], *(output_node->str));
                // Matching character found, go to that state
                if(!strcmp(comp_str, output_node->str)) {
                    //printf("Character '%c' matches an output from state[%d], traversing to state[%d]\n", statement[i], curr_state->id, output_node->state->id);
                    curr_state = output_node->state;
                    match_found = 1;
                    break;
                } else {
                    // Current out doesn't match, attempt going to next output
                    //printf("    Doesn't match, checking arc at %p\n", output_node->next);
                    output_node = output_node->next;
                }
            }
            // No matches found at all - add a new one for the character
            if(!match_found) {
                //printf("No matches found for '%c' in outputs from state[%d], creating a new output arc at ", statement[i], curr_state->id);
                // Initialise next state
                next_state = (state_t*)malloc(sizeof(state_t));
                assert(next_state!=NULL);
                init_state(next_state, num_states);
                // Initialise transition str
                new_transition_str = (char*)malloc(sizeof(char) * SINGLE_CHAR_STR_LEN);
                assert(new_transition_str!=NULL);
                new_transition_str[0] = statement[i];
                new_transition_str[1] = '\0';
                // Add new state to outputs of current state
                curr_state->outputs = insert_at_tail(curr_state->outputs, new_transition_str, next_state);
                //printf("%p\n", curr_state->outputs->tail);
                // Go to newly created state
                curr_state = curr_state->outputs->tail->state;
            }
        }
    }
}

/* Inserts a node with corresponding output state to a list
   Parameters: list_ t* `list` pointer to the list to add to
               char* `str` the transition string for the next state
               state_t* `next_state` pointer to the next state
    Returns: pointer to the list
*/
list_t*
insert_at_tail(list_t *list, char *str, state_t *next_state) {
	node_t *new;
	new = (node_t*)malloc(sizeof(node_t));
	assert(list!=NULL && new!=NULL);

	new->str = str;
    new->state = next_state;
	new->next = NULL;

	if (list->tail==NULL) {
		/* this is the first insertion into the list */
		list->head = list->tail = new;
	} else {
		list->tail->next = new;
		list->tail = new;
	}
	return list;
}

node_t*
greatest_output(node_t *current_node) {
    // Initialise chosen output
    node_t *chosen_output = current_node;
    int highest_freq = chosen_output->state->freq;

    while(current_node!=NULL) {
        
        //printf("Comparing current:%s and chosen:%s\n", current_node->str, chosen_output->str);
        if(current_node->state->freq > highest_freq) {
            // Greater frequency found

            highest_freq = current_node->state->freq;
            chosen_output = current_node;


        } else if (current_node->state->freq == highest_freq) {
            // Equal frequncy found, pick the 'greater' output
            
            if(strcmp(current_node->str, chosen_output->str) > 0) {
                highest_freq = current_node->state->freq;
                chosen_output = current_node;
            }
        }
        // Go to next output
        current_node = current_node->next;
    }
    return chosen_output;
}


void
make_prediction(automaton_t *model, char *prompt, int prompt_len) {

    // Define initial variables
    state_t *curr_state = model->ini;
    node_t *curr_output;
    int output_found, increment_len = 1, split_print=0;
    char comp_str[SINGLE_CHAR_STR_LEN];
    comp_str[1] = '\0';


    // Traverse the model to map out prompt
    for(int i=0; i<prompt_len; i+=increment_len) {
        increment_len=1;
        // Handle reaching end of model - cancel generation
        if(curr_state->outputs == NULL) {
            printf("...\n");
            return;
        }

        // Handle reaching end of prompt before the end of the model
        output_found = 0;
        curr_output = curr_state->outputs->head;
        while(curr_output!=NULL) {
            
            //printf("Checking if %s goes into %s\n", curr_output->str, prompt + i);
            // Check if transition string goes into the correct spot in prompt
            if(strstr(prompt + i, curr_output->str) == prompt + i) {
                
                printf("%s", curr_output->str);
                // Increment pointer into prompt by transition string length
                increment_len = strlen(curr_output->str);
                output_found = 1;
                // Go to chosen automaton state
                curr_state = curr_output->state;
                break;
            } else if(strstr(curr_output->str, prompt + i) ==  curr_output->str) {
                /* Handle the prompt ending during the transition string,
                   we want to start generation mid-way through prompt */
                
                curr_output = greatest_output(curr_output);
                printf("%s...%s", prompt + i, curr_output->str + prompt_len - i);
                
                curr_state = curr_output->state;
                // If the end of the model was reached - we can't generate more
                if(curr_state==NULL || curr_state->outputs==NULL) {
                    
                    putchar('\n');
                    return;
                }
                split_print = 1;
                output_found = 1;
                break;
            }
            curr_output = curr_output->next;
        }

        if(!output_found) {
            // The prompt deviates from the automaton - cancel generation
            //printf("NO OUTPUTS...\n");
            return;
        }
    }

    if(!split_print) {
        printf("...");
    }
    // We are now at a state corresponding to the final character of the prompt
    int highest_freq;
    node_t *chosen_output;
    
    // Now we can generate the output based on the prediction
    while(curr_state->outputs!=NULL) {
	    highest_freq = 0;
        // Find the output state with the highest frequency
        if(curr_state->outputs!=NULL) {
            chosen_output = curr_output = curr_state->outputs->head;
        }
        chosen_output = greatest_output(curr_output);
        if(chosen_output == NULL) {return;}
        // Now we have chosen the state we want to traverse to
        curr_state = chosen_output->state;
        printf("%s", chosen_output->str);
        //printf("Chose output state[%d]\n", curr_state->id);
        //printf("Does state[%d] have any outputs? %s\n", curr_state->id, curr_state->outputs!=NULL ? "yes" : "no");
    }
    putchar('\n');
}

/* Polymorphic recursive traversal function that calls an action on every state
   Parameters: `curr_state` pointer to the current state
               `action` function pointer to action to perform (state_t -> int)
               `reverse` flag to control whether traversal or action comes 1st
               `stop` flag to signal early termination
   Returns: 0 if traversal completed, 1 if terminated early
*/
int
rec_traverse(state_t *curr_state, int (*action)(state_t*), int reverse, int stop) {
    
    // Base case - no more outputs
    if(curr_state==NULL || stop) {
        return stop;
    }
    // Recursive case - Traverse to all output states
    
    // If it's not in reverse traversal order, call action now
    if(!reverse) {
        stop = action(curr_state);
    }
    if(curr_state->outputs != NULL) {
        node_t *current_node = curr_state->outputs->head;
        while(current_node!=NULL) {

            if(current_node->state!=NULL) {
                stop = rec_traverse(current_node->state, action, reverse, stop);
                if(stop) {return stop;}
            }
            current_node = current_node->next;   
        }
    }
    // Call action on current state for reverse traversal order
    if(reverse) {
        stop = action(curr_state);
    }
    
}


/* Frees all dynamic memory associated with a single state
   Parameters: `curr_state` pointer to the state to be freed
   Returns: CONTINUE flag, to keep recursivly freeing the automaton
*/
int
free_state(state_t *curr_state) {
    printf("Freeing state[%d]\n", curr_state->id);
    if(curr_state==NULL) {return CONTINUE;}
    // Free all outputs
    if(curr_state->outputs!=NULL){
        node_t *curr_node = curr_state->outputs->head;
        while(curr_node!=NULL) {
            free(curr_node->str);
            free(curr_node);
            curr_node = curr_node->next;
        }
        
        // Finally, free the current state
        free(curr_state->outputs);
    }
    if(curr_state->id!=0) {
        free(curr_state);
    }
    return CONTINUE;
}

/* Checks whether the compression condition is statisfied from a given state
   Parameters: `x` the first state to check from (corresponds to x in the rules)
   Returns: 1 if compressable, 0 if not.
*/
int
is_compressable(state_t *x) {
    // If `x` state has more than 1 output, it's not compressable
    if(x->outputs->head != x->outputs->tail) {
        return 0;
    }
    // If 'y' has no output states, it's not compressable
    state_t *y = x->outputs->head->state;
    printf("Checking compression condition at x[%d] y[%d]\n", x->id, y->id);
    if(y == NULL || y->outputs == NULL) {
        return 0;
    }
    return 1;
}



/* Performs a single compression pass on the automaton 
   Parameters: automaton_t* `model`: pointer to the automaton
   Returns: void
*/
void
perform_compression(automaton_t *model) {

    state_t *curr_state, *next_state;
    node_t *chosen_output, *curr_output;
    int condition_met=0, output_strlen, curr_len, compressable;
    // Traverse through the model (depth-first lowest ASCII value)
    next_state = curr_state = model->ini;

    while(next_state!=NULL) {

        if(next_state->outputs!=NULL) {
            curr_output = next_state->outputs->head;
            chosen_output = curr_output;
            /* Scan through outputs to find 'lowest' value that lead to possible
               compressions.
            */
            while (curr_output) {
                
                compressable = rec_traverse(next_state, 
                    is_compressable, REGULAR_TRAVERSAL, CONTINUE);
                
                if(strcmp(curr_output->str, chosen_output->str) < 0 && compressable) {
                    chosen_output = curr_output;
                }
                curr_output = curr_output->next;
            }
            
        } else {
            break;
        }
        curr_state = next_state;
        next_state = chosen_output->state;
        // We now have the smallest labelled output from the previous state
        
        // If the state doesn't lead to any possible compressions
        if(!compressable) {
            continue;
        }


        
        // Check if compression condition is met
        if(!(next_state->visited) && curr_state->outputs->head == curr_state->outputs->tail && next_state->outputs!=NULL) {
            condition_met = 1;
            
            
            // Concatenate the output strings of `curr_state` to `next_state`
            curr_output = next_state->outputs->head;
            output_strlen = strlen(curr_state->outputs->head->str);
            
            
            while(curr_output!=NULL) {
                // Increase length of current output str by `outout_strlen`
                curr_len = strlen(curr_output->str) + output_strlen;
                
                curr_output->str = (char*)realloc(curr_output->str, 
                    curr_len + 1);
                
                // Copy current string to end of buffer
                memmove(curr_output->str+output_strlen, curr_output->str, 
                    curr_len);
                curr_output->str[curr_len] = '\0';

                // Prepend previous state's output str
                memmove(curr_output->str, curr_state->outputs->head->str,
                    output_strlen);
                
                // Go to next output
                curr_output = curr_output->next;
            }
            
            // Free the outputs of `curr_state`
            free(curr_state->outputs->head->str);
            free(curr_state->outputs);
            // Link the updated outputs to 'curr_state'
            curr_state->outputs = next_state->outputs;
            // Free `next_state`
            free(next_state);

            break;
        }
    }
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
