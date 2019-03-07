#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
	      A, B, C, D, E, F, G, H, I, J, K, L,
	      IP,
	      NUM_OF_REGISTERS
} Registers;

typedef enum { INTEGER, STRING } ContentType;

typedef enum {
	      SET,
	      SETI,
	      SETS,
	      MOV,
	      LOG,
	      HLT
} InstructionSet;

/*

SET A 0
SET A "asdf asdf"

MOV A B

LOG A

HLT
 */

union Content {
    unsigned int integer;
    char *string;
};

typedef struct {
    union Content value;
    ContentType type;
} argument;

typedef struct {
    int instr;
    int argc;
    argument * argv;
} instruction;
void print_instruction (instruction instr) {
    printf("instr %i", instr.instr);
    for (int i = 0; i < instr.argc; ++i) {
	printf(" arg #%i: ", i);
	argument arg = instr.argv[i];
	if (arg.type == INTEGER) {
	    printf("%i", arg.value.integer);
	} else {
	    printf("%s", arg.value.string);
	}
    }
    printf("\n");
}

typedef struct {
    union Content content;
    ContentType type;
} reg;

typedef struct {
    bool running;
    reg registers[NUM_OF_REGISTERS];
    instruction * code;
    int size;
} ghost;
ghost * ghost_new(instruction code[], int size) {
    ghost * machine;
    if (!size)
	return NULL;

    machine = malloc(sizeof(ghost));
    memset(machine, '\0', sizeof(ghost));
    machine->code = malloc(size * sizeof(instruction));
    machine->code = code;
    machine->size = size;
    machine->running = true;
    for (int i = 0; i < NUM_OF_REGISTERS; i++) {
	machine->registers[i].type = INTEGER;
	machine->registers[i].content.integer = 0;
	machine->registers[i].content.string = NULL;
    }
    return machine;
};
void ghost_execute_next_instruction(ghost * machine) {
    int ip = machine->registers[IP].content.integer;
    instruction next_instruction = machine->code[ip];
    // print_instruction(next_instruction);

    if (ip >= machine->size) {
	printf("non existent instruction pointer\n");
	machine->running = false;
	return;
    }

    switch (next_instruction.instr) {
    case HLT:
	machine->running = false;
	break;

    case SET: {
	int register_number = next_instruction.argv[0].value.integer;

	int type = next_instruction.argv[1].type;
	machine->registers[register_number].type = type;

	union Content value = next_instruction.argv[1].value;
	machine->registers[register_number].content = value;
	break;
    }

    case MOV: {
	int from = next_instruction.argv[0].value.integer;
	int to = next_instruction.argv[1].value.integer;
	machine->registers[to].content = machine->registers[from].content;
	machine->registers[to].type = machine->registers[from].type;
	break;
    }

    case LOG: {
	int register_number = next_instruction.argv[0].value.integer;
	reg reg_content = machine->registers[register_number];
	printf("register %i has type ", register_number);
	switch (reg_content.type) {
	case INTEGER:
	    printf("INTEGER and value %i\n", reg_content.content.integer);
	    break;
	case STRING:
	    printf("STRING and value %s\n", reg_content.content.string);
	    break;
	}
    }
	
    }

    ++machine->registers[IP].content.integer;
}

/*
int mai () {
    union Content c = (union Content)
	{
	 .integer = A
	};
    union Content c2 = (union Content)
	{
	 .string = "Moi"
	};

    //argument a = (argument)
    //	{
    //	 .value = c2,
    //	 .type = STRING
    //	};
    argument a2 = (argument)
	{
	 .value = c,
	 .type = INTEGER
	};
    argument a3 = (argument)
	{
	 .value = {234},
	 .type = INTEGER
	};
    
    argument args[] = {a2, a3};
    instruction i = (instruction)
	{
	 .instr = SET,
	 .argc = 2,
	 .argv = args
	};
    //    print_instruction(i);

    argument lo = (argument)
	{
	 .value = c,
	 .type = INTEGER
	};

    argument logargs[] = {lo};
    instruction l = (instruction)
	{
	 .instr = LOG,
	 .argc = 1,
	 .argv = logargs
	};

    instruction hlt = (instruction)
	{
	 .instr = HLT
	};

    // TODO: construct this dinamically

    instruction is[] = {i, l, hlt};
    ghost * g = ghost_new(is, 3);
    printf(g->running ? "yes" : "no");
    printf("\nsize: %i\n", g->size);

    while (g->running) {
	ghost_execute_next_instruction(g);
    }

    return 0;
}
    */
int read_line(FILE *in, char *buffer, size_t max)
{
  return fgets(buffer, max, in) == buffer;
}

argument * extract_two_integer_arguments (char * arguments) {
    int first, second;
    sscanf(arguments, "%i %i", &first, &second);

    argument first_arg = (argument)
	{
	 .value = (union Content) {
				   .integer = first
				   },
	 .type = INTEGER
	};

    argument second_arg = (argument)
	{
	 .value = (union Content) {
				   .integer = second
				   },
	 .type = INTEGER
	};

    return (argument[]) {first_arg, second_arg};
}

argument * extract_one_integer_and_one_string_arguments (char * arguments) {
    int first;
    char second[128];
    sscanf(arguments, "%i", &first);
    sscanf(arguments, "%*[^\"]\"%[^\"]\"", second);

    argument first_arg = (argument)
	{
	 .value = (union Content) {
				   .integer = first
				   },
	 .type = INTEGER
	};

    printf("Received string argument \"%s\"...\n", second);
    argument second_arg = (argument)
	{
	 .value = (union Content) {
				   .string = second
				   },
	 .type = STRING
	};

    return (argument[]) {first_arg, second_arg};
}

int main () {
    FILE* fp = fopen("foo.txt", "r");

    int command;
    char arguments[256];
    instruction * instructions;
    int n_of_instructions = 0;

    while (fscanf(fp, "%i %[^\n]", &command, arguments) != EOF) {
	printf("commmand >> %i %i \n", command, n_of_instructions);
	instructions = malloc((n_of_instructions+1) * sizeof(instruction));
	switch (command) {
	case SETI: {
	    instruction instr = (instruction)
		{
		 .instr = SET,
		 .argc = 2,
		 .argv = extract_two_integer_arguments(arguments)
		};
	    instructions[n_of_instructions] = instr;
	    print_instruction(instr);

	    break;
	}

	case SETS: {
	    //	    printf("%s", extract_one_integer_and_one_string_arguments(arguments)[1].value.string);
	    instruction instr = (instruction)
		{
		 .instr = SET,
		 .argc = 2,
		 .argv = extract_one_integer_and_one_string_arguments(arguments)
		};
	    instructions[n_of_instructions] = instr;
	    print_instruction(instr);

	    break;
	}


	case MOV: {
	    instruction instr = (instruction)
		{
		 .instr = MOV,
		 .argc = 2,
		 .argv = extract_two_integer_arguments(arguments)
		};
	    instructions[n_of_instructions] = instr;
	    print_instruction(instr);

	    break;
	}

	}
	++n_of_instructions;
    }
    print_instruction(instructions[0]);
    /*
    char sentence []="echo \"foobar\"|cat";
    char str [20];
    sscanf (sentence,"%*[^\"]\"%[^\"]\"",str);
    printf ("%s\n",str);
    */
    fclose(fp);
    return 0;
}
