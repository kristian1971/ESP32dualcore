#pragma GCC optimize ("-O0")
#pragma GCC push_options

#define constINPUT 50
#define constHIDDEN 200
#define constOUTPUT 10

float input[constINPUT];
float w_hidden[constINPUT][constHIDDEN];
float hidden_sum[constHIDDEN];
float hidden_sum_sig[constHIDDEN];
float w_output[constHIDDEN][constOUTPUT];
float output_sum[constOUTPUT];
float output_sum_sig[constOUTPUT];

int LED_BUILTIN = 2;
int i=0;
int j=0;

//int n[4] = {10, 100, 1000, 5 };
//int nArraySize = 4;
 
struct argsStruct {
  int arrayStart;
  int arrayEnd;
  int n;
};
 
unsigned long start;
unsigned long end1;
unsigned long execTimeOneTask, execTimeTwoTask ;
 
SemaphoreHandle_t barrierSemaphore = xSemaphoreCreateCounting( 2, 0 );
 
//int bigArray[10000], resultArray[10000];
 
void setup() {

  int i = 0;
  int j = 0;

  Serial.begin(115200);
  Serial.println("START");
  
  while (i < constINPUT) {
    input[i] = 0.5+random1(0, 1);
    i++;
  }
  i=0;

  i=0;
  while (i < constHIDDEN) {
      hidden_sum[i] = 0;
      i++;
  }
  i=0;

  while (i < constINPUT) {
      while (j < constHIDDEN){
        w_hidden[i][j] = 0.5+random1(0, 1);
        //Serial.println(w_hidden[i][j], 64);
        j++;
      }
    j=0;
    i++;
  }
  i=0;
  j=0;

  while (i < constHIDDEN) {
      while (j < constOUTPUT){
        w_output[i][j] = 0.5+random1(0, 1);
        //Serial.println(w_hidden[i][j], 64);
        j++;
      }
    j=0;
    i++;
  }
  i=0;
  j=0;
Serial.println("STOP");

 
//  Serial.begin(115200);
  Serial.println();
 
//  randomSeed(analogRead(0));
 
//  for (int i = 0; i < 10000; i++) {
//    bigArray[i] = random(1, 10);
//  }
 
  //for (int i = 0; i < nArraySize; i++) {
 
    Serial.println("#############################");
    Serial.print("Starting test for n= ");
//    Serial.println(n[i]);
 
    argsStruct oneTask = { 0 , constHIDDEN , 1 };
 
    argsStruct twoTasks1 = { 0 , constHIDDEN/2-1 , 1 };
    argsStruct twoTasks2 = { constHIDDEN/2 , constHIDDEN , 1 };

 
    Serial.println("");
    Serial.println("------One task-------");
 
    start = micros();
 
    xTaskCreatePinnedToCore(
      powerTask,               /* Function to implement the task */
      "powerTask",              /* Name of the task */
      10000,                   /* Stack size in words */
      (void*)&oneTask,         /* Task input parameter */
      20,                      /* Priority of the task */
      NULL,                    /* Task handle. */
      1);                      /* Core where the task should run */
 
    xSemaphoreTake(barrierSemaphore, portMAX_DELAY);
 
    end1 = micros();
    execTimeOneTask = end1 - start;
    Serial.print("Exec time: ");
    Serial.println(execTimeOneTask);
    Serial.print("Start: ");
    Serial.println(start);
    Serial.print("end: ");
    Serial.println(end1);
    Serial.print("HS20: ");
    Serial.println(hidden_sum[33]);
 
    Serial.println("");
    i=0;
    while (i < constHIDDEN) {
      hidden_sum[i] = 0;
      i++;
    }
    i=0;
    
    Serial.println("------Two tasks-------");
 
    start = micros();
 
    xTaskCreatePinnedToCore(
      powerTask,                /* Function to implement the task */
      "powerTask",              /* Name of the task */
      1000,                    /* Stack size in words */
      (void*)&twoTasks1,        /* Task input parameter */
      20,                       /* Priority of the task */
      NULL,                     /* Task handle. */
      0);                       /* Core where the task should run */
 
    xTaskCreatePinnedToCore(
      powerTask,               /* Function to implement the task */
      "coreTask",              /* Name of the task */
      1000,                   /* Stack size in words */
      (void*)&twoTasks2,       /* Task input parameter */
      20,                      /* Priority of the task */
      NULL,                    /* Task handle. */
      1);                      /* Core where the task should run */
 
    for (int i = 0; i < 2; i++) {
      xSemaphoreTake(barrierSemaphore, portMAX_DELAY);
    }
 
    end1 = micros();
    execTimeTwoTask = end1 - start;
    Serial.print("Exec time: ");
    Serial.println(execTimeTwoTask);
    Serial.print("Start: ");
    Serial.println(start);
    Serial.print("end: ");
    Serial.println(end1);
    Serial.print("HS20: ");
    Serial.println(hidden_sum[33]);

    Serial.println();
    Serial.println("------Results-------");
 
    Serial.print("Speedup two tasks: ");
    Serial.println((float) execTimeOneTask / execTimeTwoTask, 4 );
    
  //}
 
}
 
void loop() {
 
}

float random1(float minf, float maxf)
{
  return minf + random(1UL<<31) * (maxf - minf) / (1UL<<31);  // use 1ULL<<63 for max double values)
}
 
void powerTask( void * parameters ) {
 
  argsStruct  myArgs = *((argsStruct*)parameters);
 
  int j=0;
  for (int i = myArgs.arrayStart; i < myArgs.arrayEnd; i++) {

          //while (i < constINPUT) {
            while (j < constINPUT){
              hidden_sum[i]=hidden_sum[i]+w_hidden[i][j]*input[j];
              j++;
            }
          j=0;
          //i++;
          //}
    /*
    product = 1;
    for (int j = 0; j < myArgs.n; j++) {
      product =  product * bigArray[i];
    }
 
    resultArray[i]=product;
    //resultArray [i] = pow(bigArray[i], myArgs.n);
    */
  }
 
  xSemaphoreGive(barrierSemaphore);
 
  vTaskDelete(NULL);
 
}
