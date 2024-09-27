#pragma once

#include <stdbool.h>

struct Player;
struct GameWorld;

#include "Player.h"
#include "Bullet.h"
#include "ExplosionBillboard.h"
#include "raylib/raylib.h"

typedef enum EnemyCollisionType {
    ENEMY_COLLISION_LEFT,
    ENEMY_COLLISION_RIGHT,
    ENEMY_COLLISION_BOTTOM,
    ENEMY_COLLISION_TOP,
    ENEMY_COLLISION_FAR,
    ENEMY_COLLISION_NEAR,
    ENEMY_COLLISION_ALL,
    ENEMY_COLLISION_NONE
} EnemyCollisionType;

typedef enum EnemyPositionState {
    ENEMY_POSITION_STATE_ON_GROUND,
    ENEMY_POSITION_STATE_JUMPING,
    ENEMY_POSITION_STATE_FALLING
} EnemyPositionState;

typedef enum EnemyState {
    ENEMY_STATE_ALIVE,
    ENEMY_STATE_DYING,
    ENEMY_STATE_DEAD
} EnemyState;

typedef struct Enemy {

    int id;
    
    Vector3 pos;
    Vector3 lastPos;
    Vector3 dim;
    Vector3 vel;

    float speed;
    float jumpSpeed;

    Color color;
    Color eyeColor;
    bool showWiresOnly;
    bool showCollisionProbes;

    Model model;
    Vector3 rotationAxis;
    float rotationHorizontalAngle;
    float rotationVel;
    float rotationSpeed;
    Vector3 scale;

    // cp = collision probe
    Block cpLeft;
    Block cpRight;
    Block cpBottom;
    Block cpTop;
    Block cpFar;
    Block cpNear;

    Vector3 cpDimLR;
    Vector3 cpDimBT;
    Vector3 cpDimFN;

    EnemyPositionState positionState;
    EnemyState state;

    int maxHp;
    int currentHp;

    bool detectedByPlayer;
    bool showHpBar;
    float timeShowingHpBar;
    float hpBarShowCounter;

    int damageOnContact;

    Bullet collidedBullets[10];
    int maxCollidedBullets;
    int collidedBulletCount;

    Sound deathSound;
    ExplosionBillboard eb;

} Enemy;

Enemy createEnemy( Vector3 pos, Color color, Color eyeColor );
void drawEnemy( Enemy *enemy );
void drawEnemyExplosionBillboard( Enemy *enemy, Camera3D camera );
void drawEnemyHpBar( Enemy *enemy, Camera3D camera );
void updateEnemy( Enemy *enemy, struct Player *player, struct GameWorld *gw, float delta );
void updateEnemyCollisionProbes( Enemy *enemy );
void jumpEnemy( Enemy *enemy );
EnemyCollisionType checkCollisionEnemyBlock( Enemy *enemy, Block *block, bool checkCollisionProbes );
BoundingBox getEnemyBoundingBox( Enemy *enemy );
void createEnemies( struct GameWorld *gw, Vector3 *positions, int enemyQuantity, Color color, Color eyeColor );
void createEnemiesModel( Enemy *enemies, int enemyQuantity );
void setEnemyDetectedByPlayer( Enemy *enemy, struct Player *player, bool showLines );
void addBulletToEnemy( Enemy *enemy, Vector3 bulletPos, Color bulletColor, float bulletRadius );
Matrix getEnemyTransformMatrix( Enemy *enemy );
void cleanDeadEnemies( struct GameWorld *gw );
