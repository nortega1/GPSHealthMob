/*explain how to get this running*/

#include <stdio.h> /* Standard input/output definitions */
#include <string.h> /* String function definitions */	
#include <unistd.h> /* UNIX Standard function definitions */
#include <fcntl.h> /* File control definitions */
#include <errno.h> /* Error number definitions */
#include <termios.h> /* POSIX terminal control definition */

#define GPS_dev "/dev/tq2440_serial1"
#define BAUDRATE B9600
#define SD_file "/mnt/sd/data.txt"

/* ==================== global variables ===================== */
/* Files for opening serial port and writting to sd card */

int fd = 0;
FILE * fdFile = NULL;
FILE * sdFile = NULL;

/* ===================="serial_init ()"===================== */
/* Initializes serial port. */

void serial_init (int fd)
{
	struct termios newtio;

	bzero (&newtio, sizeof (newtio));					/* clear struct for new port settings */
	
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;	/* set bps rate, 8n1, local connection, receive characters */
	newtio.c_iflag = IGNPAR;							/* ignore bytes with parity errors */
	newtio.c_oflag = 0; 								/* raw output */
	newtio.c_lflag &= ~(ECHO | ICANON);					/* enable canonical input and echo functionality */ 

	newtio.c_cc[VTIME] = 5;   							/* inter-character timer unused */
  	newtio.c_cc[VMIN] = 0;   							/* blocking read until 9 chars received */

  	tcflush (fd, TCIFLUSH); 							/* flush buffer */
  	tcsetattr (fd, TCSANOW, &newtio); 					/* activate settings */
}

/* ===================="open_serial ()"===================== */
/* Opens serial port and associates it to a stream */

int open_serial ()
{
	if ((fd = open (GPS_dev, O_RDONLY)) == -1)			/* Opens serial port and returns -1 on error */
	{
		perror ("Unable to open serial port \n");
		return -1;
	}
	else
	{
		serial_init (fd);								/* Initializes serial port */
		if ((fdFile = fdopen (fd, "rb")) == NULL)		/* Associates stream to serial port and returns -1 on error */
		{
			perror ("Unable to stream data \n");
			return -1;
		}
	}
	return 0;
}

/* ===================="open_SD ()"===================== */
/* Open SD card */

int open_SD ()
{
	sdFile = fopen (SD_file , "wb");					/* Opens txt file in SD card and returns -1 on error */
	if (sdFile == NULL)
	{
		perror ("The sd card failed to open");
		return -1;
	}
	return 0;
} 

/* ===================="main ()"===================== */
/* Reads GPS data and records it onto an SD card */

int main ()
{

	int run =0;
	const int buffsize = 255;
	char buffer [buffsize];
	memset (buffer, 0, 255);

	if (open_serial () == -1)							/* Open serial port */
		return 1;

	if (open_SD () == -1)								/* Open SD card */
		return 1;

	printf ( "This program tests the GPS \n" );
		while (run != 20)								/* Change condition to fit accelorameter/GPS needs */
		{	
			fgets (buffer, 255, fdFile);				/* Gets one line from the serial port stream */
			printf ("%s", buffer);						/* Prints line to terminal */	
			fprintf (sdFile,"%s", buffer);				/* Writes line onto txt file in SD card */
			run++;
		}

	fclose (sdFile);									/* closes SD card */
	fclose (fdFile);									/* closes serial port stream */	
	close (fd);											/* closes serial port */

	return 0;
}
