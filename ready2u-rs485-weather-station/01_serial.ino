const int numBytes = 8;
const int loopCount = 3;
 const byte modbusQuery1[numBytes] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B}; // BGT-WSD2 humi & temp
 const byte modbusQuery2[numBytes] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x38}; // RS FS wind speed
 const byte modbusQuery3[numBytes] = {0x03, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC5, 0xE9}; // RS FX wind direction

byte buffQuery[numBytes];

byte ByteArray[9];
int ByteData[9];

void serialSetup()
{
  Serial2.begin(9600); //
}

void serialLoop()
{
  for (int i = 1; i <= loopCount; i++)
  {

    switch (i)
    {
    case 1:
      Serial2.updateBaudRate(9600);
      memcpy(buffQuery, modbusQuery1, numBytes);
      break;
    case 2:
    Serial2.updateBaudRate(9600);
    memcpy(buffQuery, modbusQuery2, numBytes);
    break;
    case 3:
      Serial2.updateBaudRate(4800);
      memcpy(buffQuery, modbusQuery3, numBytes);
      break;
    default:
      break;
    }
    SerialProcessData(buffQuery);

    delay(5000);
  }
}

void SerialProcessData(byte buffQuery[8])
{

  for (int j = 0; j < numBytes; j++)
  {
    Serial2.write(buffQuery[j]);
  }
  int a = 0;
  while (Serial2.available() > 0)
  {
    ByteArray[a] = Serial2.read();
    // Serial.print(ByteArray[a]);
    // Serial.print(" ");
    a++;
  }

  ByteData[0] = ByteArray[3] * 256 + ByteArray[4];
  ByteData[1] = ByteArray[5] * 256 + ByteArray[6];

  float Data1, Data2;
  Data1 = ByteData[0];
  Data2 = ByteData[1];

  Serial.print(ByteArray[0]);
  Serial.print(" ");
  Serial.print(Data1);
  Serial.print(" ");
  Serial.print(Data2);
  Serial.println(" ");
  

  if (ByteArray[0] == 1)
  {
    TEMP = round(Data2 * 0.1);
    HUMI = round(Data1 * 0.1);
  }
  else if (ByteArray[0] == 2)
  {
    WINSPD = round(Data1 * 0.1) * 3.6;  //metre per second to kilometre per hour multiply the speed value by 3.6

  }
  else if (ByteArray[0] == 3)
  {
    WINDIR = Data2;
  }
}
