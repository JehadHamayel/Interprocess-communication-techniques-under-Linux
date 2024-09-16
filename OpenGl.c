#include "local.h"

int Shmid;
int ContainerShmid;
key_t key;

// Initial position of the ball
float plane_position[maxPlanNum][2] = {{0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}};
float Boombed_plane_position[10][2] = {{0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}};
float Reverce_Boombed_plane_position[10][2] = {{0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}, {0.8, 0.9}};
float Container_Position[10][maxContainerNum][2];
float Boombed_Container_position[10][2];
float Collecting_Worker_Position[10][5][2]; 
float Distibuting_Worker_Position[4][10][2];
int ctrl[40][20] = {0};	
int draw_flag[40][20] = {0}; 
int Plane_Boom_Flag[10] = {0};
int Revirce_Plane_Boom_Flag[10] = {0};
int Container_Boom_Flag[10] = {0};
int Boom_Worker_Flag[10][5] = {0};
int Boom_Distibuter_Flag[4][10] = {0};
int counter[10] = {0};
int Store_Before_Splitting = 0;
int Store_After_Splitting = 0;
float Families_Counters[4][5];
int test = 0;
int time_counter[40][10];
//float sleepTimePlane = 1000000/1.4;
//float sleepTimeWorker = 1000000/8;
//float sleepTimeContainer = 1000000/1.5;

float sleepTime = 400000;


struct ToOpenJl
{
    float Families_Starvation_Rate[4][4];
    int Fly_Plane_Flag[10];
    int store1;
    int store2;
    int timeToStop;
};

struct ContainerToOpenJl
{
    int Container_Move_Flag[10][10];
    int Boombed_Container_Move_Flag[10];
    int Collecting_Move_Flag[10][5];
    int Distibuter_Move_Flag[4][10];
	int Boombed_plan_Move_Flag[10];
	int Receved_Boombed_plan_Move_Flag[10];
};

// function to draw a square player
void drawArea(float x, float y)
{
	glColor3f(0.0, 0.0, 0.0); // Set color to black
	glBegin(GL_QUADS);
	glVertex2f(x - 0.15, y - 0.2);
	glVertex2f(x + 0.15, y - 0.2);
	glVertex2f(x + 0.15, y + 0.2);
	glVertex2f(x - 0.15, y + 0.2);
	glEnd();
}

void DrawContainer(float x, float y)
{
	glColor3f(0.0, 1.0, 0.0); // Green color
	glBegin(GL_QUADS);
	glVertex2f(x - 0.02, y - 0.02);
	glVertex2f(x + 0.02, y - 0.02);
	glVertex2f(x + 0.02, y + 0.02);
	glVertex2f(x - 0.02, y + 0.02);
	glEnd();
}

void DrawWorker(float x, float y)
{
	glColor3f(1.0, 1.0, 1.0); 
	glBegin(GL_QUADS);
	glVertex2f(x - 0.03, y - 0.04);
	glVertex2f(x + 0.03, y - 0.04);
	glVertex2f(x + 0.03, y + 0.04);
	glVertex2f(x - 0.03, y + 0.04);
	glEnd();
}

void DrawDistibutingWorker(float x, float y)
{
	glColor3f(1.0, 1.0, 1.0); // Set color to red
	glBegin(GL_QUADS);
	glVertex2f(x - 0.01, y - 0.05);
	glVertex2f(x + 0.01, y - 0.05);
	glVertex2f(x + 0.01, y + 0.05);
	glVertex2f(x - 0.01, y + 0.05);
	glEnd();
}

void BoomWorker(float x, float y)
{
	glColor3f(1.0, 0.0, 0.0); // Set color to red for the ball
	glPointSize(10.0);		  // Set point size larger for visibility
	glBegin(GL_POINTS);
	glVertex2f(x, y);
	glEnd();
}

void drawBoom(float x, float y)
{
	glColor3f(1.0, 0.0, 0.0); // Set color to red for the ball
	glBegin(GL_QUADS);
	glVertex2f(x - 0.05, y - 0.05);
	glVertex2f(x + 0.05, y - 0.05);
	glVertex2f(x + 0.05, y + 0.05);
	glVertex2f(x - 0.05, y + 0.05);
	glEnd();
}

void Landing_Area(float x, float y)
{
	glColor3f(0.0, 0.0, 0.0); // Set color to black
	glBegin(GL_QUADS);
	glVertex2f(x - 0.03, y - 0.05);
	glVertex2f(x + 0.03, y - 0.05);
	glVertex2f(x + 0.03, y + 0.05);
	glVertex2f(x - 0.03, y + 0.05);
	glEnd();
}

void drawStore(float x, float y)
{
	glColor3f(0.0, 0.0, 0.0); // Set color to black
	glBegin(GL_QUADS);
	glVertex2f(x - 0.05, y - 0.05);
	glVertex2f(x + 0.05, y - 0.05);
	glVertex2f(x + 0.05, y + 0.05);
	glVertex2f(x - 0.05, y + 0.05);
	glEnd();
}
// function to draw a filed
void drawField()
{
	glColor3f(0.5f, 0.5f, 0.5f); // Set color to gray
	glBegin(GL_QUADS);
	glVertex2f(-1, -1);
	glVertex2f(1, -1);
	glVertex2f(1, 1);
	glVertex2f(-1, 1);
	glEnd();
}

void drawPlane(float x, float y)
{
	glColor3f(0.0f, 0.0f, 1.0f); // Set color to blue
	glBegin(GL_QUADS);
	glVertex2f(x - 0.02, y - 0.02);
	glVertex2f(x + 0.02, y - 0.02);
	glVertex2f(x + 0.02, y + 0.02);
	glVertex2f(x - 0.02, y + 0.02);
	glEnd();
}

void drawText(float x, float y, char *text)
{
	// Set text color (white)
	glColor3f(1.0, 1.0, 1.0);

	// Set text position (behind the player)
	float text_x = x - 0.1; // Adjust this value based on your requirement
	float text_y = y - 0.1; // Adjust this value based on your requirement

	// Set text position
	glRasterPos2f(text_x, text_y);

	// Render text
	int len = strlen(text);
	for (int i = 0; i < len; ++i)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
	}
}

void drawCounter(float x, float y, int count)
{
	// Convert integer count to string
	char text[20];
	sprintf(text, "%d", count);

	// Set text color (red)
	glColor3f(1.0, 0.0, 0.0); // Red color

	// Set text position
	float text_x = x - 0.1; // Adjust this value based on your requirement
	float text_y = y - 0.1; // Adjust this value based on your requirement
	glRasterPos2f(text_x, text_y);

	// Render text
	int len = strlen(text);
	for (int i = 0; i < len; ++i)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
	}
}

void drawFloatCounter(float x, float y, float count){
	// Convert integer count to string
	char text[20];
	sprintf(text, "%.2f", count);

	// Set text color (red)
	glColor3f(1.0, 0.0, 0.0); // Red color

	// Set text position
	float text_x = x - 0.1; // Adjust this value based on your requirement
	float text_y = y - 0.1; // Adjust this value based on your requirement
	glRasterPos2f(text_x, text_y);

	// Render text
	int len = strlen(text);
	for (int i = 0; i < len; ++i)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
	}
}

void drawBlackText(float x, float y, char *text)
{
	// Set text color (white)
	glColor3f(0.0, 0.0, 0.0);

	// Set text position (behind the player)
	float text_x = x - 0.1; // Adjust this value based on your requirement
	float text_y = y - 0.1; // Adjust this value based on your requirement

	// Set text position
	glRasterPos2f(text_x, text_y);

	// Render text
	int len = strlen(text);
	for (int i = 0; i < len; ++i)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
	}
}

void drawRedText(float x, float y, char *text)
{
	// Set text color (white)
	glColor3f(1.0, 0.0, 0.0);

	// Set text position (behind the player)
	float text_x = x - 0.1; // Adjust this value based on your requirement
	float text_y = y - 0.1; // Adjust this value based on your requirement

	// Set text position
	glRasterPos2f(text_x, text_y);

	// Render text
	int len = strlen(text);
	for (int i = 0; i < len; ++i)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
	}
}

void drawFrame(float x, float y)
{
    // Set color to gray
    glColor3f(0.5f, 0.5f, 0.5f); 
    
    // Calculate half-width and half-height of the square
    float halfWidth = 0.2f;
    float halfHeight = 0.2f;
    
    // Draw filled square
    glBegin(GL_QUADS);
    glVertex2f(x - halfWidth, y - halfHeight);
    glVertex2f(x + halfWidth, y - halfHeight);
    glVertex2f(x + halfWidth, y + halfHeight);
    glVertex2f(x - halfWidth, y + halfHeight);
    glEnd();
    
    // Set color to black for the frame
    glColor3f(0.0f, 0.0f, 0.0f);
    
    // Draw the frame using lines
    glBegin(GL_LINE_LOOP);
    glVertex2f(x - halfWidth, y - halfHeight);
    glVertex2f(x + halfWidth, y - halfHeight);
    glVertex2f(x + halfWidth, y + halfHeight);
    glVertex2f(x - halfWidth, y + halfHeight);
    glEnd();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	drawField();
	drawFrame(-0.75,0.4);
	drawBlackText(-0.85, 0.95, "Gaza Simulation Game");
	drawBlackText(-0.85, 0.88, "Gaza in ");
	drawRedText(-0.75,0.88,"HEART");
	drawBlackText(-0.85, 0.81, "#Pray for Gaza");

	drawStore(-0.89, 0.54);
	drawBlackText(-0.73, 0.63, "-> Fixed area");
	drawPlane(-0.89, 0.45);
	drawBlackText(-0.73, 0.54, "-> Plane");
	BoomWorker(-0.89, 0.40);
	drawBlackText(-0.73, 0.48, "-> Explosion");
	DrawWorker(-0.89, 0.33);
	drawBlackText(-0.73, 0.41, "-> Workers");
	DrawContainer(-0.89, 0.25);
	drawBlackText(-0.73, 0.33, "-> Container");
	
	drawArea(0.85, 0.8);
	drawText(0.88, 0.9, "Refilling Area");

	drawArea(-0.85, -0.1);
	drawText(-0.82, -0.02, "Splitting Area");

	drawArea(-0.4, -0.8);
	drawText(-0.38, -0.75, "Family camp 1");

	drawArea(0, -0.8);
	drawText(0.02, -0.75, "Family camp 2");

	drawArea(0.4, -0.8);
	drawText(0.42, -0.75, "Family camp 3");

	drawArea(0.8, -0.8);
	drawText(0.82, -0.75, "Family camp 4");

	Landing_Area(0.6, 0.05);
	drawCounter(0.74,0.12,counter[0]);

	Landing_Area(0.5, 0.05);
	drawCounter(0.64,0.12,counter[1]);

	Landing_Area(0.4, 0.05);
	drawCounter(0.54,0.12,counter[2]);

	Landing_Area(0.3, 0.05);
	drawCounter(0.44,0.12,counter[3]);

	Landing_Area(0.2, 0.05);
	drawCounter(0.34,0.12,counter[4]);

	Landing_Area(0.1, 0.05);
	drawCounter(0.24,0.12,counter[5]);

	Landing_Area(0, 0.05);
	drawCounter(0.14,0.12,counter[6]);

	Landing_Area(-0.1, 0.05);
	drawCounter(0.04,0.12,counter[7]);

	Landing_Area(-0.2, 0.05);
	drawCounter(-0.06,0.12,counter[8]);

	Landing_Area(-0.3, 0.05);
	drawCounter(-0.16,0.12,counter[9]);

	drawText(-0.41, 0.12, "Landing Areas");

	drawStore(-0.65, -0.1);
	drawText(-0.58, -0.02, "Store");
	drawCounter(-0.55, 0.08,Store_Before_Splitting);

	drawStore(-0.85, -0.35);
	drawText(-0.78, -0.27, "Store");
	drawCounter(-0.85, -0.27,Store_After_Splitting);

	drawText(-0.9, -0.55, "Family 1 Starvation Rate:");
	drawText(-0.9, -0.63, "Family 2 Starvation Rate:");
	drawText(-0.9, -0.71, "Family 3 Starvation Rate:");
	drawText(-0.9, -0.79, "Family 4 Starvation Rate:");
	

	drawFloatCounter(-0.5, -0.55,Families_Counters[0][0]);
	drawFloatCounter(-0.5, -0.63,Families_Counters[0][1]);
	drawFloatCounter(-0.5, -0.71,Families_Counters[0][2]);
	drawFloatCounter(-0.5, -0.79,Families_Counters[0][3]);


	drawFloatCounter(-0.1, -0.55,Families_Counters[1][0]);
	drawFloatCounter(-0.1, -0.63,Families_Counters[1][1]);
	drawFloatCounter(-0.1, -0.71,Families_Counters[1][2]);
	drawFloatCounter(-0.1, -0.79,Families_Counters[1][3]);
	

	drawFloatCounter(0.3, -0.55,Families_Counters[2][0]);
	drawFloatCounter(0.3, -0.63,Families_Counters[2][1]);
	drawFloatCounter(0.3, -0.71,Families_Counters[2][2]);
	drawFloatCounter(0.3, -0.79,Families_Counters[2][3]);
	

	drawFloatCounter(0.7, -0.55,Families_Counters[3][0]);
	drawFloatCounter(0.7, -0.63,Families_Counters[3][1]);
	drawFloatCounter(0.7, -0.71,Families_Counters[3][2]);
	drawFloatCounter(0.7, -0.79,Families_Counters[3][3]);
	


	for (int i = 0 ; i<maxPlanNum ; i++){
		if (draw_flag[0][i] == 1)
		{
			drawPlane(plane_position[i][0], plane_position[i][1]);
		}
	}
	for (int i = 0 ; i<maxPlanNum ; i++){
		if (draw_flag[1][i] == 1){
			drawPlane(Boombed_plane_position[i][0], Boombed_plane_position[i][1]);
		}
		if (Plane_Boom_Flag[i] == 1){
			drawBoom(Boombed_plane_position[i][0], Boombed_plane_position[i][1]);
			Boombed_plane_position[i][0] = 0.8;
			Plane_Boom_Flag[i] = 0;
		}
		
	}

	for (int i = 0 ; i<maxPlanNum ; i++){
		if (draw_flag[39][i] == 1){
			drawPlane(Reverce_Boombed_plane_position[i][0], Reverce_Boombed_plane_position[i][1]);
		}
		if (Revirce_Plane_Boom_Flag[i] == 1){
			printf("\nEnterrrrrr open\n\n");
			Revirce_Plane_Boom_Flag[i] = 0;
			drawBoom(Reverce_Boombed_plane_position[i][0], Reverce_Boombed_plane_position[i][1]);
			Reverce_Boombed_plane_position[i][0] = 0.8;
			
		}
	}
	
	for (int i = 0 ; i<10 ; i++){
		for (int j = 0 ; j<maxContainerNum ; j++){
			if (draw_flag[2+i][j] == 1){
				DrawContainer(Container_Position[i][j][0], Container_Position[i][j][1]);
			}
		}	
	}
	for (int i = 0 ; i<10 ; i++){
		if (draw_flag[13][i] == 1){
			DrawContainer(Boombed_Container_position[i][0], Boombed_Container_position[i][1]);
		}
		if (Container_Boom_Flag[i] == 1){
			drawBoom(Boombed_Container_position[i][0], Boombed_Container_position[i][1]);
			Boombed_Container_position[i][1] = 0.9;
			Container_Boom_Flag[i] = 0;
		}
	}
	for (int i = 0 ; i<10 ; i++){
		for (int j = 0 ; j<5 ; j++){
			if (draw_flag[14 + i][j] == 1){
				DrawWorker(Collecting_Worker_Position[i][j][0], Collecting_Worker_Position[i][j][1]);
			}
			if(Boom_Worker_Flag[i][j] == 1){
				BoomWorker(Collecting_Worker_Position[i][j][0], Collecting_Worker_Position[i][j][1]);
				Boom_Worker_Flag[i][j] = 0;
				Collecting_Worker_Position[i][j][0] = -0.6;
				Collecting_Worker_Position[i][j][1] = -0.1;
			}
		}	
	}

	for (int i = 0 ; i<4 ; i++){
		for (int j = 0 ; j<10 ; j++){
			if (draw_flag[24 + i][j] == 1){
				DrawDistibutingWorker(Distibuting_Worker_Position[i][j][0], Distibuting_Worker_Position[i][j][1]);
			}
			if(Boom_Distibuter_Flag[i][j] == 1){
				BoomWorker(Distibuting_Worker_Position[i][j][0], Distibuting_Worker_Position[i][j][1]);
				Boom_Distibuter_Flag[i][j] = 0;
				draw_flag[24 + i][j] = 0;
				ctrl[2][j] = 0;
				Distibuting_Worker_Position[i][j][0] = -0.85;
				Distibuting_Worker_Position[i][j][1] = -0.5;
				
			}
		}	
	}

	glFlush();
}

void Plane_Move(int i)
{
	draw_flag[0][i] = 1;
	switch (ctrl[0][i])
	{
	case 0:
		plane_position[i][0] -= 0.1;
		test += 1;
		if (plane_position[i][0] <= -0.3)
		{
			ctrl[0][i] = 1;
		}
		break;
	case 1:
		plane_position[i][0] += 0.1;
		if (plane_position[i][0] >= 0.8)
		{
			draw_flag[0][i] = 0;
			ctrl[0][i] = 0;
			plane_position[i][0] = 0.8;
			plane_position[i][1] = 0.9;
			
		}
		break;
	}

	glutPostRedisplay(); // Request redrawing
	usleep(sleepTime);			 // Adjust the speed of ball movement (21 microseconds = 0.05 seconds)
}

void Boombed_Plane_Move(int i)
{
	float Threshold;
    if (i == 0){
		Threshold = 0.6;
	}
	if (i == 1){
		Threshold = 0.5;
	}
	if (i == 2){
		Threshold = 0.4;
	}
	if (i == 3){
		Threshold = 0.3;
	}
	if (i == 4){
		Threshold = 0.2;
	}
	if (i == 5){
		Threshold = 0.1;
	}
	if (i == 6){
		Threshold = 0;
	}
	if (i == 7){
		Threshold = -0.1;
	}
	if (i == 8){
		Threshold = -0.2;
	}
	if (i == 9){
		Threshold = -0.3;
	}
	draw_flag[1][i] = 1;
	Boombed_plane_position[i][0] -= 0.1;
	if (Boombed_plane_position[i][0] < Threshold)
	{
		draw_flag[1][i] = 0;
		Plane_Boom_Flag[i] = 1;

		struct ContainerToOpenJl *ContainerInfo;
		if ((ContainerShmid = shmget(99999, sizeof(struct ContainerToOpenJl), IPC_CREAT | 0666)) < 0)
		{
			printf("in container code\n");
			perror("shmget fail");
			exit(1);
		}

		ContainerInfo = (struct ContainerToOpenJl *)shmat(ContainerShmid ,0 ,0);
		ContainerInfo[0].Boombed_plan_Move_Flag[i] = 0;
	}

	glutPostRedisplay(); // Request redrawing
	usleep(sleepTime);			 // Adjust the speed of ball movement (21 microseconds = 0.05 seconds)
}

void Riverce_Boombed_Plane_Move(int i)
{
	float Threshold;
    if (i == 0){
		Threshold = 0.6;
	}
	if (i == 1){
		Threshold = 0.5;
	}
	if (i == 2){
		Threshold = 0.4;
	}
	if (i == 3){
		Threshold = 0.3;
	}
	if (i == 4){
		Threshold = 0.2;
	}
	if (i == 5){
		Threshold = 0.1;
	}
	if (i == 6){
		Threshold = 0;
	}
	if (i == 7){
		Threshold = -0.1;
	}
	if (i == 8){
		Threshold = -0.2;
	}
	if (i == 9){
		Threshold = -0.3;
	}
	

	draw_flag[39][i] = 1;
	switch (ctrl[39][i])
	{
	case 0:
		Reverce_Boombed_plane_position[i][0] -= 0.1;
		test += 1;
		if (Reverce_Boombed_plane_position[i][0] <= -0.3)
		{
			ctrl[39][i] = 1;
		}
		break;
	case 1:
		Reverce_Boombed_plane_position[i][0] += 0.1;
		
		if (Reverce_Boombed_plane_position[i][0] > Threshold && Reverce_Boombed_plane_position[i][0] < Threshold+1)
		{
			ctrl[39][i]=0;
			draw_flag[39][i] = 0;
			printf("i: %d\n\n",i);
			Revirce_Plane_Boom_Flag[i] = 1;

			struct ContainerToOpenJl *ContainerInfo;
			if ((ContainerShmid = shmget(99999, sizeof(struct ContainerToOpenJl), IPC_CREAT | 0666)) < 0)
			{
				printf("in container code\n");
				perror("shmget fail");
				exit(1);
			}

			ContainerInfo = (struct ContainerToOpenJl *)shmat(ContainerShmid ,0 ,0);
			ContainerInfo[0].Receved_Boombed_plan_Move_Flag[i] = 0;

		}
		break;
	}

	glutPostRedisplay(); // Request redrawing
	usleep(sleepTime);			 // Adjust the speed of ball movement (21 microseconds = 0.05 seconds)
}

void Container_Move(int area, int i){
	draw_flag[2+area][i] = 1;

	Container_Position[area][i][1] -= 0.08;
	if (Container_Position[area][i][1] <= 0.05)
	{
		counter[area]++;
		draw_flag[2+area][i] = 0;
		Container_Position[area][i][1] = 0.9;
		struct ContainerToOpenJl *ContainerInfo;
		if ((ContainerShmid = shmget(99999, sizeof(struct ContainerToOpenJl), IPC_CREAT | 0666)) < 0)
		{
			printf("in container code\n");
			perror("shmget fail");
			exit(1);
		}

		ContainerInfo = (struct ContainerToOpenJl *)shmat(ContainerShmid ,0 ,0);
		ContainerInfo[0].Container_Move_Flag[area][i] = 0;

	}

	glutPostRedisplay(); // Request redrawing
	usleep(sleepTime);			 // Adjust the speed of ball movement (21 microseconds = 0.05 seconds)
}

void Boombed_Container_Move(int i){
	draw_flag[13][i] = 1;

	Boombed_Container_position[i][1] -= 0.1;
	if (Boombed_Container_position[i][1] <= 0.3)
	{
		draw_flag[13][i] = 0;
		Container_Boom_Flag[i] = 1;
		struct ContainerToOpenJl *ContainerInfo;
		if ((ContainerShmid = shmget(99999, sizeof(struct ContainerToOpenJl), IPC_CREAT | 0666)) < 0)
		{
			printf("in container code\n");
			perror("shmget fail");
			exit(1);
		}

		ContainerInfo = (struct ContainerToOpenJl *)shmat(ContainerShmid ,0 ,0);
		ContainerInfo[0].Boombed_Container_Move_Flag[i]= 0;
	}

	glutPostRedisplay(); // Request redrawing
	usleep(sleepTime);			 // Adjust the speed of ball movement (21 microseconds = 0.05 seconds)
}

void Collecting_Worker_Move(int area, int i, int Boombed)
{
	float threshold;
	float Boom_threshold;
	int Counter_Pos;
	if(area == 0){
		threshold = -0.35;
		Boom_threshold = -0.4;
		Counter_Pos = 9;
	}
	if(area == 1){
		threshold = -0.25;
		Boom_threshold = -0.3;
		Counter_Pos = 8;
	}
	if(area == 2){
		threshold = -0.15;
		Boom_threshold = -0.2;
		Counter_Pos = 7;
	}
	if(area == 3){
		threshold = -0.05;
		Boom_threshold = -0.1;
		Counter_Pos = 6;
	}
	if(area == 4){
		threshold = 0.05;
		Boom_threshold = 0;
		Counter_Pos = 5;
	}
	if(area == 5){
		threshold = 0.15;
		Boom_threshold = 0.1;
		Counter_Pos = 4;
	}
	if(area == 6){
		threshold = 0.25;
		Boom_threshold = 0.2;
		Counter_Pos = 3;
	}
	if(area == 7){
		threshold = 0.35;
		Boom_threshold = 0.3;
		Counter_Pos = 2;
	}
	if(area == 8){
		threshold = 0.45;
		Boom_threshold = 0.4;
		Counter_Pos = 1;
	}
	if(area == 9){
		threshold = 0.55;
		Boom_threshold = 0.5;
		Counter_Pos = 0;
	}
	draw_flag[14 + area][i] = 1;
	
	switch (ctrl[1][i])
	{
	case 0:
		Collecting_Worker_Position[area][i][0] += 0.1;
		if(Boombed == 1){
			if (Collecting_Worker_Position[area][i][0] >= Boom_threshold)
			{
				Boom_Worker_Flag[area][i] = 1;
			}
		}	
		if (Collecting_Worker_Position[area][i][0] >= threshold )
		{
			if(Boombed != 1){
				counter[Counter_Pos]--;
			}
			ctrl[1][i] = 1;
		}
		break;
	case 1:
		Collecting_Worker_Position[area][i][0] -= 0.1;
		if (Collecting_Worker_Position[area][i][0] <= -0.65)
		{
			draw_flag[14 + area][i] = 0;
			ctrl[1][i] = 0;
			Collecting_Worker_Position[area][i][0] = -0.6;
			Collecting_Worker_Position[area][i][1] = -0.1;
			struct ContainerToOpenJl *ContainerInfo;
			if ((ContainerShmid = shmget(99999, sizeof(struct ContainerToOpenJl), IPC_CREAT | 0666)) < 0)
			{
				printf("in container code\n");
				perror("shmget fail");
				exit(1);
			}

			ContainerInfo = (struct ContainerToOpenJl *)shmat(ContainerShmid ,0 ,0);
			ContainerInfo[0].Collecting_Move_Flag[area][i]= 0;
			
		}
		break;
	}

	glutPostRedisplay(); 
	usleep(sleepTime); 
}

void Distibuting_Worker_Move(int area, int i, int Boombed)
{
	float threshold;
	float Boom_threshold;
	if(area == 0){
		threshold = -0.4;
		Boom_threshold = -0.6;
	}
	if(area == 1){
		threshold = 0;
		Boom_threshold = -0.2; 
	}
	if(area == 2){
		threshold = 0.4;
		Boom_threshold = 0.2;
	}
	if(area == 3){
		threshold = 0.8;
		Boom_threshold = 0.6;
	}
	
	draw_flag[24 + area][i] = 1;
	
	switch (ctrl[2][i])
	{
	case 0:
		Distibuting_Worker_Position[area][i][0] += 0.1;
		if(Boombed == 1){
			if (Distibuting_Worker_Position[area][i][0] >= Boom_threshold)
			{
				Boom_Distibuter_Flag[area][i] = 1;
			}
		}
		
		if (Distibuting_Worker_Position[area][i][0] >= threshold)
		{
			ctrl[2][i] = 1;
		}
		break;
	case 1:
		Distibuting_Worker_Position[area][i][0] -= 0.1;
		if (Distibuting_Worker_Position[area][i][0] <= -0.9)
		{
			draw_flag[24 + area][i] = 0;
			ctrl[2][i] = 0;
			Distibuting_Worker_Position[area][i][0] = -0.85;
			Distibuting_Worker_Position[area][i][1] = -0.5;
			struct ContainerToOpenJl *ContainerInfo;
			if ((ContainerShmid = shmget(99999, sizeof(struct ContainerToOpenJl), IPC_CREAT | 0666)) < 0)
			{
				printf("in container code\n");
				perror("shmget fail");
				exit(1);
			}

			ContainerInfo = (struct ContainerToOpenJl *)shmat(ContainerShmid ,0 ,0);
			ContainerInfo[0].Distibuter_Move_Flag[area][i] = 0;
			
		}
		break;
	}

	glutPostRedisplay(); // Request redrawing
	usleep(sleepTime);			 // Adjust the speed of ball movement (21 microseconds = 0.05 seconds)
}

void updatePosition()
{
	
	struct ToOpenJl *Info;
	
	if ((Shmid = shmget(123123, sizeof(struct ToOpenJl), IPC_CREAT | 0666)) < 0)
	{
		printf("in open\n");
		perror("shmget fail");
		exit(1);
	}
	Info = (struct ToOpenJl *)shmat(Shmid ,0 ,0);

	struct ContainerToOpenJl *ContainerInfo;
	if ((ContainerShmid = shmget(99999, sizeof(struct ContainerToOpenJl), IPC_CREAT | 0666)) < 0)
    {
        printf("in container code\n");
        perror("shmget fail");
        exit(1);
    }

    ContainerInfo = (struct ContainerToOpenJl *)shmat(ContainerShmid ,0 ,0);
    

	glutPostRedisplay();
////////////////////////////	
	for(int i=0 ; i<4 ; i++){
		for(int j=0 ; j<4 ; j++){
			Families_Counters[i][j] = Info[0].Families_Starvation_Rate[i][j];
		}
	}
//////////////////////////////
	for (int i = 0; i < 10; ++i) {
		if(Info[0].Fly_Plane_Flag[i] == 1){
			Plane_Move(i);
			time_counter[0][i] = 1;
		}
		else{
			time_counter[0][i] = 0;
		}
    }
//////////////////////////////
	for (int i = 0; i < 10; ++i) {
		if(ContainerInfo[0].Boombed_plan_Move_Flag[i] == 1){
			Boombed_Plane_Move(i);
			time_counter[0][i] = 1;
		}
		else{
			time_counter[0][i] = 0;
		}
    }	
//////////////////////////////
	for (int i = 0; i < 10; ++i) {
		if(ContainerInfo[0].Receved_Boombed_plan_Move_Flag[i] == 1){
			Riverce_Boombed_Plane_Move(i);
			time_counter[0][i] = 1;
		}
		else{
			time_counter[0][i] = 0;
		}
    }		
/////////////////////////////////
	for (int i = 0; i < 10; ++i) {
		if(ContainerInfo[0].Boombed_Container_Move_Flag[i] == 1){
			Boombed_Container_Move(i);
			time_counter[1][i] = 1;
		}
		else{
			time_counter[1][i] = 0;
		}
    }
/////////////////////////////
	Store_Before_Splitting = Info[0].store1;
	Store_After_Splitting = Info[0].store2;
////////////////////////////////////
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j) {
			if(ContainerInfo[0].Container_Move_Flag[i][j] == 1 ){
				Container_Move(i,j);
				time_counter[2 + i][j] = 1;
			}
			else{
				time_counter[2 + i][j] = 0;
			}
    	}
    }
////////////////////////////////////////
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 5; ++j) {
			if(ContainerInfo[0].Collecting_Move_Flag[i][j] == 1 ){
				Collecting_Worker_Move(i,j,0);
				time_counter[12 + i][j] = 1;
			}
			else if(ContainerInfo[0].Collecting_Move_Flag[i][j] == 2 ){
				Collecting_Worker_Move(i,j,1);
				time_counter[12 + i][j] = 1;
			}
			else{
				time_counter[12 + i][j] = 0;
			}
    	}
    }
/////////////////////////////////////////
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 10; ++j) {
			if(ContainerInfo[0].Distibuter_Move_Flag[i][j] == 1 ){
				Distibuting_Worker_Move(i,j,0);
				time_counter[22 + i][j] = 1;
			}
			else if(ContainerInfo[0].Distibuter_Move_Flag[i][j] == 2 ){
				Distibuting_Worker_Move(i,j,1);
				time_counter[22 + i][j] = 1;
			}
			else{
				time_counter[22 + i][j] = 0;
			}
    	}
    }

	int DivideToTime = 0;
	for (int i = 0; i < 30; ++i) {
		for (int j = 0; j < 10; ++j) {
			DivideToTime += time_counter[i][j];
    	}
    }
	if (DivideToTime == 0){
		sleepTime = 400000;
	}
	else{
		sleepTime = 400000 / (float)DivideToTime;
	}
	
	//Riverce_Boombed_Plane_Move(8);
	//Boombed_Plane_Move(5);
	//glutPostRedisplay();

}

// draw background and Set it color to white
void init()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	gluOrtho2D(-1, 1, -1, 1);
}

void RemoveShm(int n);
int main(int argc, char **argv)
{
	signal(SIGINT, RemoveShm);
	
	float x_axis_val = 0.6;
	for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < maxContainerNum; ++j) {
            // Assigning the values {0.6, 0.9} to each element
            Container_Position[i][j][0] = x_axis_val;
            Container_Position[i][j][1] = 0.9;
        }
		x_axis_val -= 0.1;
    }

	float Boombed_x_axis_val = 0.6;
	for (int i = 0; i < 10; ++i) {
		// Assigning the values {0.6, 0.9} to each element
		Boombed_Container_position[i][0] = Boombed_x_axis_val;
		Boombed_Container_position[i][1] = 0.9;
        
		Boombed_x_axis_val -= 0.1;
    }

	for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 5; ++j) {
            Collecting_Worker_Position[i][j][0] = -0.7;
            Collecting_Worker_Position[i][j][1] = -0.1;
        }
    }

	for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 10; ++j) {
            Distibuting_Worker_Position[i][j][0] = -0.85;
            Distibuting_Worker_Position[i][j][1] = -0.5;
        }
    }

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(1700, 900);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Gaza :(");

	init();
	glutDisplayFunc(display);
	glutIdleFunc(updatePosition);
	glutMainLoop();

	return 0;
}

void RemoveShm(int n){
	int d1 = shmctl(Shmid, IPC_RMID, NULL);
    if (d1 == -1)
    {
        perror("shmctl");
        exit(1);
    }
	int d2 = shmctl(ContainerShmid, IPC_RMID, NULL);
    if (d2 == -1)
    {
        perror("shmctl");
        exit(1);
    }
	exit(0);
}
