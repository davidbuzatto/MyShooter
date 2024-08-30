#pragma once

#include "raylib.h"

typedef enum GameWorldPlayerInputType {
    GAME_WORLD_PLAYER_INPUT_TYPE_KEYBOARD,
    GAME_WORLD_PLAYER_INPUT_TYPE_GAMEPAD
} GameWorldPlayerInputType;

typedef enum CameraType {
    CAMERA_TYPE_THIRD_PERSON_FIXED,
    CAMERA_TYPE_FIRST_PERSON
} CameraType;

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

typedef enum PlayerCollisionType {
    PLAYER_COLLISION_LEFT,
    PLAYER_COLLISION_RIGHT,
    PLAYER_COLLISION_BOTTOM,
    PLAYER_COLLISION_TOP,
    PLAYER_COLLISION_FAR,
    PLAYER_COLLISION_NEAR,
    PLAYER_COLLISION_ALL,
    PLAYER_COLLISION_NONE
} PlayerCollisionType;

typedef enum PlayerPositionState {
    PLAYER_POSITION_STATE_ON_GROUND,
    PLAYER_POSITION_STATE_JUMPING,
    PLAYER_POSITION_STATE_FALLING
} PlayerPositionState;

typedef enum PlayerState {
    PLAYER_STATE_ALIVE,
    PLAYER_STATE_DEAD
} PlayerState;

typedef enum PlayerWeaponState {
    PLAYER_WEAPON_STATE_IDLE,
    PLAYER_WEAPON_STATE_READY
} PlayerWeaponState;

typedef enum PowerUpType {
    POWER_UP_TYPE_HP,
    POWER_UP_TYPE_AMMO
} PowerUpType;

typedef enum PowerUpCollisionType {
    POWER_UP_COLLISION_ALL,
    POWER_UP_COLLISION_NONE
} PowerUpCollisionType;

typedef enum PowerUpPositionState {
    POWER_UP_POSITION_STATE_ON_GROUND,
    POWER_UP_POSITION_STATE_JUMPING,
    POWER_UP_POSITION_STATE_FALLING
} PowerUpPositionState;

typedef enum PowerUpState {
    POWER_UP_STATE_ACTIVE,
    POWER_UP_STATE_CONSUMED
} PowerUpState;

typedef enum EntityType {
    ENTITY_TYPE_NONE,
    ENTITY_TYPE_BLOCK,
    ENTITY_TYPE_OBSTACLE,
    ENTITY_TYPE_ENEMY
} EntityType;

extern int entityIdCounter;

typedef struct Block {

    int id;

    Vector3 pos;
    Vector3 dim;
    Color color;
    Color tintColor;
    Color touchColor;
    bool visible;

    Model model;
    bool renderModel;
    bool renderTouchColor;

} Block;

typedef struct Bullet {
    int id;
    Vector3 pos;
    float radius;
    Color color;
    bool collided;
} Bullet;

typedef struct Player {

    Vector3 pos;
    Vector3 lastPos;
    Vector3 dim;
    Vector3 vel;

    float speed;
    float walkingSpeed;
    float runningSpeed;
    float jumpSpeed;

    Color color;
    bool showWiresOnly;
    bool showCollisionProbes;

    Model model;

    Vector3 rotationAxis;
    float rotationHorizontalAngle;
    float rotationVerticalAngle;
    float rotationHVel;
    float rotationVVel;
    float rotationSpeed;
    Vector3 scale;

    Bullet *bullets;
    int bulletQuantity;
    int maxBullets;
    PlayerWeaponState weaponState;

    float timeToNextShot;
    float timeToNextShotCounter;

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

    PlayerPositionState positionState;

    int maxHp;
    int currentHp;
    bool immortal;
    
    int currentAmmo;

    PlayerState state;

} Player;

typedef struct PowerUp {

    int id;
    
    Vector3 pos;
    Vector3 lastPos;
    float radius;
    Vector3 vel;

    float jumpSpeed;

    Color hpColor;
    Color ammoColor;
    
    bool showWiresOnly;

    Model model;
    Vector3 rotationAxis;
    float rotationHorizontalAngle;
    float rotationVel;
    float rotationSpeed;
    Vector3 scale;

    PowerUpType type;
    PowerUpState state;
    PowerUpPositionState positionState;

} PowerUp;

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

    Bullet collidedBullets[5];
    int maxCollidedBullets;
    int collidedBulletCount;
    float collidedBulletsDistance[5];
    float collidedBulletsHAngle[5];
    float collidedBulletsVAngle[5];

} Enemy;

typedef struct IdentifiedRayCollision {
    int entityId;
    EntityType entityType;
    RayCollision collision;
} IdentifiedRayCollision;

typedef struct GameWorld {

    Camera3D camera;
    CameraType cameraType;
    
    Player player;

    Enemy *enemies;
    int enemyQuantity;

    PowerUp *powerUps;
    int powerUpQuantity;
    
    Block ground;

    Block *obstacles;
    int obstacleQuantity;

    Block leftWall;
    Block rightWall;
    Block farWall;
    Block nearWall;

    Bullet collidedBullets[50];
    int maxCollidedBullets;
    int collidedBulletCount;

    GameWorldPlayerInputType playerInputType;

} GameWorld;
