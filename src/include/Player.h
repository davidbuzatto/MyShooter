#pragma once

#include <stdbool.h>

struct Enemy;
struct GameWorld;

#include "EntitySupport.h"
#include "Block.h"
#include "Enemy.h"
#include "PowerUp.h"
#include "GameWorld.h"
#include "ResourceManager.h"
#include "raylib/raylib.h"

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

typedef enum WeaponType {
    WEAPON_TYPE_HANDGUN,
    WEAPON_TYPE_SUBMACHINEGUN,
    WEAPON_TYPE_SHOTGUN
} WeaponType;

typedef struct Weapon {
    const char *name;
    WeaponType type;
    int ammo;
    int ammoPerPowerup;
    int bulletDamage;
    Color bulletColor;
    float bulletRadius;
    Sound bulletSound;
} Weapon;

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

    //Bullet *bullets;
    //int bulletQuantity;
    //int maxBullets;
    PlayerWeaponState weaponState;

    float timeToNextShot;
    float timeToNextShotCounter;

    float timeToNextStep;
    float timeToNextStepCounter;

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
    
    /*int currentAmmo;
    WeaponType weaponType;*/

    Weapon *currentWeapon;
    Weapon handgun;
    Weapon submachinegun;
    Weapon shotgun;

    PlayerState state;
    bool running;

} Player;

Player createPlayer( Vector3 pos );
void drawPlayer( Player *player );
void drawPlayerHud( Player *player );
void updatePlayer( Player *player, float delta );
void updatePlayerCollisionProbes( Player *player );
void jumpPlayer( Player *player );
PlayerCollisionType checkCollisionPlayerBlock( Player *player, Block *block, bool checkCollisionProbes );
PlayerCollisionType checkCollisionPlayerEnemy( Player *player, struct Enemy *enemy, bool checkCollisionProbes );
PlayerCollisionType checkCollisionPlayerPowerUp( Player *player, PowerUp *powerUp );
BoundingBox getPlayerBoundingBox( Player *player );
void createPlayerModel( Player *player );
void playerShotUsingGamepad( struct GameWorld *gw, Player *player, int gamepadId );
void playerShotUsingMouse( struct GameWorld *gw, Player *player );
void playerShot( struct GameWorld *gw, Player *player, bool useGamepad, int gamepadId );
void playerShotHandgun( struct GameWorld *gw,Player *player, IdentifiedRayCollision *irc );
void playerShotMachinegun( struct GameWorld *gw, Player *player, IdentifiedRayCollision *irc );
void playerShotShotgun( struct GameWorld *gw, Player *player, MultipleIdentifiedRayCollision *mirc );
void playerSwapWeapon( Player *player );
void playerAcquirePowerUp( Player *player, PowerUp *powerUp );
Ray getPlayerToEnemyRay( Player *player, struct Enemy *enemy );
Ray getPlayerToVector3Ray( Player *player, Vector3 v3 );
