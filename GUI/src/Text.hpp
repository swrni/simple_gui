#pragma once

#include "ResourceManager.hpp"
#include "sfml_includes.hpp"


namespace GUI {

struct FontWrapper {
	const std::wstring font_resource_ =
		L"Fonts\\courier-prime\\Courier Prime.ttf";
	const sf::Font& font_ = ResourceManager<sf::Font>::Instance().getResource(
		font_resource_
	);
};

struct TextTheme {
	const unsigned character_size_ = 16;
	const sf::Color text_color_ = sf::Color::Black;
	const FontWrapper font_wrapper_ = {};
};

class TextWrapper {
	public:
		explicit TextWrapper( const sf::Vector2f& position ) : position_( position ) {}

		inline void Initialize( const TextTheme& current_theme ) {
			text_.setFont( current_theme.font_wrapper_.font_ );
			text_.setCharacterSize( current_theme.character_size_ );
			text_.setFillColor( current_theme.text_color_ );
		}

		inline const sf::Font* getFont() {
			return text_.getFont();
		}

		inline void setText(	const sf::String& string,
								const sf::Vector2f& size )
		{
			text_.setString( string );
			ResetTextPosition( size );
		}

		inline void ReadInput(	const sf::Event& event,
								const sf::Vector2f& size )
		{
			std::wstring text( text_.getString().toWideString() );
	
			if ( sf::Keyboard::isKeyPressed( sf::Keyboard::BackSpace ) ) {
				if ( !text.empty() ) {
					text.pop_back();
				}
			}
			else {
				text += static_cast<wchar_t>( event.text.unicode );
			}

			setText( text, size );
		}

		inline void ResetTextPosition( const sf::Vector2f& size ) {
			const sf::Vector2f text_center = getTextLocalCenter( text_ );
	
			const sf::Vector2f button_center = getLocalCenter( size );

			const sf::Vector2f offset = {
				button_center.x - text_center.x,
				button_center.y - text_center.y
			};

			const sf::Vector2i new_position( position_ + offset );
			text_.setPosition(
				static_cast<sf::Vector2f>( new_position )
			);
		}

		constexpr inline const sf::Text& getText() const {
			return text_;
		}

	private:
		inline sf::Vector2f getTextLocalCenter( const sf::Text& text ) const {
			constexpr const float magic_offset_ = 7.0f / 10.f;
			return {
				(float)( text.getGlobalBounds().width / 2 ),
				text_.getCharacterSize() * magic_offset_
			};
		}

		inline sf::Vector2f getLocalCenter( const sf::Vector2f& size ) const {
			return { size.x / 2, size.y / 2 };
		}

		sf::Text text_;
		const sf::Vector2f& position_;
};




};