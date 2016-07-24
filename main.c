#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include <libserialport.h>

#define FPS              (60)
#define BUF_LEN          (60)
#define NUM_ARRAY_ITEMS  (10)
#define NUM_ALPHABETS    (26)
#define READ_TIMEOUT     (1)

static const char ALPHABETS[NUM_ALPHABETS] = "abcdefghijklmnopqrstuvwxyz";
static const char EOL[] = "\r\n";

static struct sp_port *current_port;
static int nsent, nread;
static int running = 1;

static int vint;
static float vfloat;
static char vstring[NUM_ARRAY_ITEMS];
static int vints[NUM_ARRAY_ITEMS];
static float vfloats[NUM_ARRAY_ITEMS];

static const char HELP[] =
"usage: %s [DEVICE]\n";

unsigned char checksum(unsigned char *data, size_t data_len)
{
	unsigned char sum = 0;
	size_t i;

	for (i = 0; i < data_len; i++) {
		sum ^= data[i];
	}

	return sum;
}

static void randomize_string(char *string, size_t string_len)
{
	size_t i;

	for (i = 0; i < string_len; i++)
		string[i] = ALPHABETS[rand() % NUM_ALPHABETS];
}

static void update()
{
	char buf[BUF_LEN] = { 0 };
	unsigned char sum;
	size_t buf_len;
	size_t i;

	vint = (vint + 1) % 255;
	vfloat += 0.1;
	randomize_string(vstring, sizeof(vstring) - 1);

	for (i = 0; i < NUM_ARRAY_ITEMS; i++) {
		vints[i] += 1;
		vfloats[i] += 0.1;
	}

	/* checksum */
	sprintf(buf, "%d", vint);
	buf_len = strlen(buf);
	sum = checksum((unsigned char *) buf, buf_len);

	/* int */
	sprintf(buf, "%s%c%d%s", "int", '=', vint, EOL);
	buf_len = strlen(buf);
	sp_nonblocking_write(current_port, buf, buf_len);

	/* float */
	sprintf(buf, "%s%c%.2f%s", "float", '=', vfloat, EOL);
	buf_len = strlen(buf);
	sp_nonblocking_write(current_port, buf, buf_len);

	/* TODO: string, int-array, float-array */

	sprintf(buf, "%s%c%s%s", "ping", '=', "ping", EOL);
	buf_len = strlen(buf);
	sp_nonblocking_write(current_port, buf, buf_len);

	sp_drain(current_port);

	nsent += 1;

	sp_blocking_read(current_port, buf, 1, READ_TIMEOUT);
	if (sum == buf[0])
		nread += 1;

	fprintf(stdout, "checksums: %hhx %hhx\n", sum, buf[0]);
	if (nsent > 0)
		fprintf(stdout, "data integrity: %d%%\n", nread * 100 / nsent);
}

static void signal_callback(int sig)
{
	char *s;

	switch (sig) {
	case SIGINT:
		s = "interrupt";
		break;

	case SIGTERM:
		s = "terminate";
		break;

	default:
		break;
	}

	fprintf(stdout, "received signal: %s\n", s);
	running = 0;
}

int main(int argc, char **argv)
{
	struct sp_port **ports = NULL;
	struct sp_port *port = NULL;
	char *port_name;
	int ret;
	int i;

	if (argc <= 1) {
		fprintf(stdout, HELP, argv[0]);
		ret = EXIT_SUCCESS;
		goto out;
	}

	ret = sp_list_ports(&ports);
	if (ret != SP_OK) {
		fprintf(stderr, "error: failed to list ports\n");
		ret = EXIT_FAILURE;
		goto out;
	}

	for (port = ports[0], i = 0; port != NULL; port = ports[++i]) {
		port_name = sp_get_port_name(port);
		if (strcmp(port_name, argv[1]) == 0) {
			ret = sp_copy_port(port, &current_port);
			if (ret != SP_OK) {
				fprintf(stderr, "error: failed to copy port\n");
				ret = EXIT_FAILURE;
				goto out;
			}
			break;
		}
	}

	if (!current_port) {
		fprintf(stderr, "error: couldn't find port named %s\n", argv[1]);
		ret = EXIT_FAILURE;
		goto out;
	}

	ret = sp_open(current_port, SP_MODE_READ_WRITE);
	if (ret != SP_OK) {
		fprintf(stderr, "error: failed to open port\n");
		ret = EXIT_FAILURE;
		goto out;
	}

	signal(SIGINT, signal_callback);
	signal(SIGTERM, signal_callback);

	while (running)
		update();

out:
	if (current_port) {
		sp_close(current_port);
		sp_free_port(current_port);
	}

	if (ports)
		sp_free_port_list(ports);

	return ret;
}
