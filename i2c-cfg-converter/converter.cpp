#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>

// kotlin: single commands with only writeRegByte
void doStuff1(FILE *fIn, FILE *fOut)
{
	char line[500];
	memset(line, 0, 500);
	uint8_t registerAddress = 0;
	uint8_t valueData = 0;

	fprintf(fOut, "device.apply {\n");

	while (fgets(line, 500, fIn) != NULL) {
		switch (line[0]) {
			case 'w': {
				sscanf(line, "w %*2x %2hhx %2hhx\n", &registerAddress, &valueData);
				if ((registerAddress & 0x80) == 0x80)
					registerAddress ^= 0x80;
				fprintf(fOut, "\twriteRegByte(0x%02X, 0x%02X.toByte())\n", registerAddress, valueData);
			} break;

			case '>': {
				sscanf(line, "> %2hhx\n", &valueData);
				fprintf(fOut, "\twriteRegByte(0x%02X, 0x%02X.toByte())\n", ++registerAddress, valueData);
			} break;
		}
	}

	fprintf(fOut, "}");
}

// kotlin: single commands with writeRegBuffer
void doStuff2(FILE *fIn, FILE *fOut)
{
	char line[500];
	memset(line, 0, 500);
	uint8_t registerAddress = 0;
	uint8_t valueData = 0;
	size_t arrayCounter = 0;
	
	fprintf(fOut, "device.apply {\n");

	while (fgets(line, 500, fIn) != NULL) {
		switch (line[0]) {
			case 'w': {
				if (arrayCounter) {
					fprintf(fOut, "\t\t),\n\t%lu)\n", arrayCounter);
					arrayCounter = 0;
				}
				
				sscanf(line, "w %*2x %2hhx %2hhx\n", &registerAddress, &valueData);
				if ((registerAddress & 0x80) == 0x80) {
					arrayCounter = 1;
					fprintf(fOut, "\twriteRegBuffer(0x%02X,\n\t\tbyteArrayOf(\n\t\t\t0x%02X.toByte()\n", registerAddress ^ 0x80, valueData);
				} else {					
					fprintf(fOut, "\twriteRegByte(0x%02X, 0x%02X.toByte())\n", registerAddress, valueData);
				}
			} break;

			case '>': {
				++arrayCounter;
				sscanf(line, "> %2hhx\n", &valueData);
				fprintf(fOut, "\t\t\t0x%02X.toByte()\n", valueData);
			} break;
		}
	}

	if (arrayCounter) {
		fprintf(fOut, "\t\t),\n\t%lu)\n", arrayCounter);
	}

	fprintf(fOut, "}");
}

// kotlin: array
void doStuff3(FILE *fIn, FILE *fOut)
{
	char line[500];
	memset(line, 0, 500);
	uint8_t registerAddress = 0;
	uint8_t valueData = 0;
	bool firstIsDone = false;
	bool scriptBreakReached = false;
	
	//fprintf(fOut, "arrayOf(");
	
	while (!scriptBreakReached && fgets(line, 500, fIn) != NULL) {
		if (line[0] == 'w' || line[0] == '>' || line[0] == 'd') {
			if (firstIsDone)
				fprintf(fOut, ", ");
			else
				firstIsDone = true;
		}

		switch (line[0]) {
			case 'w': {
				sscanf(line, "w %*2x %2hhx %2hhx\n", &registerAddress, &valueData);
				if ((registerAddress & 0x80) == 0x80)
					registerAddress ^= 0x80;
				fprintf(fOut, "I2cCmd.Data(0x%02X, 0x%02X.toByte())", registerAddress, valueData);
			} break;

			case '>': {
				sscanf(line, "> %2hhx\n", &valueData);
				fprintf(fOut, "I2cCmd.Data(0x%02X, 0x%02X.toByte())", ++registerAddress, valueData);
			} break;

			case 'd': {
				uint32_t sleepTime;
				sscanf(line, "d %u", &sleepTime);
				fprintf(fOut, "I2cCmd.Delay(%u)", sleepTime);
			} break;

			case 'b': {
				scriptBreakReached = true;
			} break;
		}
	}

	//fprintf(fOut, ")");
}

// kotlin: array with arrays
void doStuff4(FILE *fIn, FILE *fOut)
{
	char line[500];
	memset(line, 0, 500);
	uint8_t registerAddress = 0;
	uint8_t valueData = 0;
	size_t arrayCounter = 0;
	bool firstIsDone = false;
	bool scriptBreakReached = false;

	while (!scriptBreakReached && fgets(line, 500, fIn) != NULL) {
		if (line[0] == 'w' || line[0] == '>' || line[0] == 'd') {
			if (arrayCounter && line[0] != '>') {
				fprintf(fOut, "))");
				arrayCounter = 0;
			}

			if (firstIsDone)
				fprintf(fOut, ", ");
			else
				firstIsDone = true;
		}

		switch (line[0]) {
			case 'w': {
				sscanf(line, "w %*2x %2hhx %2hhx\n", &registerAddress, &valueData);
				if ((registerAddress & 0x80) == 0x80) {
					arrayCounter = 1;
					fprintf(fOut, "I2cCmd.DataBuf(0x%02X, byteArrayOf(0x%02X.toByte()", registerAddress ^ 0x80, valueData);
				} else {					
					fprintf(fOut, "I2cCmd.Data(0x%02X, 0x%02X.toByte())", registerAddress, valueData);
				}
			} break;

			case '>': {
				++arrayCounter;
				sscanf(line, "> %2hhx\n", &valueData);
				fprintf(fOut, "0x%02X.toByte()", valueData);
			} break;

			case 'd': {
				uint32_t sleepTime;
				sscanf(line, "d %u", &sleepTime);
				fprintf(fOut, "I2cCmd.Delay(%u)", sleepTime);
			} break;

			case 'b': {
				scriptBreakReached = true;
			} break;
		}
	}

	if (arrayCounter) {
		fprintf(fOut, "))");
	}
}

// bash: single pio commands
void doStuff5(FILE *fIn, FILE *fOut)
{
	char line[500];
	memset(line, 0, 500);
	uint8_t deviceAddress = 0;
	uint8_t registerAddress = 0;
	uint8_t valueData = 0;
	bool scriptBreakReached = false;
	
	while (fgets(line, 500, fIn) != NULL && !scriptBreakReached) {
		switch (line[0]) {
			case 'w': {
				sscanf(line, "w %2hhx %2hhx %2hhx", &deviceAddress, &registerAddress, &valueData);
				if (deviceAddress > 0x7F)
					deviceAddress >>= 1;
				if ((registerAddress & 0x80) == 0x80)
					registerAddress ^= 0x80;
				fprintf(fOut, "pio i2c I2C1 0x%02X write-reg-byte 0x%02X 0x%02X\n", deviceAddress, registerAddress, valueData);
			} break;

			case '>': {
				sscanf(line, "> %2hhx", &valueData);
				fprintf(fOut, "pio i2c I2C1 0x%02X write-reg-byte 0x%02X 0x%02X\n", deviceAddress, ++registerAddress, valueData);
			} break;

			case 'd': {
				uint32_t sleepTime;
				sscanf(line, "d %u", &sleepTime);
				fprintf(fOut, "sleep %u\n", sleepTime);
			} break;

			case 'b': {
				scriptBreakReached = true;
			} break;
		}
	}
}

// bash: single pio commands with buffer
void doStuff6(FILE *fIn, FILE *fOut)
{
	char line[500];
	memset(line, 0, 500);
	uint8_t deviceAddress = 0;
	uint8_t registerAddress = 0;
	uint8_t valueData = 0;
	bool scriptBreakReached = false;
	bool wasInArray = false;
	
	while (fgets(line, 500, fIn) != NULL && !scriptBreakReached) {
		switch (line[0]) {
			case 'w': {
				if (wasInArray) {
					fprintf(fOut, "\n");
					wasInArray = false;
				}
				
				sscanf(line, "w %2hhx %2hhx %2hhx", &deviceAddress, &registerAddress, &valueData);
				if (deviceAddress > 0x7F)
					deviceAddress >>= 1;
				if ((registerAddress & 0x80) == 0x80) {
					fprintf(fOut, "pio i2c I2C1 0x%02X write-reg-buffer 0x%02X 0x%02X", deviceAddress, registerAddress ^ 0x80, valueData);
				} else {					
					fprintf(fOut, "pio i2c I2C1 0x%02X write-reg-byte 0x%02X 0x%02X\n", deviceAddress, registerAddress, valueData);
				}
			} break;

			case '>': {
				wasInArray = true;
				sscanf(line, "> %2hhx", &valueData);
				fprintf(fOut, " 0x%02X", valueData);
			} break;

			case 'd': {
				uint32_t sleepTime;
				sscanf(line, "d %u", &sleepTime);
			} break;

			case 'b': {
				scriptBreakReached = true;
			} break;
		}
	}

	if (wasInArray) {
		fprintf(fOut, "\n");
	}
}

// bash: single i2cset commands
void doStuff7(FILE *fIn, FILE *fOut)
{
	char line[500];
	memset(line, 0, 500);
	uint8_t deviceAddress = 0;
	uint8_t registerAddress = 0;
	uint8_t valueData = 0;
	bool scriptBreakReached = false;
	
	while (fgets(line, 500, fIn) != NULL && !scriptBreakReached) {
		switch (line[0]) {
			case 'w': {
				sscanf(line, "w %2hhx %2hhx %2hhx", &deviceAddress, &registerAddress, &valueData);
				if (deviceAddress > 0x7F)
					deviceAddress >>= 1;
				if ((registerAddress & 0x80) == 0x80)
					registerAddress ^= 0x80;
				fprintf(fOut, "i2cset -f -y 1 0x%02X 0x%02X 0x%02X\n", deviceAddress, registerAddress, valueData);
			} break;

			case '>': {
				sscanf(line, "> %2hhx", &valueData);
				fprintf(fOut, "i2cset -f -y 1 0x%02X 0x%02X 0x%02X\n", deviceAddress, ++registerAddress, valueData);
			} break;

			case 'd': {
				uint32_t sleepTime;
				sscanf(line, "d %u", &sleepTime);
				double dSleepTime = sleepTime / 1000.0;
				fprintf(fOut, "sleep %f\n", dSleepTime);
			} break;

			case 'b': {
				scriptBreakReached = true;
			} break;
		}
	}

	switch (deviceAddress) {
		case 0x4C:
			fprintf(fOut, "tinymix 'I2S O03_O04 Switch' 1\ntinymix 'O03 I05 Switch' 1\ntinymix 'O04 I06 Switch' 1\ntinymix 'Audio Amp Playback Volume' 7 7\n");
			break;

		case 0x1B:
			fprintf(fOut, "tinymix 'AIF TX Mux' 'Digital MIC'\n");
			break;
	}
}

// konvert i2cset commands to i2c cfg
void undoStuff7(FILE *fIn, FILE *fOut)
{
	char line[500];
	memset(line, 0, 500);
	uint8_t deviceAddress = 0;
	uint8_t registerAddress = 0;
	uint8_t valueData = 0;

	while (fgets(line, 500, fIn) != NULL) {
		size_t len = strlen(line);

		//sscanf(line, "/vendor/bin/i2cset -f -y 1");
		char *sDeviceAddress = strstr(line, "0x");
		sscanf(sDeviceAddress+2, "%2hhx", &deviceAddress);
		deviceAddress <<= 1;

		char *sRegisterAddress = strstr(sDeviceAddress+1, "0x");
		sscanf(sRegisterAddress+2, "%2hhx", &registerAddress);
		
		char *sValue = strstr(sRegisterAddress+1, "0x");
		sscanf(sValue+2, "%2hhx", &valueData);

		if (line[len-2] != 'i') {
			fprintf(fOut, "w %02x %02x %02x\n", deviceAddress, registerAddress, valueData);
		} else {
			registerAddress |= 0x80;
			fprintf(fOut, "w %02x %02x %02x\n", deviceAddress, registerAddress, valueData);

			while (true) {
				sValue = strstr(sValue+1, "0x");
				if (sValue == NULL)
					break;

				sscanf(sValue+2, "%2hhx", &valueData);
				fprintf(fOut, "> %02x\n", valueData);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		//printf("Please supply a the path to a file as a command line argument.");
		printf(
			"Usage: bla <1-7>[u] <input file>\n"
			"Result will be written to output.txt\n"
			"\nParams:\n"
			" 1: kotlin: single commands with only writeRegByte\n"
			" 2: kotlin: single commands with writeRegBuffer\n"
			" 3: kotlin: array\n"
			" 4: kotlin: array with arrays\n"
			" 5: bash: single pio commands\n"
			" 6: bash: single pio commands with buffer\n"
			" 7: bash: single i2cset commands\n"
			"\n [u] modfier calls the corresponding undo function to revert the output into the i2c cfg format"
		);
		return 0;
	}

	FILE *fIn = fopen(argv[2], "r");
	FILE *fOut = fopen("output.txt", "w");

	uint8_t method = 0;
	bool undo = false;
	char cUndo;

	int scanRes = sscanf(argv[1], "%hhu%c", &method, &cUndo);
	if (scanRes == 2 && cUndo == 'u')
		undo = true;

	if (!undo) {
		switch (method) {
			case 1: doStuff1(fIn, fOut); break;
			case 2: doStuff2(fIn, fOut); break;
			case 3: doStuff3(fIn, fOut); break;
			case 4: doStuff4(fIn, fOut); break;
			case 5: doStuff5(fIn, fOut); break;
			case 6: doStuff6(fIn, fOut); break;
			case 7: doStuff7(fIn, fOut); break;
			default: printf("invalid or unimplemented option\n"); break;
		}
	} else {
		switch (method) {
			case 7: undoStuff7(fIn, fOut); break;
			default: printf("invalid or unimplemented option\n"); break;
		}
	}

	fclose(fOut);
	fclose(fIn);

	return 0;
}