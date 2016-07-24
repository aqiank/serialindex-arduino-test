#include <SerialIndex.h>

static const int BUF_LEN    = 32;
static const int NUM_CHARS  = 10;
static const int NUM_INTS   = 10;
static const int NUM_FLOATS = 10;

static int vint;
static int vints[NUM_INTS];
static char vstring[NUM_CHARS];
static float vfloat;
static float vfloats[NUM_FLOATS];
static char vping[NUM_CHARS];

static unsigned char checksum(unsigned char *data, size_t data_len)
{
	unsigned char sum = 0;
	size_t i;

	for (i = 0; i < data_len; i++)
		sum ^= data[i];

	return sum;
}

static void fping()
{
	char buf[BUF_LEN] = { 0 };
	unsigned char sum;

	/* checksum */
	sprintf(buf, "%d", vint);
	sum = checksum((unsigned char *) buf, strlen(buf));

	Serial.write(sum);
	Serial.flush();
}

void setup()
{
	Index.begin(9600);
	Index.read(true);
	Index.add("int", vint);
	//Index.add("float", vfloat);
	//Index.add("string", vstring);
	//Index.add("ints", vints);
	//Index.add("floats", vfloats);
	//Index.add("ping", vping).listen("ping", fping);
}

void loop()
{
	Index.update();
}
