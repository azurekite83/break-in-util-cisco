#include <libserialport.h>
//#include <windows.h>
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
        //Add check for elevated privileges
        //Re-add check for valid port, or don't idc
        //Testing
        if (argc < 3 || argc > 3) 
        {
                printf("Incorrect amount of arguments.\n");
                return -1;
        }

        char *port_given = argv[1];
        int baud_rate = atoi(argv[2]);
        struct sp_port *comm_port;
        check(sp_get_port_by_name(port_given, &comm_port));

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
        }

        sp_free_port_list(port_list);
                //set port configuration
        printf("Opening port %s...\n", port_given);
        check(sp_open(comm_port, SP_MODE_READ_WRITE));
        check(sp_set_baudrate(comm_port, baud_rate));
        check(sp_set_bits(comm_port, 8));
        check(sp_set_parity(comm_port, SP_PARITY_NONE));
        check(sp_set_stopbits(comm_port, 1));
        check(sp_set_flowcontrol(comm_port, SP_FLOWCONTROL_NONE));
        
        unsigned char break_sequence = 0x03;
        unsigned int timeout = 10;

        char *recv_buffer = malloc(sizeof(unsigned char));

        //spam break sequence
        for (int i = 0; i <= 100; i++)
        {
                enum sp_return tx_result = check(sp_blocking_write(comm_port, &break_sequence, sizeof(unsigned char), timeout));

                if (tx_result != sizeof(unsigned char))
                {
                        printf("Timed out. Bytes sent: %d/%d. \n", tx_result, sizeof(unsigned char));
                }
                else
                {
                        printf("All bytes sent!\n");
                }

                enum sp_return rx_result = check(sp_blocking_read(comm_port, recv_buffer, sizeof(unsigned char), timeout));

                if (rx_result != sizeof(break_sequence))
                {
                        printf("Timed out. Bytes received: %d/%d. \n", rx_result, sizeof(unsigned char));
                }
                else
                {
                        printf("All bytes received!\n");
                }

        }

        free(recv_buffer);
        check(sp_close(comm_port));
        sp_free_port(comm_port);

        printf("Operation successful.\n");

        //printf("Freeing port list.\n");
        /* Free the array created by sp_list_ports(). */
        //sp_free_port_list(port_list);
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
