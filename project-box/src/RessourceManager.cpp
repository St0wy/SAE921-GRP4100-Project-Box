#include "RessourceManager.h"
#include <optional>
#include <filesystem>

RessourceManager* RessourceManager::instance_ = nullptr;
std::mutex RessourceManager::mutex_;

RessourceManager* RessourceManager::GetInstance()
{
	std::lock_guard lock(mutex_);
	if (instance_ == nullptr)
	{
		instance_ = new RessourceManager();
	}
	return instance_;
}

std::optional<sf::Texture*> RessourceManager::GetTexture(const std::string& path)
{
	// ReSharper disable once CppTooWideScopeInitStatement
	const auto texture = textures_.find(path);
	if (texture != textures_.end())
	{
		return texture->second.get();
	}
	if (std::filesystem::exists(path))
	{
		textures_[path] = std::make_unique<sf::Texture>();
		textures_[path]->loadFromFile(path);
		return textures_[path].get();
	}
	return {};
}