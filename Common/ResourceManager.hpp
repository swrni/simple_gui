#pragma once

#include <map>

#include "Common.hpp"
#include "sfml_includes.hpp"

template <typename T>
class ResourceManager : NoCopy {
	public:
		static constexpr ResourceManager& Instance() {
			static ResourceManager instance;
			return instance;
		}
		const T& getResource( const std::wstring& file_name ) {
			const std::string file( ToString( file_name ) );

			const auto result = resource_map_.find( file );
			if ( result == std::end( resource_map_ ) ) {
				LoadResource<T>( file );
			}

			return resource_map_[ file ];
		}
		void setWorkingPath( const std::wstring& working_path ) {
			working_path_ = working_path;
		}
	private:
		ResourceManager() = default;

		template <typename T>
		void LoadResource( const std::string& file ) {
			T& t = resource_map_[ file ];
			if ( !t.loadFromFile( file ) ) throw;
		}
		
		template <>
		void LoadResource<sf::Texture>( const std::string& file ) {
			sf::Texture& texture = resource_map_[ file ];
			if ( !texture.loadFromFile( file ) ) throw;
			texture.setSmooth( true );
		}
		
		template <>
		void LoadResource<sf::Font>( const std::string& file ) {
			sf::Font& font = resource_map_[ file ];
			if ( !font.loadFromFile( file ) ) throw;
		}

		std::map<std::string, T> resource_map_;
		std::wstring working_path_;
};