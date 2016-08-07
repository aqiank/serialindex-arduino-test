#include <SerialIndex.h>

SerialIndex Index;

static int vint;
static int vints[3];
static char vstring[16];
static float vfloat;
static float vfloats[3];

void setup()
{
	Serial.begin(9600);
	Index.setSerial(Serial);
	Index.read(true);
	Index.add("int", vint, 2).listen("int", fint);
	Index.add("ints", vints).listen("ints", fints);
	Index.add("string", vstring).listen("string", fstring);
	Index.add("float", vfloat, 0.1).listen("float", ffloat);
	Index.add("floats", vfloats).listen("floats", ffloats);
}

void loop()
{
	Index.update();
}

void fint()
{
	Serial.print("int: ");
	Serial.println(vint);
}

void ffloat()
{
	Serial.print("float: ");
	Serial.println(vfloat);
}

void fstring()
{
	Serial.print("string: ");
	Serial.println(vstring);
}

void fints()
{
	Serial.print("ints: ");

	for (int i = 0; i < LEN(vints); i++) {
		Serial.print(vints[i]);
		if (i < LEN(vints) - 1)
			Serial.print(",");
	}

	Serial.println();
}

void ffloats()
{
	Serial.print("floats: ");

	for (int i = 0; i < LEN(vfloats); i++) {
		Serial.print(vfloats[i]);
		if (i < LEN(vfloats) - 1)
			Serial.print(",");
	}

	Serial.println();
}
