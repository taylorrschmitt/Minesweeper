// Created by Taylor Schmitt on 11/25/24.

#ifndef SPRITEMANAGER_H
#define SPRITEMANAGER_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>

class SpriteManager {
    static std::map<std::string, sf::Sprite> sprites;
    static std::map<std::string, sf::Texture> textures;

public:
    static sf::Sprite& getSprite(const std::string& spriteName) {
        auto it = sprites.find(spriteName);
        return it->second;
    }

    sf::Sprite& loadSprite(const std::string& spriteName, const std::string& filePath);
    void setPosition(const std::string& spriteName, float x, float y);
    void clear();
};




std::map<std::string, sf::Sprite> SpriteManager::sprites;
std::map<std::string, sf::Texture> SpriteManager::textures;


sf::Sprite& SpriteManager::loadSprite(const std::string& spriteName, const std::string& filePath) {
    if (textures.find(filePath) == textures.end()) {
        sf::Texture texture;
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Failed to load texture: " << filePath << std::endl;
        }
        textures[filePath] = texture;
    }

    if (sprites.find(spriteName) == sprites.end()) {
        sf::Sprite sprite;
        sprite.setTexture(textures[filePath]);
        sprites[spriteName] = sprite;
    }

    return sprites[spriteName];
}

void SpriteManager::setPosition(const std::string& spriteName, float x, float y) {
    auto& sprite = getSprite(spriteName);
    sprite.setPosition(x, y);
}

void SpriteManager::clear() {
    sprites.clear();
    textures.clear();
}

#endif //SPRITEMANAGER_H
