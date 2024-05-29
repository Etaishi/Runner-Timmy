#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include "rlgl.h"

typedef struct{
    Model bariyer;
    BoundingBox bariyerBoundingBox;
    Vector3 positionBariyer;
    Vector3 Scale;
    Vector3 RotationAxis;
    float RotationAngle;
    bool gecis;
    bool draw;
}Engel;

typedef struct Part{
    Engel engeller[3];
    Vector3 positionPart;
    struct Part* nextPart;
}Part;

typedef struct Agac{
    Model agacModel;
    Vector3 positionAgac;
    struct Agac* nextAgac;
}Agac;

int isEmptyAgac(Agac* headAgac){
return headAgac == NULL;
}

void generateAgac(Agac** headAgac, Agac** tailAgac,int aralıkAgac, Model agacModel){
    Agac* newAgac;
    newAgac = malloc(sizeof(Agac));
    if (newAgac != NULL) {
        newAgac->nextAgac = NULL;
        newAgac->agacModel = agacModel;
        newAgac->positionAgac.z = 5*aralıkAgac;
        newAgac->positionAgac.x = -4.5;
        newAgac->positionAgac.y = 0;
        if(isEmptyAgac(*headAgac))
        *headAgac = newAgac;
        else
        (*tailAgac)->nextAgac = newAgac;
        *tailAgac = newAgac;
    }
};

void dequeueAgac(Agac** headAgac, Agac** tailAgac)
{
Agac* tempPtr;
tempPtr = *headAgac;
*headAgac = (*headAgac)->nextAgac;
if (*headAgac == NULL)
*tailAgac = NULL;
free(tempPtr);
}

void dequeue(Part** headPart, Part** tailPart)
{
Part* tempPtr;
tempPtr = *headPart;
*headPart = (*headPart)->nextPart;
if (*headPart == NULL)
*tailPart = NULL;
free(tempPtr);
}

int isEmpty(Part* headPart){
return headPart == NULL;
}

void generatePart(Part** headPart, Part** tailPart,Engel generatedPart[3],Engel engeller[4],float lane[3],int aralıkPart){
    for(int i=0;i<3;i++){
        int rast=GetRandomValue(0,3);     
        generatedPart[i] = engeller[rast];
        generatedPart[i].positionBariyer.x = lane[i];
    }
    Part* newPart;
    newPart = malloc(sizeof(Part));
    if (newPart != NULL) {
        newPart->nextPart = NULL;
        newPart->positionPart.z = 12*aralıkPart;
        for (int i = 0; i < 3; i++) {
        newPart->engeller[i] = generatedPart[i];
        newPart->engeller[i].positionBariyer.x = lane[i];
        //boundingbox ataması
        newPart->engeller[i].bariyerBoundingBox.min.x += newPart->engeller[i].positionBariyer.x ;
        newPart->engeller[i].bariyerBoundingBox.min.y += newPart->engeller[i].positionBariyer.y ;
        newPart->engeller[i].bariyerBoundingBox.min.z += newPart->positionPart.z ; 
        newPart->engeller[i].bariyerBoundingBox.max.x += newPart->engeller[i].positionBariyer.x ;
        newPart->engeller[i].bariyerBoundingBox.max.y += newPart->engeller[i].positionBariyer.y ;
        newPart->engeller[i].bariyerBoundingBox.max.z += newPart->positionPart.z ;
        }
        if (isEmpty(*headPart))
        *headPart = newPart;
        else
        (*tailPart)->nextPart = newPart;
        *tailPart = newPart;
    }
}

void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color);

void DrawingConditions(Part **currentPart,Part **headPart); 

// FARKLI EKRANLAR İÇİN ENUM TANIMLAMA İŞLEMİ
typedef enum { MAIN_MENU, ABOUT , START_GAME , PAUSE , END_GAME } Screen;

int main(void)
{
    const int screenWidth = 1600;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Runner Timmy");
    InitAudioDevice();              

    Music music = LoadMusicStream("31_subwaysurfers_modeldosyası/31_subwaysurfers_music.mp3");
    //Music efekt = LoadMusicStream("31_subwaysurfers_modeldosyası/31_subwaysurfers_efekt.mp3");

Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 4.0f, -3.5f };    // Camera position
    camera.target = (Vector3){ 0.0f, 1.5f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 5.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    Model timmy = LoadModel("31_subwaysurfers_modeldosyası/31_subwaysurfers_timmy2.glb");// timmynin modelinin yüklenmesi
    float rotationAngle = 90.0f;  
    Vector3 scale = {0.01f,0.01f,0.01f}; //timmynin boyutu
    Vector3 rotationAxis = {1.0f,0.0f,0.0f};
    Vector3 position = { 0.0f, 0.0f, 0.0f }; //timmynin konumu
    BoundingBox  timmyBoundingBox = {
    (Vector3) {
    position.x -0.25, //BOUNDİNGBOX UN MİN DEGELERLERİ
    0,
    -0.25
},
    (Vector3) {
    position.x + 0.25,
    1.5,    // BOUİNDİNG BOXUN MAX DEGERLERİ
    0.25
    }
} ; 

    Model agacModel = LoadModel("31_subwaysurfers_modeldosyası/31_subwaysurfers_agac.glb");// agac modelinin yüklenmesi
    float agacrotationAngle = -90.0f;  
    Vector3 agacscale = {0.2f,0.2f,0.2f}; //agac boyut
    Vector3 agacrotationAxis = {1.0f,0.0f,0.0f};
    
    /****** RAİL MODEL *******/
    
    Texture2D railTex =LoadTexture("31_subwaysurfers_modeldosyası/31_subwaysurfers_rail.png");
    Model railModel = LoadModelFromMesh(GenMeshCube(2.0f, 1.0f, 2.0f));
    railModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = railTex;
    Vector3 railSol ={ -3.0f, -0.5f, -10.0f};
    Vector3 railOrta ={ 0.0f, -0.5f, -10.0f};
    Vector3 railSag ={ 3.0f, -0.5f, -10.0f};
    int numRailSegments = 1000;
    float railSegmentSpacing = 2.0f;
    
    //bariyerler
    //--------------------------------------------------------------------
    Model altModel = LoadModel("31_subwaysurfers_modeldosyası/31_subwaysurfers_ust2.obj");
    Texture2D altTex = LoadTexture("31_subwaysurfers_modeldosyası/31_subwaysurfers_ust.png");
    altModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = altTex;
    float altRotationAngle = 0.0f;
    Vector3 altScale ={16.0f, 10.0f, 14.0f}; //büyüklük
    Vector3 altPosition ={-3.5f, 0.0f, 3.0f}; 
    Vector3 altRotationAxis ={0.0f, 0.5f, 90.0f};

    Model ustModel = LoadModel("31_subwaysurfers_modeldosyası/31_subwaysurfers_ust1.obj");
    Texture2D ustTex = LoadTexture("31_subwaysurfers_modeldosyası/31_subwaysurfers_ust.png");
    ustModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = ustTex;
    float ustRotationAngle = 0.0f;
    Vector3 ustScale ={16.0f, 9.0f, 14.0f};
    Vector3 ustPosition ={0.0f, 0.0f, 0.0f};
    Vector3 ustRotationAxis ={0.0f, 0.0f, 0.0f};

    Model altustModel = LoadModel("31_subwaysurfers_modeldosyası/31_subwaysurfers_altustdeneme.obj");
    Texture2D altustTex = LoadTexture("31_subwaysurfers_modeldosyası/31_subwaysurfers_altustdeneme.png");
    altustModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = altustTex;
    float altustRotationAngle = 0.0f;
    Vector3 altustScale ={16.0f, 15.0f, 14.0f};
    Vector3 altustPosition ={0.0f, 0.8f, 0.0f};
    Vector3 altustRotationAxis ={0.0f, 1.0f, 0.0f};

    Model trenModel = LoadModel("31_subwaysurfers_modeldosyası/31_subwaysurfers_trendeneme.obj");
    Texture2D trenTex = LoadTexture("31_subwaysurfers_modeldosyası/31_subwaysurfers_Subway_Car_DIFF.png");
    trenModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = trenTex;
    float trenRotationAngle = 0.0f;
    Vector3 trenScale ={0.007f, 0.007f, 0.0035f};
    Vector3 trenPosition ={0.0f, 0.0f, 0.0f};
    Vector3 trenRotationAxis ={1.0f, 0.0f, 0.0f};
    //--------------------------------------------------------------------
    int animsCount = 0; //modeldeki animasyon sayısı
    unsigned int animIndex = 0; //kaçıncı animasyonunun oynadığını belirten değişken
    unsigned int animCurrentFrame = 0; //animasyonun kaçıncı karede olduğunu tutan değişken
    ModelAnimation *modelAnimations = LoadModelAnimations("31_subwaysurfers_modeldosyası/31_subwaysurfers_timmy2.glb", &animsCount);

            //boundingboxlar
            //boundingboxlar
        BoundingBox  altBoundingBox = {
            (Vector3) {
            2.0,
            1,
            0.8
        },
            (Vector3) {
            5.0,
            3,
            1.1
            }
        } ;

            BoundingBox  altustBoundingBox = {
            (Vector3) {
            2,
            -0.2,
            0.8
        },
            (Vector3) {
            5,
            0.0,
            1.1
            }
        } ;

            BoundingBox  ustBoundingBox = {
            (Vector3) {
            2,
            0,
            0.8
        },
            (Vector3) {
            5,
            0.8,
            1.1
            }
        } ;

            BoundingBox  trenBoundingBox = {
            (Vector3) {
            2,
            0,
            - 6
        },
            (Vector3) {
            5,
            2,
            0.8
            }
        } ;
    //--------------------------------------------------------------------
    //engel değişkenlerinin tanımlanması
    Part *headPart = NULL, *tailPart = NULL, *currentPart = headPart;
    float lane[3] = {-0.5f, -3.5f, -6.5f};
    Engel alt = {altModel,altBoundingBox,altPosition,altScale,altRotationAxis,altRotationAngle,true,true};
    Engel ust = {ustModel,ustBoundingBox,ustPosition,ustScale,ustRotationAxis,ustRotationAngle,true,true};
    Engel altust = {altustModel,altustBoundingBox,altustPosition,altustScale,altustRotationAxis,altustRotationAngle,true,true};
    Engel tren = {trenModel,trenBoundingBox,trenPosition,trenScale,trenRotationAxis,trenRotationAngle,false,true};
    Engel engeller[4] = {alt,ust,altust,tren};
    Engel generatedPart[3] = {};

    Agac *headAgac = NULL, *tailAgac = NULL, *currentAgac = headAgac;
    
    int startTime;
    int puan = 0;
    float puanArtis = 1.0f;
    float speed;
    bool jump =false;
    bool sol=false;                     
    bool sag=false;
    bool asagi=false;
    int jumpTimer;
    int sagTimer;
    int solTimer;
    int asagiTimer;

    SetTargetFPS(60);
    
    Rectangle startButtonBounds = { screenWidth / 2 - 100, screenHeight / 2 - 100, 200, 50 };    //BUTONLARIN KOORDİNATLARINI TANIMLIYORUZ
    Rectangle aboutButtonBounds = { screenWidth / 2 - 100, screenHeight / 2 - 25, 200, 50 };    //BUTONLARIN KOORDİNATLARINI TANIMLIYORUZ
    Rectangle quitButtonBounds = { screenWidth / 2 - 100, screenHeight / 2 + 50, 200, 50 };    //BUTONLARIN KOORDİNATLARINI TANIMLIYORUZ

    bool startButtonPressed = false;
    bool aboutButtonPressed = false;
    bool quitButtonPressed = false;

    Screen currentScreen = MAIN_MENU;

    bool pauseOpen = false;
    bool gamePaused = false; 

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKBLUE);
        UpdateMusicStream(music);
        //UpdateMusicStream(efekt);
        
        PlayMusicStream(music);

        if (currentScreen == MAIN_MENU)
        {
            // MAIN MENU YÜ ÇİZDİRİYORUZ
            DrawText("Runner Timmy", 450, 200, 100, BLACK);
            
            if (startButtonPressed)
                DrawRectangleRec(startButtonBounds, MAROON);
            else
                DrawRectangleRec(startButtonBounds, LIGHTGRAY);
            
            DrawText("Start Game", startButtonBounds.x + startButtonBounds.width / 2 - MeasureText("Start Game", 20) / 2,
                     startButtonBounds.y + startButtonBounds.height / 2 - 10, 20, BLACK);

            if (aboutButtonPressed)
                DrawRectangleRec(aboutButtonBounds, MAROON);
            else
                DrawRectangleRec(aboutButtonBounds, LIGHTGRAY);
            
            DrawText("About", aboutButtonBounds.x + aboutButtonBounds.width / 2 - MeasureText("About", 20) / 2,
                     aboutButtonBounds.y + aboutButtonBounds.height / 2 - 10, 20, BLACK);

            if (quitButtonPressed)
                DrawRectangleRec(quitButtonBounds, MAROON);
            else
                DrawRectangleRec(quitButtonBounds, LIGHTGRAY);
            
            DrawText("Quit", quitButtonBounds.x + quitButtonBounds.width / 2 - MeasureText("Quit", 20) / 2,
                     quitButtonBounds.y + quitButtonBounds.height / 2 - 10, 20, BLACK);
        }
        else if (currentScreen == PAUSE)
        {
            DrawText(TextFormat("Puan: %d", puan), 700 , (-70) + screenHeight/2, 40, BLACK);
            DrawText(TextFormat("PAUSED" ), screenWidth -200 , 60, 40, BLACK);
        }

        

        // Oyun durdurulmuşsa, sadece pause menüsünü işle
        if (currentScreen == PAUSE)
        {
            if (IsKeyPressed(KEY_P))
            {
                // Pause menüsü kapanacak
                pauseOpen = false;
                // Oyun devam ediyor
                gamePaused = false;
                currentScreen = START_GAME;
            }
        }
        else
        {
            // Oyun başladığında veya pause menüsü kapandığında kontrolleri yap
            if (currentScreen == MAIN_MENU)
            {
                if (CheckCollisionPointRec(GetMousePosition(), startButtonBounds))
                {
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                    {
                        startButtonPressed = true;
                    }
                    else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && startButtonPressed)
                    {
                        startButtonPressed = false;
                        currentScreen = START_GAME;
                    }
                }
                else
                {
                    startButtonPressed = false;
                }

                if (CheckCollisionPointRec(GetMousePosition(), aboutButtonBounds))
                {
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                    {
                        aboutButtonPressed = true;
                    }
                    else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && aboutButtonPressed)
                    {
                        currentScreen = ABOUT;
                        aboutButtonPressed = false;
                    }
                }
                else
                {
                    aboutButtonPressed = false;
                }

                if (CheckCollisionPointRec(GetMousePosition(), quitButtonBounds))
                {
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                    {
                        quitButtonPressed = true;
                    }
                    else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && quitButtonPressed)
                    {
                        CloseWindow();
                        quitButtonPressed = false;
                    }
                }
                else
                {
                    quitButtonPressed = false;
                }
            }
            else if (currentScreen == ABOUT)
            {
                DrawText("2024 goty ?", 100, 100, 50, BLACK);
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {
                    currentScreen = MAIN_MENU;
                }
            }
            else if(currentScreen == START_GAME){                                   
            puan +=puanArtis;
        
        // hangi animasyonun oynayacağı seçiliyor
        //if (IsKeyPressed(KEY_W)) animIndex = 0;
        if (IsKeyPressed(KEY_SPACE)) animIndex = 1;
        if (IsKeyPressed(KEY_A)) animIndex = 2;
        if (IsKeyPressed(KEY_D)) animIndex = 3;
        if (IsKeyPressed(KEY_S)) animIndex = 4;
        
        ModelAnimation anim = modelAnimations[animIndex];//seçilen animasyon verilerinin tutulduğu değişken
        animCurrentFrame = (animCurrentFrame + 1)%anim.frameCount;//anim.framecount--> animasyonun kaç frameden oluştuğunu tutuyor
        UpdateModelAnimation(timmy, anim, animCurrentFrame);//model animasyona göre update ediliyor

        if(headAgac==NULL){
        for(int i=1;i<10;i++){
        generateAgac(&headAgac,&tailAgac,i,agacModel);}}

        if(headAgac->positionAgac.z<(-3.0f)){
            generateAgac(&headAgac,&tailAgac,9,agacModel);
            dequeueAgac(&headAgac,&tailAgac);
        }
        
        if(headPart==NULL){
        for(int i=1;i<5;i++){
        generatePart(&headPart,&tailPart,generatedPart,engeller,lane,i);}}
        
        if(headPart->positionPart.z<(-12.0f)){
        generatePart(&headPart,&tailPart,generatedPart,engeller,lane,3);
        dequeue(&headPart,&tailPart);}

        DrawingConditions(&currentPart,&headPart);
        
        if (IsKeyPressed(KEY_SPACE) && !jump) {
                        jump = true;
                        jumpTimer = 0;
                        //PlayMusicStream(efekt);
                    }

                        if (jump) {
                        jumpTimer++;

                        if (jumpTimer <= 20) {
                            position.y += 0.05f; // Zıplama hızı
                            timmyBoundingBox.min.y = position.y + 1.0f;
                            timmyBoundingBox.max.y = position.y + 1.0f;

                            } 
                        else if (jumpTimer >20 && jumpTimer <40) { 

                        }
                        else if(jumpTimer >= 20 && jumpTimer <60){
                            position.y -= 0.05f; 
                            timmyBoundingBox.min.y = 0;
                            timmyBoundingBox.max.y = 1.5;
                        }
                    

                    if (jumpTimer >= 60) {
                        jump = false;
                        jumpTimer = 0;
                        animIndex = 0;
                        //PlayMusicStream(efekt);
                    }
                        }
            
            
            if(IsKeyPressed(KEY_A)){
                sol =true;
                //PlayMusicStream(efekt);
            }
            if(sol){
                solTimer +=1 ;
                    if(solTimer <= 10){
                        position.x +=0.3f; /* LANE ler arası ışınlanma olmaması için jump algoritmasının aynısından yazdık */
                        timmyBoundingBox.min.x += 0.3f;
                        timmyBoundingBox.max.x += 0.3f;
                        
                        camera.position.x +=0.3f;
                        camera.target.x += 0.3f;
            }
            }
            if(solTimer>=11){
                solTimer=0;
                sol=false;
                //PlayMusicStream(efekt);
                animIndex = 0;
            }
            
            
            if(IsKeyPressed(KEY_D)){
                sag=true;
                //PlayMusicStream(efekt);
                
            }
            
            if(sag){
                sagTimer +=1;
                    if(sagTimer <= 10){
                        position.x -=0.3f;
                        timmyBoundingBox.min.x -= 0.3f;
                        timmyBoundingBox.max.x -= 0.3f;
                        
                        camera.position.x -=0.3f;
                        camera.target.x -=0.3f;
                    }
                    
            }
            
            if(sagTimer >= 11){
                sagTimer=0;
                sag=false;
                //PlayMusicStream(efekt);
                animIndex=0;
            }
            
            if(IsKeyPressed(KEY_S)){
                asagi=true;
                //PlayMusicStream(efekt);
            }
            
            if(asagi){
                asagiTimer +=1;
                timmyBoundingBox.max.y = 0.0f;
                timmyBoundingBox.min.y = 0.0f;
            }
            if(asagiTimer >= 58){
                timmyBoundingBox.max.y = 1.5f;
                timmyBoundingBox.min.y = 0.0f;
                asagiTimer=0;
                asagi =false;
                //PlayMusicStream(efekt);
                animIndex =0;
            }
            //------------ ALANDAN ÇIKMA --------------/
            if(position.x >6.0f || position.x <(-6.0f)){
                /*******PUAN EKRANİ GELMELİ*********/
                currentScreen = END_GAME;
            }
            
            railSol.z -= speed + 0.02;
            railOrta.z -= speed +0.02;
            railSag.z -= speed + 0.02;
            float lastRailZ = railSol.z + (numRailSegments - 1) * railSegmentSpacing;
            if (lastRailZ < -15) {
            railSol.z += numRailSegments * railSegmentSpacing;
            railOrta.z += numRailSegments * railSegmentSpacing;
            railSag.z += numRailSegments * railSegmentSpacing;
            }
            ClearBackground(DARKGREEN);  
           
            BeginMode3D(camera);
            DrawModelEx(timmy, position,rotationAxis,rotationAngle,scale,WHITE);    // Draw animated model
            
            //DrawCube((Vector3){ 0.0f , -1.0f, 0.0f  },100.0f,0.0f,100.0f,(Color){ 211,211,211, 255 });
            
            for (int i = 0; i < numRailSegments; i++) {
                 Vector3 railSolDraw = { railSol.x, railSol.y, railSol.z + i * railSegmentSpacing };
                  DrawCubeTexture(railTex, railSolDraw,3.0f, 1.0f, 2.0f, WHITE);
            }
                    for (int i = 0; i < numRailSegments; i++) {
                        Vector3 railOrtaDraw = { railOrta.x, railOrta.y, railOrta.z + i * railSegmentSpacing };
                        DrawCubeTexture(railTex, railOrtaDraw, 3.0f, 1.0f, 2.0f, WHITE);
            }
                    for (int i = 0; i < numRailSegments; i++) {
                        Vector3 railSagDraw = { railSag.x, railSag.y, railSag.z + i * railSegmentSpacing };
                        DrawCubeTexture(railTex, railSagDraw, 3.0f, 1.0f, 2.0f, WHITE);
            }
            
            currentAgac = headAgac;
            while(currentAgac != NULL){
                currentAgac->positionAgac.x = -6.5;
                currentAgac = currentAgac->nextAgac;
            }
            currentAgac = headAgac;
            while(currentAgac != NULL){
                DrawModelEx(currentAgac->agacModel,
                currentAgac->positionAgac,
                agacrotationAxis,
                agacrotationAngle,
                agacscale,
                WHITE);
                currentAgac = currentAgac->nextAgac;
            }
            currentAgac = headAgac;
            while(currentAgac != NULL){
                currentAgac->positionAgac.x = 6.5;
                currentAgac = currentAgac->nextAgac;
            }
            currentAgac = headAgac;
            while(currentAgac != NULL){
            DrawModelEx(currentAgac->agacModel,
                currentAgac->positionAgac,
                agacrotationAxis,
                agacrotationAngle,
                agacscale,
                WHITE);
            currentAgac = currentAgac->nextAgac;}

            currentPart = headPart;
            while (currentPart != NULL) {
                for (int i = 0; i < 3; i++){
                    if(currentPart->engeller[i].draw==true){
                    currentPart->engeller[i].positionBariyer.z = currentPart->positionPart.z;
                    DrawModelEx(currentPart->engeller[i].bariyer,
                                currentPart->engeller[i].positionBariyer,
                                currentPart->engeller[i].RotationAxis,
                                currentPart->engeller[i].RotationAngle,
                                currentPart->engeller[i].Scale,
                                WHITE);
                }}
                currentPart = currentPart->nextPart; 
            }
            currentPart = headPart;
            if (headPart->positionPart.z<-1.5)
            {
                for(int i =0; i<3;i++){
                    if(currentPart->nextPart->engeller[i].draw==true){
                    if(CheckCollisionBoxes(currentPart->nextPart->engeller[i].bariyerBoundingBox,timmyBoundingBox)){
                          currentScreen = END_GAME;
                          jump = false ;
                          //StopMusicStream(efekt);
                    }
                    
                    }
            }}
            else{
            for(int i =0; i<3;i++){
                if(currentPart->engeller[i].draw==true){
                if(CheckCollisionBoxes(currentPart->engeller[i].bariyerBoundingBox,timmyBoundingBox)){
                        currentScreen = END_GAME;
                        jump = false ;
                        //StopMusicStream(efekt);
                     
                }
                

                }
            }}

            EndMode3D();

            //DrawText(TextFormat("Animation: %s", anim.name), 10, GetScreenHeight() - 20, 10, DARKGRAY);
            DrawText(TextFormat("Puan: %d", puan), screenWidth -200 , 20, 40, BLACK);
        
        currentPart = headPart;
        while(currentPart != NULL){
        currentPart->positionPart.z -= 0.09 - speed;
        currentPart = currentPart->nextPart;
        /**** HIZLANDIRMA *****/
        
        startTime = GetFrameTime();
        if(startTime%7 == 0 ){ speed = speed - 0.0000001;}
        }
        
        currentPart = headPart;
        while(currentPart != NULL){
        for(int i=0;i<3;i++){
            currentPart->engeller[i].bariyerBoundingBox.min.z -= (0.09f - speed); 
            currentPart->engeller[i].bariyerBoundingBox.max.z -= (0.09f - speed); 
            }
            currentPart = currentPart->nextPart;
            /**** HIZLANDIRMA *****/
        
            startTime = GetFrameTime();
            if(startTime%7 == 0 ){ speed = speed - 0.0000001;}
            
        }

        currentAgac = headAgac;
        while(currentAgac != NULL){
        currentAgac->positionAgac.z -= 0.09 - speed;
        currentAgac = currentAgac->nextAgac;
        /**** HIZLANDIRMA *****/
        
        startTime = GetFrameTime();
        if(startTime%7 == 0 ){ speed = speed - 0.0000001;}
        
        }
                if(IsKeyPressed(KEY_P)){
                    pauseOpen = !pauseOpen;
                    if (pauseOpen) {
                        gamePaused = true; // Oyun durduruluyor
                        currentScreen = PAUSE;
                    } else {
                        gamePaused = false; // Oyun devam ediyor
                    }
                }

                /*if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    currentScreen = MAIN_MENU;
                }*/
            }
            else if(currentScreen == END_GAME){
                DrawText(TextFormat("GAME OVER" ), 700, 50 + screenHeight/2, 40, BLACK);
                DrawText(TextFormat("PUAN : %d", puan ), 700 , (-70) + screenHeight/2, 40, BLACK);
                if(IsKeyPressed(KEY_T)){
                    puan=1;
                    currentScreen=MAIN_MENU;
                    headPart=NULL;
                    railSol.x = -3.0f;
                    railOrta.x = 0.0f; 
                    railSag.x =3.0f;
                    railSol.z =-10.0f;
                    railOrta.z =-10.0f;
                    railSag.z =-10.0f;
                    position.x = 0;
                    position.y = 0;
                    camera.position.x=0.0f;
                    camera.position.y=4.0f;
                    camera.position.z=-3.5f;
                    camera.target.x=0.0f;
                    camera.target.y=1.5f;
                    camera.target.z=0.0f;
                    timmyBoundingBox.min.x = position.x - 0.25f;
                    timmyBoundingBox.min.y = position.y - 0.0f;
                    timmyBoundingBox.min.z = position.z - 0.25f;
                    timmyBoundingBox.max.x = position.x + 0.25f;
                    timmyBoundingBox.max.y = position.y + 1.50f;
                    timmyBoundingBox.max.z = position.z + 0.25f;
                    sag=false;
                    sagTimer = 0;
                    sol = false;
                    solTimer = 0;
                    jump =false;
                    jumpTimer=0;
                    animIndex = 0;
                    
                }

            }        
        }

    EndDrawing();   
        
    }

    CloseWindow();

    return 0;
}
void DrawingConditions(Part **currentPart,Part **headPart){
        *currentPart = *headPart;
        while (*currentPart != NULL){
            if((*currentPart)->engeller[0].gecis==false && 
            (*currentPart)->engeller[1].gecis==false &&
            (*currentPart)->engeller[2].gecis==false)
            {(*currentPart)->engeller[0].draw=false;}
            if((*currentPart)->nextPart != NULL){
            for(int i=0;i<3;i++){
                if((*currentPart)->engeller[i].gecis==false){
                    if((*currentPart)->nextPart->engeller[i].gecis==false){
                        (*currentPart)->nextPart->engeller[i].draw=false;
                    }
                }
            }
            }
                *currentPart=(*currentPart)->nextPart;}
}

void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color)
{
    float x = position.x;
    float y = position.y;
    float z = position.z;

    // Set desired texture to be enabled while drawing following vertex data
    rlSetTexture(texture.id);

    // Vertex data transformation can be defined with the commented lines,
    // but in this example we calculate the transformed vertex data directly when calling rlVertex3f()
    //rlPushMatrix();
        // NOTE: Transformation is applied in inverse order (scale -> rotate -> translate)
        //rlTranslatef(2.0f, 0.0f, 0.0f);
        //rlRotatef(45, 0, 1, 0);
        //rlScalef(2.0f, 2.0f, 2.0f);

        rlBegin(RL_QUADS);
            rlColor4ub(color.r, color.g, color.b, color.a);
            // Front Face
            rlNormal3f(0.0f, 0.0f, 1.0f);       // Normal Pointing Towards Viewer
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Left Of The Texture and Quad
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right Of The Texture and Quad
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Right Of The Texture and Quad
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left Of The Texture and Quad
            // Back Face
            rlNormal3f(0.0f, 0.0f, - 1.0f);     // Normal Pointing Away From Viewer
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Right Of The Texture and Quad
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Right Of The Texture and Quad
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Left Of The Texture and Quad
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Left Of The Texture and Quad
            // Top Face
            rlNormal3f(0.0f, 1.0f, 0.0f);       // Normal Pointing Up
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left Of The Texture and Quad
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width/2, y + height/2, z + length/2);  // Bottom Left Of The Texture and Quad
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width/2, y + height/2, z + length/2);  // Bottom Right Of The Texture and Quad
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right Of The Texture and Quad
            // Bottom Face
            rlNormal3f(0.0f, - 1.0f, 0.0f);     // Normal Pointing Down
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width/2, y - height/2, z - length/2);  // Top Right Of The Texture and Quad
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width/2, y - height/2, z - length/2);  // Top Left Of The Texture and Quad
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Left Of The Texture and Quad
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Right Of The Texture and Quad
            // Right face
            rlNormal3f(1.0f, 0.0f, 0.0f);       // Normal Pointing Right
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right Of The Texture and Quad
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right Of The Texture and Quad
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Left Of The Texture and Quad
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Left Of The Texture and Quad
            // Left Face
            rlNormal3f( - 1.0f, 0.0f, 0.0f);    // Normal Pointing Left
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Left Of The Texture and Quad
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Right Of The Texture and Quad
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Right Of The Texture and Quad
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left Of The Texture and Quad
        rlEnd();
    //rlPopMatrix();

    rlSetTexture(0);
}