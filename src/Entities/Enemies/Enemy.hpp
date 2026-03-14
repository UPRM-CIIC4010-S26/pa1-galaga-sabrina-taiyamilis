#pragma once
#include "Projectile.hpp"
#include "Math.hpp"
#include "ImageManager.hpp"
#include "Animation.hpp"
#include <iostream>

class Enemy {
    protected:
        float angle = 90;
        int cooldown = 60;
        int speed = 3;
        bool spawning = false;
        bool frame = false;
        int frameCooldown = 30;
        
    public:
        int health = 1;
        int scoreValue = 0; // points this enemy gived when defeated
        std::pair<float, float> position;
        HitBox hitBox;

        inline static float direction = 0.5;
        inline static int directionChange = 100;
        inline static std::vector<std::pair<std::pair<float, float>, Enemy*>> enemies;

        Enemy() {}

        Enemy(float x, float y) {
            position.first = x;
            position.second = y;
            hitBox = HitBox(x, y, 30, 30);
        }

        virtual void draw() = 0;
        virtual void update(std::pair<float, float> pos, HitBox target) = 0;
        virtual void attack(HitBox target) = 0;

        void frameChange() {
            frameCooldown--;

             if (frameCooldown <= 0) {
                frame = !frame;
                frameCooldown = 30;
             }
        }

        static int ManageEnemies(HitBox target) {
            
            int gainedScore = 0; //store score earned from enemies defeated in this update
            
            for (std::pair<std::pair<float, float>, Enemy*>& p : Enemy::enemies) {
                p.first.first += (p.first.first == 0) ? 0 : direction;
                if (p.second) {
                    p.second->update(p.first, target);

                    for (Projectile& p2 : Projectile::projectiles) {
                        if (p2.ID != 1 && HitBox::Collision(p.second->hitBox, p2.getHitBox())) {
                            p.second->health--;
                            p2.del = true;
                            if (p.second->health > 0) {
                                PlaySound(SoundManager::hit);  // Adds hit sound 
                            }
                        }
                    }

                    if (p.second->health <= 0) {
                        PlaySound(SoundManager::dead);  // Adds dead sound 
                        gainedScore = gainedScore + p.second->scoreValue; //add this enemy's score value

                        Animation::animations.push_back(
                            Animation(p.second->position.first, p.second->position.second, 155, 0, 33, 33, 30, 30, 4, ImageManager::SpriteSheet)
                        );
                        p.second = nullptr;
                    }
                }
            }
            
            for (int i = 0; i < Enemy::enemies.size(); i++) {
                if ((Enemy::enemies[i].second && Enemy::enemies[i].second->position.first <= -30) || 
                    (!Enemy::enemies[i].second && Enemy::enemies[i].first.first == 0 && Enemy::enemies[i].first.second == 0)) {
                    Enemy::enemies.erase(Enemy::enemies.begin() + i);
                }
            }

            directionChange++;

            if (directionChange >= 200) {
                directionChange = 0;
                direction *= -1;
            }

            return gainedScore; //return total score earned from defeated enemies

        }
        static void BulletHellAttack() {
            for (std::pair<std::pair<float, float>, Enemy*>& p : Enemy::enemies) {
                if (!p.second) continue;

        // Each enemy fires every ~90 frames randomly to avoid all firing at once
                if (GetRandomValue(0, 90) == 0) {
                    float ex = p.second->position.first + 15;
                    float ey = p.second->position.second + 15;

            // 2-way spread downward
                    Projectile::projectiles.push_back(Projectile(ex - 10, ey, 1));  // left bullet
                    Projectile::projectiles.push_back(Projectile(ex + 10, ey, 1));  // right bullet
        }
    }
}
};
