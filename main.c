#include <libserialport.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


// Iterate through COM ports
//      Only one should be in use, so use that one.
// Set baud, speed, and other settings
// Spam break on boot or press button on device
// send commands to change environment command
//      have some kind of successful execution check or something

int check(enum sp_return result);

int main(int argc, char **argv) 
{
        //TODO: Use getopts.h if investing in this program
        //valid arguments are <COM port> <Baud rate>
        if (argc < 3 || argc > 3) 
        {
                printf("Incorrect amount of arguments.\n");
                return -1;
        }

        char *port_given = argv[1];
        int baud_rate = atoi(argv[2]);
        bool port_found = false;
        struct sp_port *port_to_open;

        struct sp_port **port_list;
        printf("Getting port list.\n");
        /* Call sp_list_ports() to get the ports. The port_list
         * pointer will be updated to refer to the array created. */
        check(sp_list_ports(&port_list));
        
        /* Iterate through the ports. When port_list[i] is NULL
         * this indicates the end of the list. */
        int i;
        for (i = 0; port_list[i] != NULL; i++) 
        {
                struct sp_port *port = port_list[i];
                /* Get the name of the port. */
                char *port_name = sp_get_port_name(port);
                printf("Found port: %s\n", port_name);

                if (strcmp(port_given, port_name) == 0)
                {
                        printf("Port given exists. Continuing...\n");
                        port_found = true;
                        port_to_open = port;
                        break;
                }
        }

        if (port_found == false)
        {
                printf("Port given not found. Exiting...\n");
        }
        else
        {
                //set port configuration
                printf("Opening port...\n");
                check(sp_open(port_to_open, SP_MODE_READ_WRITE));
                check(sp_set_baudrate(port_to_open, baud_rate));
                check(sp_set_bits(port_to_open, 8));
                check(sp_set_parity(port_to_open, SP_PARITY_NONE));
                check(sp_set_stopbits(port_to_open, 1));
                check(sp_set_flowcontrol(port_to_open, SP_FLOWCONTROL_NONE));
        }

        char *break_sequence = "";
        char *recv_buffer = malloc(sizeof(char) * 
        //spam break sequence
        for (int i = 0; i <= 1500; i++)
        {
                ; //placeholder
        }

        printf("Freeing port list.\n");
        /* Free the array created by sp_list_ports(). */
        sp_free_port_list(port_list);
        /* Note that this will also free all the sp_port structures
         * it points to. If you want to keep one of them (e.g. to
         * use that port in the rest of your program), take a copy
         * of it first using sp_copy_port(). */
        return 0;
}

int check(enum sp_return result)
{
        /* For this example we'll just exit on any error by calling abort(). */
        char *error_message;
        switch (result) {
        case SP_ERR_ARG:
                printf("Error: Invalid argument.\n");
                abort();
        case SP_ERR_FAIL:
                error_message = sp_last_error_message();
                printf("Error: Failed: %s\n", error_message);
                sp_free_error_message(error_message);
                abort();
        case SP_ERR_SUPP:
                printf("Error: Not supported.\n");
                abort();
        case SP_ERR_MEM:
                printf("Error: Couldn't allocate memory.\n");
                abort();
        case SP_OK:
        default:
                return result;
        }
}
