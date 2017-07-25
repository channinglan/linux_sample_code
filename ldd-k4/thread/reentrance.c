/* non-reentrant function */
char *strtoupper(char *string)
{
                static char buffer[MAX_STRING_SIZE];
                int index;

                for (index = 0; string[index]; index++)
                                buffer[index] = toupper(string[index]);
                buffer[index] = 0

                return buffer;
}

/* reentrant function (a better solution) */
char *strtoupper_r(char *in_str, char *out_str)
{
                int index;

                for (index = 0; in_str[index]; index++)
                out_str[index] = toupper(in_str[index]);
                out_str[index] = 0

                return out_str;
}

/* non-reentrant function */
char lowercase_c(char *string)
{
                static char *buffer;
                static int index;
                char c = 0;

                /* stores the string on first call */
                if (string != NULL) {
                                buffer = string;
                                index = 0;
        }

                /* searches a lowercase character */
                for (; c = buffer[index]; index++) {
                                if (islower(c)) {
                                                index++;
                                                break;
                }
        }
                return c;
}

/* reentrant function */
char reentrant_lowercase_c(char *string, int *p_index)
{
                char c = 0;

                /* no initialization - the caller should have done it */

                /* searches a lowercase character */
                for (; c = string[*p_index]; (*p_index)++) {
                                if (islower(c)) {
                                                (*p_index)++;
                                                break;
                  }
        }
                return c;
}

----------

char *my_string;
char my_char;
int my_index;
...
my_index = 0;
while (my_char = reentrant_lowercase_c(my_string, &my_index)) {
        ...
}

-----------------------

/* thread-unsafe function */
int increment_counter()
{
	static int counter = 0;

	counter++;
	return counter;
}

/* pseudo-code threadsafe function */
int increment_counter();
{
	static int counter = 0;
	static lock_type counter_lock = LOCK_INITIALIZER;

	pthread_mutex_lock(counter_lock);
	counter++;
	pthread_mutex_unlock(counter_lock);
	return counter;
}


----------

/*In the following piece of C code, the function is thread-safe, but not reentrant:
*/

#include <pthread.h>
 
int increment_counter ()
{
	static int counter = 0;
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
 
	pthread_mutex_lock(&mutex);
 
	// only allow one thread to increment at a time
	++counter;
	// store value before any other threads increment it further
	int result = counter;	
 
	pthread_mutex_unlock(&mutex);
 
	return result;
}

//Workarounds for thread-unsafe functions
//It is possible to use a workaround to use thread-unsafe functions called by multiple threads. This can be useful, especially when using a thread-unsafe library in a multithreaded program, for testing or while waiting for a threadsafe version of the library to be available. The workaround leads to some overhead, because it consists of serializing the entire function or even a group of functions. The following are possible workarounds:

  //  Use a global lock for the library, and lock it each time you use the library (calling a library routine or using a library global variable). This solution can create performance bottlenecks because only one thread can access any part of the library at any given time. The solution in the following pseudocode is acceptable only if the library is seldom accessed, or as an initial, quickly implemented workaround.

    /* this is pseudo code! */

    lock(library_lock);
    library_call();
    unlock(library_lock);

    lock(library_lock);
    x = library_var;
    unlock(library_lock);

    //Use a lock for each library component (routine or global variable) or group of components. This solution is somewhat more complicated to implement than the previous example, but it can improve performance. Because this workaround should only be used in application programs and not in libraries, mutexes can be used for locking the library.

    /* this is pseudo-code! */

    lock(library_moduleA_lock);
    library_moduleA_call();
    unlock(library_moduleA_lock);

    lock(library_moduleB_lock);
    x = library_moduleB_var;
    unlock(library_moduleB_lock);

//Reentrant and threadsafe libraries

//Reentrant and threadsafe libraries are useful in a wide range of parallel (and asynchronous) programming environments, not just within threads. It is a good programming practice to always use and write reentrant and threadsafe functions.

//Some of the standard C subroutines are non-reentrant, such as the ctime and strtok subroutines. The reentrant version of the subroutines have the name of the original subroutine with a suffix _r (underscore followed by the letter r).
//When writing multithreaded programs, use the reentrant versions of subroutines instead of the original version. For example, the following code fragment:

token[0] = strtok(string, separators);
i = 0;
do {
        i++;
        token[i] = strtok(NULL, separators);
} while (token[i] != NULL);

//should be replaced in a multithreaded program by the following code fragment:

char *pointer;
...
token[0] = strtok_r(string, separators, &pointer);
i = 0;
do {
        i++;
        token[i] = strtok_r(NULL, separators, &pointer);
} while (token[i] != NULL);
/*
Thread-unsafe libraries may be used by only one thread in a program. Ensure the uniqueness of the thread using the library; otherwise, the program will have unexpected behavior, or may even stop.
*/
