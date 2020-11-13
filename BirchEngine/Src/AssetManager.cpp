#include "AssetManager.h"
#include "ECS\Components.h"

AssetManager::AssetManager(Manager* man) : manager(man)
{}

AssetManager::~AssetManager()
{}

void AssetManager::CreateProjectile(Vector2D pos, Vector2D vel, int range, int speed, std::string id)
{
	auto& projectile(manager->addEntity());
	projectile.addComponent<TransformComponent>(pos.x, pos.y, 32, 32, 1);
	projectile.addComponent<SpriteComponent>(id, false);
	projectile.addComponent<ProjectileComponent>(range, speed, vel);
	projectile.addComponent<ColliderComponent>("projectile");
	projectile.addGroup(Game::groupProjectiles);
}

void AssetManager::CreateKey(Vector2D pos, std::string id)
{
	auto& key(manager->addEntity());
	key.addComponent<TransformComponent>(pos.x, pos.y, 16, 16, 3);
	key.addComponent<SpriteComponent>(id, true);
	key.addComponent<ColliderComponent>("key");
	key.addGroup(Game::groupKeys);
	key.getComponent<SpriteComponent>().Play("Keyidle");
}

void AssetManager::CreateBullet(Vector2D pos, Vector2D vel, std::string id)
{
	auto& bullet(manager->addEntity());
	bullet.addComponent<TransformComponent>(pos.x, pos.y, 6, 6, 2);
	bullet.getComponent<TransformComponent>().velocity = vel; 
	bullet.addComponent<SpriteComponent>(id, false);
	bullet.addComponent<ColliderComponent>("bullet");
	bullet.addGroup(Game::groupBullets);
}

void AssetManager::AddTexture(std::string id, const char* path)
{
	textures.emplace(id, TextureManager::LoadTexture(path));
}

SDL_Texture* AssetManager::GetTexture(std::string id)
{
	return textures[id];
}

void AssetManager::AddFont(std::string id, std::string path, int fontSize)
{
	fonts.emplace(id, TTF_OpenFont(path.c_str(), fontSize));
}
TTF_Font* AssetManager::GetFont(std::string id)
{
	return fonts[id];
}