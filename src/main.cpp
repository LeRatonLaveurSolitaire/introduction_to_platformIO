#include "model_float32.h"


#include "tensorflow/lite/micro/micro_mutable_op_resolver.h" 
#define NUM_TF_OPS 2 // this is the number of ops in the model, used by the micro_mutable_op_resolver

#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
#include "tensorflow/lite/schema/schema_generated.h"

#define RAM_SIZE 40000 // 40000 bytes - this is large because of the model, but it can be reduce for smaller models
uint8_t tensor_arena[RAM_SIZE];

// Globally accessible interpreter
std::unique_ptr<tflite::MicroInterpreter> interpreter;

#include <Arduino.h>

#ifdef ARDUINO
#define abs(x) ((x)>0?(x):-(x))
#endif 

#include <Wire.h>
#include <U8g2lib.h>
#include <math.h>

// LCD print data function
void print_lcd(float R, float M, float t);

//define OLED display object
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);


// tensorflow namespace declaration




void setup(){

  // Initialize UART communication
  Serial.begin(115200);

  // Initialize I2C communication
  Wire.begin(); 

  // Initialize the display
  u8g2.begin();

  // Set text size and color
  u8g2.setFont(u8g2_font_7x14_mf);
  u8g2.setColorIndex(1);

  // Set display
  print_lcd(0,0,0);

	delay(5000);
  // TensorFlow lite Micro Initialization
  

	// set up the error reporter
	static tflite::MicroErrorReporter micro_error_reporter;
	tflite::ErrorReporter* error_reporter = &micro_error_reporter;

	// set up the model
	const tflite::Model* model = tflite::GetModel(model_float32);
	// check to make sure the model is compatible
	if (model->version() != TFLITE_SCHEMA_VERSION) {
		#ifdef ARDUINO // serial out for Arduino instead of stdout
		Serial.print("Model provided is schema version ");
		Serial.print(model->version());
		Serial.print(" not equal to supported version ");
		Serial.println(TFLITE_SCHEMA_VERSION);
		#endif
		TF_LITE_REPORT_ERROR(error_reporter, "Model provided is schema version %d not equal to supported version %d.", model->version(), TFLITE_SCHEMA_VERSION);
		return;
	}

	// set up the optional micro mutable ops resolver, and add needed operations
	static tflite::MicroMutableOpResolver<NUM_TF_OPS> resolver;
	resolver.AddRelu();
	resolver.AddFullyConnected();


	// Declare the TF lite interpreter
	static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, RAM_SIZE);
  interpreter = std::unique_ptr<tflite::MicroInterpreter>(&static_interpreter);

	// Allocate memory for the model's input buffers
	TfLiteStatus allocate_status = interpreter->AllocateTensors();
	if (allocate_status != kTfLiteOk)
	{
		TF_LITE_REPORT_ERROR(error_reporter, "Tensor allocation failed");
	}
	else
	{
		Serial.println("Tensor allocation success");
		Serial.print("Used bytes: ");
		Serial.println(interpreter->arena_used_bytes());
	}

	// Obtain a pointer to the model's input tensor
	TfLiteTensor *input = interpreter->input(0);

	// Print out the input tensor's details to verify
	// the model is working as expected
	// Serial.print("Input size: ");
	// Serial.println(input->dims->size);
	// Serial.print("Input bytes: ");
	// Serial.println(input->bytes);

	// for (int i = 0; i < input->dims->size; i++)
	// {
	// 	Serial.print("Input dim ");
	// 	Serial.print(i);
	// 	Serial.print(": ");
	// 	Serial.println(input->dims->data[i]);
	// }

	// Supply data to the model
	// This model has a 1x30 input, so the array needs to be 30 floats long
	// The sequence of the array goes in case of 2 dim data : {0:0, 0:1, 0:2, 0:3, 0:4, 1:0, 1:1, 1:2, 1:3, 1:4, ...}
	
	// 	|-------------------------------------|
	// 	| Parameter | Real value | Estimation |
	// 	|-----------|------------|------------|
	// 	| R_l (Ohm) |   1.338    |   1.094    |
	// 	|-----------|------------|------------|
	// 	|  M (µH)   |    1.70    |    1.75    |
	// 	|-------------------------------------|

	// input : [320.59124755859375, -2.689563274383545, 302.7422180175781, -2.7393453121185303, 287.4570617675781, -2.7949178218841553, 274.7843933105469, -2.85619854927063, 264.7668151855469, -2.922821044921875, 257.4147644042969, -2.9940006732940674, 252.63893127441406, -3.0681350231170654, 250.6195068359375, 3.1415927410125732, 252.32302856445312, 3.06889009475708, 257.0472412109375, 2.995483636856079, 264.40069580078125, 2.924595832824707, 274.4128723144531, 2.858070135116577, 287.0713195800781, 2.79679536819458, 302.3336486816406, 2.741194486618042, 320.1588134765625, 2.6913552284240723]

	float input_data[30] = {320.59124755859375, -2.689563274383545, 302.7422180175781, -2.7393453121185303, 287.4570617675781, -2.7949178218841553, 274.7843933105469, -2.85619854927063, 264.7668151855469, -2.922821044921875, 257.4147644042969, -2.9940006732940674, 252.63893127441406, -3.0681350231170654, 250.6195068359375, 3.1415927410125732, 252.32302856445312, 3.06889009475708, 257.0472412109375, 2.995483636856079, 264.40069580078125, 2.924595832824707, 274.4128723144531, 2.858070135116577, 287.0713195800781, 2.79679536819458, 302.3336486816406, 2.741194486618042, 320.1588134765625, 2.6913552284240723};


	// Copy the data into the input tensor
	for (unsigned int i = 0; i < input->bytes; i++)
	{
		input->data.f[i] = input_data[i];
	}

	// Invoke the model
	int start = micros();
	TfLiteStatus invoke_status = interpreter->Invoke();
	int stop = micros();
	if (invoke_status != kTfLiteOk)
	{
		TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed");
	}
	else
	{
		Serial.println("Invoke completed");
	}

	// Obtain a pointer to the model's output tensor
	TfLiteTensor *output = interpreter->output(0);

	// Print out the output tensor's details to verify
	// the model is working as expected
	// Serial.print("Output size: ");
	// Serial.println(output->dims->size);
	// Serial.print("Output bytes: ");
	// Serial.println(output->bytes);

	// for (int i = 0; i < output->dims->size; i++)
	// {
	// 	Serial.print("Output dim ");
	// 	Serial.print(i);
	// 	Serial.print(": ");
	// 	Serial.println(output->data.f[i]);
	// }

	float R = output->data.f[0];
	float M = output->data.f[1];
	
	R = pow(10,R*0.15 + 0.5);
	M = pow(10,(0.1 * M) ) * (0.1 * 33.727140406503484) ;
	float time = stop - start;
	print_lcd(R,M,time/1000);


}


void loop(){
    if (Serial.available()){
      
    	float input_values[30];
    	for (int i = 0; i < 30; i++) {
        	// Read 4 bytes (size of float)
        	char buffer[4];
        	Serial.readBytes(buffer, sizeof(float));
	         // Convert the received bytes to a float
        	float value;
        	memcpy(&value, buffer, sizeof(float));
        	input_values[i]=value;
		  
      	}
      	Serial.println("OK");

     
      	Serial.println("Revcieved ! ");

      	for (int i = 0; i < 30; i++) {
        	char to_print[15];
        	sprintf(to_print,"%f",input_values[i]);
        	Serial.println(to_print);
        
      	}

	  	Serial.println("Computing...");
	  	
		int start = micros();

		TfLiteTensor *input = interpreter->input(0);

		for (int i = 0; i < 30; i++) {
			input->data.f[i] = input_values[i];
		}
		TfLiteStatus invoke_status = interpreter->Invoke();
		if (invoke_status != kTfLiteOk){
			Serial.println("Invoke failed");
		}
		else{
			Serial.println("Invoke completed");
		}

		TfLiteTensor *output = interpreter->output(0);
		float R = output->data.f[0];
		float M = output->data.f[1];
		int stop = micros();
		R = pow(10,R*0.15 + 0.5);
		M = pow(10,(0.1 * M) ) * (0.1 * 33.727140406503484) ;
		float time = stop - start;
		print_lcd(R,M,time/1000);
    }
}


void print_lcd(float R, float M,float t){
  digitalWrite(25,1);
  char to_print[50];
  u8g2.clearBuffer();
  u8g2.setCursor(0, 10);
  u8g2.print("NN Param Estimator");
  u8g2.setCursor(0, 32-6);
  sprintf(to_print,"R_l : %.2f Ohm",R);
  u8g2.print(to_print);
  u8g2.setCursor(0, 48-6);
  sprintf(to_print,"M   : %.2f uH",M);
  u8g2.print(to_print);
  u8g2.setCursor(0, 64-6);
  sprintf(to_print,"Took  %.2f ms",t);
  u8g2.print(to_print);
  u8g2.sendBuffer();
  digitalWrite(25,0);
}