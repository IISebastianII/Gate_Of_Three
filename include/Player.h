#pragma once

#include "Entity.h"
#include "Spell.h"

#include <SFML/Graphics.hpp>

#include <map>
#include <string>
#include <vector>

// Contains player movement, combat, statistics and animations.
class Player : public Entity
{
public:
    Player();

    using Entity::update;

    void handleEvent(const sf::Event& event);
    void update(float deltaTime) override;
    void update(float deltaTime, const std::vector<sf::FloatRect>& solidColliders, const sf::FloatRect& worldBounds);
    void draw(sf::RenderTarget& target) const override;
    void receiveDamage(int damage, sf::Vector2f sourcePosition) override;

    void setFeetPosition(sf::Vector2f feetPosition);
    void setDeadFeetPosition(sf::Vector2f feetPosition);
    void resetForRestart(sf::Vector2f feetPosition);
    void healToFull();
    bool consumeSpellCastRequest();
    bool consumeSpellProjectileSpawnRequest();
    bool trySpendSpellResources();
    bool beginSpellCast();
    sf::Vector2f getCenter() const;
    sf::Vector2f getFacingDirection() const;
    sf::Vector2f getSpellSpawnPosition() const;
    const Spell& getLongBlastSpell() const;
    bool hasLongBlastUnlocked() const;
    void unlockLongBlastSpell();
    sf::FloatRect getBounds() const override;
    sf::FloatRect getAttackBounds() const;
    bool isAttackActive() const;
    bool isDodging() const;
    bool isDead() const;
    int getAttackDamage() const;
    int getHealth() const;
    int getMaxHealth() const;
    int getMana() const;
    int getMaxMana() const;

private:
    enum class AnimationState
    {
        Idle,
        Run,
        Jump,
        Fall,
        Attack,
        DashAttack,
        Slide,
        Death
    };

    struct Animation
    {
        std::vector<sf::Texture> frames;
        float frameTime = 0.12f;
        bool loop = true;
    };

    void loadAnimations();
    void loadFrameSeries(AnimationState state, const std::string& directory, const std::string& prefix, int first, int last, float frameTime, bool loop);
    void applyInput();
    void moveHorizontally(float deltaTime, const std::vector<sf::FloatRect>& solidColliders);
    void moveVertically(float deltaTime, const std::vector<sf::FloatRect>& solidColliders);
    void keepInsideWorld(const sf::FloatRect& worldBounds);
    void updateActionTimers(float deltaTime);
    void updateAnimation(float deltaTime);
    void setAnimationState(AnimationState state);
    void startDeath();
    const Animation* currentAnimation() const;
    float animationDuration(AnimationState state, float fallbackDuration) const;
    void syncDrawable();

    static constexpr float moveSpeed_ = 300.f;
    static constexpr float slideSpeed_ = 580.f;
    static constexpr float slideDuration_ = 0.36f;
    static constexpr float attackActiveStart_ = 0.16f;
    static constexpr float attackActiveEnd_ = 0.38f;
    static constexpr float attackRange_ = 76.f;
    static constexpr float attackHeight_ = 196.f;
    static constexpr int attackDamage_ = 1;
    static constexpr float jumpVelocity_ = -820.f;
    static constexpr float gravity_ = 1850.f;
    static constexpr float maxStepHeight_ = 36.f;
    static constexpr float textureScale_ = 3.f;
    static constexpr float damageInvulnerabilityDuration_ = 0.85f;
    static constexpr float manaRegenerationInterval_ = 4.f;
    static constexpr int maxHealth_ = 8;
    static constexpr int maxMana_ = 4;
    static constexpr std::size_t spellProjectileSpawnFrame_ = 3;
    static const sf::Vector2f colliderSize_;

    Spell longBlastSpell_ = Spell(2, 860.f, 0.34f, 2);
    bool longBlastUnlocked_ = false;
    int health_ = maxHealth_;
    int mana_ = maxMana_;
    bool facingRight_ = true;
    bool onGround_ = false;
    bool jumpQueued_ = false;
    bool attackQueued_ = false;
    bool slideQueued_ = false;
    bool spellCastRequested_ = false;
    bool spellProjectileSpawnRequested_ = false;
    bool spellProjectileSpawned_ = false;
    bool attacking_ = false;
    bool spellCasting_ = false;
    bool sliding_ = false;
    bool dead_ = false;
    float attackTimer_ = 0.f;
    float attackDuration_ = 0.f;
    float spellTimer_ = 0.f;
    float spellDuration_ = 0.f;
    float slideTimer_ = 0.f;
    float slideDirection_ = 1.f;
    float damageInvulnerabilityTimer_ = 0.f;
    float spellCooldownTimer_ = 0.f;
    float manaRegenerationTimer_ = 0.f;

    std::map<AnimationState, Animation> animations_;
    AnimationState currentState_ = AnimationState::Idle;
    std::size_t currentFrame_ = 0;
    float animationAccumulator_ = 0.f;

    sf::Sprite sprite_;
    sf::RectangleShape placeholder_;
};
